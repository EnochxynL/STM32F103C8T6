#include "main.h"
#include "cmsis_os.h"

void HAL_Delay(uint32_t Delay) {
  osDelay(Delay);
}
