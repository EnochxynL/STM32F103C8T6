#include "main.h"
#include <rtthread.h>

uint32_t HAL_GetTick(void) {
  return rt_tick_get();
}