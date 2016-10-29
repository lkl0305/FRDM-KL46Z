/*
 * gizwits_protocol.h
 *
 *  Created on: 2016��10��29��
 *      Author: lkl03
 */

#ifndef GIZWITS_GIZWITS_PROTOCOL_H_
#define GIZWITS_GIZWITS_PROTOCOL_H_


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fsl_debug_console.h"

/**
* @name ��־��ӡ�궨��
* @{
*/
#define GIZWITS_LOG PRINTF                    ///<������־��ӡ
#define PROTOCOL_DEBUG                        ///<Э�����ݴ�ӡ
/**@} */

/** WiFiģ�����ò���*/
typedef enum
{
  WIFI_RESET_MODE = 0x00,                     ///< WIFIģ�鸴λ
  WIFI_SOFTAP_MODE,                           ///< WIFIģ��softAPģʽ
  WIFI_AIRLINK_MODE,                          ///< WIFIģ��AirLinkģʽ
}WIFI_MODE_TYPE_T;

#define MAX_PACKAGE_LEN    950                ///< ���ݻ�������󳤶�
#define RB_MAX_LEN         (MAX_PACKAGE_LEN*2)///< ���λ�������󳤶�
#define min(a, b) (a)<(b)?(a):(b)             ///< ��ȡ��Сֵ

#define SEND_MAX_TIME   3000                  ///< 3000ms�ط�
#define SEND_MAX_NUM    2                     ///< �ط�����

#define protocol_VERSION "00000004"           ///< protocol�汾��
#define P0_VERSION  "00000002"                ///< P0Э��汾��
#define DEV_DEFAULT  "00000000"               ///< �豸����

/**@name Product Key (��Ʒ��ʶ��)
* @{
*/
#define PRODUCT_KEY "5cd6d83fe2b545018c6cb63dd9d1bd3d"
/**@} */

/**@name ���ݵ���ض���
* @{
*/
#define GREEN_LED_BYTEOFFSET                    0
#define GREEN_LED_BITOFFSET                     0
#define GREEN_LED_LEN                           1


/**@} */


/** ��д�����ݵ� ������ö�ٱ��� ��ռ�ֽڴ�С*/
#define COUNT_W_BIT 1






/** �¼��ṹ��*/
typedef enum
{
  WIFI_SOFTAP = 0x00,                         ///< WiFi SOFTAP �����¼�
  WIFI_AIRLINK,                               ///< WiFiģ�� AIRLINK �����¼�
  WIFI_STATION,                               ///< WiFiģ�� STATION �����¼�
  WIFI_OPEN_BINDING,                          ///< WiFiģ�鿪�����¼�
  WIFI_CLOSE_BINDING,                         ///< WiFiģ��رհ��¼�
  WIFI_CON_ROUTER,                            ///< WiFiģ��������·���¼�
  WIFI_DISCON_ROUTER,                         ///< WiFiģ���ѶϿ�����·���¼�
  WIFI_CON_M2M,                               ///< WiFiģ������������M2M�¼�
  WIFI_DISCON_M2M,                            ///< WiFiģ���ѶϿ���������M2M�¼�
  WIFI_OPEN_TESTMODE,                         ///< WiFiģ�鿪������ģʽ�¼�
  WIFI_CLOSE_TESTMODE,                        ///< WiFiģ��رղ���ģʽ�¼�
  WIFI_CON_APP,                               ///< WiFiģ������APP�¼�
  WIFI_DISCON_APP,                            ///< WiFiģ��Ͽ�APP�¼�
  WIFI_RSSI,                                  ///< WiFiģ���ź��¼�
  TRANSPARENT_DATA,                           ///< ͸���¼�
  EVENT_GREEN_LED,
  EVENT_TYPE_MAX
} EVENT_TYPE_T;

/** �û����豸״̬�ṹ��*/
typedef struct {
  bool valuegreen_led;
}__packed dataPoint_t;

/** ��ӦЭ�顰4.10 WiFiģ������豸���еı�־λ"attr_flags" */
typedef struct {
  uint8_t flaggreen_led:1;
} __packed attrFlags_t;

/** ��ӦЭ�顰4.10 WiFiģ������豸���е�����ֵ"attr_vals" */
typedef struct {
  uint8_t wBitBuf[COUNT_W_BIT];
} __packed attrVals_t;

/** ��ӦЭ�顰4.10 WiFiģ������豸���С�P0Э�������ı�־λ"attr_flags" + ����ֵ"attr_vals" */
typedef struct {
    attrFlags_t attrFlags;
    attrVals_t  attrVals;
} __packed gizwitsIssued_t;

/** ��ӦЭ�顰4.9 �豸MCU��WiFiģ�������ϱ���ǰ״̬���е��豸״̬"dev_status" */
typedef struct {
  uint8_t wBitBuf[COUNT_W_BIT];





} __packed devStatus_t;

/** ��ӦЭ�顰4.9 �豸MCU��WiFiģ�������ϱ���ǰ״̬���е� �豸״̬λ"dev_status"  */
typedef struct {
    devStatus_t devStatus;                          ///< ���ã�������Ŵ��ϱ����豸״̬����
} __packed gizwitsReport_t;

/** �¼����нṹ�� */
typedef struct {
    uint8_t num;                                    ///< ���г�Ա����
    uint8_t event[EVENT_TYPE_MAX];                  ///< ���г�Ա�¼�����
} __packed eventInfo_t;

/** wifi�ź�ǿ����ֵ�ṹ�� */
typedef struct {
    uint8_t rssi;                                   ///< WIFI�ź�ǿ��
} __packed moduleStatusInfo_t;

/** Э�������� */
typedef enum
{
    CMD_GET_DEVICE_INTO             = 0x01,         ///< �����֣���ӦЭ�飺4.1 WiFiģ�������豸��Ϣ �� WiFiģ�鷢��
    ACK_GET_DEVICE_INFO             = 0x02,         ///< �����֣���ӦЭ�飺4.1 WiFiģ�������豸��Ϣ �� �豸MCU�ظ�

    CMD_ISSUED_P0                   = 0x03,         ///< �����֣���ӦЭ�飺4.8 WiFiģ���ȡ�豸�ĵ�ǰ״̬��4.10 WiFiģ������豸 �� WiFiģ�鷢��
    ACK_ISSUED_P0                   = 0x04,         ///< �����֣���ӦЭ�飺4.8 WiFiģ���ȡ�豸�ĵ�ǰ״̬��4.10 WiFiģ������豸 �� �豸MCU�ظ�

    CMD_REPORT_P0                   = 0x05,         ///< �����֣���ӦЭ�飺4.9 �豸MCU��WiFiģ�������ϱ���ǰ״̬ �� �豸MCU����
    ACK_REPORT_P0                   = 0x06,         ///< �����֣���ӦЭ�飺4.9 �豸MCU��WiFiģ�������ϱ���ǰ״̬ �� WiFiģ��ظ�

    CMD_HEARTBEAT                   = 0x07,         ///< �����֣���ӦЭ�飺4.2 WiFiģ�����豸MCU������ �� WiFiģ�鷢��
    ACK_HEARTBEAT                   = 0x08,         ///< �����֣���ӦЭ�飺4.2 WiFiģ�����豸MCU������ �� �豸MCU�ظ�

    CMD_WIFI_CONFIG                 = 0x09,         ///< �����֣���ӦЭ�飺4.3 �豸MCU֪ͨWiFiģ���������ģʽ �� �豸MCU����
    ACK_WIFI_CONFIG                 = 0x0A,         ///< �����֣���ӦЭ�飺4.3 �豸MCU֪ͨWiFiģ���������ģʽ �� WiFiģ��ظ�

    CMD_SET_DEFAULT                 = 0x0B,         ///< �����֣���ӦЭ�飺4.4 �豸MCU����WiFiģ�� �� �豸MCU����
    ACK_SET_DEFAULT                 = 0x0C,         ///< �����֣���ӦЭ�飺4.4 �豸MCU����WiFiģ�� �� WiFiģ��ظ�

    CMD_WIFISTATUS                  = 0x0D,         ///< �����֣���ӦЭ�飺4.5 WiFiģ�����豸MCU֪ͨWiFiģ�鹤��״̬�ı仯 �� WiFiģ�鷢��
    ACK_WIFISTATUS                  = 0x0E,         ///< �����֣���ӦЭ�飺4.5 WiFiģ�����豸MCU֪ͨWiFiģ�鹤��״̬�ı仯 �� �豸MCU�ظ�

    CMD_MCU_REBOOT                  = 0x0F,         ///< �����֣���ӦЭ�飺4.6 WiFiģ����������MCU �� WiFiģ�鷢��
    ACK_MCU_REBOOT                  = 0x10,         ///< �����֣���ӦЭ�飺4.6 WiFiģ����������MCU �� �豸MCU�ظ�

    CMD_ERROR_PACKAGE               = 0x11,         ///< �����֣���ӦЭ�飺4.7 �Ƿ���Ϣ֪ͨ �� WiFiģ���ӦMCU��Ӧ����ŵ����ݰ��Ƿ�
    ACK_ERROR_PACKAGE               = 0x12,         ///< �����֣���ӦЭ�飺4.7 �Ƿ���Ϣ֪ͨ �� MCU��ӦWiFiģ���Ӧ����ŵ����ݰ��Ƿ�

} PROTOCOL_CMDTYPE;

/** �ط����ƽṹ�� */
typedef struct {
    uint8_t                 num;                    ///< �ط�����
    uint8_t                 flag;                   ///< 1,��ʾ����Ҫ�ȴ���ACK;0,��ʾ����Ҫ�ȴ���ACK
    uint8_t                 buf[MAX_PACKAGE_LEN];   ///< �ط����ݻ�����
    uint16_t                dataLen;                ///< �ط����ݳ���
    uint32_t                sendTime;               ///< �ط���ϵͳʱ��
} __packed protocolWaitAck_t;

/** Э���׼ͷ�ṹ�� */
typedef struct
{
    uint8_t                 head[2];                ///< ��ͷ(header)�̶�Ϊ0xFFFF
    uint16_t                len;                    ///< ����(len)��ָ��cmd ��ʼ���������ݰ�������ռ�õ��ֽ���
    uint8_t                 cmd;                    ///< �����֣�cmd����ʾ���������壬���Э�����
    uint8_t                 sn;                     ///< ��Ϣ���(sn)�ɷ��ͷ�����,���շ���Ӧ����ʱ�����Ϣ��ŷ��ظ����ͷ�
    uint8_t                 flags[2];               ///< ��־λ��flag������Ʒ��дĬ��0
} __packed protocolHead_t;

/** 4.1 WiFiģ�������豸��Ϣ Э��ṹ�� */
typedef struct
{
    protocolHead_t          head;                   ///< Э���׼ͷ�ṹ��
    uint8_t                 protocolVer[8];         ///< Э��汾��
    uint8_t                 p0Ver[8];               ///< p0Э��汾��
    uint8_t                 hardVer[8];             ///< Ӳ���汾��
    uint8_t                 softVer[8];             ///< ����汾��
    uint8_t                 productKey[32];         ///< ��Ʒ��ʶ��
    uint16_t                ninableTime;            ///< �󶨳�ʱ(��)
    uint8_t                 devAttr[8];             ///< �豸����
    uint8_t                 sum;                    ///< �����
} __packed protocolDeviceInfo_t;

/** Э��ͨ������֡(4.2��4.4��4.6��4.9��4.10) Э��ṹ�� */
typedef struct
{
    protocolHead_t          head;                   ///< Э���׼ͷ�ṹ��
    uint8_t                 sum;                    ///< �����
} __packed protocolCommon_t;

/** 4.3 �豸MCU֪ͨWiFiģ���������ģʽ Э��ṹ�� */
typedef struct
{
    protocolHead_t          head;                   ///< Э���׼ͷ�ṹ��
    uint8_t                 cfgMode;                ///< ���ò���
    uint8_t                 sum;                    ///< �����
} __packed protocolCfgMode_t;

/** WiFiģ�鹤��״̬*/
typedef union
{
    uint16_t                value;
    __packed  struct
    {
        uint16_t            softap:1;               ///< ��ʾWiFiģ��������SOFTAPģʽ״̬������Ϊbool
        uint16_t            station:1;              ///< ��ʾWiFiģ��������STATIONģʽ״̬������Ϊbool
        uint16_t            onboarding:1;           ///< ��ʾWiFiģ������������״̬������Ϊbool
        uint16_t            binding:1;              ///< ��ʾWiFiģ�������İ�״̬������Ϊbool
        uint16_t            con_route:1;            ///< ��ʾWiFiģ����·����������״̬������Ϊbool
        uint16_t            con_m2m:1;              ///< ��ʾWiFiģ�����ƶ�m2m��״̬������Ϊbool
        uint16_t            reserve1:2;             ///< ����λ����
        uint16_t            rssi:3;                 ///< ��ʾ·�ɵ��ź�ǿ�ȣ�����Ϊ��ֵ
        uint16_t            app:1;                  ///< ��ʾWiFiģ����APP�˵�����״̬������Ϊbool
        uint16_t            test:1;                 ///< ��ʾWiFiģ�������ĳ���ģʽ״̬������Ϊbool
        uint16_t            reserve2:3;             ///< ����λ����
    }types;

} __packed wifiStatus_t;

/** WiFi״̬���� Э��ṹ�� */
typedef struct
{
    protocolHead_t          head;                   ///< Э���׼ͷ�ṹ��
    wifiStatus_t            ststus;                 ///< WIFI״̬
    uint8_t                 sum;                    ///< �����
} __packed protocolWifiStatus_t;

/** �Ƿ���������*/
typedef enum
{
    ERROR_ACK_SUM = 0x01,                           ///< У�����
    ERROR_CMD     = 0x02,                           ///< ���������
    ERROR_OTHER   = 0x03,                           ///< ����
} errorPacketsType_t;

/** 4.7 �Ƿ���Ϣ֪ͨ Э��ṹ��*/
typedef struct
{
    protocolHead_t          head;                   ///< Э���׼ͷ�ṹ��
    uint8_t                 error;                  ///< ����ֵ
    uint8_t                 sum;                    ///< �����
} __packed protocolErrorType_t;

/** P0 command ������*/
typedef enum
{
    ACTION_CONTROL_DEVICE       = 0x01,             ///< Э��4.10 WiFiģ������豸 WiFiģ�鷢��
    ACTION_READ_DEV_STATUS      = 0x02,             ///< Э��4.8 WiFiģ���ȡ�豸�ĵ�ǰ״̬ WiFiģ�鷢��
    ACTION_READ_DEV_STATUS_ACK  = 0x03,             ///< Э��4.8 WiFiģ���ȡ�豸�ĵ�ǰ״̬ �豸MCU�ظ�
    ACTION_REPORT_DEV_STATUS    = 0X04,             ///< Э��4.9 �豸MCU��WiFiģ�������ϱ���ǰ״̬ �豸MCU����
    ACTION_W2D_TRANSPARENT_DATA = 0x05,             ///< WiFi���豸MCU͸��
    ACTION_D2W_TRANSPARENT_DATA = 0x06,             ///< �豸MCU��WiFi͸��
} actionType_t;

/** 4.8 WiFiģ���ȡ�豸�ĵ�ǰ״̬ �豸MCU�ظ��ṹ�� */
typedef struct
{
    protocolHead_t          head;                   ///< Э���׼ͷ�ṹ��
    uint8_t                 action;                 ///< p0 ����λ
    gizwitsReport_t         reportData;             ///< �ϱ�������
    uint8_t                 sum;                    ///< �����
} __packed protocolReport_t;

/** P0���ı�׼ͷ */
typedef struct
{
    protocolHead_t          head;                   ///< Э���׼ͷ�ṹ��
    uint8_t                 action;                 ///< p0 ����λ
} __packed protocolP0Head_t;

/** ���λ��������ݽṹ */
typedef struct {
    size_t rbCapacity;
    uint8_t  *rbHead;
    uint8_t  *rbTail;
    uint8_t  *rbBuff;
} __packed rb_t;

/** Э�鴦���ܽṹ�� */
typedef struct
{
    uint8_t issuedFlag;                             ///< �����Ӧ�� p0��������
    uint8_t protocolBuf[MAX_PACKAGE_LEN];           ///< Э�鱨�Ĵ洢��
    uint8_t transparentBuff[MAX_PACKAGE_LEN];       ///< ͸�����ݴ洢��
    uint32_t transparentLen;                        ///< ͸�����ݳ���

    uint32_t sn;                                    ///< ��Ϣ���
    uint32_t timerMsCount;                          ///< ʱ�����(Ms)
    uint32_t lastReportTime;                        ///< �ϴ��ϱ�ʱ��
    protocolWaitAck_t waitAck;                      ///< Э��ACK���ݽṹ

    eventInfo_t issuedProcessEvent;                 ///< �����¼�
    eventInfo_t wifiStatusEvent;                    ///< WIFI״̬ �¼�

    dataPoint_t gizCurrentDataPoint;                ///< ��ǰ�豸״̬����
    dataPoint_t gizLastDataPoint;                   ///< �ϴ��ϱ����豸״̬����
    gizwitsReport_t reportData;                     ///< Э���ϱ�ʵ������
    moduleStatusInfo_t wifiStatusData;              ///< WIFI ״̬��Ϣ(�ź�ǿ��)
} __packed gizwitsProtocol_t;

/**@name Gizwits �û�API�ӿ�
* @{
*/
void gizwitsInit(void);
int32_t gizwitsHandle(dataPoint_t *currentData);
int32_t gizwitsSetMode(uint8_t mode);
/**@} */

int32_t gizPutData(uint8_t *buf, uint32_t len);
uint32_t gizGetTimerCount(void);
void gizTimerMs(void);



#endif /* GIZWITS_GIZWITS_PROTOCOL_H_ */
