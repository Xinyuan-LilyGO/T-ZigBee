/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include "zbhci.h"
#include "hci_uart.h"
#include "hci_display.h"

#include "esp_log.h"
#include "esp32-hal-log.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

#define TAG "zbhci"

/* Macros take the buffer pointer and return the data */
#define BUFFER_TO_U64( BUFFER, i ) ( \
    ( ( uint64_t ) ( BUFFER )[ i     ] << 56 & 0xFF00000000000000) | \
    ( ( uint64_t ) ( BUFFER )[ i + 1 ] << 48 & 0x00FF000000000000) | \
    ( ( uint64_t ) ( BUFFER )[ i + 2 ] << 40 & 0x0000FF0000000000) | \
    ( ( uint64_t ) ( BUFFER )[ i + 3 ] << 32 & 0x000000FF00000000) | \
    ( ( uint64_t ) ( BUFFER )[ i + 4 ] << 24 & 0x00000000FF000000) | \
    ( ( uint64_t ) ( BUFFER )[ i + 5 ] << 16 & 0x0000000000FF0000) | \
    ( ( uint64_t ) ( BUFFER )[ i + 6 ] << 8  & 0x000000000000FF00) | \
    ( ( uint64_t ) ( BUFFER )[ i + 7 ]       & 0x00000000000000FF))

#define BUFFER_TO_U56( BUFFER, i ) ( \
    ( ( uint64_t ) ( BUFFER )[ i     ] << 48 & 0x00FF000000000000 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 1 ] << 40 & 0x0000FF0000000000 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 2 ] << 32 & 0x000000FF00000000 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 3 ] << 24 & 0x00000000FF000000 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 4 ] << 16 & 0x0000000000FF0000 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 5 ] << 8  & 0x000000000000FF00 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 6 ]       & 0x00000000000000FF ))

#define BUFFER_TO_U48( BUFFER, i ) ( \
    ( ( uint64_t ) ( BUFFER )[ i     ] << 40 & 0x0000FF0000000000 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 1 ] << 32 & 0x000000FF00000000 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 2 ] << 24 & 0x00000000FF000000 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 3 ] << 16 & 0x0000000000FF0000 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 4 ] << 8  & 0x000000000000FF00 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 5 ]       & 0x00000000000000FF ))

#define BUFFER_TO_U40( BUFFER, i ) ( \
    ( ( uint64_t ) ( BUFFER )[ i     ] << 32 & 0x000000FF00000000 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 1 ] << 24 & 0x00000000FF000000 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 2 ] << 16 & 0x0000000000FF0000 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 3 ] << 8  & 0x000000000000FF00 ) | \
    ( ( uint64_t ) ( BUFFER )[ i + 4 ]       & 0x00000000000000FF ))

#define BUFFER_TO_U32( BUFFER, i ) ( \
    ( ( uint32_t ) ( BUFFER )[ i     ] << 24 & 0xFF000000) | \
    ( ( uint32_t ) ( BUFFER )[ i + 1 ] << 16 & 0x00FF0000) | \
    ( ( uint32_t ) ( BUFFER )[ i + 2 ] << 8  & 0x0000FF00) | \
    ( ( uint32_t ) ( BUFFER )[ i + 3 ]       & 0x000000FF))

#define BUFFER_TO_U24( BUFFER, i ) ( \
    ( ( uint32_t ) ( BUFFER )[ i     ] << 16 & 0x00FF0000) | \
    ( ( uint32_t ) ( BUFFER )[ i + 1 ] << 8  & 0x0000FF00) | \
    ( ( uint32_t ) ( BUFFER )[ i + 2 ]       & 0x000000FF))

#define BUFFER_TO_U16( BUFFER, i ) ( \
    ( ( uint16_t ) ( BUFFER )[ i     ] << 8  & 0xFF00) | \
    ( ( uint16_t ) ( BUFFER )[ i + 1 ]       & 0x00FF))

#define BUFFER_TO_U8( BUFFER, i ) ( \
    ( ( uint8_t  ) ( BUFFER )[ i     ]       & 0xFF))


/* Macros take buffer and return data and the next offset of within the buffer */
#define BUFFER_TO_U64_OFFSET(BUFFER, i, OFFSET ) ( \
    BUFFER_TO_U64 (BUFFER, i) ); \
    ( ( OFFSET ) += sizeof ( uint64_t ) )

#define BUFFER_TO_U32_OFFSET(BUFFER, i, OFFSET ) ( \
    BUFFER_TO_U32 (BUFFER, i) ); \
    ( ( OFFSET ) += sizeof ( uint32_t ) )

#define BUFFER_TO_U16_OFFSET(BUFFER, i, OFFSET ) ( \
    BUFFER_TO_U16 (BUFFER, i) ); \
    ( ( OFFSET ) += sizeof ( uint16_t ) )

#define BUFFER_TO_U8_OFFSET(BUFFER, i, OFFSET ) ( \
    BUFFER_TO_U8 (BUFFER, i) ); \
    ( ( OFFSET ) += sizeof ( uint8_t  ) )

/* Macros take the value and put it into a buffer */
#define U64_TO_BUFFER( BUFFER, U64VALUE, LEN) ( \
    ( *( uint8_t* ) ( ( BUFFER )     ) = ( uint8_t ) ( ( ( ( uint64_t ) ( U64VALUE ) ) >> 56 ) & 0xFF ) ), \
    ( *( uint8_t* ) ( ( BUFFER ) + 1 ) = ( uint8_t ) ( ( ( ( uint64_t ) ( U64VALUE ) ) >> 48 ) & 0xFF ) ), \
    ( *( uint8_t* ) ( ( BUFFER ) + 2 ) = ( uint8_t ) ( ( ( ( uint64_t ) ( U64VALUE ) ) >> 40 ) & 0xFF ) ), \
    ( *( uint8_t* ) ( ( BUFFER ) + 3 ) = ( uint8_t ) ( ( ( ( uint64_t ) ( U64VALUE ) ) >> 32 ) & 0xFF ) ), \
    ( *( uint8_t* ) ( ( BUFFER ) + 4 ) = ( uint8_t ) ( ( ( ( uint64_t ) ( U64VALUE ) ) >> 24 ) & 0xFF ) ), \
    ( *( uint8_t* ) ( ( BUFFER ) + 5 ) = ( uint8_t ) ( ( ( ( uint64_t ) ( U64VALUE ) ) >> 16 ) & 0xFF ) ), \
    ( *( uint8_t* ) ( ( BUFFER ) + 6 ) = ( uint8_t ) ( ( ( ( uint64_t ) ( U64VALUE ) ) >>  8 ) & 0xFF ) ), \
    ( *( uint8_t* ) ( ( BUFFER ) + 7 ) = ( uint8_t ) ( ( ( ( uint64_t ) ( U64VALUE ) )       ) & 0xFF ) ), \
    ( ( LEN ) += sizeof( uint64_t ) ) )

#define U32_TO_BUFFER( BUFFER, U32VALUE, LEN ) ( \
    ( *( uint8_t* ) ( ( BUFFER )     ) = ( uint8_t ) ( ( ( ( uint32_t ) ( U32VALUE ) ) >> 24 ) & 0xFF ) ), \
    ( *( uint8_t* ) ( ( BUFFER ) + 1 ) = ( uint8_t ) ( ( ( ( uint32_t ) ( U32VALUE ) ) >> 16 ) & 0xFF ) ), \
    ( *( uint8_t* ) ( ( BUFFER ) + 2 ) = ( uint8_t ) ( ( ( ( uint32_t ) ( U32VALUE ) ) >>  8 ) & 0xFF ) ), \
    ( *( uint8_t* ) ( ( BUFFER ) + 3 ) = ( uint8_t ) ( ( ( ( uint32_t ) ( U32VALUE ) )       ) & 0xFF ) ), \
    ( ( LEN ) += sizeof( uint32_t ) ) )

#define U16_TO_BUFFER( BUFFER, U16VALUE, LEN )     ( \
    ( *( uint8_t* ) ( ( BUFFER )     ) = ( uint8_t ) ( ( ( ( uint16_t ) ( U16VALUE ) ) >>  8 ) & 0xFF ) ), \
    ( *( uint8_t* ) ( ( BUFFER ) + 1 ) = ( uint8_t ) ( ( ( ( uint16_t ) ( U16VALUE ) )       ) & 0xFF ) ), \
    ( ( LEN ) += sizeof( uint16_t ) ) )

#define U8_TO_BUFFER( BUFFER, U8VALUE, LEN) ( \
    ( *( uint8_t* ) ( ( BUFFER )     ) = ( uint8_t ) ( ( ( ( uint8_t  ) ( U8VALUE  ) )       ) & 0xFF ) ), \
    ( ( LEN ) += sizeof( uint8_t  ) ) )

#define ZB_LEBESWAP(ptr,len)                                \
    for(int i=0; i<(len>>1);i++){                           \
        unsigned char temp = ptr[len - i - 1];              \
        ptr[len - i - 1] = ptr[i];                          \
        ptr[i] = temp;                                      \
    }

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

typedef struct ts_HCIBuffer
{
    uint16_t u16MessageType;
    uint16_t u16MessageLength;
    uint8_t au8Payload[256];
} ts_HCIBuffer;

typedef struct ts_MsgBdbCommissionTouchlinkPayload
{
    te_MsgBdbCommissionTouchlinkRole eRole;
} ts_MsgBdbCommissionTouchlinkPayload;

typedef struct ts_MsgBdbCommissionFindbindPayload
{
    te_MsgBdbCommissionTouchlinkRole eRole;
} ts_MsgBdbCommissionFindbindPayload;

typedef struct ts_MsgBdbPreInstallCodePayload
{
    uint64_t u64DevAddr;           /**< The IEEE address of the pre-configured device. */
    uint8_t  au8UniqueLinkKey[16]; /**< The link key for the device. */
} ts_MsgBdbPreInstallCodePayload;

/******************************************************************************/
/***        local function prototypes                                       ***/
/******************************************************************************/

static void zbhci_task(void *pvParameters);

static uint8_t zbhci_CRC8Calculate(uint16_t u16Type, uint16_t u16Length, uint8_t *pu8Data);

static void zbhci_Tx(uint16_t u16Type, uint16_t u16Length, uint8_t *pu8Data);

static void zbhci_UnpackAcknowledgPayload(ts_MsgAckPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackBdbCommissionFormationRspPayload(ts_MsgBdbCommissionFormationRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackNetworkStateRspPayload(ts_MsgNetworkStateRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackDiscoveryNwkAddrRspPayload(ts_MsgDiscoveryNwkAddrRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackDiscoveryIEEEAddrRspPayload(ts_MsgDiscoveryIEEEAddrRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackDiscoveryNodeDescRspPayload(ts_MsgDiscoveryNodeDescRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackDiscoverySimpleDescRspPayload(ts_MsgDiscoverySimpleDescRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackDiscoveryMatchDescRspPayload(ts_MsgDiscoveryMatchDescRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackDiscoveryActiveEpRspPayload(ts_MsgDiscoveryActiveEpRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackBindingRspPayload(ts_MsgBindRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackUnbindingRspPayload(ts_MsgBindRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackMgmtLqiRspPayload(ts_MsgMgmtLqiRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackMgmtBindRspPayload(ts_MsgMgmtBindRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackMgmtLeaveRspPayload(ts_MsgMgmtLeaveRspPayload *psPayload, uint8_t *pu8Payload);

#if 0
static void zbhci_UnpackMgmtDirectJoinRspPayload();
#endif

static void zbhci_UnpackMgmtPermitJoinRspPayload(ts_MsgMgmtPermitJoinRspPayload *psPayload, uint8_t *pu8Payload);

#if 0
static void zbhci_UnpackMgmtNwkUpdateRspPayload();
#endif

static void zbhci_UnpackNodesJoinedGetRspPayload(ts_MsgNodesJoinedGetRspPayload *psPayload, uint8_t *pu8Payload);

#if 0
static void zbhci_UnpackNodesTogleTestRspPayload();
#endif

static void zbhci_UnpackTxRxPerformanceTestRspPayload(ts_MsgTxRxPerformceTestRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackNodesDevAnnceIndPayload(ts_MsgNodesDevAnnceRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackAfDataSendTestRspPayload(ts_MsgAfDataSendTestRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackLeaveIndicationPayload(ts_MsgLeaveIndicationPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclAttrReadRspPayload(ts_MsgZclAttrReadRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclAttrWriteRspPayload(ts_MsgZclAttrWriteRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclConfigReportRspPayload(ts_MsgZclConfigReportRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclReadReportCfgRspPayload(ts_MsgZclReadReportCfgRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclReportMsgRcvPayload(ts_MsgZclReportMsgRcvPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclGroupAddRspPayload(ts_MsgZclGroupAddRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclGroupViewRspPayload(ts_MsgZclGroupViewRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclGroupGetMembershipRspPayload(ts_MsgZclGroupGetMembershipRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclGroupRemoveRspPayload(ts_MsgZclGroupRemoveRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclIdentifyQueryRspPayload(ts_MsgZclIdentifyQueryRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclOnOffCmdRcvPayload(ts_MsgZclOnOffCmdRcvPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclSceneAddRspPayload(ts_MsgZclSceneAddRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclSceneViewRspPayload(ts_MsgZclSceneViewRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclSceneRemoveRspPayload(ts_MsgZclSceneRemoveRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclSceneRemoveAllRspPayload(ts_MsgZclSceneRemoveAllRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclSceneStoreRspPayload(ts_MsgZclSceneStoreRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackZclSceneGetMembershipRspPayload(ts_MsgZclSceneGetMenbershipRspPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackDataConfirmPayload(ts_MsgDataConfirmPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackMacAddrIndPayload(ts_MsgMacAddrIndPayload *psPayload, uint8_t *pu8Payload);

static void zbhci_UnpackNodeLeaveIndPayload(ts_MsgNodeLeaveIndPayload *psPayload, uint8_t *pu8Payload);

#if 0
static void zbhci_HandleAcknowledg(ts_MsgAckPayload *psPayload);

static void zbhci_HandleDiscoveryNwkAddrRsp();

static void zbhci_HandleDiscoveryIEEEAddrRsp();

static void zbhci_HandleDiscoveryNodeDescRsp();

static void zbhci_HandleDiscoverySimpleDescRsp();

static void zbhci_HandleDiscoveryMatchDescRsp();

static void zbhci_HandleDiscoveryActiveEpRsp();

static void zbhci_HandleBindingRsp();

static void zbhci_HandleUnbindingRsp();

static void zbhci_HandleMgmtLqiRsp();

static void zbhci_HandleMgmtBindRsp();

static void zbhci_HandleMgmtLeaveRsp();

static void zbhci_HandleMgmtDirectJoinRsp();

static void zbhci_HandleMgmtPermitJoinRsp();

static void zbhci_HandleMgmtNwkUpdateRsp();

static void zbhci_HandleNodesJoinedGetRsp();

static void zbhci_HandleNodesTogleTestRsp();

static void zbhci_HandleTxRxPerformanceTestRsp();

static void zbhci_HandleNodesDevAnnceInd();

static void zbhci_HandleAfDataSendTestRsp();

static void zbhci_HandleZclAttrReadRsp();

static void zbhci_HandleZclAttrWriteRsp();

static void zbhci_HandleZclConfigReportRsp();

static void zbhci_HandleZclReadReportCfgRsp();

static void zbhci_HandleZclReportMsgRcv();

static void zbhci_HandleZclGroupAddRsp();

static void zbhci_HandleZclGroupViewRsp();

static void zbhci_HandleZclGroupGetMembershipRsp();

static void zbhci_HandleZclGroupRemoveRsp();

static void zbhci_HandleZclIdentifyQueryRsp();

static void zbhci_HandleZclSceneAddRsp();

static void zbhci_HandleZclSceneViewRsp();

static void zbhci_HandleZclSceneRemoveRsp();

static void zbhci_HandleZclSceneRemoveAllRsp();

static void zbhci_HandleZclSceneStoreRsp();

static void zbhci_HandleZclSceneGetMembershipRsp();

static void zbhci_HandleDataConfirm();

static void zbhci_HandleMacAddrInd();

static void zbhci_HandleNodeLeaveInd();
#endif

static int32_t zbhci_CmdUnpack(uint8_t data, uint16_t *pu16Type, uint16_t *pu16Length, uint8_t *pu8Data);

static void zbhciCmdHandler(uint16_t u16MsgType, uint16_t u16MsgLen, uint8_t *pu8Payload, void *puPayload);

static uint8_t zcl_getDataTypeLen(uint8_t u8DataType);

static uint16_t zcl_getAttrSize(uint8_t u8DataType, uint8_t *pu8Data);

static bool zcl_analogDataType(uint8_t u8DataType);

static void zcl_getAttrData(uint8_t u8DataType, t_AttrData *psAttrData, uint8_t *pu8Data);

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        local variables                                                 ***/
/******************************************************************************/

static QueueHandle_t msg_queue;
static TaskHandle_t zhbci_handle;

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

void zbhci_Init(QueueHandle_t queue)
{
    msg_queue = queue;
    uart_init();
    xTaskCreate(zbhci_task, "zbhci_task", 8012, NULL, 12, &zhbci_handle);
}

void zbhci_Deinit(void)
{
    msg_queue = NULL;
    vTaskDelete(zhbci_handle);
    uart_deinit();
}


void zbhci_BdbCommissionFormation(void)
{
    zbhci_Tx(ZBHCI_CMD_BDB_COMMISSION_FORMATION, 0, NULL);
}


void zbhci_BdbCommissionSteer(void)
{
    zbhci_Tx(ZBHCI_CMD_BDB_COMMISSION_STEER, 0, NULL);
}


void zbhci_BdbCommissionTouchlink(te_MsgBdbCommissionTouchlinkRole eRole)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[1] = { 0 };

    U8_TO_BUFFER(&au8Payload[u16MsgLength], eRole, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_BDB_COMMISSION_TOUCHLINK, u16MsgLength, au8Payload);
}


void zbhci_BdbCommissionFindbind(te_MsgBdbCommissionFindbindRole eRole)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[1] = { 0 };

    U8_TO_BUFFER(&au8Payload[u16MsgLength], eRole, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_BDB_COMMISSION_FINDBIND, u16MsgLength, au8Payload);
}


void zbhci_BdbFactoryReset(void)
{
    zbhci_Tx(ZBHCI_CMD_BDB_FACTORY_RESET, 0, NULL);
}


void zbhci_BdbPreInstallCode(uint64_t u64DevAddr, uint8_t *pu8UniqueLinkKey)
{
    uint16_t u16MsgLength   = 0;
    uint8_t  au8Payload[24] = { 0 };
    uint16_t i              = 0;

    if (!pu8UniqueLinkKey) return ;

    U64_TO_BUFFER(&au8Payload[u16MsgLength], u64DevAddr, u16MsgLength);
    for (i = 0; i < 16; i++)
    {
        U8_TO_BUFFER(&au8Payload[u16MsgLength], pu8UniqueLinkKey[i], u16MsgLength);
    }

    zbhci_Tx(ZBHCI_CMD_BDB_PRE_INSTALL_CODE, u16MsgLength, au8Payload);
}


void zbhci_BdbChannelSet(uint8_t u8ChannelIdx)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[1] = { 0 };

    U8_TO_BUFFER(&au8Payload[u16MsgLength], u8ChannelIdx, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_BDB_CHANNEL_SET, u16MsgLength, au8Payload);
}


void zbhci_BdbDongleWorkingModeSet(te_MsgBdbDongleWorkingMode eMode)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[1] = { 0 };

    U8_TO_BUFFER(&au8Payload[u16MsgLength], eMode, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_BDB_DONGLE_WORKING_MODE_SET, u16MsgLength, au8Payload);
}


void zbhci_BdbNodeDelete(uint64_t u64DevAddr)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[8] = { 0 };

    U64_TO_BUFFER(&au8Payload[u16MsgLength], u64DevAddr, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_BDB_NODE_DELETE, u16MsgLength, au8Payload);
}


void zbhci_BdbTxPowerSet(te_BdbTxPowerLevel eLevel)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[1] = { 0 };

    U8_TO_BUFFER(&au8Payload[u16MsgLength], eLevel, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_BDB_TX_POWER_SET, u16MsgLength, au8Payload);
}


void zbhci_NetworkStateReq(void)
{
    zbhci_Tx(ZBHCI_CMD_NETWORK_STATE_REQ, 0, NULL);
}


void zbhci_DiscoveryNwkAddrReq(uint16_t u16DstAddr,
                               uint64_t u64IEEEAddr,
                               uint8_t  u8ReqType,
                               uint8_t  u8StartIdx)
{
    uint16_t u16MsgLength   = 0;
    uint8_t  au8Payload[12] = { 0 };

    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DstAddr,  u16MsgLength);
    U64_TO_BUFFER(&au8Payload[u16MsgLength], u64IEEEAddr, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8ReqType,   u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8StartIdx,  u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_DISCOVERY_NWK_ADDR_REQ, u16MsgLength, au8Payload);
}


void zbhci_DiscoveryIeeeAddrReq(uint16_t u16DstAddr,
                                uint16_t u16NwkAddrOfInterest,
                                uint8_t  u8ReqType,
                                uint8_t  u8StartIdx)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[6] = { 0 };

    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DstAddr,           u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16NwkAddrOfInterest, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8ReqType,            u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8StartIdx,           u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_DISCOVERY_IEEE_ADDR_REQ, u16MsgLength, au8Payload);
}


void zbhci_DiscoveryNodeDescReq(uint16_t u16DstAddr,
                                uint16_t u16NwkAddrOfInterest)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[4] = { 0 };

    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DstAddr,           u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16NwkAddrOfInterest, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_DISCOVERY_NODE_DESC_REQ, u16MsgLength, au8Payload);
}


void zbhci_DiscoverySimpleDescReq(uint16_t u16DstAddr,
                                  uint16_t u16NwkAddrOfInterest,
                                  uint8_t  u8Endpoint)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[5] = { 0 };

    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DstAddr,           u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16NwkAddrOfInterest, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Endpoint,           u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_DISCOVERY_SIMPLE_DESC_REQ, u16MsgLength, au8Payload);
}


void zbhci_DiscoveryMatchDescReq(uint16_t u16DstAddr,
                                 uint16_t u16NwkAddrOfInterest,
                                 uint16_t u16ProfileID,
                                 uint8_t  u8NumInClusters,
                                 uint8_t  u8NumOutClusters,
                                 uint16_t *pu16ClusterList)
{
    uint16_t u16MsgLength    = 0;
    /** @bug The array may be out of bounds */
    uint8_t  au8Payload[256] = { 0 };
    int32_t  i               = 0;

    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DstAddr,           u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16NwkAddrOfInterest, u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16ProfileID,         u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8NumInClusters,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8NumOutClusters,     u16MsgLength);
    for (i = 0; i < u8NumInClusters; i++)
    {
        U16_TO_BUFFER(&au8Payload[u16MsgLength], pu16ClusterList[i], u16MsgLength);
    }
    for (i = 0; i < u8NumOutClusters; i++)
    {
        U16_TO_BUFFER(&au8Payload[u16MsgLength], pu16ClusterList[u8NumInClusters + i], u16MsgLength);
    }

    zbhci_Tx(ZBHCI_CMD_DISCOVERY_MATCH_DESC_REQ, u16MsgLength, au8Payload);
}


void zbhci_DiscoveryActiveEpReq(uint16_t u16DstAddr,
                                uint16_t u16NwkAddrOfInterest)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[4] = { 0 };

    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DstAddr,           u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16NwkAddrOfInterest, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_DISCOVERY_ACTIVE_EP_REQ, u16MsgLength, au8Payload);
}


void zbhci_DiscoveryLeaveReq(uint64_t u64DevAddr,
                             uint8_t  u8Rejoin,
                             uint8_t  u8RemoveChildren)
{
    uint16_t u16MsgLength   = 0;
    uint8_t  au8Payload[10] = { 0 };

    U64_TO_BUFFER(&au8Payload[u16MsgLength], u64DevAddr,       u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Rejoin,         u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8RemoveChildren, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_DISCOVERY_LEAVE_REQ, u16MsgLength, au8Payload);
}


void zbhci_BindingReq(uint64_t   u64SrcIEEEAddr,
                      uint8_t    u8SrcEndpoint,
                      uint16_t   u16ClusterID,
                      uint8_t    u8DstAddrMode,
                      ts_DstAddr sDstAddr,
                      uint8_t    u8DstEndpoint)
{
    uint16_t u16MsgLength   = 0;
    uint8_t  au8Payload[21] = { 0 };

    U64_TO_BUFFER(&au8Payload[u16MsgLength], u64SrcIEEEAddr, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEndpoint,  u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16ClusterID,   u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode,  u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER(&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER(&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEndpoint, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_BINDING_REQ, u16MsgLength, au8Payload);
}


void zbhci_UnbindingReq(uint64_t   u64SrcIEEEAddr,
                        uint8_t    u8SrcEndpoint,
                        uint16_t   u16ClusterID,
                        uint8_t    u8DstAddrMode,
                        ts_DstAddr sDstAddr,
                        uint8_t    u8DstEndpoint)
{
    uint16_t u16MsgLength   = 0;
    uint8_t  au8Payload[21] = { 0 };

    U64_TO_BUFFER(&au8Payload[u16MsgLength], u64SrcIEEEAddr, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEndpoint,  u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16ClusterID,   u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode,  u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER(&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER(&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEndpoint, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_UNBINDING_REQ, u16MsgLength, au8Payload);
}


void zbhci_MgmtLqiReq(uint16_t u16DstAddr, uint8_t u8StartIdx)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[3] = { 0 };

    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DstAddr, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8StartIdx, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_MGMT_LQI_REQ, u16MsgLength, au8Payload);
}


void zbhci_MgmtBindReq(uint16_t u16DstAddr, uint8_t u8StartIdx)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[3] = { 0 };

    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DstAddr, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8StartIdx, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_MGMT_BIND_REQ, u16MsgLength, au8Payload);
}


void zbhci_MgmtLeaveReq(uint16_t u16DstAddr,
                        uint64_t u64DevAddr,
                        uint8_t  u8Rejoin,
                        uint8_t  u8RemoveChildren)
{
    uint16_t u16MsgLength   = 0;
    uint8_t  au8Payload[12] = { 0 };

    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DstAddr,       u16MsgLength);
    U64_TO_BUFFER(&au8Payload[u16MsgLength], u64DevAddr,       u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Rejoin,         u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8RemoveChildren, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_MGMT_LEAVE_REQ, u16MsgLength, au8Payload);
}


void zbhci_MgmtDirectJoinReq(void)
{
    // None
    zbhci_Tx(ZBHCI_CMD_MGMT_DIRECT_JOIN_REQ, 0, NULL);
}


void zbhci_MgmtPermitJoinReq(uint16_t u16DstAddr,
                             uint8_t  u8PermitDuration,
                             uint8_t  u8TCSignificance)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[4] = { 0 };

    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DstAddr,       u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8PermitDuration, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8TCSignificance, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_MGMT_PERMIT_JOIN_REQ, u16MsgLength, au8Payload);
}


void zbhci_MgmtNwkUpdateReq(uint16_t u16DstAddr,
                            uint16_t u16NwkManagerAddr,
                            uint32_t u32ScanChannels,
                            uint8_t  u8ScanDuration,
                            uint8_t  u8ScanCount,
                            uint8_t  u8NwkUpdateId)
{
    uint16_t u16MsgLength   = 0;
    uint8_t  au8Payload[11] = { 0 };

    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DstAddr,        u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16NwkManagerAddr, u16MsgLength);
    U32_TO_BUFFER(&au8Payload[u16MsgLength], u32ScanChannels,   u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8ScanDuration,    u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8ScanCount,       u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8NwkUpdateId,     u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_MGMT_NWK_UPDATE_REQ, u16MsgLength, au8Payload);
}


void zbhci_NodesJoinedGetReq(uint16_t u16StartIdx)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[2] = { 0 };

    U16_TO_BUFFER (&au8Payload[u16MsgLength], u16StartIdx, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_NODES_JOINED_GET_REQ, 2, au8Payload);
}


void zbhci_NodesTogleTestReq(uint8_t u8OnOff, uint8_t u8TimerInterval)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[2] = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8OnOff,         u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8TimerInterval, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_NODES_TOGLE_TEST_REQ, u16MsgLength, au8Payload);
}


void zbhci_TxRxPerformanceTestReq(uint16_t u16DstAddr,
                                  uint8_t  u8SrcEp,
                                  uint8_t  u8DstEp,
                                  uint16_t u16SendCnt,
                                  uint8_t  u8Interval,    //unit: 10ms
                                  uint8_t  u8TxPowerSet,
                                  uint8_t *pu8Payload)
{
    uint16_t u16MsgLength  = 0;
    uint8_t  au8Payload[14] = { 0 };

    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DstAddr,   u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16SendCnt,   u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Interval,   u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8TxPowerSet, u16MsgLength);
    memcpy(&au8Payload[u16MsgLength], pu8Payload, 6);
    u16MsgLength += 6;

    zbhci_Tx(ZBHCI_CMD_TXRX_PERFORMANCE_TEST_REQ, u16MsgLength, au8Payload);
}


void zbhci_AfDataSendTestReq(uint16_t u16DstAddr,
                             uint8_t  u8SrcEp,
                             uint8_t  u8DstEp,
                             uint16_t u16ClusterId,
                             uint16_t u16DataLen,
                             uint8_t *pu8Payload)
{
    uint16_t u16MsgLength    = 0;
    /** @bug The array may be out of bounds */
    uint8_t  au8Payload[256] = { 0 };

    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DstAddr,   u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16ClusterId, u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16DataLen,   u16MsgLength);
    memcpy(&au8Payload[u16MsgLength], pu8Payload, u16DataLen);
    u16MsgLength += u16DataLen;

    zbhci_Tx(ZBHCI_CMD_AF_DATA_SEND_TEST_REQ, u16MsgLength, au8Payload);
}


void zbhci_ZclAttrRead(uint8_t    u8DstAddrMode,
                       ts_DstAddr sDstAddr,
                       uint8_t    u8SrcEp,
                       uint8_t    u8DstEp,
                       uint8_t    u8Direction,
                       uint16_t   u16ClusterID,
                       uint8_t    u8AttrNum,
                       uint16_t  *pu16AttrList)
{
    uint16_t u16MsgLength    = 0;
    /** @bug The array may be out of bounds */
    uint8_t  au8Payload[256] = { 0 };
    int32_t  i               = 0;

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Direction,  u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16ClusterID, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8AttrNum,     u16MsgLength);
    for (i = 0; i < u8AttrNum; i++)
    {
        U16_TO_BUFFER(&au8Payload[u16MsgLength], pu16AttrList[i], u16MsgLength);
    }

    zbhci_Tx(ZBHCI_CMD_ZCL_ATTR_READ, u16MsgLength, au8Payload);
}


void zbhci_ZclAttrWrite(uint8_t     u8DstAddrMode,
                       ts_DstAddr   sDstAddr,
                       uint8_t      u8SrcEp,
                       uint8_t      u8DstEp,
                       uint8_t      u8Direction,
                       uint16_t     u16ClusterID,
                       uint8_t      u8AttrNum,
                       ts_AttrList *psAttrList)
{
    uint16_t u16MsgLength    = 0;
    /** @bug The array may be out of bounds */
    uint8_t  au8Payload[256] = { 0 };
    int32_t  i               = 0;
    uint8_t  dataLen         = 0;

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Direction,  u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16ClusterID, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8AttrNum,     u16MsgLength);
    for (i = 0; i < u8AttrNum; i++)
    {
        U16_TO_BUFFER(&au8Payload[u16MsgLength], psAttrList[i].u16AttrID, u16MsgLength);
        U8_TO_BUFFER(&au8Payload[u16MsgLength], psAttrList[i].u8DataType, u16MsgLength);
        dataLen = zcl_getAttrSize(psAttrList[i].u8DataType, psAttrList[i].uAttrData.au8AttrData);
        memcpy(&au8Payload[u16MsgLength], &psAttrList[i].uAttrData, dataLen);
        // if((psAttrList[i].u8DataType != ZCL_DATA_TYPE_LONG_CHAR_STR)  && \
        //    (psAttrList[i].u8DataType != ZCL_DATA_TYPE_LONG_OCTET_STR) && \
        //    (psAttrList[i].u8DataType != ZCL_DATA_TYPE_CHAR_STR)       && \
        //    (psAttrList[i].u8DataType != ZCL_DATA_TYPE_OCTET_STR)      && \
        //    (psAttrList[i].u8DataType != ZCL_DATA_TYPE_STRUCT))
        // {
        //     ZB_LEBESWAP(&au8Payload[u16MsgLength], dataLen);
        // }
        u16MsgLength += dataLen;
    }

    zbhci_Tx(ZBHCI_CMD_ZCL_ATTR_WRITE, u16MsgLength, au8Payload);
}


void zbhci_ZclConfigReport(uint8_t     u8DstAddrMode,
                           ts_DstAddr   sDstAddr,
                           uint8_t      u8SrcEp,
                           uint8_t      u8DstEp,
                           uint8_t      u8Direction,
                           uint16_t     u16ClusterID,
                           uint8_t      u8AttrNum,
                           ts_AttrList *psAttrList)
{
    // TODO
    zbhci_Tx(ZBHCI_CMD_ZCL_CONFIG_REPORT, 0, NULL);
}


void zbhci_ZclReadReportCfg(uint8_t    u8DstAddrMode,
                            ts_DstAddr sDstAddr,
                            uint8_t    u8SrcEp,
                            uint8_t    u8DstEp,
                            uint8_t    u8Direction,
                            uint16_t   u16ClusterID,
                            uint8_t    u8AttrNum,
                            uint16_t  *pu16AttrList)
{
    // TODO
    zbhci_Tx(ZBHCI_CMD_ZCL_CONFIG_REPORT, 0, NULL);
}


void zbhci_ZclLocalAttrWrite(uint8_t  u8Endpoint,
                             uint16_t u16ClusterId,
                             uint16_t u16AttrId,
                             uint8_t  u8DataLen,
                             uint8_t *pu8Data)
{
    uint16_t u16MsgLength    = 0;
    /** @bug The array may be out of bounds */
    uint8_t  au8Payload[256] = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Endpoint,   u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16ClusterId, u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16AttrId,    u16MsgLength);
    memcpy(&au8Payload[u16MsgLength], pu8Data, u8DataLen);
    u16MsgLength += u8DataLen;

    zbhci_Tx(ZBHCI_CMD_ZCL_LOCAL_ATTR_WRITE, u16MsgLength, au8Payload);
}


void zbhci_ZclSendReportCmd(uint8_t      u8DstAddrMode,
                            ts_DstAddr   sDstAddr,
                            uint8_t      u8SrcEp,
                            uint8_t      u8DstEp,
                            uint8_t      u8DisableDefaultRsp,
                            uint8_t      u8Direction,
                            uint16_t     u16ClusterID,
                            uint16_t     u16AttrID,
                            uint8_t      u8DataType,
                            uint8_t      u8DataLen,
                            uint8_t     *pu8Data)
{
    uint16_t u16MsgLength    = 0;
    /** @bug The array may be out of bounds */
    uint8_t  au8Payload[256] = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,             u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,             u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DisableDefaultRsp, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Direction,         u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16ClusterID,        u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16AttrID,           u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DataType,          u16MsgLength);
    memcpy(&au8Payload[u16MsgLength], pu8Data, u8DataLen);
    u16MsgLength += u8DataLen;

    zbhci_Tx(ZBHCI_CMD_ZCL_SEND_REPORT_CMD, u16MsgLength, au8Payload);
}

void zbhci_ZclBasicReset(uint8_t    u8DstAddrMode,
                         ts_DstAddr sDstAddr,
                         uint8_t    u8SrcEp,
                         uint8_t    u8DstEp)
{
    uint16_t u16MsgLength    = 0;
    /** @bug The array may be out of bounds */
    uint8_t  au8Payload[11]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_BASIC_RESET, u16MsgLength, au8Payload);
}


void zbhci_ZclGroupAdd(uint8_t    u8DstAddrMode,
                       ts_DstAddr sDstAddr,
                       uint8_t    u8SrcEp,
                       uint8_t    u8DstEp,
                       uint16_t   u16GroupId,
                       uint8_t   *pu8GroupName)
{
    uint16_t u16MsgLength    = 0;
    /** @bug The array may be out of bounds */
    uint8_t  au8Payload[256] = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,    u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,    u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16GroupId, u16MsgLength);
    memcpy(&au8Payload[u16MsgLength], pu8GroupName, strlen((const char *)pu8GroupName));
    u16MsgLength += strlen((const char *)pu8GroupName);

    zbhci_Tx(ZBHCI_CMD_ZCL_GROUP_ADD, u16MsgLength, au8Payload);
}


void zbhci_ZclGroupView(uint8_t    u8DstAddrMode,
                        ts_DstAddr sDstAddr,
                        uint8_t    u8SrcEp,
                        uint8_t    u8DstEp,
                        uint16_t   u16GroupId)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[13]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,    u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,    u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16GroupId, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_GROUP_VIEW, u16MsgLength, au8Payload);
}


void zbhci_ZclGroupGetMembership(uint8_t    u8DstAddrMode,
                                 ts_DstAddr sDstAddr,
                                 uint8_t    u8SrcEp,
                                 uint8_t    u8DstEp,
                                 uint8_t    u8GroupCount,
                                 uint16_t  *pu16GroupList)
{
    uint16_t u16MsgLength    = 0;
    /** @bug The array may be out of bounds */
    uint8_t  au8Payload[256] = { 0 };
    int32_t  i               = 0;

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8GroupCount, u16MsgLength);
    for (i = 0; i < u8GroupCount; i++)
    {
        U16_TO_BUFFER(&au8Payload[u16MsgLength], pu16GroupList[i], u16MsgLength);
    }

    zbhci_Tx(ZBHCI_CMD_ZCL_GROUP_GET_MEMBERSHIP, u16MsgLength, au8Payload);
}


void zbhci_ZclGroupRemove(uint8_t    u8DstAddrMode,
                          ts_DstAddr sDstAddr,
                          uint8_t    u8SrcEp,
                          uint8_t    u8DstEp,
                          uint16_t   u16GroupId)
{
    uint16_t u16MsgLength   = 0;
    uint8_t  au8Payload[13] = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,    u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,    u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16GroupId, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_GROUP_REMOVE, u16MsgLength, au8Payload);
}


void zbhci_ZclGroupRemoveAll(uint8_t    u8DstAddrMode,
                             ts_DstAddr sDstAddr,
                             uint8_t    u8SrcEp,
                             uint8_t    u8DstEp)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[11] = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,    u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,    u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_GROUP_REMOVE_ALL, u16MsgLength, au8Payload);
}


void zbhci_ZclGroupAddIfIdentify(uint8_t    u8DstAddrMode,
                                 ts_DstAddr sDstAddr,
                                 uint8_t    u8SrcEp,
                                 uint8_t    u8DstEp,
                                 uint16_t   u16GroupId,
                                 uint8_t   *pu8GroupName)
{
    uint16_t u16MsgLength    = 0;
    /** @bug The array may be out of bounds */
    uint8_t  au8Payload[256] = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,    u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,    u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16GroupId, u16MsgLength);
    memcpy(&au8Payload[u16MsgLength], pu8GroupName, strlen((const char *)pu8GroupName));
    u16MsgLength += strlen((const char *)pu8GroupName);

    zbhci_Tx(ZBHCI_CMD_ZCL_GROUP_ADD_IF_IDENTIFY, u16MsgLength, au8Payload);
}


void zbhci_ZclIdentifyQuery(uint8_t    u8DstAddrMode,
                            ts_DstAddr sDstAddr,
                            uint8_t    u8SrcEp,
                            uint8_t    u8DstEp,
                            uint16_t   u16IdentifyTime)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[13]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,         u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,         u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16IdentifyTime, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_IDENTIFY_QUERY, u16MsgLength, au8Payload);
}


void zbhci_ZclOnoffOn(uint8_t    u8DstAddrMode,
                      ts_DstAddr sDstAddr,
                      uint8_t    u8SrcEp,
                      uint8_t    u8DstEp)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[11]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_ONOFF_ON, u16MsgLength, au8Payload);
}


void zbhci_ZclOnoffOff(uint8_t    u8DstAddrMode,
                       ts_DstAddr sDstAddr,
                       uint8_t    u8SrcEp,
                       uint8_t    u8DstEp)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[11]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_ONOFF_OFF, u16MsgLength, au8Payload);
}


void zbhci_ZclOnoffToggle(uint8_t    u8DstAddrMode,
                          ts_DstAddr sDstAddr,
                          uint8_t    u8SrcEp,
                          uint8_t    u8DstEp)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[11]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_ONOFF_TOGGLE, u16MsgLength, au8Payload);
}


void zbhci_ZclLevelMove2level(uint8_t    u8DstAddrMode,
                              ts_DstAddr sDstAddr,
                              uint8_t    u8SrcEp,
                              uint8_t    u8DstEp,
                              uint8_t    u8Level,
                              uint16_t   u16TransTime)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[14]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Level,      u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16TransTime, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_LEVEL_MOVE2LEVEL, u16MsgLength, au8Payload);
}


void zbhci_ZclLevelMove(uint8_t    u8DstAddrMode,
                        ts_DstAddr sDstAddr,
                        uint8_t    u8SrcEp,
                        uint8_t    u8DstEp,
                        uint8_t    u8Mode,
                        uint8_t    u8Rate)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[13]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Mode,  u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Rate,  u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_LEVEL_MOVE, u16MsgLength, au8Payload);
}


void zbhci_ZclLevelStep(uint8_t    u8DstAddrMode,
                        ts_DstAddr sDstAddr,
                        uint8_t    u8SrcEp,
                        uint8_t    u8DstEp,
                        uint8_t    u8Mode,
                        uint8_t    u8StepSize,
                        uint16_t   u16TransTime)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[15]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Mode,       u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8StepSize,   u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u16TransTime, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_LEVEL_STEP, u16MsgLength, au8Payload);
}


void zbhci_ZclLevelStop(uint8_t    u8DstAddrMode,
                        ts_DstAddr sDstAddr,
                        uint8_t    u8SrcEp,
                        uint8_t    u8DstEp)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[11]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_LEVEL_STOP, u16MsgLength, au8Payload);
}


void zbhci_ZclLevelMove2levelWithonoff(uint8_t    u8DstAddrMode,
                                       ts_DstAddr sDstAddr,
                                       uint8_t    u8SrcEp,
                                       uint8_t    u8DstEp,
                                       uint8_t    u8Level,
                                       uint16_t   u16TransTime)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[14]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Level,      u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16TransTime, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_LEVEL_MOVE2LEVEL_WITHONOFF, u16MsgLength, au8Payload);
}


void zbhci_ZclLevelMoveWithonoff(uint8_t    u8DstAddrMode,
                                 ts_DstAddr sDstAddr,
                                 uint8_t    u8SrcEp,
                                 uint8_t    u8DstEp,
                                 uint8_t    u8Mode,
                                 uint8_t    u8Rate)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[13]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Mode,  u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Rate,  u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_LEVEL_MOVE_WITHONOFF, u16MsgLength, au8Payload);
}


void zbhci_ZclLevelStepWithonoff(uint8_t    u8DstAddrMode,
                                 ts_DstAddr sDstAddr,
                                 uint8_t    u8SrcEp,
                                 uint8_t    u8DstEp,
                                 uint8_t    u8Mode,
                                 uint8_t    u8StepSize,
                                 uint16_t   u16TransTime)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[15]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Mode,       u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8StepSize,   u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16TransTime, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_LEVEL_STEP_WITHONOFF, u16MsgLength, au8Payload);
}


void zbhci_ZclLevelStopWithonoff(uint8_t    u8DstAddrMode,
                                 ts_DstAddr sDstAddr,
                                 uint8_t    u8SrcEp,
                                 uint8_t    u8DstEp)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[12]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_LEVEL_STOP_WITHONOFF, u16MsgLength, au8Payload);
}


void zbhci_ZclSceneAdd(uint8_t    u8DstAddrMode,
                       ts_DstAddr sDstAddr,
                       uint8_t    u8SrcEp,
                       uint8_t    u8DstEp,
                       uint16_t   u16GroupId,
                       uint8_t    u8SceneId,
                       uint16_t   u16TransTime,
                       uint8_t    u8SceneNameLen,
                       uint8_t   *pu8SceneName,
                       uint8_t    u8ExtFieldLen,
                       uint8_t   *pu8ExtFieldSets)
{
    uint16_t u16MsgLength    = 0;
    /** @bug The array may be out of bounds */
    uint8_t  au8Payload[256] = { 0 };
    int32_t  i               = 0;

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,        u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,        u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16GroupId,     u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SceneId,      u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16TransTime,   u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SceneNameLen, u16MsgLength);
    for (i = 0; i < u8SceneNameLen; i++)
    {
        U8_TO_BUFFER (&au8Payload[u16MsgLength], pu8SceneName[i], u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8ExtFieldLen, u16MsgLength);
    for (i = 0; i < u8ExtFieldLen; i++)
    {
        U8_TO_BUFFER (&au8Payload[u16MsgLength], pu8ExtFieldSets[i], u16MsgLength);
    }

    zbhci_Tx(ZBHCI_CMD_ZCL_SCENE_ADD, u16MsgLength, au8Payload);
}


void zbhci_ZclSceneView(uint8_t    u8DstAddrMode,
                        ts_DstAddr sDstAddr,
                        uint8_t    u8SrcEp,
                        uint8_t    u8DstEp,
                        uint16_t   u16GroupId,
                        uint8_t    u8SceneId)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[14]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,        u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,        u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16GroupId,     u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SceneId,      u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_SCENE_VIEW, u16MsgLength, au8Payload);
}


void zbhci_ZclSceneRemove(uint8_t    u8DstAddrMode,
                          ts_DstAddr sDstAddr,
                          uint8_t    u8SrcEp,
                          uint8_t    u8DstEp,
                          uint16_t   u16GroupId,
                          uint8_t    u8SceneId)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[14]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,        u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,        u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16GroupId,     u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SceneId,      u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_SCENE_REMOVE, u16MsgLength, au8Payload);
}


void zbhci_ZclSceneRemoveAll(uint8_t    u8DstAddrMode,
                             ts_DstAddr sDstAddr,
                             uint8_t    u8SrcEp,
                             uint8_t    u8DstEp,
                             uint16_t   u16GroupId)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[13]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,        u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,        u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16GroupId,     u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_SCENE_REMOVE_ALL, u16MsgLength, au8Payload);
}


void zbhci_ZclSceneStore(uint8_t    u8DstAddrMode,
                         ts_DstAddr sDstAddr,
                         uint8_t    u8SrcEp,
                         uint8_t    u8DstEp,
                         uint16_t   u16GroupId,
                         uint8_t    u8SceneId)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[14]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,    u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,    u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16GroupId, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SceneId,  u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_SCENE_STORE, 0, NULL);
}


void zbhci_ZclSceneRecall(uint8_t    u8DstAddrMode,
                          ts_DstAddr sDstAddr,
                          uint8_t    u8SrcEp,
                          uint8_t    u8DstEp,
                          uint16_t   u16GroupId,
                          uint8_t    u8SceneId)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[14]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,    u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,    u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16GroupId, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SceneId,  u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_SCENE_RECALL, u16MsgLength, au8Payload);
}


void zbhci_ZclSceneGetMembership(uint8_t    u8DstAddrMode,
                                 ts_DstAddr sDstAddr,
                                 uint8_t    u8SrcEp,
                                 uint8_t    u8DstEp,
                                 uint16_t   u16GroupId)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[13]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,    u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,    u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16GroupId, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_SCENE_GET_MEMBERSHIP, u16MsgLength, au8Payload);
}


void zbhci_ZclColorMove2hue(uint8_t    u8DstAddrMode,
                            ts_DstAddr sDstAddr,
                            uint8_t    u8SrcEp,
                            uint8_t    u8DstEp,
                            uint8_t    u8Hue,
                            uint8_t    u8Direction,
                            uint16_t   u16TransTime)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[15]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Hue,        u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Direction,  u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16TransTime, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_COLOR_MOVE2HUE, u16MsgLength, au8Payload);
}


void zbhci_ZclColorMove2Color(uint8_t    u8DstAddrMode,
                              ts_DstAddr sDstAddr,
                              uint8_t    u8SrcEp,
                              uint8_t    u8DstEp,
                              uint16_t   u16ColorX,
                              uint16_t   u16ColorY,
                              uint16_t   u16TransTime)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[17]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16ColorX,    u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16ColorY,    u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16TransTime, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_COLOR_MOVE2COLOR, u16MsgLength, au8Payload);
}


void zbhci_ZclColorMove2sat(uint8_t    u8DstAddrMode,
                            ts_DstAddr sDstAddr,
                            uint8_t    u8SrcEp,
                            uint8_t    u8DstEp,
                            uint8_t    u8Saturation,
                            uint16_t   u16TransTime)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[14]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Saturation, u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16TransTime, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_COLOR_MOVE2SAT, u16MsgLength, au8Payload);
}


void zbhci_ZclColorMove2temp(uint8_t    u8DstAddrMode,
                             ts_DstAddr sDstAddr,
                             uint8_t    u8SrcEp,
                             uint8_t    u8DstEp,
                             uint16_t   u16ColorTemperature,
                             uint16_t   u16TransTime)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[15]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,             u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,             u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16ColorTemperature, u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16TransTime,        u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_COLOR_MOVE2TEMP, u16MsgLength, au8Payload);
}


void zbhci_ZclOtaImageNotify(uint8_t    u8DstAddrMode,
                             ts_DstAddr sDstAddr,
                             uint8_t    u8SrcEp,
                             uint8_t    u8DstEp,
                             uint8_t    u8PayloadType,
                             uint8_t    u8QueryJitter)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[13]  = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,       u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,       u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8PayloadType, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8QueryJitter, u16MsgLength);

    zbhci_Tx(ZBHCI_CMD_ZCL_OTA_IMAGE_NOTIFY, u16MsgLength, au8Payload);
}


void zbhci_AfRawDataSend(uint8_t    u8DstAddrMode,
                         ts_DstAddr sDstAddr,
                         uint8_t    u8SrcEp,
                         uint8_t    u8DstEp,
                         uint16_t   u16ClusterId,
                         uint16_t   u16ProfileId,
                         uint8_t    u8TxOptions,
                         uint8_t    u8Radius,
                         uint8_t    u8Datalen,
                         uint8_t   *pu8Data)
{
    uint16_t u16MsgLength    = 0;
    uint8_t  au8Payload[256] = { 0 };

    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstAddrMode, u16MsgLength);
    if (u8DstAddrMode == 0x01 || u8DstAddrMode == 0x02)
    {
        U16_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u16DstAddr, u16MsgLength);
    }
    else if (u8DstAddrMode == 0x03)
    {
        U64_TO_BUFFER (&au8Payload[u16MsgLength], sDstAddr.u64DstAddr, u16MsgLength);
    }
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8SrcEp,      u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8DstEp,      u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16ClusterId, u16MsgLength);
    U16_TO_BUFFER(&au8Payload[u16MsgLength], u16ProfileId, u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8TxOptions,  u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Radius,     u16MsgLength);
    U8_TO_BUFFER (&au8Payload[u16MsgLength], u8Datalen,    u16MsgLength);
    memcpy(&au8Payload[u16MsgLength], pu8Data, u8Datalen);
    u16MsgLength += u8Datalen;

    zbhci_Tx(ZBHCI_CMD_ZCL_OTA_IMAGE_NOTIFY, u16MsgLength, au8Payload);
}

/******************************************************************************/
/***        local functions                                                 ***/
/******************************************************************************/

static void zbhci_task(void * pvParameters)
{
    // uint16_t u16MsgType      = 0;
    // uint16_t u16MsgLength       = 0;
    uint8_t  au8Payload[64]  = { 0 };
    ts_HciMsg sHciMsg;
    uint8_t  recvdata[64]    = { 0 };
    size_t size = 0;

    for( ;; )
    {
        if (uart_recv(recvdata, &size))
        {
            for (size_t i = 0; i < size; i++)
            {
                if (!zbhci_CmdUnpack(recvdata[i], &sHciMsg.u16MsgType, &sHciMsg.u16MsgLength, au8Payload))
                {
                    zbhciCmdHandler(sHciMsg.u16MsgType, sHciMsg.u16MsgLength, au8Payload, &sHciMsg.uPayload);
                    xQueueSend(msg_queue, &sHciMsg, portMAX_DELAY);
                }
            }

        }
        else
        {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }

    vTaskDelete(NULL);
}


static uint8_t zbhci_CRC8Calculate(uint16_t u16Type, uint16_t u16Length, uint8_t *pu8Data)
{
    int32_t n = 0;
    uint8_t crc8 = 0;

    crc8  = (u16Type   >> 0) & 0xff;
    crc8 ^= (u16Type   >> 8) & 0xff;
    crc8 ^= (u16Length >> 0) & 0xff;
    crc8 ^= (u16Length >> 8) & 0xff;

    for(n = 0; n < u16Length; n++)
    {
        crc8 ^= pu8Data[n];
    }

    return crc8;
}


static void zbhci_Tx(uint16_t u16Type, uint16_t u16Length, uint8_t *pu8Data)
{
    int32_t n = 0;
    uint8_t uartTxBuf[256] = { 0 };
    uint8_t crc8 = zbhci_CRC8Calculate(u16Type, u16Length, pu8Data);

    uint8_t *p = uartTxBuf;
    *p++ = 0x55;
    *p++ = (u16Type >> 8) & 0xff;
    *p++ = (u16Type >> 0) & 0xff;
    *p++ = (u16Length >> 8) & 0xff;
    *p++ = (u16Length >> 0) & 0xff;
    *p++ = crc8;
    for(n = 0; n < u16Length; n++)
    {
        *p++ = pu8Data[n];
    }
    *p++ = 0xAA;

    uart_send(uartTxBuf, p - uartTxBuf);
}

static int32_t zbhci_CmdUnpack(uint8_t data, uint16_t *pu16Type, uint16_t *pu16Length, uint8_t *pu8Data)
{
    static uint16_t u16Bytes = 0;
    static uint8_t status = 0;
    static uint8_t crc8 = 0;

    // printf("recv: 0x%x\n", data);
    switch (status)
    {
        case 0:
            if (data == 0x55)
            {
                u16Bytes = 0;
                status++;
            }
        break;

        case 1:
            *pu16Type = data << 8 & 0xFF00;
            status++;
        break;

        case 2:
            *pu16Type |= data & 0x00FF;
            status++;
        break;

        case 3:
            *pu16Length = data << 8 & 0xFF00;
            status++;
        break;

        case 4:
            *pu16Length |= data & 0x00FF;
            status++;
        break;

        case 5:
            crc8 = data;
            status++;
        break;

        case 6:
            if (u16Bytes < *pu16Length)
                pu8Data[u16Bytes++] = data;
            if (u16Bytes == *pu16Length &&data == 0xAA)
            {
                if (crc8 == zbhci_CRC8Calculate(*pu16Type, *pu16Length, pu8Data))
                {
                    status = 0;
                    return 0;
                }
                else
                {
                    log_w("error\n");
                    status = 0;
                }
            }
        break;

        default:
        break;
    }
    return -1;
}


static void zbhciCmdHandler(uint16_t u16MsgType, uint16_t u16MsgLen, uint8_t *pu8Payload, void *psPayload)
{
    if (!pu8Payload || !psPayload) return ;

    switch (u16MsgType)
    {
        case ZBHCI_CMD_ACKNOWLEDGE:
        {
            zbhci_UnpackAcknowledgPayload((ts_MsgAckPayload *)psPayload, pu8Payload);
            displayAcknowledg((ts_MsgAckPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_BDB_COMMISSION_FORMATION_RSP:
        {
            zbhci_UnpackBdbCommissionFormationRspPayload((ts_MsgBdbCommissionFormationRspPayload *)psPayload, pu8Payload);
            displayBdbCommissionFormationRsp((ts_MsgBdbCommissionFormationRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_NETWORK_STATE_RSP:
        case ZBHCI_CMD_NETWORK_STATE_REPORT:
        {
            zbhci_UnpackNetworkStateRspPayload((ts_MsgNetworkStateRspPayload *)psPayload, pu8Payload);
            displayNetworkStateRsp((ts_MsgNetworkStateRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_DISCOVERY_NWK_ADDR_RSP:
        {
            zbhci_UnpackDiscoveryNwkAddrRspPayload((ts_MsgDiscoveryNwkAddrRspPayload *)psPayload, pu8Payload);
            displayDiscoveryNwkAddrRsp((ts_MsgDiscoveryNwkAddrRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_DISCOVERY_IEEE_ADDR_RSP:
        {
            zbhci_UnpackDiscoveryIEEEAddrRspPayload((ts_MsgDiscoveryIEEEAddrRspPayload *)psPayload, pu8Payload);
            displayDiscoveryIEEEAddrRsp((ts_MsgDiscoveryIEEEAddrRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_DISCOVERY_NODE_DESC_RSP:
        {
            zbhci_UnpackDiscoveryNodeDescRspPayload((ts_MsgDiscoveryNodeDescRspPayload *)psPayload, pu8Payload);
            displayDiscoveryNodeDescRsp((ts_MsgDiscoveryNodeDescRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_DISCOVERY_SIMPLE_DESC_RSP:
        {
            zbhci_UnpackDiscoverySimpleDescRspPayload((ts_MsgDiscoverySimpleDescRspPayload *)psPayload, pu8Payload);
            displayDiscoverySimpleDescRsp((ts_MsgDiscoverySimpleDescRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_DISCOVERY_MATCH_DESC_RSP:
        {
            zbhci_UnpackDiscoveryMatchDescRspPayload((ts_MsgDiscoveryMatchDescRspPayload *)psPayload, pu8Payload);
            displayDiscoveryMatchDescRsp((ts_MsgDiscoveryMatchDescRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_DISCOVERY_ACTIVE_EP_RSP:
        {
            zbhci_UnpackDiscoveryActiveEpRspPayload((ts_MsgDiscoveryActiveEpRspPayload *)psPayload, pu8Payload);
            displayDiscoveryActiveEpRsp((ts_MsgDiscoveryActiveEpRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_BINDING_RSP:
        {
            zbhci_UnpackBindingRspPayload((ts_MsgBindRspPayload *)psPayload, pu8Payload);
            displayBindingRsp((ts_MsgBindRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_UNBINDING_RSP:
        {
            zbhci_UnpackUnbindingRspPayload((ts_MsgUnbindRspPayload *)psPayload, pu8Payload);
            displayUnbindingRsp((ts_MsgUnbindRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_MGMT_LQI_RSP:
        {
            zbhci_UnpackMgmtLqiRspPayload((ts_MsgMgmtLqiRspPayload *)psPayload, pu8Payload);
            displayMgmtLqiRsp((ts_MsgMgmtLqiRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_MGMT_BIND_RSP:
        {
            zbhci_UnpackMgmtBindRspPayload((ts_MsgMgmtBindRspPayload *)psPayload, pu8Payload);
            displayMgmtBindRsp((ts_MsgMgmtBindRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_MGMT_LEAVE_RSP:
        {
            zbhci_UnpackMgmtLeaveRspPayload((ts_MsgMgmtLeaveRspPayload *)psPayload, pu8Payload);
            displayMgmtLeaveRsp((ts_MsgMgmtLeaveRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_MGMT_DIRECT_JOIN_RSP:
        {
            /** None */
            // zbhci_UnpackMgmtDirectJoinRspPayload();
        }
        break;

        case ZBHCI_CMD_MGMT_PERMIT_JOIN_RSP:
        {
            zbhci_UnpackMgmtPermitJoinRspPayload((ts_MsgMgmtPermitJoinRspPayload *)psPayload, pu8Payload);
            displayMgmtPermitJoinRsp((ts_MsgMgmtPermitJoinRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_MGMT_NWK_UPDATE_RSP:
        {
            /** None */
            // zbhci_UnpackMgmtNwkUpdateRspPayload();
        }
        break;

        case ZBHCI_CMD_NODES_JOINED_GET_RSP:
        {
            zbhci_UnpackNodesJoinedGetRspPayload((ts_MsgNodesJoinedGetRspPayload *)psPayload, pu8Payload);
            displayNodesJoinedGetRsp((ts_MsgNodesJoinedGetRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_NODES_TOGLE_TEST_RSP:
        {
            /** None */
            // zbhci_UnpackNodesTogleTestRspPayload();
        }
        break;

        case ZBHCI_CMD_TXRX_PERFORMANCE_TEST_RSP:
        {
            zbhci_UnpackTxRxPerformanceTestRspPayload((ts_MsgTxRxPerformceTestRspPayload *)psPayload, pu8Payload);
            displayTxRxPerformanceTestRsp((ts_MsgTxRxPerformceTestRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_NODES_DEV_ANNCE_IND:
        {
            zbhci_UnpackNodesDevAnnceIndPayload((ts_MsgNodesDevAnnceRspPayload *)psPayload, pu8Payload);
            displayNodesDevAnnceInd((ts_MsgNodesDevAnnceRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_AF_DATA_SEND_TEST_RSP:
        {
            zbhci_UnpackAfDataSendTestRspPayload((ts_MsgAfDataSendTestRspPayload *)psPayload, pu8Payload);
            displayAfDataSendTestRsp((ts_MsgAfDataSendTestRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_LEAVE_INDICATION:
        {
            zbhci_UnpackLeaveIndicationPayload((ts_MsgLeaveIndicationPayload *)psPayload, pu8Payload);
            displayLeaveIndication((ts_MsgLeaveIndicationPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_ATTR_READ_RSP:
        {
            zbhci_UnpackZclAttrReadRspPayload((ts_MsgZclAttrReadRspPayload *)psPayload, pu8Payload);
            displayZclAttrReadRsp((ts_MsgZclAttrReadRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_ATTR_WRITE_RSP:
        {
            zbhci_UnpackZclAttrWriteRspPayload((ts_MsgZclAttrWriteRspPayload *)psPayload, pu8Payload);
            displayZclAttrWriteRsp((ts_MsgZclAttrWriteRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_CONFIG_REPORT_RSP:
        {
            zbhci_UnpackZclConfigReportRspPayload((ts_MsgZclConfigReportRspPayload *)psPayload, pu8Payload);
            displayZclConfigReportRsp((ts_MsgZclConfigReportRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_READ_REPORT_CFG_RSP:
        {
            zbhci_UnpackZclReadReportCfgRspPayload((ts_MsgZclReadReportCfgRspPayload *)psPayload, pu8Payload);
            displayZclReadReportCfgRsp((ts_MsgZclReadReportCfgRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_REPORT_MSG_RCV:
        {
            zbhci_UnpackZclReportMsgRcvPayload((ts_MsgZclReportMsgRcvPayload *)psPayload, pu8Payload);
            displayZclReportMsgRcv((ts_MsgZclReportMsgRcvPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_GROUP_ADD_RSP:
        {
            zbhci_UnpackZclGroupAddRspPayload((ts_MsgZclGroupAddRspPayload *)psPayload, pu8Payload);
            displayZclGroupAddRsp((ts_MsgZclGroupAddRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_GROUP_VIEW_RSP:
        {
            zbhci_UnpackZclGroupViewRspPayload((ts_MsgZclGroupViewRspPayload *)psPayload, pu8Payload);
            displayZclGroupViewRsp((ts_MsgZclGroupViewRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_GROUP_GET_MEMBERSHIP_RSP:
        {
            zbhci_UnpackZclGroupGetMembershipRspPayload((ts_MsgZclGroupGetMembershipRspPayload *)psPayload, pu8Payload);
            displayZclGroupGetMembershipRsp((ts_MsgZclGroupGetMembershipRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_GROUP_REMOVE_RSP:
        {
            zbhci_UnpackZclGroupRemoveRspPayload((ts_MsgZclGroupRemoveRspPayload *)psPayload, pu8Payload);
            displayZclGroupRemoveRsp((ts_MsgZclGroupRemoveRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_IDENTIFY_QUERY_RSP:
        {
            zbhci_UnpackZclIdentifyQueryRspPayload((ts_MsgZclIdentifyQueryRspPayload *)psPayload, pu8Payload);
            displayZclIdentifyQueryRsp((ts_MsgZclIdentifyQueryRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_ONOFF_CMD_RCV:
        {
            zbhci_UnpackZclOnOffCmdRcvPayload((ts_MsgZclOnOffCmdRcvPayload *)psPayload, pu8Payload);
            displayZclOnOffCmdRcv((ts_MsgZclOnOffCmdRcvPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_SCENE_ADD_RSP:
        {
            zbhci_UnpackZclSceneAddRspPayload((ts_MsgZclSceneAddRspPayload *)psPayload, pu8Payload);
            displayZclSceneAddRsp((ts_MsgZclSceneAddRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_SCENE_VIEW_RSP:
        {
            zbhci_UnpackZclSceneViewRspPayload((ts_MsgZclSceneViewRspPayload *)psPayload, pu8Payload);
            displayZclSceneViewRsp((ts_MsgZclSceneViewRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_SCENE_REMOVE_RSP:
        {
            zbhci_UnpackZclSceneRemoveRspPayload((ts_MsgZclSceneRemoveRspPayload *)psPayload, pu8Payload);
            displayZclSceneRemoveRsp((ts_MsgZclSceneRemoveRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_SCENE_REMOVE_ALL_RSP:
        {
            zbhci_UnpackZclSceneRemoveAllRspPayload((ts_MsgZclSceneRemoveAllRspPayload *)psPayload, pu8Payload);
            displayZclSceneRemoveAllRsp((ts_MsgZclSceneRemoveAllRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_SCENE_STORE_RSP:
        {
            zbhci_UnpackZclSceneStoreRspPayload((ts_MsgZclSceneStoreRspPayload *)psPayload, pu8Payload);
            displayZclSceneStoreRsp((ts_MsgZclSceneStoreRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_ZCL_SCENE_GET_MEMBERSHIP_RSP:
        {
            zbhci_UnpackZclSceneGetMembershipRspPayload((ts_MsgZclSceneGetMenbershipRspPayload *)psPayload, pu8Payload);
            displayZclSceneGetMembershipRsp((ts_MsgZclSceneGetMenbershipRspPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_DATA_CONFIRM:
        {
            zbhci_UnpackDataConfirmPayload((ts_MsgDataConfirmPayload *)psPayload, pu8Payload);
            displayDataConfirm((ts_MsgDataConfirmPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_MAC_ADDR_IND:
        {
            zbhci_UnpackMacAddrIndPayload((ts_MsgMacAddrIndPayload *)psPayload, pu8Payload);
            displayMacAddrInd((ts_MsgMacAddrIndPayload *)psPayload);
        }
        break;

        case ZBHCI_CMD_NODE_LEAVE_IND:
        {
            zbhci_UnpackNodeLeaveIndPayload((ts_MsgNodeLeaveIndPayload *)psPayload, pu8Payload);
            displayNodeLeaveInd((ts_MsgNodeLeaveIndPayload *)psPayload);
        }
        break;

        default:
            printf("other cmd type\n");
        break;

    }
}


static void zbhci_UnpackAcknowledgPayload(ts_MsgAckPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgAckPayload));

    psPayload->u16MsgType = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->eStatus    = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8Reserved = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackBdbCommissionFormationRspPayload(ts_MsgBdbCommissionFormationRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgBdbCommissionFormationRspPayload));

    psPayload->u8Status = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}

static void zbhci_UnpackNetworkStateRspPayload(ts_MsgNetworkStateRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgNetworkStateRspPayload));

    psPayload->u16NwkAddr  = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u64IeeeAddr = BUFFER_TO_U64_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u16PanId    = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u64extPanId = BUFFER_TO_U64_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8Channel   = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackDiscoveryNwkAddrRspPayload(ts_MsgDiscoveryNwkAddrRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgDiscoveryNwkAddrRspPayload));

    psPayload->u8SeqNum      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8Status      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u64IEEEAddr   = BUFFER_TO_U64_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u16NwkAddr    = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8NumAssocDev = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8StartIdx    = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    for (size_t n = 0; n < psPayload->u8NumAssocDev; n++)
    {
        psPayload->au16AssocDevList[n] = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    }

}

static void zbhci_UnpackDiscoveryIEEEAddrRspPayload(ts_MsgDiscoveryIEEEAddrRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgDiscoveryIEEEAddrRspPayload));

    psPayload->u8SeqNum    = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8Status    = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u64IEEEAddr = BUFFER_TO_U64_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u16NwkAddr  = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8NumAssocDev = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8StartIdx    = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    for (size_t n = 0; n < psPayload->u8NumAssocDev; n++)
    {
        psPayload->au16AssocDevList[n] = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    }
}


static void zbhci_UnpackDiscoveryNodeDescRspPayload(ts_MsgDiscoveryNodeDescRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgDiscoveryNodeDescRspPayload));

    psPayload->u8SeqNum             = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8Status             = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16NwkAddrOfInterest = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    // TODO
    // memcpy(psPayload->au8NodeDesc, &pu8Payload[u16Offset], u16MsgLen - (u16Offset + 1));
}


static void zbhci_UnpackDiscoverySimpleDescRspPayload(ts_MsgDiscoverySimpleDescRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgDiscoverySimpleDescRspPayload));

    psPayload->u8Status             = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8SeqNum             = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16NwkAddrOfInterest = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8Length             = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    memcpy(psPayload->au8SimpleDesc, &pu8Payload[u16Offset], psPayload->u8Length);
}


static void zbhci_UnpackDiscoveryMatchDescRspPayload(ts_MsgDiscoveryMatchDescRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgDiscoveryMatchDescRspPayload));

    psPayload->u8Status             = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8SeqNum             = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16NwkAddrOfInterest = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8MatchLen           = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    memcpy(psPayload->au8MatchList, &pu8Payload[u16Offset], psPayload->u8MatchLen);
}


static void zbhci_UnpackDiscoveryActiveEpRspPayload(ts_MsgDiscoveryActiveEpRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgDiscoveryActiveEpRspPayload));

    psPayload->u8Status             = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8SeqNum             = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16NwkAddrOfInterest = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8ActiveEpCount      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    memcpy(psPayload->au8EpList, &pu8Payload[u16Offset], psPayload->u8ActiveEpCount);
}


static void zbhci_UnpackBindingRspPayload(ts_MsgBindRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgBindRspPayload));

    psPayload->u8SeqNum = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8Status = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackUnbindingRspPayload(ts_MsgBindRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgUnbindRspPayload));

    psPayload->u8SeqNum = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8Status = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackMgmtLqiRspPayload(ts_MsgMgmtLqiRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;
    uint8_t u8Temp = 0;

    bzero(psPayload, sizeof(ts_MsgMgmtLqiRspPayload));

    psPayload->u8SeqNum               = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8Status               = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8NeighborTabEntries   = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8StartIdx             = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8NeighborTabListCount = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    for (size_t n = 0; n < psPayload->u8NeighborTabListCount; n++)
    {
        psPayload->asNeighborTable[n].ext_pan_id   = BUFFER_TO_U64_OFFSET(pu8Payload, u16Offset, u16Offset);
        psPayload->asNeighborTable[n].ext_addr     = BUFFER_TO_U64_OFFSET(pu8Payload, u16Offset, u16Offset);
        psPayload->asNeighborTable[n].network_addr = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
        u8Temp = BUFFER_TO_U8_OFFSET(pu8Payload, u16Offset, u16Offset);
        psPayload->asNeighborTable[n].deviceType   = u8Temp & 0b00000011;
        psPayload->asNeighborTable[n].rxOnWhenIdle = (uint8_t)((u8Temp & 0b00001100) >> 2);
        psPayload->asNeighborTable[n].relationship = (uint8_t)((u8Temp & 0b01110000) >> 4);
        psPayload->asNeighborTable[n].reserved1    = (uint8_t)((u8Temp & 0b10000000) >> 7);
        u8Temp = BUFFER_TO_U8_OFFSET(pu8Payload, u16Offset, u16Offset);
        psPayload->asNeighborTable[n].permitJoining = u8Temp & 0b00000011;
        psPayload->asNeighborTable[n].reserved2     = (uint8_t)((u8Temp & 0b11111100) >> 2);
        psPayload->asNeighborTable[n].depth         = BUFFER_TO_U8_OFFSET(pu8Payload, u16Offset, u16Offset);
        psPayload->asNeighborTable[n].lqi           = BUFFER_TO_U8_OFFSET(pu8Payload, u16Offset, u16Offset);
    }
}


static void zbhci_UnpackMgmtBindRspPayload(ts_MsgMgmtBindRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgMgmtBindRspPayload));

    psPayload->u8SeqNum              = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8Status              = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8BindingTabEntries   = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8StartIdx            = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8BindingTabListCount = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    for (size_t n = 0; n < psPayload->u8BindingTabListCount; n++)
    {
        psPayload->asBindingTabList[n].u64SrcAddr = BUFFER_TO_U64_OFFSET(pu8Payload, u16Offset, u16Offset);
        psPayload->asBindingTabList[n].u8SrcEndpoint = BUFFER_TO_U8_OFFSET(pu8Payload, u16Offset, u16Offset);
        psPayload->asBindingTabList[n].u16ClusterId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
        psPayload->asBindingTabList[n].u8DstAddrMode = BUFFER_TO_U8_OFFSET(pu8Payload, u16Offset, u16Offset);
        if (psPayload->asBindingTabList[n].u8DstAddrMode == 0x01)
        {
            psPayload->asBindingTabList[n].uDstAddr.u16DstGroupAddr = BUFFER_TO_U8_OFFSET(pu8Payload, u16Offset, u16Offset);
        }
        else if(psPayload->asBindingTabList[n].u8DstAddrMode == 0x03)
        {
            psPayload->asBindingTabList[n].uDstAddr.sDstExtAddr.u64DstExtAddr = BUFFER_TO_U64_OFFSET(pu8Payload, u16Offset, u16Offset);
            psPayload->asBindingTabList[n].uDstAddr.sDstExtAddr.u8DstEndpoint = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
        }
    }
}


static void zbhci_UnpackMgmtLeaveRspPayload(ts_MsgMgmtLeaveRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgMgmtLeaveRspPayload));

    psPayload->u8SeqNum = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8Status = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}


#if 0
static void zbhci_UnpackMgmtDirectJoinRspPayload()
{
    /** None */
}
#endif


static void zbhci_UnpackMgmtPermitJoinRspPayload(ts_MsgMgmtPermitJoinRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgMgmtPermitJoinRspPayload));

    psPayload->u8SeqNum = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8Status = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}

#if 0
static void zbhci_UnpackMgmtNwkUpdateRspPayload()
{
    /** None */
}
#endif

static void zbhci_UnpackNodesJoinedGetRspPayload(ts_MsgNodesJoinedGetRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgNodesJoinedGetRspPayload));

    psPayload->u16TotalCnt = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u16StartIdx = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8ListCount = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8Status    = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    for (size_t n = 0; n < psPayload->u8ListCount; n++)
    {
        psPayload->au64MacAddrList[n]   = BUFFER_TO_U64_OFFSET(pu8Payload, u16Offset, u16Offset);
        psPayload->au16ShortAddrList[n] = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    }
}


#if 0
static void zbhci_UnpackNodesTogleTestRspPayload()
{
    /** None */
}
#endif


static void zbhci_UnpackTxRxPerformanceTestRspPayload(ts_MsgTxRxPerformceTestRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgTxRxPerformceTestRspPayload));

    psPayload->u16DstAddr = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u16SendCnt = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u16AckCnt = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackNodesDevAnnceIndPayload(ts_MsgNodesDevAnnceRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgNodesDevAnnceRspPayload));

    psPayload->u16NwkAddr   = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u64IEEEAddr  = BUFFER_TO_U64_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8Capability = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackAfDataSendTestRspPayload(ts_MsgAfDataSendTestRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgAfDataSendTestRspPayload));

    psPayload->u16SrcAddr   = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8SrcEp      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8DstEp      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16ClusterId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u16DataLen   = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    memcpy(psPayload->au8Payload, &pu8Payload[u16Offset], psPayload->u16DataLen);
}


static void zbhci_UnpackLeaveIndicationPayload(ts_MsgLeaveIndicationPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgLeaveIndicationPayload));

    psPayload->u64MacAddr = BUFFER_TO_U64_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8Rejoin   = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackZclAttrReadRspPayload(ts_MsgZclAttrReadRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;
    uint16_t u16DataLen = 0;

    bzero(psPayload, sizeof(ts_MsgZclAttrReadRspPayload));

    psPayload->u8SeqNum     = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16SrcAddr   = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8SrcEp      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16ClusterId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8AttrNum    = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    for (size_t n = 0; n < psPayload->u8AttrNum; n++)
    {
        psPayload->asAttrReadList[n].u16AttrID  = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
        psPayload->asAttrReadList[n].u8Status   = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
        psPayload->asAttrReadList[n].u8DataType = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);

        if((psPayload->asAttrReadList[n].u8DataType == ZCL_DATA_TYPE_LONG_CHAR_STR) || \
            (psPayload->asAttrReadList[n].u8DataType == ZCL_DATA_TYPE_LONG_OCTET_STR))
        {
            psPayload->asAttrReadList[n].u16DataLen = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
            memcpy(psPayload->asAttrReadList[n].uAttrData.au8AttrData, &pu8Payload[u16Offset], psPayload->asAttrReadList[n].u16DataLen);
            u16Offset += psPayload->asAttrReadList[n].u16DataLen;
        }
        else if((psPayload->asAttrReadList[n].u8DataType == ZCL_DATA_TYPE_CHAR_STR) || \
                (psPayload->asAttrReadList[n].u8DataType == ZCL_DATA_TYPE_OCTET_STR))
        {
            psPayload->asAttrReadList[n].u16DataLen = BUFFER_TO_U8_OFFSET(pu8Payload, u16Offset, u16Offset);
            memcpy(psPayload->asAttrReadList[n].uAttrData.au8AttrData, &pu8Payload[u16Offset], psPayload->asAttrReadList[n].u16DataLen);
            u16Offset += psPayload->asAttrReadList[n].u16DataLen;
        }
        else if(psPayload->asAttrReadList[n].u8DataType == ZCL_DATA_TYPE_STRUCT)
        {
            psPayload->asAttrReadList[n].u16DataLen += 2;
            for (size_t i = 0; i < pu8Payload[u16Offset]; i++)
            {
                psPayload->asAttrReadList[n].u16DataLen += zcl_getDataTypeLen(pu8Payload[u16Offset + psPayload->asAttrReadList[n].u16DataLen]);
            }
            memcpy(psPayload->asAttrReadList[n].uAttrData.au8AttrData, &pu8Payload[u16Offset], psPayload->asAttrReadList[n].u16DataLen);
            u16Offset += psPayload->asAttrReadList[n].u16DataLen;
        }
        else if(psPayload->asAttrReadList[n].u8DataType == ZCL_DATA_TYPE_128_BIT_SEC_KEY)
        {
            psPayload->asAttrReadList[n].u16DataLen = zcl_getDataTypeLen(psPayload->asAttrReadList[n].u8DataType);
            memcpy(psPayload->asAttrReadList[n].uAttrData.au8AttrData, &pu8Payload[u16Offset], psPayload->asAttrReadList[n].u16DataLen);
            u16Offset += psPayload->asAttrReadList[n].u16DataLen;
        }
        else
        {
            psPayload->asAttrReadList[n].u16DataLen = zcl_getDataTypeLen(psPayload->asAttrReadList[n].u8DataType);
            zcl_getAttrData(psPayload->asAttrReadList[n].u8DataType, &psPayload->asAttrReadList[n].uAttrData, &pu8Payload[u16Offset]);
            u16Offset += psPayload->asAttrReadList[n].u16DataLen;
        }

        u16DataLen = zcl_getAttrSize(psPayload->asAttrReadList[n].u8DataType, &pu8Payload[u16Offset]);
        memcpy(psPayload->asAttrReadList[n].uAttrData.au8AttrData, &pu8Payload[u16Offset], u16DataLen);
        u16Offset += u16DataLen;
    }
}


static void zbhci_UnpackZclAttrWriteRspPayload(ts_MsgZclAttrWriteRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclAttrWriteRspPayload));

    psPayload->u8SeqNum     = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16SrcAddr   = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8SrcEp      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16ClusterId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8AttrNum    = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    for (size_t n = 0; n < psPayload->u8AttrNum; n++)
    {
        psPayload->asAttrWriteList[n].u8Status  = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
        psPayload->asAttrWriteList[n].u16AttrID = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    }
}


static void zbhci_UnpackZclConfigReportRspPayload(ts_MsgZclConfigReportRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclConfigReportRspPayload));

    psPayload->u8SeqNum     = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16SrcAddr   = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8SrcEp      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16ClusterId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8AttrNum    = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    for (size_t n = 0; n < psPayload->u8AttrNum; n++)
    {
        psPayload->asAttrConfigReportList[n].u8Status          = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
        psPayload->asAttrConfigReportList[n].u8ReportDirection = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
        psPayload->asAttrConfigReportList[n].u16AttrID         = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    }
}


static void zbhci_UnpackZclReadReportCfgRspPayload(ts_MsgZclReadReportCfgRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;
    uint16_t u16DataLen = 0;

    bzero(psPayload, sizeof(ts_MsgZclReadReportCfgRspPayload));

    psPayload->u8SeqNum     = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16SrcAddr   = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8SrcEp      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16ClusterId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8AttrNum    = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    for (size_t n = 0; n < psPayload->u8AttrNum; n++)
    {
        psPayload->asAttrList[n].u8Status          = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
        psPayload->asAttrList[n].u8ReportDirection = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
        psPayload->asAttrList[n].u16AttrID         = BUFFER_TO_U16_OFFSET (pu8Payload, u16Offset, u16Offset);
        if (psPayload->asAttrList[n].u8ReportDirection == 0x00)
        {
            psPayload->asAttrList[n].u8DataType        = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
            psPayload->asAttrList[n].u16MinRepInterval = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
            psPayload->asAttrList[n].u16MaxRepInterval = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
            if (zcl_analogDataType(psPayload->asAttrList[n].u8DataType))
            {
                u16DataLen = zcl_getAttrSize(psPayload->asAttrList[n].u8DataType, &pu8Payload[u16Offset]);
                memcpy(psPayload->asAttrList[n].au8ReportableChange, &pu8Payload[u16Offset], u16DataLen);
            }
        }
        else
        {
            psPayload->asAttrList[n].u16TimeoutPeriod = BUFFER_TO_U16_OFFSET (pu8Payload, u16Offset, u16Offset);
        }
    }
}


static void zbhci_UnpackZclReportMsgRcvPayload(ts_MsgZclReportMsgRcvPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclReportMsgRcvPayload));

    psPayload->u8SeqNum     = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16SrcAddr   = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8SrcEp      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16ClusterId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8AttrNum    = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    for (size_t n = 0; n < psPayload->u8AttrNum; n++)
    {
        psPayload->asAttrList[n].u16AttrID  = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
        psPayload->asAttrList[n].u8DataType = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
        if((psPayload->asAttrList[n].u8DataType == ZCL_DATA_TYPE_LONG_CHAR_STR) || \
            (psPayload->asAttrList[n].u8DataType == ZCL_DATA_TYPE_LONG_OCTET_STR))
        {
            psPayload->asAttrList[n].u16DataLen = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
            memcpy(psPayload->asAttrList[n].uAttrData.au8AttrData, &pu8Payload[u16Offset], psPayload->asAttrList[n].u16DataLen);
            u16Offset += psPayload->asAttrList[n].u16DataLen;
        }
        else if((psPayload->asAttrList[n].u8DataType == ZCL_DATA_TYPE_CHAR_STR) || \
                (psPayload->asAttrList[n].u8DataType == ZCL_DATA_TYPE_OCTET_STR))
        {
            psPayload->asAttrList[n].u16DataLen = BUFFER_TO_U8_OFFSET(pu8Payload, u16Offset, u16Offset);
            memcpy(psPayload->asAttrList[n].uAttrData.au8AttrData, &pu8Payload[u16Offset], psPayload->asAttrList[n].u16DataLen);
            u16Offset += psPayload->asAttrList[n].u16DataLen;
        }
        else if(psPayload->asAttrList[n].u8DataType == ZCL_DATA_TYPE_STRUCT)
        {
            psPayload->asAttrList[n].u16DataLen += 2;
            for (size_t i = 0; i < pu8Payload[u16Offset]; i++)
            {
                psPayload->asAttrList[n].u16DataLen += zcl_getDataTypeLen(pu8Payload[u16Offset + psPayload->asAttrList[n].u16DataLen]);
            }
            memcpy(psPayload->asAttrList[n].uAttrData.au8AttrData, &pu8Payload[u16Offset], psPayload->asAttrList[n].u16DataLen);
            u16Offset += psPayload->asAttrList[n].u16DataLen;
        }
        else if(psPayload->asAttrList[n].u8DataType == ZCL_DATA_TYPE_128_BIT_SEC_KEY)
        {
            psPayload->asAttrList[n].u16DataLen = zcl_getDataTypeLen(psPayload->asAttrList[n].u8DataType);
            memcpy(psPayload->asAttrList[n].uAttrData.au8AttrData, &pu8Payload[u16Offset], psPayload->asAttrList[n].u16DataLen);
            u16Offset += psPayload->asAttrList[n].u16DataLen;
        }
        else
        {
            psPayload->asAttrList[n].u16DataLen = zcl_getDataTypeLen(psPayload->asAttrList[n].u8DataType);
            zcl_getAttrData(psPayload->asAttrList[n].u8DataType, &psPayload->asAttrList[n].uAttrData, &pu8Payload[u16Offset]);
            u16Offset += psPayload->asAttrList[n].u16DataLen;
        }
    }
}


static void zbhci_UnpackZclGroupAddRspPayload(ts_MsgZclGroupAddRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclGroupAddRspPayload));

    psPayload->u8Status   = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16GroupId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackZclGroupViewRspPayload(ts_MsgZclGroupViewRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclGroupViewRspPayload));

    psPayload->u8Status          = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16GroupId        = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8GroupNameLength = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    memcpy(psPayload->au8GroupName, &pu8Payload[u16Offset], psPayload->u8GroupNameLength);
}


static void zbhci_UnpackZclGroupGetMembershipRspPayload(ts_MsgZclGroupGetMembershipRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclGroupGetMembershipRspPayload));

    psPayload->u8Capability = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8GroupCount = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    for (size_t n = 0; n < psPayload->u8GroupCount; n++)
    {
        psPayload->au16GroupId[n] = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    }
}


static void zbhci_UnpackZclGroupRemoveRspPayload(ts_MsgZclGroupRemoveRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclGroupRemoveRspPayload));

    psPayload->u8Status   = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16GroupId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackZclIdentifyQueryRspPayload(ts_MsgZclIdentifyQueryRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclIdentifyQueryRspPayload));

    psPayload->u16ShortAddr = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8SrcEp      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16Timeout   = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackZclOnOffCmdRcvPayload(ts_MsgZclOnOffCmdRcvPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclOnOffCmdRcvPayload));

    psPayload->u8SrcEp      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8DstEp      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16ClusterId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8CmdId      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackZclSceneAddRspPayload(ts_MsgZclSceneAddRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclSceneAddRspPayload));

    psPayload->u8Status   = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16GroupId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8SceneId  = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackZclSceneViewRspPayload(ts_MsgZclSceneViewRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclSceneViewRspPayload));

    psPayload->u8Status     = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16GroupId   = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8SceneId    = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16TransTime = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8SceneNameLength = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    memcpy(psPayload->au8SceneName, &pu8Payload[u16Offset], psPayload->u8SceneNameLength);
    u16Offset += psPayload->u8SceneNameLength;
    psPayload->extFieldLength = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    memcpy(psPayload->au8ExtFieldSets, &pu8Payload[u16Offset], psPayload->extFieldLength);
    u16Offset += psPayload->extFieldLength;
}


static void zbhci_UnpackZclSceneRemoveRspPayload(ts_MsgZclSceneRemoveRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclSceneRemoveRspPayload));

    psPayload->u8Status   = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16GroupId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8SceneId  = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackZclSceneRemoveAllRspPayload(ts_MsgZclSceneRemoveAllRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclSceneRemoveAllRspPayload));

    psPayload->u8Status   = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16GroupId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackZclSceneStoreRspPayload(ts_MsgZclSceneStoreRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclSceneRemoveAllRspPayload));

    psPayload->u8Status   = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16GroupId = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8SceneId  = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackZclSceneGetMembershipRspPayload(ts_MsgZclSceneGetMenbershipRspPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgZclSceneRemoveAllRspPayload));

    psPayload->u8Status     = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8Capability = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u16GroupId   = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u8SceneCnt   = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    memcpy(psPayload->au8SceneList, &pu8Payload[u16Offset], psPayload->u8SceneCnt);
}


static void zbhci_UnpackDataConfirmPayload(ts_MsgDataConfirmPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgDataConfirmPayload));

    psPayload->u8SrcEndpoint = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8Status      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
    psPayload->u8ApsCnt      = BUFFER_TO_U8_OFFSET (pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackMacAddrIndPayload(ts_MsgMacAddrIndPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgMacAddrIndPayload));

    psPayload->u64DeviceExtAddr = BUFFER_TO_U64_OFFSET (pu8Payload, u16Offset, u16Offset);
}


static void zbhci_UnpackNodeLeaveIndPayload(ts_MsgNodeLeaveIndPayload *psPayload, uint8_t *pu8Payload)
{
    uint16_t u16Offset = 0;

    bzero(psPayload, sizeof(ts_MsgNodeLeaveIndPayload));

    psPayload->u16TotalCnt = BUFFER_TO_U16_OFFSET(pu8Payload, u16Offset, u16Offset);
    psPayload->u64MacAddr =  BUFFER_TO_U64_OFFSET(pu8Payload, u16Offset, u16Offset);
}

#if 0
static void zbhci_HandleAcknowledg(ts_MsgAckPayload *psPayload)
{

}


static void zbhci_HandleDiscoveryNwkAddrRsp()
{

}


static void zbhci_HandleDiscoveryIEEEAddrRsp()
{

}


static void zbhci_HandleDiscoveryNodeDescRsp()
{

}


static void zbhci_HandleDiscoverySimpleDescRsp()
{

}


static void zbhci_HandleDiscoveryMatchDescRsp()
{

}


static void zbhci_HandleDiscoveryActiveEpRsp()
{

}


static void zbhci_HandleBindingRsp()
{

}


static void zbhci_HandleUnbindingRsp()
{

}


static void zbhci_HandleMgmtLqiRsp()
{

}


static void zbhci_HandleMgmtBindRsp()
{

}


static void zbhci_HandleMgmtLeaveRsp()
{

}


static void zbhci_HandleMgmtDirectJoinRsp()
{
    /** None */
}


static void zbhci_HandleMgmtPermitJoinRsp()
{

}


static void zbhci_HandleMgmtNwkUpdateRsp()
{
    /** None */
}


static void zbhci_HandleNodesJoinedGetRsp()
{

}


static void zbhci_HandleNodesTogleTestRsp()
{

}


static void zbhci_HandleTxRxPerformanceTestRsp()
{

}


static void zbhci_HandleNodesDevAnnceInd()
{

}


static void zbhci_HandleAfDataSendTestRsp()
{

}


static void zbhci_HandleZclAttrReadRsp()
{

}


static void zbhci_HandleZclAttrWriteRsp()
{

}


static void zbhci_HandleZclConfigReportRsp()
{

}


static void zbhci_HandleZclReadReportCfgRsp()
{

}


static void zbhci_HandleZclReportMsgRcv()
{

}

static void zbhci_HandleZclGroupAddRsp()
{

}

static void zbhci_HandleZclGroupViewRsp()
{

}

static void zbhci_HandleZclGroupGetMembershipRsp()
{

}

static void zbhci_HandleZclGroupRemoveRsp()
{

}

static void zbhci_HandleZclIdentifyQueryRsp()
{

}

static void zbhci_HandleZclSceneAddRsp()
{

}

static void zbhci_HandleZclSceneViewRsp()
{

}

static void zbhci_HandleZclSceneRemoveRsp()
{

}

static void zbhci_HandleZclSceneRemoveAllRsp()
{

}

static void zbhci_HandleZclSceneStoreRsp()
{

}

static void zbhci_HandleZclSceneGetMembershipRsp()
{

}

static void zbhci_HandleDataConfirm()
{

}

static void zbhci_HandleMacAddrInd()
{

}

static void zbhci_HandleNodeLeaveInd()
{

}
#endif

static uint8_t zcl_getDataTypeLen(uint8_t u8DataType)
{
    switch(u8DataType)
    {
        case ZCL_DATA_TYPE_BOOLEAN:
            return sizeof(uint8_t);

        case ZCL_DATA_TYPE_DATA8:
        case ZCL_DATA_TYPE_BITMAP8:
        case ZCL_DATA_TYPE_UINT8:
        case ZCL_DATA_TYPE_INT8:
        case ZCL_DATA_TYPE_ENUM8:
            return sizeof(uint8_t);

        case ZCL_DATA_TYPE_DATA16:
        case ZCL_DATA_TYPE_BITMAP16:
        case ZCL_DATA_TYPE_UINT16:
        case ZCL_DATA_TYPE_INT16:
        case ZCL_DATA_TYPE_ENUM16:
        case ZCL_DATA_TYPE_SEMI_PREC:
        case ZCL_DATA_TYPE_CLUSTER_ID:
        case ZCL_DATA_TYPE_ATTR_ID:
            return sizeof(uint16_t);

        case ZCL_DATA_TYPE_DATA24:
        case ZCL_DATA_TYPE_BITMAP24:
        case ZCL_DATA_TYPE_UINT24:
        case ZCL_DATA_TYPE_INT24:
            return 3;

        case ZCL_DATA_TYPE_DATA32:
        case ZCL_DATA_TYPE_BITMAP32:
        case ZCL_DATA_TYPE_UINT32:
        case ZCL_DATA_TYPE_INT32:
        case ZCL_DATA_TYPE_SINGLE_PREC:
        case ZCL_DATA_TYPE_UTC:
        case ZCL_DATA_TYPE_DATE:
        case ZCL_DATA_TYPE_BAC_OID:
            return sizeof(uint32_t);

        case ZCL_DATA_TYPE_DATA40:
        case ZCL_DATA_TYPE_BITMAP40:
        case ZCL_DATA_TYPE_UINT40:
        case ZCL_DATA_TYPE_INT40:
            return 5;

        case ZCL_DATA_TYPE_DATA48:
        case ZCL_DATA_TYPE_BITMAP48:
        case ZCL_DATA_TYPE_UINT48:
        case ZCL_DATA_TYPE_INT48:
            return 6;

        case ZCL_DATA_TYPE_DATA56:
        case ZCL_DATA_TYPE_BITMAP56:
        case ZCL_DATA_TYPE_UINT56:
        case ZCL_DATA_TYPE_INT56:
            return 7;

        case ZCL_DATA_TYPE_DATA64:
        case ZCL_DATA_TYPE_BITMAP64:
        case ZCL_DATA_TYPE_UINT64:
        case ZCL_DATA_TYPE_INT64:
        case ZCL_DATA_TYPE_DOUBLE_PREC:
        case ZCL_DATA_TYPE_IEEE_ADDR:
            return 8;

        case ZCL_DATA_TYPE_128_BIT_SEC_KEY:
            return 16;

        case ZCL_DATA_TYPE_NO_DATA:
        case ZCL_DATA_TYPE_UNKNOWN:

        default:
        return 0;
    }
}


static uint16_t zcl_getAttrSize(uint8_t u8DataType, uint8_t *pu8Data)
{
    uint16_t u16DataLen = 0;

    if((u8DataType == ZCL_DATA_TYPE_LONG_CHAR_STR) || (u8DataType == ZCL_DATA_TYPE_LONG_OCTET_STR))
    {
        u16DataLen = BUFFER_TO_U16(pu8Data, 0) + 2; //long string length + 2 for length field
    }
    else if((u8DataType == ZCL_DATA_TYPE_CHAR_STR) || (u8DataType == ZCL_DATA_TYPE_OCTET_STR))
    {
        u16DataLen = *pu8Data + 1; //string length + 1 for length field
    }
    else if(u8DataType == ZCL_DATA_TYPE_STRUCT)
    {
        uint8_t itemNum = *pu8Data;
        u16DataLen += 2;
        for(uint8_t i = 0; i < itemNum; i++)
        {
            uint8_t dtUnion = pu8Data[u16DataLen];
            uint8_t lenTemp = zcl_getDataTypeLen(dtUnion);
            u16DataLen += (lenTemp + 1);
        }
    }
    else
    {
        u16DataLen = zcl_getDataTypeLen(u8DataType);
    }

    return u16DataLen;
}


static bool zcl_analogDataType(uint8_t u8DataType)
{
    switch(u8DataType)
    {
        case ZCL_DATA_TYPE_UINT8:
        case ZCL_DATA_TYPE_UINT16:
        case ZCL_DATA_TYPE_UINT24:
        case ZCL_DATA_TYPE_UINT32:
        case ZCL_DATA_TYPE_UINT40:
        case ZCL_DATA_TYPE_UINT48:
        case ZCL_DATA_TYPE_UINT56:
        case ZCL_DATA_TYPE_UINT64:
        case ZCL_DATA_TYPE_INT8:
        case ZCL_DATA_TYPE_INT16:
        case ZCL_DATA_TYPE_INT24:
        case ZCL_DATA_TYPE_INT32:
        case ZCL_DATA_TYPE_INT40:
        case ZCL_DATA_TYPE_INT48:
        case ZCL_DATA_TYPE_INT56:
        case ZCL_DATA_TYPE_INT64:
        case ZCL_DATA_TYPE_SEMI_PREC:
        case ZCL_DATA_TYPE_SINGLE_PREC:
        case ZCL_DATA_TYPE_DOUBLE_PREC:
        case ZCL_DATA_TYPE_TOD:
        case ZCL_DATA_TYPE_DATE:
        case ZCL_DATA_TYPE_UTC:
            return true;
            break;

        default:
            return false;
            break;
    }
}


static void zcl_getAttrData(uint8_t u8DataType, t_AttrData *psAttrData, uint8_t *pu8Data)
{
    switch(u8DataType)
    {
        case ZCL_DATA_TYPE_BOOLEAN:
        case ZCL_DATA_TYPE_DATA8:
        case ZCL_DATA_TYPE_BITMAP8:
        case ZCL_DATA_TYPE_UINT8:
        case ZCL_DATA_TYPE_INT8:
        case ZCL_DATA_TYPE_ENUM8:
            psAttrData->u8AttrData = BUFFER_TO_U8(pu8Data, 0);
        break;

        case ZCL_DATA_TYPE_DATA16:
        case ZCL_DATA_TYPE_BITMAP16:
        case ZCL_DATA_TYPE_UINT16:
        case ZCL_DATA_TYPE_INT16:
        case ZCL_DATA_TYPE_ENUM16:
        case ZCL_DATA_TYPE_SEMI_PREC:
        case ZCL_DATA_TYPE_CLUSTER_ID:
        case ZCL_DATA_TYPE_ATTR_ID:
            psAttrData->u16AttrData = BUFFER_TO_U16(pu8Data, 0);
        break;

        case ZCL_DATA_TYPE_DATA24:
        case ZCL_DATA_TYPE_BITMAP24:
        case ZCL_DATA_TYPE_UINT24:
        case ZCL_DATA_TYPE_INT24:
            psAttrData->u32AttrData = BUFFER_TO_U24(pu8Data, 0);
        break;

        case ZCL_DATA_TYPE_DATA32:
        case ZCL_DATA_TYPE_BITMAP32:
        case ZCL_DATA_TYPE_UINT32:
        case ZCL_DATA_TYPE_INT32:
        case ZCL_DATA_TYPE_SINGLE_PREC:
        case ZCL_DATA_TYPE_UTC:
        case ZCL_DATA_TYPE_DATE:
        case ZCL_DATA_TYPE_BAC_OID:
            psAttrData->u32AttrData = BUFFER_TO_U32(pu8Data, 0);
        break;

        case ZCL_DATA_TYPE_DATA40:
        case ZCL_DATA_TYPE_BITMAP40:
        case ZCL_DATA_TYPE_UINT40:
        case ZCL_DATA_TYPE_INT40:
            psAttrData->u64AttrData = BUFFER_TO_U40(pu8Data, 0);
        break;

        case ZCL_DATA_TYPE_DATA48:
        case ZCL_DATA_TYPE_BITMAP48:
        case ZCL_DATA_TYPE_UINT48:
        case ZCL_DATA_TYPE_INT48:
            psAttrData->u64AttrData = BUFFER_TO_U48(pu8Data, 0);
        break;

        case ZCL_DATA_TYPE_DATA56:
        case ZCL_DATA_TYPE_BITMAP56:
        case ZCL_DATA_TYPE_UINT56:
        case ZCL_DATA_TYPE_INT56:
            psAttrData->u64AttrData = BUFFER_TO_U56(pu8Data, 0);
        break;

        case ZCL_DATA_TYPE_DATA64:
        case ZCL_DATA_TYPE_BITMAP64:
        case ZCL_DATA_TYPE_UINT64:
        case ZCL_DATA_TYPE_INT64:
        case ZCL_DATA_TYPE_DOUBLE_PREC:
        case ZCL_DATA_TYPE_IEEE_ADDR:
            psAttrData->u64AttrData = BUFFER_TO_U64(pu8Data, 0);
        break;

        case ZCL_DATA_TYPE_NO_DATA:
        case ZCL_DATA_TYPE_UNKNOWN:

        default:
        break;
    }
}

/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/