#include "unity_config.h"
#include <libopencm3/lm4f/systemcontrol.h>
#include <libopencm3/lm4f/rcc.h>
#include <libopencm3/lm4f/uart.h>

void unityOutputStart()
{
    periph_clock_enable(RCC_UART1);
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");

    uart_disable(UART1);

    uart_clock_from_piosc(UART1);

    uart_set_baudrate(UART1, 115200);
    uart_set_databits(UART1, 8);
    uart_set_parity(UART1, UART_PARITY_NONE);
    uart_set_stopbits(UART1, 1);

    uart_set_flow_control(UART1, UART_FLOWCTL_RTS_CTS);

    uart_enable(UART1);
}

void unityOutputChar(char c)
{
    uart_send_blocking(UART1, c);
}

void unityOutputFlush()
{

}

void unityOutputComplete()
{
    //uart_disable(UART1);
    //periph_clock_disable(RCC_UART1);
}