/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"

#define PS_GPIO_BASE 0xE000A000

#define GPIO_DATA_0  ((volatile uint32_t *)(PS_GPIO_BASE + 0x040))
#define GPIO_DIRM_0  ((volatile uint32_t *)(PS_GPIO_BASE + 0x204))
#define GPIO_OEN_0   ((volatile uint32_t *)(PS_GPIO_BASE + 0x208))

#define LED_PIN 0 // MIO 0


int main()
{
    init_platform();

    print("Hello World\n\r");
    print("Successfully ran Hello World application");
    *GPIO_DIRM_0 |= (1 << LED_PIN);
    *GPIO_OEN_0  |= (1 << LED_PIN);

    while(1) {
        *GPIO_DATA_0 |= (1 << LED_PIN);  // Turn ON (Set bit 0 to 1)
        sleep(1);
        print("Hello World\n\r");
        *GPIO_DATA_0 &= ~(1 << LED_PIN); // Turn OFF (Clear bit 0 to 0)
        sleep(1);
    }

    
    cleanup_platform();
    return 0;
}
