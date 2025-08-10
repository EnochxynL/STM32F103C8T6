#include "main.h"
#include <rtthread.h>

void HAL_Delay(uint32_t Delay) {
  rt_thread_mdelay(Delay);
}
