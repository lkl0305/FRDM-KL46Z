/*
 * gizwits_protocol.c
 *
 *  Created on: 2016��10��29��
 *      Author: lkl03
 */


#include "gizwits_product.h"
#include "gizwits_protocol.h"

/** Э��ȫ�ֱ��� **/
gizwitsProtocol_t gizwitsProtocol;

/**@name ���ڽ��ջ��λ�����ʵ��
* @{
*/
rb_t pRb;                                               ///< ���λ������ṹ�����
static uint8_t rbBuf[RB_MAX_LEN];                       ///< ���λ��������ݻ�����

static void rbCreate(rb_t* rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\n");
        return;
    }

    rb->rbHead = rb->rbBuff;
    rb->rbTail = rb->rbBuff;
}

static void rbDelete(rb_t* rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\n");
        return;
    }

    rb->rbBuff = NULL;
    rb->rbHead = NULL;
    rb->rbTail = NULL;
    rb->rbCapacity = 0;
}

static int32_t rbCapacity(rb_t *rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\n");
        return -1;
    }

    return rb->rbCapacity;
}

static int32_t rbCanRead(rb_t *rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\n");
        return -1;
    }

    if (rb->rbHead == rb->rbTail)
    {
        return 0;
    }

    if (rb->rbHead < rb->rbTail)
    {
        return rb->rbTail - rb->rbHead;
    }

    return rbCapacity(rb) - (rb->rbHead - rb->rbTail);
}

static int32_t rbCanWrite(rb_t *rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\n");
        return -1;
    }

    return rbCapacity(rb) - rbCanRead(rb);
}

static int32_t rbRead(rb_t *rb, void *data, size_t count)
{
    int copySz = 0;

    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\n");
        return -1;
    }

    if(NULL == data)
    {
        GIZWITS_LOG("ERROR: input data is NULL\n");
        return -1;
    }

    if (rb->rbHead < rb->rbTail)
    {
        copySz = min(count, rbCanRead(rb));
        memcpy(data, rb->rbHead, copySz);
        rb->rbHead += copySz;
        return copySz;
    }
    else
    {
        if (count < rbCapacity(rb)-(rb->rbHead - rb->rbBuff))
        {
            copySz = count;
            memcpy(data, rb->rbHead, copySz);
            rb->rbHead += copySz;
            return copySz;
        }
        else
        {
            copySz = rbCapacity(rb) - (rb->rbHead - rb->rbBuff);
            memcpy(data, rb->rbHead, copySz);
            rb->rbHead = rb->rbBuff;
            copySz += rbRead(rb, (char*)data+copySz, count-copySz);
            return copySz;
        }
    }
}

static int32_t rbWrite(rb_t *rb, const void *data, size_t count)
{
    int tailAvailSz = 0;

    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: rb is empty \n");
        return -1;
    }

    if(NULL == data)
    {
        GIZWITS_LOG("ERROR: data is empty \n");
        return -1;
    }

    if (count >= rbCanWrite(rb))
    {
        GIZWITS_LOG("ERROR: no memory %d \n", rbCanWrite(rb));
        return -1;
    }

    if (rb->rbHead <= rb->rbTail)
    {
        tailAvailSz = rbCapacity(rb) - (rb->rbTail - rb->rbBuff);
        if (count <= tailAvailSz)
        {
            memcpy(rb->rbTail, data, count);
            rb->rbTail += count;
            if (rb->rbTail == rb->rbBuff+rbCapacity(rb))
            {
                rb->rbTail = rb->rbBuff;
            }
            return count;
        }
        else
        {
            memcpy(rb->rbTail, data, tailAvailSz);
            rb->rbTail = rb->rbBuff;

            return tailAvailSz + rbWrite(rb, (char*)data+tailAvailSz, count-tailAvailSz);
        }
    }
    else
    {
        memcpy(rb->rbTail, data, count);
        rb->rbTail += count;
        return count;
    }
}
/**@} */

/**
* @brief ���λ�����д������
* @param [in] buf        : buf��ַ
* @param [in] len        : �ֽڳ���
* @return   ��ȷ : ����д������ݳ���
            ʧ�� : -1
*/
int32_t gizPutData(uint8_t *buf, uint32_t len)
{
    int32_t count = 0;

    if(NULL == buf)
    {
        GIZWITS_LOG("ERROR: gizPutData buf is empty \n");
        return -1;
    }

    count = rbWrite(&pRb, buf, len);
    if(count != len)
    {
        GIZWITS_LOG("ERROR: Failed to rbWrite \n");
        return -1;
    }

    return count;
}

/**
* @brief ϵͳ�����ʱά������,��������,�������

* @param none
* @return none
*/
void gizTimerMs(void)
{
    gizwitsProtocol.timerMsCount++;
}

/**
* @brief ��ȡϵͳʱ������ʱ��

* @param none
* @return ϵͳʱ�������
*/
uint32_t gizGetTimerCount(void)
{
    return gizwitsProtocol.timerMsCount;
}

/**
* @brief ��������У��ͼ���
*
* @param [in] buf   : buf��ַ
* @param [in] len   : �ֽڳ���
*
* @return sum : �ӻ�������3���ֽں����е��ֽ��ۼ����
*/
static uint8_t gizProtocolSum(uint8_t *buf, uint32_t len)
{
    uint8_t     sum = 0;
    uint32_t    i = 0;

    if(buf == NULL || len <= 0)
    {
        GIZWITS_LOG("ERROR: gizProtocolSum , buf is empty or len error %d \n", len);
        return 0;
    }

    for(i=2; i<len-1; i++)
    {
        sum += buf[i];
    }

    return sum;
}

/**
* @brief 16λ�����ֽ���ת��
*
* @param [in] value : ��Ҫת��������
*
* @return  tmp_value: ת���������
*/
static uint16_t gizProtocolExchangeBytes(uint16_t value)
{
    uint16_t    tmp_value;
    uint8_t     *index_1, *index_2;

    index_1 = (uint8_t *)&tmp_value;
    index_2 = (uint8_t *)&value;

    *index_1 = *(index_2+1);
    *(index_1+1) = *index_2;

    return tmp_value;
}

/**
* @brief 32λ�����ֽ���ת��
*
* @param [in] value : ��Ҫת��������
*
* @return  tmp_value: ת���������
*/
static uint32_t gizExchangeWord(uint32_t  value)
{
    return ((value & 0x000000FF) << 24) |
        ((value & 0x0000FF00) << 8) |
        ((value & 0x00FF0000) >> 8) |
        ((value & 0xFF000000) >> 24) ;
}



/**
* @brief ���黺���������ֽ���ת��
*
* @param [in] buf      : buf��ַ
* @param [in] dataLen  : �ֽڳ���
*
* @return ��ȷ : 0
          ʧ�� : -1
*/
static int8_t gizByteOrderExchange(uint8_t *buf,uint32_t dataLen)
{
    uint32_t i = 0;
    uint8_t preData = 0;
    uint8_t aftData = 0;

    if(NULL == buf)
    {
        GIZWITS_LOG("gizByteOrderExchange Error , Illegal Param\n");
        return -1;
    }

    for(i = 0;i<dataLen/2;i++)
    {
        preData = buf[i];
        aftData = buf[dataLen - i - 1];
        buf[i] = aftData;
        buf[dataLen - i - 1] = preData;
    }
    return 0;
}

/**
* @brief ת��ΪЭ���е�xֵ��ʵ��ͨѶ�����ֵ
*
* @param [in] ratio    : ����ϵ��k
* @param [in] addition : ����m
* @param [in] preValue : ��ΪЭ���е�yֵ, ��App UI�������ʾֵ
*
* @return aft_value    : ��ΪЭ���е�xֵ, ��ʵ��ͨѶ�����ֵ
*/
static uint32_t gizY2X(uint32_t ratio, int32_t addition, int32_t preValue)
{
    uint32_t aftValue = 0;

    //x=(y - m)/k
    aftValue = ((preValue - addition) / ratio);

    return aftValue;
}

/**
* @brief ת��ΪЭ���е�yֵ��App UI�������ʾֵ
*
* @param [in] ratio    : ����ϵ��k
* @param [in] addition : ����m
* @param [in] preValue : ��ΪЭ���е�xֵ, ��ʵ��ͨѶ�����ֵ
*
* @return aftValue : ��ΪЭ���е�yֵ, ��App UI�������ʾֵ
*/
static int32_t gizX2Y(uint32_t ratio, int32_t addition, uint32_t preValue)
{
    int32_t aftValue = 0;

    //y=k * x + m
    aftValue = (preValue * ratio + addition);

    return aftValue;
}

/**
* @brief ת��ΪЭ���е�xֵ��ʵ��ͨѶ�����ֵ,ֻ�����Ը���������������
*
* @param [in] ratio    : ����ϵ��k
* @param [in] addition : ����m
* @param [in] preValue : ��ΪЭ���е�yֵ, ��App UI�������ʾֵ
*
* @return  aft_value : ��ΪЭ���е�xֵ, ��ʵ��ͨѶ�����ֵ
*/
static uint32_t gizY2XFloat(float ratio, float addition, float preValue)
{
    uint32_t aftValue = 0;

    //x=(y - m)/k
    aftValue = ((preValue - addition) / ratio);

    return aftValue;
}

/**
* @brief ת��ΪЭ���е�yֵ��App UI�������ʾֵ,ֻ�����Ը���������������
*
* @param [in] ratio    : ����ϵ��k
* @param [in] addition : ����m
* @param [in] preValue : ��ΪЭ���е�xֵ, ��ʵ��ͨѶ�����ֵ
*
* @return : ��ΪЭ���е�yֵ, ��App UI�������ʾֵ
*/
static float gizX2YFloat(float ratio, float addition, uint32_t preValue)
{
    float aftValue = 0;

    //y=k * x + m
    aftValue = (preValue * ratio + addition);

    return aftValue;
}

/**
* @brief ���ݵ���ֽ��ж�
*
* @param [in] bitOffset     : λƫ��
* @param [in] bitLen        : ռ��λ����
*
* @return δ���ֽ� : 0
            ���ֽ� : 1
*/
static uint8_t gizAcrossByteJudge(uint32_t bitOffset,uint32_t bitLen)
{
    if((0 == bitOffset)||(0 == bitOffset%8))
    {
        if(bitLen <= 8)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        if(8 - bitOffset%8 >= bitLen)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}

/**
* @brief bool��enum�������ݵ����ݽ�ѹ
*
* @param [in] byteOffset    : �ֽ�ƫ��
* @param [in] bitOffset     : λƫ��
* @param [in] bitLen        : ռ��λ����
* @param [in] arrayAddr     : �����ַ
* @param [in] arrayLen      : ���鳤��
*
* @return destValue         : ��ѹ��������,-1��ʾ���󷵻�
*/
static int32_t gizDecompressionValue(uint32_t byteOffset,uint32_t bitOffset,uint32_t bitLen,uint8_t *arrayAddr,uint32_t arrayLen)
{
    uint8_t ret = 0;
    uint8_t highBit = 0 ,lowBit = 0;
    uint8_t destBufTemp[COUNT_W_BIT];
    int32_t destValue = 0;

    if(NULL == arrayAddr || 0 == arrayLen)
    {
        GIZWITS_LOG("gizDecompressionValue Error , Illegal Param\n");
        return -1;
    }

    memcpy(destBufTemp,arrayAddr,arrayLen);
    if(arrayLen > 1)// Judge Byte order conversion
    {
        if(-1 == gizByteOrderExchange(destBufTemp,arrayLen))
        {
            GIZWITS_LOG("gizDecompressionValue gizByteOrderExchange Error \n");
            return -1;
        }
    }
    ret = gizAcrossByteJudge(bitOffset,bitLen);//Judge Byte Step
    if(0 == ret)
    {
        destValue |= ((destBufTemp[byteOffset] >> (bitOffset%8)) & (0xff >> (8 - bitLen)));
    }
    else if(1 == ret)
    {
        /* ��ʱֻ֧������2�ֽ� */
        highBit = destBufTemp[byteOffset + 1]& (0xFF >> (8-(bitLen-(8-bitOffset%8))));
        lowBit = destBufTemp[byteOffset]>> (bitOffset%8);
        destValue |=  (highBit << (8-bitOffset%8));
        destValue |= lowBit;
    }
    return destValue;
}

/**
* @brief bool��enum�������ݵ�����ѹ��
*
* @param [in] byteOffset    : �ֽ�ƫ��
* @param [in] bitOffset     : λƫ��
* @param [in] bitLen        : ռ��λ����
* @param [out] arrayAddr    : �����ַ
* @param [in] srcData       : ԭʼ����
*
* @return : 0,��ȷ����;-1�����󷵻�
*/
static int32_t gizCompressValue(uint32_t byteOffset,uint32_t bitOffset,uint32_t bitLen,uint8_t *bufAddr,uint32_t srcData)
{
    uint8_t highBit = 0;
    uint8_t lowBit = 0;
    uint8_t ret = 0;

    if(NULL == bufAddr)
    {
        GIZWITS_LOG("gizCompressValue Error , Illegal Param\n");
        return -1;
    }

    ret = gizAcrossByteJudge(bitOffset,bitLen);
    if(0 == ret)
    {
        bufAddr[byteOffset] |= (((uint8_t)srcData)<<(bitOffset%8));
    }
    else if(1 == ret)
    {
        /* ��ʱ֧���������ֽڵ�ѹ�� */
        highBit = ((uint8_t)srcData)>>(8-bitOffset%8);
        lowBit = (uint8_t)srcData & (0xFF >> (8-bitOffset%8));
        bufAddr[byteOffset + 1] |=  highBit;
        bufAddr[byteOffset] |= (lowBit<<(bitOffset%8));
    }

    return 0;
}

/**
* @brief �ӻ��λ�������ץȡһ������
*
* @param [in]  rb                  : �������ݵ�ַ
* @param [out] data                : ������ݵ�ַ
* @param [out] len                 : ������ݳ���
*
* @return : 0,��ȷ����;-1�����󷵻�;-2������У��ʧ��
*/
static int8_t gizProtocolGetOnePacket(rb_t *rb, uint8_t *data, uint16_t *len)
{
    uint8_t ret = 0;
    uint8_t sum = 0;
    uint8_t i = 0;
    uint8_t tmpData;
    uint8_t tmpLen = 0;
    uint16_t tmpCount = 0;
    static uint8_t protocolFlag = 0;
    static uint16_t protocolCount = 0;
    static uint8_t lastData = 0;
    static uint8_t debugCount = 0;
    uint8_t *protocolBuff = data;
    protocolHead_t *head = NULL;

    if((NULL == rb) || (NULL == data) ||(NULL == len))
    {
        GIZWITS_LOG("gizProtocolGetOnePacket Error , Illegal Param\n");
        return -1;
    }

    tmpLen = rbCanRead(rb);
    if(0 == tmpLen)
    {
        return -1;
    }

    for(i=0; i<tmpLen; i++)
    {
        ret = rbRead(rb, &tmpData, 1);
        if(0 != ret)
        {
            if((0xFF == lastData) && (0xFF == tmpData))
            {
                if(0 == protocolFlag)
                {
                    protocolBuff[0] = 0xFF;
                    protocolBuff[1] = 0xFF;
                    protocolCount = 2;
                    protocolFlag = 1;
                }
                else
                {
                    if((protocolCount > 4) && (protocolCount != tmpCount))
                    {
                        protocolBuff[0] = 0xFF;
                        protocolBuff[1] = 0xFF;
                        protocolCount = 2;
                    }
                }
            }
            else if((0xFF == lastData) && (0x55 == tmpData))
            {
            }
            else
            {
                if(1 == protocolFlag)
                {
                    protocolBuff[protocolCount] = tmpData;
                    protocolCount++;

                    if(protocolCount > 4)
                    {
                        head = (protocolHead_t *)protocolBuff;
                        tmpCount = gizProtocolExchangeBytes(head->len)+4;
                        if(protocolCount == tmpCount)
                        {
                            break;
                        }
                    }
                }
            }

            lastData = tmpData;
            debugCount++;
        }
    }

    if((protocolCount > 4) && (protocolCount == tmpCount))
    {
        sum = gizProtocolSum(protocolBuff, protocolCount);

        if(protocolBuff[protocolCount-1] == sum)
        {
            memcpy(data, protocolBuff, tmpCount);
            *len = tmpCount;
            protocolFlag = 0;

            protocolCount = 0;
            debugCount = 0;
            lastData = 0;

            return 0;
        }
        else
        {
            return -2;
        }
    }

    return 1;
}

/**
* @brief ����Э�����ɡ��������¼���

* @param [in] issuedData  ������������
* @param [out] info       ���¼�����
* @param [out] dataPoints �����ݵ�����
* @return 0��ִ�гɹ��� �� 0��ʧ��
*/
static int8_t gizDataPoint2Event(gizwitsIssued_t *issuedData, eventInfo_t *info, dataPoint_t *dataPoints)
{
    if((NULL == issuedData) || (NULL == info) ||(NULL == dataPoints))
    {
        GIZWITS_LOG("gizDataPoint2Event Error , Illegal Param\n");
        return -1;
    }
    /** ����1�ֽ���λ��ת�� **/
    if(sizeof(issuedData->attrFlags) > 1)
    {
        if(-1 == gizByteOrderExchange((uint8_t *)&issuedData->attrFlags,sizeof(attrFlags_t)))
        {
            GIZWITS_LOG("gizByteOrderExchange Error\n");
            return -1;
        }
    }
    if(0x01 == issuedData->attrFlags.flaggreen_led)
    {
        info->event[info->num] = EVENT_GREEN_LED;
        info->num++;
        dataPoints->valuegreen_led = gizDecompressionValue(GREEN_LED_BYTEOFFSET,GREEN_LED_BITOFFSET,GREEN_LED_LEN,(uint8_t *)&issuedData->attrVals.wBitBuf,sizeof(issuedData->attrVals.wBitBuf));
    }



    return 0;
}

/**
* @brief �Աȵ�ǰ���ݺ��ϴ�����
*
* @param [in] cur        : ��ǰ���ݵ�����
* @param [in] last       : �ϴ����ݵ�����
*
* @return : 0,�����ޱ仯;1�������б仯
*/
static int8_t gizCheckReport(dataPoint_t *cur, dataPoint_t *last)
{
    int8_t ret = 0;
    static uint32_t lastReportTime = 0;

    if((NULL == cur) || (NULL == last))
    {
        GIZWITS_LOG("gizCheckReport Error , Illegal Param\n");
        return -1;
    }
    if(last->valuegreen_led != cur->valuegreen_led)
    {
        GIZWITS_LOG("valuegreen_led Changed\n");
        ret = 1;
    }


    return ret;
}

/**
* @brief �û����ݵ�����ת��Ϊ�������ϱ����ݵ�����
*
* @param [in]  dataPoints           : �û����ݵ����ݵ�ַ
* @param [out] devStatusPtr         : �������ϱ����ݵ����ݵ�ַ
*
* @return 0,��ȷ����;-1�����󷵻�
*/
static int8_t gizDataPoints2ReportData(dataPoint_t *dataPoints , devStatus_t *devStatusPtr)
{
    if((NULL == dataPoints) || (NULL == devStatusPtr))
    {
        GIZWITS_LOG("gizDataPoints2ReportData Error , Illegal Param\n");
        return -1;
    }

    memset((uint8_t *)devStatusPtr->wBitBuf,0,sizeof(devStatusPtr->wBitBuf));

    gizCompressValue(GREEN_LED_BYTEOFFSET,GREEN_LED_BITOFFSET,GREEN_LED_LEN,(uint8_t *)devStatusPtr,dataPoints->valuegreen_led);
    gizByteOrderExchange((uint8_t *)devStatusPtr->wBitBuf,sizeof(devStatusPtr->wBitBuf));






    return 0;
}

/**
* @brief Э��ͷ��ʼ��
*
* @param [out] head         : Э��ͷָ��
*
* @return 0�� ִ�гɹ��� �� 0�� ʧ��
*/
static int8_t gizProtocolHeadInit(protocolHead_t *head)
{
    if(NULL == head)
    {
        GIZWITS_LOG("ERROR: gizProtocolHeadInit head is empty \n");
        return -1;
    }

    memset((uint8_t *)head, 0, sizeof(protocolHead_t));
    head->head[0] = 0xFF;
    head->head[1] = 0xFF;

    return 0;
}

/**
* @brief Э��ACKУ�鴦����
*
* @param [in] data            : ���ݵ�ַ
* @param [in] len             : ���ݳ���
*
* @return 0�� ִ�гɹ��� �� 0�� ʧ��
*/
static int8_t gizProtocolWaitAck(uint8_t *data, uint32_t len)
{
    if(NULL == data)
    {
        GIZWITS_LOG("ERROR: data is empty \n");
        return -1;
    }

    memset((uint8_t *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
    memcpy((uint8_t *)gizwitsProtocol.waitAck.buf, data, len);
    gizwitsProtocol.waitAck.dataLen = (uint16_t)len;

    gizwitsProtocol.waitAck.flag = 1;
    gizwitsProtocol.waitAck.sendTime = gizGetTimerCount();

    return 0;
}

/**
* @brief Э�������ط�

* У�鳬ʱ�������ط��������Ƶ�Э�����ݽ����ط�

* @param none
*
* @return none
*/
static void gizProtocolResendData(void)
{
    uint8_t ret = 0;

    if(0 == gizwitsProtocol.waitAck.flag)
    {
        return;
    }

    GIZWITS_LOG("Warning: timeout, resend data \n");

    ret = uartWrite(gizwitsProtocol.waitAck.buf, gizwitsProtocol.waitAck.dataLen);
    if(ret != gizwitsProtocol.waitAck.dataLen)
    {
        GIZWITS_LOG("ERROR: resend data error\n");
    }

    gizwitsProtocol.waitAck.sendTime = gizGetTimerCount();
}

/**
* @brief ���ACKЭ�鱨��
*
* @param [in] head : Э��ͷ��ַ
*
* @return 0�� ִ�гɹ��� �� 0�� ʧ��
*/
static int8_t gizProtocolWaitAckCheck(protocolHead_t *head)
{
    protocolHead_t *waitAckHead = (protocolHead_t *)gizwitsProtocol.waitAck.buf;

    if(NULL == head)
    {
        GIZWITS_LOG("ERROR: data is empty \n");
        return -1;
    }

    if(waitAckHead->cmd+1 == head->cmd)
    {
        memset((uint8_t *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
    }

    return 0;
}

/**
* @brief ����ͨ��Э�鱨������
*
* @param [in] head              : Э��ͷָ��
*
* @return : ��Ч���ݳ���,��ȷ����;-1�����󷵻�
*/
static int32_t gizProtocolCommonAck(protocolHead_t *head)
{
    int32_t ret = 0;
    protocolCommon_t ack;

    if(NULL == head)
    {
        GIZWITS_LOG("ERROR: gizProtocolCommonAck data is empty \n");
        return -1;
    }
    memcpy((uint8_t *)&ack, (uint8_t *)head, sizeof(protocolHead_t));
    ack.head.cmd = ack.head.cmd+1;
    ack.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
    ack.sum = gizProtocolSum((uint8_t *)&ack, sizeof(protocolCommon_t));

    ret = uartWrite((uint8_t *)&ack, sizeof(protocolCommon_t));
    if(ret < 0)
    {
        //��ӡ��־
        GIZWITS_LOG("ERROR: uart write error %d \n", ret);
    }

    return ret;
}

/**
* @brief ACK�߼�������

* ���ͺ��Э�����ݽ��г�ʱ�жϣ���ʱ200ms�����ط����ط�����Ϊ����

* @param none
*
* @return none
*/
static void gizProtocolAckHandle(void)
{
    if(1 == gizwitsProtocol.waitAck.flag)
    {
        if(SEND_MAX_NUM >= gizwitsProtocol.waitAck.num)
        {
            //300msδ�յ�ACK�ط�
            if(SEND_MAX_TIME < (gizGetTimerCount() - gizwitsProtocol.waitAck.sendTime))
            {
                GIZWITS_LOG("Warning:gizProtocolResendData %d %d %d\n", gizGetTimerCount(), gizwitsProtocol.waitAck.sendTime, gizwitsProtocol.waitAck.num);
                gizProtocolResendData();
                gizwitsProtocol.waitAck.num++;
            }
        }
        else
        {
            memset((uint8_t *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
        }
    }
}

/**
* @brief Э��4.1 WiFiģ�������豸��Ϣ
*
* @param head : �����͵�Э�鱨������
*
* @return ������Ч���ݳ���; -1�����󷵻�
*/
static int32_t gizProtocolGetDeviceInfo(protocolHead_t * head)
{
    int32_t ret = 0;
    protocolDeviceInfo_t deviceInfo;

    if(NULL == head)
    {
        GIZWITS_LOG("gizProtocolGetDeviceInfo Error , Illegal Param\n");
        return -1;
    }

    gizProtocolHeadInit((protocolHead_t *)&deviceInfo);
    deviceInfo.head.cmd = ACK_GET_DEVICE_INFO;
    deviceInfo.head.sn = head->sn;
    memcpy((uint8_t *)deviceInfo.protocolVer, protocol_VERSION, 8);
    memcpy((uint8_t *)deviceInfo.p0Ver, P0_VERSION, 8);
    memcpy((uint8_t *)deviceInfo.softVer, SOFTWARE_VERSION, 8);
    memcpy((uint8_t *)deviceInfo.hardVer, HARDWARE_VERSION, 8);
    memcpy((uint8_t *)deviceInfo.productKey, PRODUCT_KEY, 32);
    memcpy((uint8_t *)deviceInfo.devAttr, DEV_DEFAULT, 8);
    deviceInfo.ninableTime = 0;
    deviceInfo.head.len = gizProtocolExchangeBytes(sizeof(protocolDeviceInfo_t)-4);
    deviceInfo.sum = gizProtocolSum((uint8_t *)&deviceInfo, sizeof(protocolDeviceInfo_t));

    ret = uartWrite((uint8_t *)&deviceInfo, sizeof(protocolDeviceInfo_t));
    if(ret < 0)
    {
        //��ӡ��־
        GIZWITS_LOG("ERROR: uart write error %d \n", ret);
    }

    return ret;
}

/**
* @brief Э��4.7 �Ƿ���Ϣ֪ͨ �Ĵ���

* @param[in] head  : Э��ͷָ��
* @param[in] errno : �Ƿ���Ϣ֪ͨ����
* @return 0�� ִ�гɹ��� �� 0�� ʧ��
*/
static int32_t gizProtocolErrorCmd(protocolHead_t *head,errorPacketsType_t errno)
{
    int32_t ret = 0;
    protocolErrorType_t errorType;

    if(NULL == head)
    {
        GIZWITS_LOG("gizProtocolErrorCmd Error , Illegal Param\n");
        return -1;
    }
    gizProtocolHeadInit((protocolHead_t *)&errorType);
    errorType.head.cmd = ACK_ERROR_PACKAGE;
    errorType.head.sn = head->sn;

    errorType.head.len = gizProtocolExchangeBytes(sizeof(protocolErrorType_t)-4);
    errorType.error = errno;
    errorType.sum = gizProtocolSum((uint8_t *)&errorType, sizeof(protocolErrorType_t));

    ret = uartWrite((uint8_t *)&errorType, sizeof(protocolErrorType_t));
    if(ret < 0)
    {
        //��ӡ��־
        GIZWITS_LOG("ERROR: uart write error %d \n", ret);
    }

    return ret;
}

/**
* @brief Э��4.4 �豸MCU����WiFiģ�� ����ز���

* @param none
* @return none
*/
static void gizProtocolReboot(void)
{
    uint32_t timeDelay = gizGetTimerCount();

    /*Wait 600ms*/
    while((gizGetTimerCount() - timeDelay) <= 600);
    mcuRestart();
}

/**
* @brief Э�� 4.5 WiFiģ�����豸MCU֪ͨWiFiģ�鹤��״̬�ı仯

* @param[in] status WiFiģ�鹤��״̬
* @return none
*/
static int8_t gizProtocolModuleStatus(protocolWifiStatus_t *status)
{
    static wifiStatus_t lastStatus;

    if(NULL == status)
    {
        GIZWITS_LOG("gizProtocolModuleStatus Error , Illegal Param\n");
        return -1;
    }

    status->ststus.value = gizProtocolExchangeBytes(status->ststus.value);

    //OnBoarding mode status
    if(lastStatus.types.onboarding != status->ststus.types.onboarding)
    {
        if(1 == status->ststus.types.onboarding)
        {
            if(1 == status->ststus.types.softap)
            {
                gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_SOFTAP;
                gizwitsProtocol.wifiStatusEvent.num++;
                GIZWITS_LOG("OnBoarding: SoftAP or Web mode\n");
            }

            if(1 == status->ststus.types.station)
            {
                gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_AIRLINK;
                gizwitsProtocol.wifiStatusEvent.num++;
                GIZWITS_LOG("OnBoarding: AirLink mode\n");
            }
        }
        else
        {
            if(1 == status->ststus.types.softap)
            {
                gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_SOFTAP;
                gizwitsProtocol.wifiStatusEvent.num++;
                GIZWITS_LOG("OnBoarding: SoftAP or Web mode\n");
            }

            if(1 == status->ststus.types.station)
            {
                gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_STATION;
                gizwitsProtocol.wifiStatusEvent.num++;
                GIZWITS_LOG("OnBoarding: Station mode\n");
            }
        }
    }

    //binding mode status
    if(lastStatus.types.binding != status->ststus.types.binding)
    {
        lastStatus.types.binding = status->ststus.types.binding;
        if(1 == status->ststus.types.binding)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_OPEN_BINDING;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: in binding mode\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CLOSE_BINDING;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: out binding mode\n");
        }
    }

    //router status
    if(lastStatus.types.con_route != status->ststus.types.con_route)
    {
        lastStatus.types.con_route = status->ststus.types.con_route;
        if(1 == status->ststus.types.con_route)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_ROUTER;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: connected router\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_ROUTER;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: disconnected router\n");
        }
    }

    //M2M server status
    if(lastStatus.types.con_m2m != status->ststus.types.con_m2m)
    {
        lastStatus.types.con_m2m = status->ststus.types.con_m2m;
        if(1 == status->ststus.types.con_m2m)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_M2M;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: connected m2m\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_M2M;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: disconnected m2m\n");
        }
    }

    //APP status
    if(lastStatus.types.app != status->ststus.types.app)
    {
        lastStatus.types.app = status->ststus.types.app;
        if(1 == status->ststus.types.app)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_APP;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: app connect\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_APP;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: no app connect\n");
        }
    }

    //test mode status
    if(lastStatus.types.test != status->ststus.types.test)
    {
        lastStatus.types.test = status->ststus.types.test;
        if(1 == status->ststus.types.test)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_OPEN_TESTMODE;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: in test mode\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CLOSE_TESTMODE;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: out test mode\n");
        }
    }

    gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_RSSI;
    gizwitsProtocol.wifiStatusEvent.num++;
    gizwitsProtocol.wifiStatusData.rssi = status->ststus.types.rssi;
    GIZWITS_LOG("RSSI is %d \n", gizwitsProtocol.wifiStatusData.rssi);

    gizwitsProtocol.issuedFlag = 2;

    return 0;
}

/**
* @brief Э���·����ݷ���ACK
*
* @param [in] head                  : Э��ͷָ��
* @param [in] data                  : ���ݵ�ַ
* @param [in] len                   : ���ݳ���
*
* @return : ��Ч���ݳ���,��ȷ����;-1�����󷵻�
*/
static int32_t gizProtocolIssuedDataAck(protocolHead_t *head, uint8_t *data, uint32_t len)
{
    int32_t ret = 0;
    uint8_t *ptrData = NULL;
    uint32_t dataLen = 0;
    protocolReport_t protocolReport;
    protocolCommon_t protocolCommon;
    protocolP0Head_t *p0Head = (protocolP0Head_t *)head;

    if((NULL == head)||(NULL == data))
    {
        GIZWITS_LOG("gizProtocolIssuedDataAck Error , Illegal Param\n");
        return -1;
    }
    if(0 == len)
    {
        gizProtocolHeadInit((protocolHead_t *)&protocolCommon);
        protocolCommon.head.cmd = head->cmd+1;
        protocolCommon.head.sn = head->sn;
        protocolCommon.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
        protocolCommon.sum = gizProtocolSum((uint8_t *)&protocolCommon, sizeof(protocolCommon_t));

        ptrData = (uint8_t *)&protocolCommon;
        dataLen = sizeof(protocolCommon_t);
    }
    else
    {
        gizProtocolHeadInit((protocolHead_t *)&protocolReport);
        protocolReport.head.cmd = p0Head->head.cmd+1;
        protocolReport.head.sn = p0Head->head.sn;
        protocolReport.head.len = gizProtocolExchangeBytes(sizeof(protocolP0Head_t)+len+1-4);
        protocolReport.action = p0Head->action+1;

        memcpy((uint8_t *)&protocolReport.reportData, data, len);
        protocolReport.sum = gizProtocolSum((uint8_t *)&protocolReport, sizeof(protocolReport_t));

        ptrData = (uint8_t *)&protocolReport;
        dataLen = sizeof(protocolReport_t);
    }

    ret = uartWrite(ptrData, dataLen);
    if(ret < 0)
    {
        //��ӡ��־
        GIZWITS_LOG("ERROR: gizProtocolIssuedDataAck uart write error %d dataLen %d \n", ret, dataLen);
    }

    return ret;
}

/**
* @brief �ϱ�����
*
* @param [in] action            : PO cmd
* @param [in] data              : ���ݵ�ַ
* @param [in] len               : ���ݳ���
*
* @return : ��ȷ������Ч���ݳ���;-1�����󷵻�
*/
static int32_t gizReportData(uint8_t action, uint8_t *data, uint32_t len)
{
    int32_t ret = 0;
    protocolReport_t protocolReport;

    if(NULL == data)
    {
        GIZWITS_LOG("gizReportData Error , Illegal Param\n");
        return -1;
    }
    gizProtocolHeadInit((protocolHead_t *)&protocolReport);
    protocolReport.head.cmd = CMD_REPORT_P0;
    protocolReport.head.sn = gizwitsProtocol.sn++;
    protocolReport.action = action;
    protocolReport.head.len = gizProtocolExchangeBytes(sizeof(protocolReport_t)-4);
    memcpy((gizwitsReport_t *)&protocolReport.reportData, (gizwitsReport_t *)data,len);
    protocolReport.sum = gizProtocolSum((uint8_t *)&protocolReport, sizeof(protocolReport_t));

    ret = uartWrite((uint8_t *)&protocolReport, sizeof(protocolReport_t));
    if(ret < 0)
    {
        //��ӡ��־
        GIZWITS_LOG("ERROR: uart write error %d \n", ret);
    }

    gizProtocolWaitAck((uint8_t *)&protocolReport, sizeof(protocolReport_t));
    gizwitsProtocol.lastReportTime = gizGetTimerCount();

    return ret;
}

/**
* @brief ����Э�鱨�Ĳ�������Ӧ��Э�鴦��

* Wifiģ����������ƶ˻�APP���·������Э�����ݷ��͵�MCU�ˣ�����Э�鱨�Ľ��������Э�����ݴ���κ�����������һ����Э�鴦��

* @param[in] inData   : �����Э������
* @param[in] inLen    : �������ݵĳ���
* @param[out] outData : �����Э������
* @param[out] outLen  : ������ݵĳ���
* @return 0�� ִ�гɹ��� �� 0�� ʧ��
*/
static int8_t gizProtocolIssuedProcess(uint8_t *inData, uint32_t inLen, uint8_t *outData, uint32_t *outLen)
{
    protocolReport_t *protocolIssuedData = (protocolReport_t *)inData;
    uint8_t issuedAction = 0;
    issuedAction = protocolIssuedData->action;

    if((NULL == inData)||(NULL == outData)||(NULL == outLen))
    {
        GIZWITS_LOG("gizProtocolIssuedProcess Error , Illegal Param\n");
        return -1;
    }

    memset((uint8_t *)&gizwitsProtocol.issuedProcessEvent, 0, sizeof(eventInfo_t));
    switch(issuedAction)
    {
        case ACTION_CONTROL_DEVICE:
            gizDataPoint2Event((gizwitsIssued_t *)(inData+sizeof(protocolP0Head_t)), &gizwitsProtocol.issuedProcessEvent,&gizwitsProtocol.gizCurrentDataPoint);
            gizwitsProtocol.issuedFlag = 1;
            outData = NULL;
            *outLen = 0;
            break;

        case ACTION_READ_DEV_STATUS:
            gizDataPoints2ReportData(&gizwitsProtocol.gizLastDataPoint,&gizwitsProtocol.reportData.devStatus);
            //memcpy((uint8_t *)&gizwitsProtocol.gizLastDataPoint, (uint8_t *)currentData, sizeof(dataPoint_t));

            memcpy(outData, (uint8_t *)&gizwitsProtocol.reportData.devStatus, sizeof(gizwitsReport_t));
            *outLen = sizeof(gizwitsReport_t);
            break;

        case ACTION_W2D_TRANSPARENT_DATA:
            memcpy(gizwitsProtocol.transparentBuff, inData+sizeof(protocolP0Head_t), inLen-sizeof(protocolP0Head_t)-1);
            gizwitsProtocol.transparentLen = inLen - sizeof(protocolP0Head_t) - 1;
            gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = TRANSPARENT_DATA;
            gizwitsProtocol.issuedProcessEvent.num++;
            gizwitsProtocol.issuedFlag = 3;
            break;

        default:
            break;
    }

    return 0;
}

/**@name Gizwits �û�API�ӿ�
* @{
*/

/**
* @brief gizwitsЭ���ʼ���ӿ�

* �û����øýӿڿ������GizwitsЭ����س�ʼ��������Э����ض�ʱ�������ڵĳ�ʼ��

* �û������ڴ˽ӿ���������ݵ�ĳ�ʼ��״̬����

* @param none
* @return none
*/
void gizwitsInit(void)
{
    uartInit();

    pRb.rbCapacity = RB_MAX_LEN;
    pRb.rbBuff = rbBuf;
    rbCreate(&pRb);

    memset((uint8_t *)&gizwitsProtocol, 0, sizeof(gizwitsProtocol_t));
}

/**
* @brief WiFi���ýӿ�

* �û����Ե��øýӿ�ʹWiFiģ�������Ӧ������ģʽ���߸�λģ��

* @param[in] mode ����ģʽѡ��0x0�� ģ�鸴λ ;0x01�� SoftApģʽ ;0x02�� AirLinkģʽ
* @return ����������
*/
int32_t gizwitsSetMode(uint8_t mode)
{
    int32_t ret = 0;
    protocolCfgMode_t cfgMode;
    protocolCommon_t setDefault;

    if(mode == WIFI_RESET_MODE)
    {
        gizProtocolHeadInit((protocolHead_t *)&setDefault);
        setDefault.head.cmd = CMD_SET_DEFAULT;
        setDefault.head.sn = gizwitsProtocol.sn++;
        setDefault.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
        setDefault.sum = gizProtocolSum((uint8_t *)&setDefault, sizeof(protocolCommon_t));
        ret = uartWrite((uint8_t *)&setDefault, sizeof(protocolCommon_t));
        if(ret < 0)
        {
            GIZWITS_LOG("ERROR: uart write error %d \n", ret);
        }

        gizProtocolWaitAck((uint8_t *)&setDefault, sizeof(protocolCommon_t));
    }
    else if((mode == WIFI_SOFTAP_MODE)||(mode == WIFI_AIRLINK_MODE))
    {
        gizProtocolHeadInit((protocolHead_t *)&cfgMode);
        cfgMode.head.cmd = CMD_WIFI_CONFIG;
        cfgMode.head.sn = gizwitsProtocol.sn++;
        cfgMode.cfgMode = mode;
        cfgMode.head.len = gizProtocolExchangeBytes(sizeof(protocolCfgMode_t)-4);
        cfgMode.sum = gizProtocolSum((uint8_t *)&cfgMode, sizeof(protocolCfgMode_t));
        ret = uartWrite((uint8_t *)&cfgMode, sizeof(protocolCfgMode_t));
        if(ret < 0)
        {
            GIZWITS_LOG("ERROR: uart write error %d \n", ret);
        }
        gizProtocolWaitAck((uint8_t *)&cfgMode, sizeof(protocolCfgMode_t));
    }

    return ret;
}

/**
* @brief Э�鴦����

* �ú������������ӦЭ�����ݵĴ������������ϱ�����ز���

* @param [in] currentData ���ϱ���Э������ָ��
* @return none
*/
int32_t gizwitsHandle(dataPoint_t *currentData)
{
    int8_t ret = 0;
    uint16_t i = 0;
    uint8_t ackData[RB_MAX_LEN];
    uint16_t protocolLen = 0;
    uint32_t ackLen = 0;
    protocolHead_t *recvHead = NULL;

    if(NULL == currentData)
    {
        GIZWITS_LOG("GizwitsHandle Error , Illegal Param\n");
        return -1;
    }

    /*�ط�����*/
    gizProtocolAckHandle();
    ret = gizProtocolGetOnePacket(&pRb, gizwitsProtocol.protocolBuf, &protocolLen);

    if(0 == ret)
    {
        GIZWITS_LOG("Get One Packet!\n");

#ifdef PROTOCOL_DEBUG
        GIZWITS_LOG("WiFi2MCU[%4d:%4d]: ", gizGetTimerCount(), protocolLen);
        for(i=0; i<protocolLen;i++)
        {
            GIZWITS_LOG("%02x ", gizwitsProtocol.protocolBuf[i]);
        }
        GIZWITS_LOG("\n");
#endif

        recvHead = (protocolHead_t *)gizwitsProtocol.protocolBuf;
        switch (recvHead->cmd)
        {
            case CMD_GET_DEVICE_INTO:
                gizProtocolGetDeviceInfo(recvHead);
                break;
            case CMD_ISSUED_P0:
                ret = gizProtocolIssuedProcess(gizwitsProtocol.protocolBuf, protocolLen, ackData, &ackLen);
                if(0 == ret)
                {
                    gizProtocolIssuedDataAck(recvHead, ackData, ackLen);
                }
                break;
            case CMD_HEARTBEAT:
                gizProtocolCommonAck(recvHead);
                break;
            case CMD_WIFISTATUS:
                gizProtocolCommonAck(recvHead);
                gizProtocolModuleStatus((protocolWifiStatus_t *)recvHead);
                break;
            case ACK_REPORT_P0:
            case ACK_WIFI_CONFIG:
            case ACK_SET_DEFAULT:
                gizProtocolWaitAckCheck(recvHead);
                break;
            case CMD_MCU_REBOOT:
                gizProtocolCommonAck(recvHead);
                GIZWITS_LOG("report:MCU reboot!\n");

                gizProtocolReboot();
                break;
            case CMD_ERROR_PACKAGE:
                break;
            default:
                gizProtocolErrorCmd(recvHead,ERROR_CMD);
                GIZWITS_LOG("ERROR: cmd code error!\n");
                break;
        }
    }
    else if(-2 == ret)
    {
        //У��ʧ�ܣ������쳣
        recvHead = (protocolHead_t *)gizwitsProtocol.protocolBuf;
        gizProtocolErrorCmd(recvHead,ERROR_ACK_SUM);
        GIZWITS_LOG("ERROR: check sum error!\n");
        return -2;
    }

    if(1 == gizwitsProtocol.issuedFlag)
    {
        gizwitsProtocol.issuedFlag = 0;
        gizwitsEventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8_t *)&gizwitsProtocol.gizCurrentDataPoint, sizeof(dataPoint_t));
        memset((uint8_t *)&gizwitsProtocol.issuedProcessEvent,0x0,sizeof(gizwitsProtocol.issuedProcessEvent));
    }
    else if(2 == gizwitsProtocol.issuedFlag)
    {
        gizwitsProtocol.issuedFlag = 0;
        gizwitsEventProcess(&gizwitsProtocol.wifiStatusEvent, (uint8_t *)&gizwitsProtocol.wifiStatusData, sizeof(moduleStatusInfo_t));
        memset((uint8_t *)&gizwitsProtocol.wifiStatusEvent,0x0,sizeof(gizwitsProtocol.wifiStatusEvent));
    }
    else if(3 == gizwitsProtocol.issuedFlag)
    {
        gizwitsProtocol.issuedFlag = 0;
        gizwitsEventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8_t *)gizwitsProtocol.transparentBuff, gizwitsProtocol.transparentLen);
    }

    if((1 == gizCheckReport(currentData, (dataPoint_t *)&gizwitsProtocol.gizLastDataPoint)))
    {
        GIZWITS_LOG("changed, report data\n");
        gizDataPoints2ReportData(currentData,&gizwitsProtocol.reportData.devStatus);
        gizReportData(ACTION_REPORT_DEV_STATUS, (uint8_t *)&gizwitsProtocol.reportData.devStatus, sizeof(devStatus_t));
        memcpy((uint8_t *)&gizwitsProtocol.gizLastDataPoint, (uint8_t *)currentData, sizeof(dataPoint_t));
    }

    if(1000*60*10 <= (gizGetTimerCount() - gizwitsProtocol.lastReportTime))
    {
        GIZWITS_LOG("Info: 600S report data\n");
        gizDataPoints2ReportData(currentData,&gizwitsProtocol.reportData.devStatus);
        gizReportData(ACTION_REPORT_DEV_STATUS, (uint8_t *)&gizwitsProtocol.reportData.devStatus, sizeof(devStatus_t));
        memcpy((uint8_t *)&gizwitsProtocol.gizLastDataPoint, (uint8_t *)currentData, sizeof(dataPoint_t));
    }

    return 0;
}
/**@} */

