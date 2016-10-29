/*
 * hal_key.h
 *
 *  Created on: 2016Äê10ÔÂ27ÈÕ
 *      Author: lkl03
 */

#ifndef HAL_KEY_H_
#define HAL_KEY_H_

#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "stdio.h"
#include "math.h"

#define G_SET_BIT(a,b) (a |= (1 << b))
#define G_CLEAR_BIT(a,b) (a &= ~(1 << b))
#define G_IS_BIT_SET(a,b) (a & (1 << b))

#define KEY_TIMER_MS 1
#define KEY_MAX_NUMBER 12
#define DEBOUNCE_TIME 30
#define PRESS_LONG_TIME 3000

#define NO_KEY 0x0000
#define KEY_DOWN 0x1000
#define KEY_UP  0x2000
#define KEY_LIAN 0x4000
#define KEY_LONG 0x8000

typedef void (*gokitKeyFunction)(void);

typedef struct _key_typedef
{
	uint8_t keyNum;
	clock_ip_name_t keyClockIpName;
	PORT_Type *keyPortBase;
	GPIO_Type *keyGpioBase;
	uint32_t keyPin;
	gokitKeyFunction shortPress;
	gokitKeyFunction longPress;
} __packed key_typedef_t;

typedef struct _keys_typedef
{
	uint8_t keyTotolNum;
	key_typedef_t *singleKey;
} __packed keys_typedef_t;

void Key_GpioInit(void);
void Key_Handle(void);
void Key_ParaInit(void);
uint16_t Get_KeyState(void);
uint16_t Read_KeyValue(void);
key_typedef_t Key_InitOne(clock_ip_name_t keyClockIpName, PORT_Type *keyPortBase, GPIO_Type *keyGpioBase, uint32_t keyPin, gokitKeyFunction shortPress, gokitKeyFunction longPress);

#endif /* HAL_KEY_H_ */
