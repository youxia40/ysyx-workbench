#ifndef WAVE_H
#define WAVE_H

#include <cstdint>

class Vysyx_25040118_top;


void wave_init(Vysyx_25040118_top* top);
void wave_dump(uint64_t time);
void wave_close();


#endif
