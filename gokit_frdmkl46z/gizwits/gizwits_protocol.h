/*
 * gizwits_protocol.h
 *
 *  Created on: 2016年10月29日
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
* @name 日志打印宏定义
* @{
*/
#define GIZWITS_LOG PRINTF                    ///<运行日志打印
#define PROTOCOL_DEBUG                        ///<协议数据打印
/**@} */

/** WiFi模组配置参数*/
typedef enum
{
  WIFI_RESET_MODE = 0x00,                     ///< WIFI模组复位
  WIFI_SOFTAP_MODE,                           ///< WIFI模组softAP模式
  WIFI_AIRLINK_MODE,                          ///< WIFI模组AirLink模式
}WIFI_MODE_TYPE_T;

#define MAX_PACKAGE_LEN    950                ///< 数据缓冲区最大长度
#define RB_MAX_LEN         (MAX_PACKAGE_LEN*2)///< 环形缓冲区最大长度
#define min(a, b) (a)<(b)?(a):(b)             ///< 获取最小值

#define SEND_MAX_TIME   3000                  ///< 3000ms重发
#define SEND_MAX_NUM    2                     ///< 重发次数

#define protocol_VERSION "00000004"           ///< protocol版本号
#define P0_VERSION  "00000002"                ///< P0协议版本号
#define DEV_DEFAULT  "00000000"               ///< 设备属性

/**@name Product Key (产品标识码)
* @{
*/
#define PRODUCT_KEY "5cd6d83fe2b545018c6cb63dd9d1bd3d"
/**@} */

/**@name 数据点相关定义
* @{
*/
#define GREEN_LED_BYTEOFFSET                    0
#define GREEN_LED_BITOFFSET                     0
#define GREEN_LED_LEN                           1


/**@} */


/** 可写型数据点 布尔和枚举变量 所占字节大小*/
#define COUNT_W_BIT 1






/** 事件结构体*/
typedef enum
{
  WIFI_SOFTAP = 0x00,                         ///< WiFi SOFTAP 配置事件
  WIFI_AIRLINK,                               ///< WiFi模块 AIRLINK 配置事件
  WIFI_STATION,                               ///< WiFi模块 STATION 配置事件
  WIFI_OPEN_BINDING,                          ///< WiFi模块开启绑定事件
  WIFI_CLOSE_BINDING,                         ///< WiFi模块关闭绑定事件
  WIFI_CON_ROUTER,                            ///< WiFi模块已连接路由事件
  WIFI_DISCON_ROUTER,                         ///< WiFi模块已断开连接路由事件
  WIFI_CON_M2M,                               ///< WiFi模块已连服务器M2M事件
  WIFI_DISCON_M2M,                            ///< WiFi模块已断开连服务器M2M事件
  WIFI_OPEN_TESTMODE,                         ///< WiFi模块开启测试模式事件
  WIFI_CLOSE_TESTMODE,                        ///< WiFi模块关闭测试模式事件
  WIFI_CON_APP,                               ///< WiFi模块连接APP事件
  WIFI_DISCON_APP,                            ///< WiFi模块断开APP事件
  WIFI_RSSI,                                  ///< WiFi模块信号事件
  TRANSPARENT_DATA,                           ///< 透传事件
  EVENT_GREEN_LED,
  EVENT_TYPE_MAX
} EVENT_TYPE_T;

/** 用户区设备状态结构体*/
typedef struct {
  bool valuegreen_led;
}__packed dataPoint_t;

/** 对应协议“4.10 WiFi模组控制设备”中的标志位"attr_flags" */
typedef struct {
  uint8_t flaggreen_led:1;
} __packed attrFlags_t;

/** 对应协议“4.10 WiFi模组控制设备”中的数据值"attr_vals" */
typedef struct {
  uint8_t wBitBuf[COUNT_W_BIT];
} __packed attrVals_t;

/** 对应协议“4.10 WiFi模组控制设备”中“P0协议区”的标志位"attr_flags" + 数据值"attr_vals" */
typedef struct {
    attrFlags_t attrFlags;
    attrVals_t  attrVals;
} __packed gizwitsIssued_t;

/** 对应协议“4.9 设备MCU向WiFi模组主动上报当前状态”中的设备状态"dev_status" */
typedef struct {
  uint8_t wBitBuf[COUNT_W_BIT];





} __packed devStatus_t;

/** 对应协议“4.9 设备MCU向WiFi模组主动上报当前状态”中的 设备状态位"dev_status"  */
typedef struct {
    devStatus_t devStatus;                          ///< 作用：用来存放待上报的设备状态数据
} __packed gizwitsReport_t;

/** 事件队列结构体 */
typedef struct {
    uint8_t num;                                    ///< 队列成员个数
    uint8_t event[EVENT_TYPE_MAX];                  ///< 队列成员事件内容
} __packed eventInfo_t;

/** wifi信号强度数值结构体 */
typedef struct {
    uint8_t rssi;                                   ///< WIFI信号强度
} __packed moduleStatusInfo_t;

/** 协议命令码 */
typedef enum
{
    CMD_GET_DEVICE_INTO             = 0x01,         ///< 命令字，对应协议：4.1 WiFi模组请求设备信息 中 WiFi模组发送
    ACK_GET_DEVICE_INFO             = 0x02,         ///< 命令字，对应协议：4.1 WiFi模组请求设备信息 中 设备MCU回复

    CMD_ISSUED_P0                   = 0x03,         ///< 命令字，对应协议：4.8 WiFi模组读取设备的当前状态、4.10 WiFi模组控制设备 中 WiFi模组发送
    ACK_ISSUED_P0                   = 0x04,         ///< 命令字，对应协议：4.8 WiFi模组读取设备的当前状态、4.10 WiFi模组控制设备 中 设备MCU回复

    CMD_REPORT_P0                   = 0x05,         ///< 命令字，对应协议：4.9 设备MCU向WiFi模组主动上报当前状态 中 设备MCU发送
    ACK_REPORT_P0                   = 0x06,         ///< 命令字，对应协议：4.9 设备MCU向WiFi模组主动上报当前状态 中 WiFi模组回复

    CMD_HEARTBEAT                   = 0x07,         ///< 命令字，对应协议：4.2 WiFi模组与设备MCU的心跳 中 WiFi模组发送
    ACK_HEARTBEAT                   = 0x08,         ///< 命令字，对应协议：4.2 WiFi模组与设备MCU的心跳 中 设备MCU回复

    CMD_WIFI_CONFIG                 = 0x09,         ///< 命令字，对应协议：4.3 设备MCU通知WiFi模组进入配置模式 中 设备MCU发送
    ACK_WIFI_CONFIG                 = 0x0A,         ///< 命令字，对应协议：4.3 设备MCU通知WiFi模组进入配置模式 中 WiFi模组回复

    CMD_SET_DEFAULT                 = 0x0B,         ///< 命令字，对应协议：4.4 设备MCU重置WiFi模组 中 设备MCU发送
    ACK_SET_DEFAULT                 = 0x0C,         ///< 命令字，对应协议：4.4 设备MCU重置WiFi模组 中 WiFi模组回复

    CMD_WIFISTATUS                  = 0x0D,         ///< 命令字，对应协议：4.5 WiFi模组向设备MCU通知WiFi模组工作状态的变化 中 WiFi模组发送
    ACK_WIFISTATUS                  = 0x0E,         ///< 命令字，对应协议：4.5 WiFi模组向设备MCU通知WiFi模组工作状态的变化 中 设备MCU回复

    CMD_MCU_REBOOT                  = 0x0F,         ///< 命令字，对应协议：4.6 WiFi模组请求重启MCU 中 WiFi模组发送
    ACK_MCU_REBOOT                  = 0x10,         ///< 命令字，对应协议：4.6 WiFi模组请求重启MCU 中 设备MCU回复

    CMD_ERROR_PACKAGE               = 0x11,         ///< 命令字，对应协议：4.7 非法消息通知 中 WiFi模组回应MCU对应包序号的数据包非法
    ACK_ERROR_PACKAGE               = 0x12,         ///< 命令字，对应协议：4.7 非法消息通知 中 MCU回应WiFi模组对应包序号的数据包非法

} PROTOCOL_CMDTYPE;

/** 重发机制结构体 */
typedef struct {
    uint8_t                 num;                    ///< 重发次数
    uint8_t                 flag;                   ///< 1,表示有需要等待的ACK;0,表示无需要等待的ACK
    uint8_t                 buf[MAX_PACKAGE_LEN];   ///< 重发数据缓冲区
    uint16_t                dataLen;                ///< 重发数据长度
    uint32_t                sendTime;               ///< 重发的系统时间
} __packed protocolWaitAck_t;

/** 协议标准头结构体 */
typedef struct
{
    uint8_t                 head[2];                ///< 包头(header)固定为0xFFFF
    uint16_t                len;                    ///< 长度(len)是指从cmd 开始到整个数据包结束所占用的字节数
    uint8_t                 cmd;                    ///< 命令字（cmd）表示具体的命令含义，详见协议举例
    uint8_t                 sn;                     ///< 消息序号(sn)由发送方给出,接收方响应命令时需把消息序号返回给发送方
    uint8_t                 flags[2];               ///< 标志位（flag），产品填写默认0
} __packed protocolHead_t;

/** 4.1 WiFi模组请求设备信息 协议结构体 */
typedef struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8_t                 protocolVer[8];         ///< 协议版本号
    uint8_t                 p0Ver[8];               ///< p0协议版本号
    uint8_t                 hardVer[8];             ///< 硬件版本号
    uint8_t                 softVer[8];             ///< 软件版本号
    uint8_t                 productKey[32];         ///< 产品标识码
    uint16_t                ninableTime;            ///< 绑定超时(秒)
    uint8_t                 devAttr[8];             ///< 设备属性
    uint8_t                 sum;                    ///< 检验和
} __packed protocolDeviceInfo_t;

/** 协议通用数据帧(4.2、4.4、4.6、4.9、4.10) 协议结构体 */
typedef struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8_t                 sum;                    ///< 检验和
} __packed protocolCommon_t;

/** 4.3 设备MCU通知WiFi模组进入配置模式 协议结构体 */
typedef struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8_t                 cfgMode;                ///< 配置参数
    uint8_t                 sum;                    ///< 检验和
} __packed protocolCfgMode_t;

/** WiFi模组工作状态*/
typedef union
{
    uint16_t                value;
    __packed  struct
    {
        uint16_t            softap:1;               ///< 表示WiFi模组所处的SOFTAP模式状态，类型为bool
        uint16_t            station:1;              ///< 表示WiFi模组所处的STATION模式状态，类型为bool
        uint16_t            onboarding:1;           ///< 表示WiFi模组所处的配置状态，类型为bool
        uint16_t            binding:1;              ///< 表示WiFi模组所处的绑定状态，类型为bool
        uint16_t            con_route:1;            ///< 表示WiFi模组与路由器的连接状态，类型为bool
        uint16_t            con_m2m:1;              ///< 表示WiFi模组与云端m2m的状态，类型为bool
        uint16_t            reserve1:2;             ///< 数据位补齐
        uint16_t            rssi:3;                 ///< 表示路由的信号强度，类型为数值
        uint16_t            app:1;                  ///< 表示WiFi模组与APP端的连接状态，类型为bool
        uint16_t            test:1;                 ///< 表示WiFi模组所处的场测模式状态，类型为bool
        uint16_t            reserve2:3;             ///< 数据位补齐
    }types;

} __packed wifiStatus_t;

/** WiFi状态类型 协议结构体 */
typedef struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    wifiStatus_t            ststus;                 ///< WIFI状态
    uint8_t                 sum;                    ///< 检验和
} __packed protocolWifiStatus_t;

/** 非法报文类型*/
typedef enum
{
    ERROR_ACK_SUM = 0x01,                           ///< 校验错误
    ERROR_CMD     = 0x02,                           ///< 命令码错误
    ERROR_OTHER   = 0x03,                           ///< 其他
} errorPacketsType_t;

/** 4.7 非法消息通知 协议结构体*/
typedef struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8_t                 error;                  ///< 错误值
    uint8_t                 sum;                    ///< 检验和
} __packed protocolErrorType_t;

/** P0 command 命令码*/
typedef enum
{
    ACTION_CONTROL_DEVICE       = 0x01,             ///< 协议4.10 WiFi模组控制设备 WiFi模组发送
    ACTION_READ_DEV_STATUS      = 0x02,             ///< 协议4.8 WiFi模组读取设备的当前状态 WiFi模组发送
    ACTION_READ_DEV_STATUS_ACK  = 0x03,             ///< 协议4.8 WiFi模组读取设备的当前状态 设备MCU回复
    ACTION_REPORT_DEV_STATUS    = 0X04,             ///< 协议4.9 设备MCU向WiFi模组主动上报当前状态 设备MCU发送
    ACTION_W2D_TRANSPARENT_DATA = 0x05,             ///< WiFi到设备MCU透传
    ACTION_D2W_TRANSPARENT_DATA = 0x06,             ///< 设备MCU到WiFi透传
} actionType_t;

/** 4.8 WiFi模组读取设备的当前状态 设备MCU回复结构体 */
typedef struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8_t                 action;                 ///< p0 命令位
    gizwitsReport_t         reportData;             ///< 上报数据区
    uint8_t                 sum;                    ///< 检验和
} __packed protocolReport_t;

/** P0报文标准头 */
typedef struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8_t                 action;                 ///< p0 命令位
} __packed protocolP0Head_t;

/** 环形缓冲区数据结构 */
typedef struct {
    size_t rbCapacity;
    uint8_t  *rbHead;
    uint8_t  *rbTail;
    uint8_t  *rbBuff;
} __packed rb_t;

/** 协议处理总结构体 */
typedef struct
{
    uint8_t issuedFlag;                             ///< 保存对应的 p0命令类型
    uint8_t protocolBuf[MAX_PACKAGE_LEN];           ///< 协议报文存储区
    uint8_t transparentBuff[MAX_PACKAGE_LEN];       ///< 透传数据存储区
    uint32_t transparentLen;                        ///< 透传数据长度

    uint32_t sn;                                    ///< 消息序号
    uint32_t timerMsCount;                          ///< 时间计数(Ms)
    uint32_t lastReportTime;                        ///< 上次上报时间
    protocolWaitAck_t waitAck;                      ///< 协议ACK数据结构

    eventInfo_t issuedProcessEvent;                 ///< 控制事件
    eventInfo_t wifiStatusEvent;                    ///< WIFI状态 事件

    dataPoint_t gizCurrentDataPoint;                ///< 当前设备状态数据
    dataPoint_t gizLastDataPoint;                   ///< 上次上报的设备状态数据
    gizwitsReport_t reportData;                     ///< 协议上报实际数据
    moduleStatusInfo_t wifiStatusData;              ///< WIFI 状态信息(信号强度)
} __packed gizwitsProtocol_t;

/**@name Gizwits 用户API接口
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
