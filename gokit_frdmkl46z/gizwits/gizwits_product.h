/*
 * gizwits_product.h
 *
 *  Created on: 2016��10��29��
 *      Author: lkl03
 */

#ifndef GIZWITS_GIZWITS_PRODUCT_H_
#define GIZWITS_GIZWITS_PRODUCT_H_


#include <stdint.h>
#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_uart.h"
#include "gizwits_protocol.h"

/**
* MCU����汾��
*/
#define SOFTWARE_VERSION "03010101"
/**
* MCUӲ���汾��
*/
#define HARDWARE_VERSION "03010100"

#ifndef SOFTWARE_VERSION
    #error "no define SOFTWARE_VERSION"
#endif

#ifndef HARDWARE_VERSION
    #error "no define HARDWARE_VERSION"
#endif

/**@name �豸״̬�����ϱ����ʱ��궨��
* @{
*/
#define REPORT_TIME_MAX 6000 //6S
/**@} */

void uartInit(void);
void mcuRestart(void);
int32_t uartWrite(uint8_t *buf, uint32_t len);

/**@name Gizwits �û�API�ӿ�
* @{
*/
int8_t gizwitsEventProcess(eventInfo_t *info, uint8_t *data, uint32_t len);


#endif /* GIZWITS_GIZWITS_PRODUCT_H_ */
