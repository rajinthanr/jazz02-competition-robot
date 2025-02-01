#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus

#include "loop.h" // cpp headers

extern "C"
{
#endif

     #include "main.h"  // c headers

    void init_debug(void);
    void debug(void);
    void floats(std::stringstream *_rxdata, float *k);
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif // DEBUG_H