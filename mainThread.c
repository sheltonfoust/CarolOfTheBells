
#include <stdint.h>
#include <stddef.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

#include "ti_drivers_config.h"
void *mainThread(void *arg0)
{
    char        input;
    UART_Handle uart;
    UART_Params uartParams;
    GPIO_init();
    UART_init();
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.baudRate = 115200;
    uart = UART_open(CONFIG_UART_0, &uartParams);
    if (uart == NULL) {
        while (1);
    }
    while (1) {
        UART_read(uart, &input, 1);
        UART_write(uart, &input, 1);
    }
}
