/*
 * gizwits_product.c
 *
 *  Created on: 2016Äê10ÔÂ29ÈÕ
 *      Author: lkl03
 */


#include <stdio.h>
#include <string.h>

#include "gizwits_product.h"

extern dataPoint_t currentDataPoint;

int8_t gizwitsEventProcess(eventInfo_t *info, uint8_t *data, uint32_t len)
{
  uint8_t i = 0;
  dataPoint_t *dataPointPtr = (dataPoint_t *)data;
  moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)data;

  if((NULL == info) || (NULL == data))
  {
    return -1;
  }

  for(i=0; i<info->num; i++)
  {
    switch(info->event[i])
    {
      case EVENT_GREEN_LED:
        currentDataPoint.valuegreen_led = dataPointPtr->valuegreen_led;
        GIZWITS_LOG("Evt: EVENT_GREEN_LED %d \n", currentDataPoint.valuegreen_led);
        if(0x01 == currentDataPoint.valuegreen_led)
        {
        	GPIO_ClearPinsOutput(GPIOD, 1u << 5u);
          //user handle
        }
        else
        {
        	GPIO_SetPinsOutput(GPIOD, 1u << 5u);
          //user handle
        }
        break;




      case WIFI_SOFTAP:
        break;
      case WIFI_AIRLINK:
        break;
      case WIFI_STATION:
        break;
      case WIFI_CON_ROUTER:
      	GPIO_SetPinsOutput(GPIOE, 1u << 29u);
        break;
      case WIFI_DISCON_ROUTER:
        break;
      case WIFI_CON_M2M:
        break;
      case WIFI_DISCON_M2M:
        break;
      case WIFI_RSSI:
        GIZWITS_LOG("RSSI %d\n", wifiData->rssi);
        break;
      case TRANSPARENT_DATA:
        GIZWITS_LOG("TRANSPARENT_DATA \n");
        //user handle , Fetch data from [data] , size is [len]
        break;
      default:
        break;
    }
  }

  return 0;
}

void mcuRestart(void)
{
    NVIC_SystemReset();
}

void UART1_IRQHandler(void)
{
	uint8_t data = 0;

	if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART1))
	{
		data = UART_ReadByte(UART1);
		gizPutData(&data, 1);
	}
}

int32_t uartWrite(uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;
    uint8_t data = 0x55;

    if(NULL == buf)
    {
        return -1;
    }
#ifdef PROTOCOL_DEBUG
    GIZWITS_LOG("MCU2WiFi[%4d:%4d]: ", gizGetTimerCount(), len);
#endif

    for(i=0; i<len; i++)
    {
    	UART_WriteBlocking(UART1, &buf[i], 1);
#ifdef PROTOCOL_DEBUG
        GIZWITS_LOG("%02x ", buf[i]);
#endif
        if(i >=2 && buf[i] == 0xFF)
        {
        	UART_WriteBlocking(UART1, &data, 1);
#ifdef PROTOCOL_DEBUG
        GIZWITS_LOG("%02x ", 0x55);
#endif
        }
    }

#ifdef PROTOCOL_DEBUG
    GIZWITS_LOG("\n");
#endif

    return len;
}

void uartInit(void)
{
  CLOCK_EnableClock(kCLOCK_PortE);
  PORT_SetPinMux(PORTE, 0u, kPORT_MuxAlt3);
  PORT_SetPinMux(PORTE, 1u, kPORT_MuxAlt3);

  uart_config_t uart1_Config;
  UART_GetDefaultConfig(&uart1_Config);
  uart1_Config.baudRate_Bps = 9600;
  uart1_Config.enableRx = true;
  uart1_Config.enableTx = true;
  UART_Init(UART1, &uart1_Config, CLOCK_GetFreq(BUS_CLK));

  UART_EnableInterrupts(UART1, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
  NVIC_SetPriority(UART1_IRQn, 0x01);
  NVIC_EnableIRQ(UART1_IRQn);
}



