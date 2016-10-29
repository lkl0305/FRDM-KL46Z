/*
 * Copyright (c) 2013 - 2016, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 **/

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "fsl_debug_console.h"
#include "hal_key.h"
#include "gizwits_product.h"
#include "gizwits_protocol.h"

#define SW1_CLOCK_IP_NAME kCLOCK_PortC
#define SW1_PORT PORTC
#define SW1_GPIO GPIOC
#define SW1_PIN 3u

#define SW2_CLOCK_IP_NAME kCLOCK_PortC
#define SW2_PORT PORTC
#define SW2_GPIO GPIOC
#define SW2_PIN 12u

dataPoint_t currentDataPoint;

#define GPIO_KEY_NUM 2
key_typedef_t singleKey[GPIO_KEY_NUM];
keys_typedef_t keys;

void Key1_ShortPress(void)
{
	PRINTF("KEY1 SHORT PRESS\r\n");
}

void Key1_LongPress(void)
{
	PRINTF("KEY1 LONG PRESS, Wifi Reset\r\n");
	gizwitsSetMode(WIFI_RESET_MODE);
}

void Key2_ShortPress(void)
{
	PRINTF("KEY2 SHORT PRESS, Soft AP mode\r\n");
  gizwitsSetMode(WIFI_SOFTAP_MODE);
}

void Key2_LongPress(void)
{
	PRINTF("KEY2 LONG PRESS, AirLink mode\r\n");
	GPIO_ClearPinsOutput(GPIOE, 1u << 29u);
  gizwitsSetMode(WIFI_AIRLINK_MODE);
}

void keyInit(void)
{
    singleKey[0] = Key_InitOne(SW1_CLOCK_IP_NAME, SW1_PORT, SW1_GPIO, SW1_PIN, Key1_ShortPress, Key1_LongPress);
    singleKey[1] = Key_InitOne(SW1_CLOCK_IP_NAME, SW2_PORT, SW2_GPIO, SW2_PIN, Key2_ShortPress, Key2_LongPress);
    keys.singleKey = (key_typedef_t *)&singleKey;
    Key_ParaInit();
}

void LedInit(void)
{
	gpio_pin_config_t red_Led_Config = {
			kGPIO_DigitalOutput,
			1,
	};
	GPIO_PinInit(GPIOE, 29u, &red_Led_Config);

	gpio_pin_config_t blue_Led_Config = {
			kGPIO_DigitalOutput,
			1,
	};
	GPIO_PinInit(GPIOD, 5u, &blue_Led_Config);
}

void userInit(void)
{
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));

	SysTick_Config(USEC_TO_COUNT(1000, CLOCK_GetCoreSysClkFreq()));
	NVIC_EnableIRQ(SysTick_IRQn);
	LedInit();
	keyInit();
	uartInit();

	gizwitsInit();

	PRINTF("MCU Init Success\r\n");
}

void userHandle(void)
{
    /*

    */

}


void SysTick_Handler(void)
{
	gizTimerMs();
	Key_Handle();
}

/*!
 * @brief Application entry point.
 */
int main(void) {
  /* Init board hardware. */
  BOARD_InitPins();
  BOARD_BootClockRUN();
  BOARD_InitDebugConsole();

  /* Add your code here */
  userInit();

  for(;;) { /* Infinite loop to avoid leaving the main function */
  	userHandle();
  	gizwitsHandle((dataPoint_t *)&currentDataPoint);
    __asm("NOP"); /* something to use as a breakpoint stop while looping */
  }
}
