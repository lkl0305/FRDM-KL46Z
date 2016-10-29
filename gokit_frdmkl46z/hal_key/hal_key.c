/*
 * hal_key.c
 *
 *  Created on: 2016年10月27日
 *      Author: lkl03
 */

#include "hal_key.h"

extern keys_typedef_t keys;

uint8_t keyCountTime;
static uint8_t keyTotolNum = 0;

//
//void TIM2_IRQHandler(void)
//{
//        Key_Handle();
//}
//
//void timer2Init(void)
//{
//	;
//}



uint16_t Get_KeyState(void)
{
  uint8_t i = 0;
  uint16_t readKey = 0;              //必要,初始化

  //GPIO Cyclic scan
  for(i = 0; i < keys.keyTotolNum; i++)
  {
		if (!GPIO_ReadPinInput(keys.singleKey[i].keyGpioBase, keys.singleKey[i].keyPin))
		{
			G_SET_BIT(readKey, keys.singleKey[i].keyNum);
		}
  }

  return readKey;
}

uint16_t Read_KeyValue(void)
{
    static uint8_t keyCheck = 0;
    static uint8_t keyState = 0;
    static uint16_t keyLongCheck = 0;
    static uint16_t keyPrev = 0;      //上一次按键

    uint16_t keyPress = 0;
    uint16_t keyReturn = 0;

    keyCountTime ++;

    if(keyCountTime >= (DEBOUNCE_TIME / KEY_TIMER_MS))          //keyCountTime 1MS+1  按键消抖10MS
    {
        keyCountTime = 0;
        keyCheck = 1;
    }
    if(1 == keyCheck)
    {
        keyCheck = 0;
        keyPress = Get_KeyState();
        switch (keyState)
        {
            case 0:
                if(keyPress != 0)
                {
                    keyPrev = keyPress;
                    keyState = 1;
                }
                break;

            case 1:
                if(keyPress == keyPrev)
                {
                    keyState = 2;
                    keyReturn= keyPrev | KEY_DOWN;
                }
                else                //按键抬起,是抖动,不响应按键
                {
                    keyState = 0;
                }
                break;

            case 2:

                if(keyPress != keyPrev)
                {
                    keyState = 0;
                    keyLongCheck = 0;
                    keyReturn = keyPrev | KEY_UP;
                    return keyReturn;
                }
                if(keyPress == keyPrev)
                {
                    keyLongCheck++;
                    if(keyLongCheck >= (PRESS_LONG_TIME / DEBOUNCE_TIME))    //长按3S有效
                    {
                        keyLongCheck = 0;
                        keyState = 3;
                        keyReturn= keyPress | KEY_LONG;
                        return keyReturn;
                    }
                }
                break;

            case 3:
                if(keyPress != keyPrev)
                {
                    keyState = 0;
                }
                break;
        }
    }
    return  NO_KEY;
}

void Key_Handle(void)
{
    uint8_t i = 0;
    uint16_t key_value = 0;

    key_value = Read_KeyValue();

    if(!key_value) return;

    //Check short press button
    if(key_value & KEY_UP)
    {
        //Valid key is detected
        for(i = 0; i < keys.keyTotolNum; i++)
        {
            if(G_IS_BIT_SET(key_value, keys.singleKey[i].keyNum))
            {
                //key callback function of short press
                if(keys.singleKey[i].shortPress)
                {
                    keys.singleKey[i].shortPress();

                }
            }
        }
    }

    //Check short long button
    if(key_value & KEY_LONG)
    {
        //Valid key is detected
        for(i = 0; i < keys.keyTotolNum; i++)
        {
            if(G_IS_BIT_SET(key_value, keys.singleKey[i].keyNum))
            {
                //key callback function of long press
                if(keys.singleKey[i].longPress)
                {
                    keys.singleKey[i].longPress();

                }
            }
        }
    }
}

key_typedef_t Key_InitOne(clock_ip_name_t keyClockIpName, PORT_Type *keyPortBase, GPIO_Type *keyGpioBase, uint32_t keyPin, gokitKeyFunction shortPress, gokitKeyFunction longPress)
{
	static int8_t key_total = -1;

	key_typedef_t singleKey;

	singleKey.keyClockIpName = keyClockIpName;
	singleKey.keyPortBase = keyPortBase;
	singleKey.keyGpioBase = keyGpioBase;
	singleKey.keyPin = keyPin;
	singleKey.keyNum = ++key_total;
	singleKey.longPress = longPress;
	singleKey.shortPress = shortPress;

	keyTotolNum++;

	return singleKey;
}

void Key_ParaInit(void)
{
  uint8_t temI = 0;

  keys.keyTotolNum = keyTotolNum;

  //Limit on the number keys (Allowable number: 0~12)
  if(KEY_MAX_NUMBER < keys.keyTotolNum)
  {
      keys.keyTotolNum = KEY_MAX_NUMBER;
  }

  port_pin_config_t port_Pin_Config = {
		kPORT_PullUp,
		kPORT_FastSlewRate,
		kPORT_PassiveFilterDisable,
		kPORT_LowDriveStrength,
		kPORT_MuxAsGpio,
	};

	gpio_pin_config_t gpio_Pin_Config = {
			kGPIO_DigitalInput,
			0,
	};

  for(temI = 0; temI < keys.keyTotolNum; temI++)
  {
  	CLOCK_EnableClock(keys.singleKey[temI].keyClockIpName);
  	PORT_SetPinConfig(keys.singleKey[temI].keyPortBase, keys.singleKey[temI].keyPin, &port_Pin_Config);
		GPIO_PinInit(keys.singleKey[temI].keyGpioBase, keys.singleKey[temI].keyPin, &gpio_Pin_Config);
  }

  //初始化周期为1MS定时器
//  timer2Init();
}
