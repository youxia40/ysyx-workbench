/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <common.h>
#include <device/map.h>
#include <SDL2/SDL.h>

enum {
  reg_freq,//可写入相应的初始化参数
  reg_channels,//可写入相应的初始化参数
  reg_samples,//可写入相应的初始化参数
  reg_sbuf_size,//可读出流缓冲区的大小
  reg_init,//用于初始化, 写入后将根据设置好的freq, channels和samples来对SDL的音频子系统进行初始化
  reg_count,//可以读出当前流缓冲区已经使用的大小
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;
static SDL_AudioDeviceID audio_dev = 0;

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  int reg = offset / sizeof(uint32_t);
  if (!is_write) {
    if (reg == reg_count && audio_dev) {
      audio_base[reg_count] = SDL_GetQueuedAudioSize(audio_dev);
    }
    return;
  }

  if (reg == reg_init) {
    SDL_AudioSpec want = {};
    want.freq = audio_base[reg_freq];
    want.format = AUDIO_S16SYS;
    want.channels = (uint8_t)audio_base[reg_channels];
    want.samples = audio_base[reg_samples];

    SDL_InitSubSystem(SDL_INIT_AUDIO);
    audio_dev = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
    if (audio_dev) {
      SDL_PauseAudioDevice(audio_dev, 0);
    }
  } else if (reg == reg_count) {
    uint32_t cnt = audio_base[reg_count];
    if (cnt > 0) {
      if (audio_dev) {
        // queue audio from sbuf
        SDL_QueueAudio(audio_dev, sbuf, cnt);
        audio_base[reg_count] = SDL_GetQueuedAudioSize(audio_dev);
      } else {
        // if no device, report count as 0 so guest won't wait forever
        audio_base[reg_count] = 0;
      }
    }
  }
}

void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (uint32_t *)new_space(space_size);
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif

  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, NULL);
  if (audio_base) audio_base[reg_sbuf_size] = CONFIG_SB_SIZE;
}
