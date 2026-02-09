#include <am.h>
#include <nemu.h>
#include <string.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00) //采样率寄存器(Hz)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04) //通道数寄存器(1=单声道,2=立体声)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08) //每次缓冲/回调的样本数
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c) //流缓冲区大小寄存器（字节）
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10) //写1触发NEMU端初始化
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14) //音频已写入/队列中字节数（状态寄存器）

void __am_audio_init() {
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {//读出存在标志present以及流缓冲区的大小bufsize
  cfg->present = true;
  cfg->bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {//声卡控制寄存器,可根据写入的freq, channels和samples对声卡进行初始化.
  //将期望的参数写入MMIO寄存器并触发NEMU端进行初始化
  outl(AUDIO_FREQ_ADDR, ctrl->freq);
  outl(AUDIO_CHANNELS_ADDR, ctrl->channels);
  outl(AUDIO_SAMPLES_ADDR, ctrl->samples);
  outl(AUDIO_INIT_ADDR, 1);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {//声卡状态寄存器,可读出当前流缓冲区已经使用的大小count
  stat->count = inl(AUDIO_COUNT_ADDR);
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {//AM声卡播放寄存器,可将[buf.start, buf.end)区间的内容作为音频数据写入流缓冲区. 
                                            //若当前流缓冲区的空闲空间少于即将写入的音频数据,此次写入将会一直等待,直到有足够的空闲空间将音频数据完全写入流缓冲区才会返回.
  int len = ctl->buf.end - ctl->buf.start;//内存长度
  //源指针，指向要播放的数据
  uint8_t *src = (uint8_t *)ctl->buf.start;
  //目的指针，指向nemu给的写入地址
  uint8_t *dst = (uint8_t *)(uintptr_t)AUDIO_SBUF_ADDR;//nemu.h里面定义的音频缓冲区地址
  int sbufsize = inl(AUDIO_SBUF_SIZE_ADDR);//流缓冲区大小，单次最大写入量


  //分块拷贝，每块大小不超过NEMU暴露的sbuf大小
  int off = 0;//表示已经从源缓冲区src复制的字节偏移量，用来控制进度
  while (off < len) {
    int chunk = len - off;//当前循环准备写入的字节大小，用来控制单次写入量
    if (chunk > sbufsize) chunk = sbufsize;
    memcpy(dst, src + off, chunk);//写入chunk字节的数据

    //告知NEMU已写入的缓冲区字节长度
    outl(AUDIO_COUNT_ADDR, chunk);
    
    //更新
    off += chunk;
  }
}
