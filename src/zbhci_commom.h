#ifndef ZBHCI_COMMON_H
#define ZBHCI_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

/** @addtogroup zcl_data_types ZCL Attribute Data Types
 * @{
 */
#define ZCL_DATA_TYPE_NO_DATA                            0x00
#define ZCL_DATA_TYPE_DATA8                              0x08
#define ZCL_DATA_TYPE_DATA16                             0x09
#define ZCL_DATA_TYPE_DATA24                             0x0a
#define ZCL_DATA_TYPE_DATA32                             0x0b
#define ZCL_DATA_TYPE_DATA40                             0x0c
#define ZCL_DATA_TYPE_DATA48                             0x0d
#define ZCL_DATA_TYPE_DATA56                             0x0e
#define ZCL_DATA_TYPE_DATA64                             0x0f
#define ZCL_DATA_TYPE_BOOLEAN                            0x10
#define ZCL_DATA_TYPE_BITMAP8                            0x18
#define ZCL_DATA_TYPE_BITMAP16                           0x19
#define ZCL_DATA_TYPE_BITMAP24                           0x1a
#define ZCL_DATA_TYPE_BITMAP32                           0x1b
#define ZCL_DATA_TYPE_BITMAP40                           0x1c
#define ZCL_DATA_TYPE_BITMAP48                           0x1d
#define ZCL_DATA_TYPE_BITMAP56                           0x1e
#define ZCL_DATA_TYPE_BITMAP64                           0x1f
#define ZCL_DATA_TYPE_UINT8                              0x20
#define ZCL_DATA_TYPE_UINT16                             0x21
#define ZCL_DATA_TYPE_UINT24                             0x22
#define ZCL_DATA_TYPE_UINT32                             0x23
#define ZCL_DATA_TYPE_UINT40                             0x24
#define ZCL_DATA_TYPE_UINT48                             0x25
#define ZCL_DATA_TYPE_UINT56                             0x26
#define ZCL_DATA_TYPE_UINT64                             0x27
#define ZCL_DATA_TYPE_INT8                               0x28
#define ZCL_DATA_TYPE_INT16                              0x29
#define ZCL_DATA_TYPE_INT24                              0x2a
#define ZCL_DATA_TYPE_INT32                              0x2b
#define ZCL_DATA_TYPE_INT40                              0x2c
#define ZCL_DATA_TYPE_INT48                              0x2d
#define ZCL_DATA_TYPE_INT56                              0x2e
#define ZCL_DATA_TYPE_INT64                              0x2f
#define ZCL_DATA_TYPE_ENUM8                              0x30
#define ZCL_DATA_TYPE_ENUM16                             0x31
#define ZCL_DATA_TYPE_SEMI_PREC                          0x38
#define ZCL_DATA_TYPE_SINGLE_PREC                        0x39
#define ZCL_DATA_TYPE_DOUBLE_PREC                        0x3a
#define ZCL_DATA_TYPE_OCTET_STR                          0x41
#define ZCL_DATA_TYPE_CHAR_STR                           0x42
#define ZCL_DATA_TYPE_LONG_OCTET_STR                     0x43
#define ZCL_DATA_TYPE_LONG_CHAR_STR                      0x44
#define ZCL_DATA_TYPE_ARRAY                              0x48
#define ZCL_DATA_TYPE_STRUCT                             0x4c
#define ZCL_DATA_TYPE_SET                                0x50
#define ZCL_DATA_TYPE_BAG                                0x51
#define ZCL_DATA_TYPE_TOD                                0xe0
#define ZCL_DATA_TYPE_DATE                               0xe1
#define ZCL_DATA_TYPE_UTC                                0xe2
#define ZCL_DATA_TYPE_CLUSTER_ID                         0xe8
#define ZCL_DATA_TYPE_ATTR_ID                            0xe9
#define ZCL_DATA_TYPE_BAC_OID                            0xea
#define ZCL_DATA_TYPE_IEEE_ADDR                          0xf0
#define ZCL_DATA_TYPE_128_BIT_SEC_KEY                    0xf1
#define ZCL_DATA_TYPE_UNKNOWN                            0xff
/** @} end of group zcl_data_types */

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

/**
 * @enum te_HCIMsgType ZigBee HCI message type
 */
typedef enum te_HCIMsgType
{
    ZBHCI_CMD_BDB_COMMISSION_FORMATION       = 0x0001,
    ZBHCI_CMD_BDB_COMMISSION_STEER           = 0x0002,
    ZBHCI_CMD_BDB_COMMISSION_TOUCHLINK       = 0x0003,
    ZBHCI_CMD_BDB_COMMISSION_FINDBIND        = 0x0004,
    ZBHCI_CMD_BDB_FACTORY_RESET              = 0x0005,
    ZBHCI_CMD_BDB_PRE_INSTALL_CODE           = 0x0006,
    ZBHCI_CMD_BDB_CHANNEL_SET                = 0x0007,
    ZBHCI_CMD_BDB_DONGLE_WORKING_MODE_SET    = 0x0008,
    ZBHCI_CMD_BDB_NODE_DELETE                = 0x0009,
    ZBHCI_CMD_BDB_TX_POWER_SET               = 0x000A,
    ZBHCI_CMD_ACKNOWLEDGE                    = 0x8000,
    ZBHCI_CMD_BDB_COMMISSION_FORMATION_RSP   = 0x8001,

    ZBHCI_CMD_NETWORK_STATE_REQ              = 0x000B,
    ZBHCI_CMD_NETWORK_STATE_RSP              = 0x800B,
    ZBHCI_CMD_NETWORK_STATE_REPORT           = 0x800C,

    ZBHCI_CMD_DISCOVERY_NWK_ADDR_REQ         = 0x0010,
    ZBHCI_CMD_DISCOVERY_IEEE_ADDR_REQ        = 0x0011,
    ZBHCI_CMD_DISCOVERY_NODE_DESC_REQ        = 0x0012,
    ZBHCI_CMD_DISCOVERY_SIMPLE_DESC_REQ      = 0x0013,
    ZBHCI_CMD_DISCOVERY_MATCH_DESC_REQ       = 0x0014,
    ZBHCI_CMD_DISCOVERY_ACTIVE_EP_REQ        = 0x0015,
    ZBHCI_CMD_DISCOVERY_LEAVE_REQ            = 0x0016,

    ZBHCI_CMD_DISCOVERY_NWK_ADDR_RSP         = 0x8010,
    ZBHCI_CMD_DISCOVERY_IEEE_ADDR_RSP        = 0x8011,
    ZBHCI_CMD_DISCOVERY_NODE_DESC_RSP        = 0x8012,
    ZBHCI_CMD_DISCOVERY_SIMPLE_DESC_RSP      = 0x8013,
    ZBHCI_CMD_DISCOVERY_MATCH_DESC_RSP       = 0x8014,
    ZBHCI_CMD_DISCOVERY_ACTIVE_EP_RSP        = 0x8015,

    ZBHCI_CMD_BINDING_REQ                    = 0x0020,
    ZBHCI_CMD_UNBINDING_REQ                  = 0x0021,

    ZBHCI_CMD_BINDING_RSP                    = 0x8020,
    ZBHCI_CMD_UNBINDING_RSP                  = 0x8021,

    ZBHCI_CMD_MGMT_LQI_REQ                   = 0x0030,
    ZBHCI_CMD_MGMT_BIND_REQ                  = 0x0031,
    ZBHCI_CMD_MGMT_LEAVE_REQ                 = 0x0032,
    ZBHCI_CMD_MGMT_DIRECT_JOIN_REQ           = 0x0033,
    ZBHCI_CMD_MGMT_PERMIT_JOIN_REQ           = 0x0034,
    ZBHCI_CMD_MGMT_NWK_UPDATE_REQ            = 0x0035,

    ZBHCI_CMD_MGMT_LQI_RSP                   = 0x8030,
    ZBHCI_CMD_MGMT_BIND_RSP                  = 0x8031,
    ZBHCI_CMD_MGMT_LEAVE_RSP                 = 0x8032,
    ZBHCI_CMD_MGMT_DIRECT_JOIN_RSP           = 0x8033,
    ZBHCI_CMD_MGMT_PERMIT_JOIN_RSP           = 0x8034,
    ZBHCI_CMD_MGMT_NWK_UPDATE_RSP            = 0x8035,

    ZBHCI_CMD_NODES_JOINED_GET_REQ           = 0x0040,
    ZBHCI_CMD_NODES_TOGLE_TEST_REQ           = 0x0041,
    ZBHCI_CMD_TXRX_PERFORMANCE_TEST_REQ      = 0x0042,
    ZBHCI_CMD_AF_DATA_SEND_TEST_REQ          = 0x0044,

    ZBHCI_CMD_NODES_JOINED_GET_RSP           = 0x8040,
    ZBHCI_CMD_NODES_TOGLE_TEST_RSP           = 0x8041,
    ZBHCI_CMD_TXRX_PERFORMANCE_TEST_RSP      = 0x8042,
    ZBHCI_CMD_NODES_DEV_ANNCE_IND            = 0x8043,
    ZBHCI_CMD_AF_DATA_SEND_TEST_RSP          = 0x8044,
    ZBHCI_CMD_LEAVE_INDICATION               = 0x8048,

    ZBHCI_CMD_ZCL_ATTR_READ                  = 0x0100,
    ZBHCI_CMD_ZCL_ATTR_WRITE                 = 0x0101,
    ZBHCI_CMD_ZCL_CONFIG_REPORT              = 0x0102,
    ZBHCI_CMD_ZCL_READ_REPORT_CFG            = 0x0103,
    ZBHCI_CMD_ZCL_LOCAL_ATTR_READ            = 0x0105,
    ZBHCI_CMD_ZCL_LOCAL_ATTR_WRITE           = 0x0106,
    ZBHCI_CMD_ZCL_SEND_REPORT_CMD            = 0x0108,

    ZBHCI_CMD_ZCL_ATTR_READ_RSP              = 0x8100,
    ZBHCI_CMD_ZCL_ATTR_WRITE_RSP             = 0x8101,
    ZBHCI_CMD_ZCL_CONFIG_REPORT_RSP          = 0x8102,
    ZBHCI_CMD_ZCL_READ_REPORT_CFG_RSP        = 0x8103,
    ZBHCI_CMD_ZCL_REPORT_MSG_RCV             = 0x8104,
    ZBHCI_CMD_ZCL_LOCAL_ATTR_READ_RSP        = 0x8105,
    ZBHCI_CMD_ZCL_LOCAL_ATTR_WRITE_RSP       = 0x8106,
    ZBHCI_CMD_ZCL_ATTR_WRITE_RCV             = 0x8107,

    ZBHCI_CMD_ZCL_BASIC                      = 0x0110,
    ZBHCI_CMD_ZCL_BASIC_RESET                = 0x0110,

    ZBHCI_CMD_ZCL_GROUP                      = 0x0120,
    ZBHCI_CMD_ZCL_GROUP_ADD                  = 0x0120,
    ZBHCI_CMD_ZCL_GROUP_VIEW                 = 0x0121,
    ZBHCI_CMD_ZCL_GROUP_GET_MEMBERSHIP       = 0x0122,
    ZBHCI_CMD_ZCL_GROUP_REMOVE               = 0x0123,
    ZBHCI_CMD_ZCL_GROUP_REMOVE_ALL           = 0x0124,
    ZBHCI_CMD_ZCL_GROUP_ADD_IF_IDENTIFY      = 0x0125,

    ZBHCI_CMD_ZCL_GROUP_ADD_RSP              = 0x8120,
    ZBHCI_CMD_ZCL_GROUP_VIEW_RSP             = 0x8121,
    ZBHCI_CMD_ZCL_GROUP_GET_MEMBERSHIP_RSP   = 0x8122,
    ZBHCI_CMD_ZCL_GROUP_REMOVE_RSP           = 0x8123,

    ZBHCI_CMD_ZCL_IDENTIFY                   = 0x0130,
    ZBHCI_CMD_ZCL_IDENTIFY_QUERY             = 0x0131,

    ZBHCI_CMD_ZCL_IDENTIFY_QUERY_RSP         = 0x8131,

    ZBHCI_CMD_ZCL_ONOFF                      = 0x0140,
    ZBHCI_CMD_ZCL_ONOFF_ON                   = 0x0140,
    ZBHCI_CMD_ZCL_ONOFF_OFF                  = 0x0141,
    ZBHCI_CMD_ZCL_ONOFF_TOGGLE               = 0x0142,
    ZBHCI_CMD_ZCL_ONOFF_CMD_RCV              = 0x8140,

    ZBHCI_CMD_ZCL_LEVEL                      = 0x0150,
    ZBHCI_CMD_ZCL_LEVEL_MOVE2LEVEL           = 0x0150,
    ZBHCI_CMD_ZCL_LEVEL_MOVE                 = 0x0151,
    ZBHCI_CMD_ZCL_LEVEL_STEP                 = 0x0152,
    ZBHCI_CMD_ZCL_LEVEL_STOP                 = 0x0153,
    ZBHCI_CMD_ZCL_LEVEL_MOVE2LEVEL_WITHONOFF = 0x0154,
    ZBHCI_CMD_ZCL_LEVEL_MOVE_WITHONOFF       = 0x0155,
    ZBHCI_CMD_ZCL_LEVEL_STEP_WITHONOFF       = 0x0156,
    ZBHCI_CMD_ZCL_LEVEL_STOP_WITHONOFF       = 0x0157,

    ZBHCI_CMD_ZCL_SCENE                      = 0x0160,
    ZBHCI_CMD_ZCL_SCENE_ADD                  = 0x0160,
    ZBHCI_CMD_ZCL_SCENE_VIEW                 = 0x0161,
    ZBHCI_CMD_ZCL_SCENE_REMOVE               = 0x0162,
    ZBHCI_CMD_ZCL_SCENE_REMOVE_ALL           = 0x0163,
    ZBHCI_CMD_ZCL_SCENE_STORE                = 0x0164,
    ZBHCI_CMD_ZCL_SCENE_RECALL               = 0x0165,
    ZBHCI_CMD_ZCL_SCENE_GET_MEMBERSHIP       = 0x0166,

    ZBHCI_CMD_ZCL_SCENE_ADD_RSP              = 0x8160,
    ZBHCI_CMD_ZCL_SCENE_VIEW_RSP             = 0x8161,
    ZBHCI_CMD_ZCL_SCENE_REMOVE_RSP           = 0x8162,
    ZBHCI_CMD_ZCL_SCENE_REMOVE_ALL_RSP       = 0x8163,
    ZBHCI_CMD_ZCL_SCENE_STORE_RSP            = 0x8164,
    ZBHCI_CMD_ZCL_SCENE_GET_MEMBERSHIP_RSP   = 0x8166,

    ZBHCI_CMD_ZCL_COLOR                      = 0x0170,
    ZBHCI_CMD_ZCL_COLOR_MOVE2HUE             = 0x0170,
    ZBHCI_CMD_ZCL_COLOR_MOVE2COLOR           = 0x0171,
    ZBHCI_CMD_ZCL_COLOR_MOVE2SAT             = 0x0172,
    ZBHCI_CMD_ZCL_COLOR_MOVE2TEMP            = 0x0173,

    ZBHCI_CMD_ZCL_IAS_ZONE                   = 0x0180,

    ZBHCI_CMD_ZCL_OTA_IMAGE_NOTIFY           = 0x0190,

    ZBHCI_CMD_DATA_CONFIRM                   = 0x8200, //data confirm
    ZBHCI_CMD_MAC_ADDR_IND                   = 0x8201,
    ZBHCI_CMD_NODE_LEAVE_IND                 = 0x8202,

    ZBHCI_CMD_AF_DATA_SEND                   = 0x0300,
} te_HCIMsgType;

typedef enum te_AddrMode
{
    E_ADDR_MODE_BIND_ADDR  = 0x00, /**< for bind, without address and Endpoint @note Use with caution, it is invalid */
    E_ADDR_MODE_GROUP_ADDR = 0x01, /**< for group-casting: only need group address */
    E_ADDR_MODE_SHORT_ADDR = 0x02, /**< for unicasting with nwk address, with Endpoint */
    E_ADDR_MODE_IEEE_ADDR  = 0x03, /**< for unicasting with ieee address, with Endpoint */
} te_AddrMode;

typedef enum te_MsgBdbCommissionTouchlinkRole
{
    E_BDB_COMMISSION_TOUCHLINK_ROLE_INITIATOR = 1, /**< Touch link initiator */
    E_BDB_COMMISSION_TOUCHLINK_ROLE_TARGET    = 2, /**< Touch link target */
} te_MsgBdbCommissionTouchlinkRole;

typedef enum te_MsgBdbCommissionFindbindRole
{
    E_BDB_COMMISSION_FINDBIND_ROLE_INITIATOR = 1, /**< Find & Bind initiator */
    E_BDB_COMMISSION_FINDBIND_ROLE_TARGET    = 2, /**< Find & Bind target */
} te_MsgBdbCommissionFindbindRole;

typedef enum te_MsgBdbDongleWorkingMode
{
    E_BDB_DONGLE_WORKING_MODE_GET_MAC_ADDR_MODE = 0,
    E_BDB_DONGLE_WORKING_MODE_NORMAL_MODE       = 1,
} te_MsgBdbDongleWorkingMode;

typedef enum te_BdbTxPowerLevel
{
	/*VBAT*/
	E_BDB_TX_POWER_LEVEL_P11p26dBm,
	E_BDB_TX_POWER_LEVEL_P11p09dBm,
	E_BDB_TX_POWER_LEVEL_P10p83dBm,
	E_BDB_TX_POWER_LEVEL_P10p62dBm,
	E_BDB_TX_POWER_LEVEL_P10p30dBm,
	E_BDB_TX_POWER_LEVEL_P10p05dBm,
	E_BDB_TX_POWER_LEVEL_P9p79dBm,
	E_BDB_TX_POWER_LEVEL_P9p54dBm,
	E_BDB_TX_POWER_LEVEL_P9p23dBm,
	E_BDB_TX_POWER_LEVEL_P8p92dBm,
	E_BDB_TX_POWER_LEVEL_P8p57dBm,
	E_BDB_TX_POWER_LEVEL_P8p20dBm,
	E_BDB_TX_POWER_LEVEL_P7p80dBm,
	E_BDB_TX_POWER_LEVEL_P7p37dBm,
	E_BDB_TX_POWER_LEVEL_P6p91dBm,
	E_BDB_TX_POWER_LEVEL_P6p45dBm,
	E_BDB_TX_POWER_LEVEL_P5p92dBm,
	E_BDB_TX_POWER_LEVEL_P5p33dBm,
	E_BDB_TX_POWER_LEVEL_P4p69dBm,
	E_BDB_TX_POWER_LEVEL_P3p99dBm,
	/*VANT*/
	E_BDB_TX_POWER_LEVEL_P3p50dBm,
	E_BDB_TX_POWER_LEVEL_P3p33dBm,
	E_BDB_TX_POWER_LEVEL_P3p13dBm,
	E_BDB_TX_POWER_LEVEL_P2p93dBm,
	E_BDB_TX_POWER_LEVEL_P2p60dBm,
	E_BDB_TX_POWER_LEVEL_P2p36dBm,
	E_BDB_TX_POWER_LEVEL_P2p10dBm,
	E_BDB_TX_POWER_LEVEL_P1p83dBm,
	E_BDB_TX_POWER_LEVEL_P1p56dBm,
	E_BDB_TX_POWER_LEVEL_P1p25dBm,
	E_BDB_TX_POWER_LEVEL_P0p71dBm,
	E_BDB_TX_POWER_LEVEL_P0p52dBm,
	E_BDB_TX_POWER_LEVEL_N0p28dBm,
	E_BDB_TX_POWER_LEVEL_N0p51dBm,
	E_BDB_TX_POWER_LEVEL_N0p74dBm,
	E_BDB_TX_POWER_LEVEL_N1p21dBm,
	E_BDB_TX_POWER_LEVEL_N1p69dBm,
	E_BDB_TX_POWER_LEVEL_N2p23dBm,
	E_BDB_TX_POWER_LEVEL_N2p84dBm,
	E_BDB_TX_POWER_LEVEL_N3p48dBm,
	E_BDB_TX_POWER_LEVEL_N4p18dBm,
	E_BDB_TX_POWER_LEVEL_N4p97dBm,
	E_BDB_TX_POWER_LEVEL_N5p85dBm,
	E_BDB_TX_POWER_LEVEL_N6p83dBm,
	E_BDB_TX_POWER_LEVEL_N7p88dBm,
	E_BDB_TX_POWER_LEVEL_N9p14dBm,
	E_BDB_TX_POWER_LEVEL_N10p70dBm,
	E_BDB_TX_POWER_LEVEL_N12p57dBm,
	E_BDB_TX_POWER_LEVEL_N15p01dBm,
	E_BDB_TX_POWER_LEVEL_N18p40dBm,
	E_BDB_TX_POWER_LEVEL_N24p28dBm,
} te_BdbTxPowerLevel;

typedef union ts_DstAddr
{
    uint16_t u16DstAddr;
    uint64_t u64DstAddr;
} ts_DstAddr;

typedef union t_AttrData
{
    uint8_t  u8AttrData;
    uint16_t u16AttrData;
    uint32_t u32AttrData;
    uint64_t u64AttrData;
    uint8_t  au8AttrData[128];
} t_AttrData;

typedef struct ts_AttrList
{
    uint16_t   u16AttrID;
    uint8_t    u8DataType;
    uint16_t   u16DataLen;
    t_AttrData uAttrData;
} ts_AttrList;


/**
 * @enum te_MsgAckStatus Status message.
 */
typedef enum te_MsgAckStatus
{
    ZBHCI_MSG_STATUS_SUCCESS               = 0x00, /**< Success */
    ZBHCI_MSG_STATUS_INCORRECT_PARAMETERS  = 0x01, /**< Incorrect parameters */
    ZBHCI_MSG_STATUS_UNHANDLED_COMMAND     = 0x02, /**< Unhandled command */
    ZBHCI_MSG_STATUS_BUSY                  = 0x03, /**< Busy */
    ZBHCI_MSG_STATUS_NO_MEMORY             = 0x04, /**< No memory */
    ZBHCI_MSG_STATUS_STACK_ALREADY_STARTED = 0x05, /**< Stack already started */
} te_MsgAckStatus;

typedef struct ts_MsgAckPayload
{
    uint16_t        u16MsgType; /**< HCI command message type. */
    te_MsgAckStatus eStatus;    /**< 0 = Success;
                                     1 = Wrong parameter;
                                     2 = Unsupported command;
                                     3 = Busy;
                                     4 = No memory. */
    uint8_t         u8Reserved; /**< Reserved */
} ts_MsgAckPayload;

typedef struct ts_MsgBdbCommissionFormationRspPayload
{
    uint8_t u8Status;
} ts_MsgBdbCommissionFormationRspPayload;

typedef struct ts_MsgNetworkStateRspPayload
{
    uint16_t u16NwkAddr;
    uint64_t u64IeeeAddr;
    uint16_t u16PanId;
    uint64_t u64extPanId;
    uint8_t  u8Channel;
} ts_MsgNetworkStateRspPayload;


typedef struct ts_MsgDiscoveryNwkAddrRspPayload
{
    uint8_t  u8SeqNum;             /**< ZDP transaction sequence number. */
    uint8_t  u8Status;             /**< The status of the request command. */
    uint64_t u64IEEEAddr;          /**< 64-bit address for the Remote Device. */
    uint64_t u16NwkAddr;           /**< 16-bit address for the Remote Device. */
    uint8_t  u8NumAssocDev;        /**< Count of the number of 16-bit short
                                        address to follow. */
    uint8_t  u8StartIdx;           /**< Starting index into the list of
                                        associated devices for this report. */
    uint16_t  au16AssocDevList[256]; /**< The list of associated devices. */
} ts_MsgDiscoveryNwkAddrRspPayload,
  ts_MsgDiscoveryIEEEAddrRspPayload;

typedef struct ts_MsgDiscoveryNodeDescRspPayload
{
    uint8_t  u8SeqNum;             /**< ZDP transaction sequence number. */
    uint8_t  u8Status;             /**< The status of the request command. */
    uint16_t u16NwkAddrOfInterest; /**< NWK address for the request. */
    uint8_t  au8NodeDesc[256];     /**< This field shall only be included in the
                                        frame if the status field is SUCCESS. */
} ts_MsgDiscoveryNodeDescRspPayload;

typedef struct ts_MsgDiscoverySimpleDescRspPayload
{
    uint8_t  u8SeqNum;             /**< ZDP transaction sequence number. */
    uint8_t  u8Status;             /**< The status of the request command. */
    uint16_t u16NwkAddrOfInterest; /**< NWK address for the request. */
    uint8_t  u8Length;             /**< The length of simple description. */
    uint8_t  au8SimpleDesc[256];   /**< This field shall only be included in the
                                        frame if the status field is SUCCESS. */
} ts_MsgDiscoverySimpleDescRspPayload;

typedef struct ts_MsgDiscoveryMatchDescRspPayload
{
    uint8_t  u8SeqNum;             /**< ZDP transaction sequence number. */
    uint8_t  u8Status;             /**< The status of the request command. */
    uint16_t u16NwkAddrOfInterest; /**< NWK address for the request. */
    uint8_t  u8MatchLen;           /**< The count of endpoints on the Remote
                                        Device that match the request criteria. */
    uint8_t  au8MatchList[256];    /**< List of bytes each of which represents
                                        an 8-bit endpoint. */
} ts_MsgDiscoveryMatchDescRspPayload;

typedef struct ts_MsgDiscoveryActiveEpRspPayload
{
    uint8_t  u8SeqNum;             /**< ZDP transaction sequence number. */
    uint8_t  u8Status;             /**< The status of the request command. */
    uint16_t u16NwkAddrOfInterest; /**< NWK address for the request. */
    uint8_t  u8ActiveEpCount;      /**< The count of active endpoints. */
    uint8_t  au8EpList[256];       /**< List of active endpoints. */
} ts_MsgDiscoveryActiveEpRspPayload;

typedef struct ts_MsgBindRspPayload
{
    uint8_t u8SeqNum; /**< ZDP transaction sequence number. */
    uint8_t u8Status; /**< The status of the request command. */
} ts_MsgBindRspPayload,
  ts_MsgUnbindRspPayload;

typedef struct ts_NeighborTable
{
    uint64_t ext_pan_id;
    uint64_t ext_addr;
    uint16_t network_addr;
    uint8_t  deviceType:2;
    uint8_t  rxOnWhenIdle:2;
    uint8_t  relationship:3;
    uint8_t  reserved1:1;
    uint8_t  permitJoining:2;
    uint8_t  reserved2:6;
    uint8_t  depth;
    uint8_t  lqi;
} ts_NeighborTable;


typedef struct ts_MsgMgmtLqiRspPayload
{
    uint8_t u8SeqNum;                    /**< ZDP transaction sequence number. */
    uint8_t u8Status;                    /**< The status of the request command. */
    uint8_t u8NeighborTabEntries;        /**< Total number of Neighbor Table entries
                                              within the Remote Device. */
    uint8_t u8StartIdx;                  /**< Starting index within the Neighbor
                                              Table to begin reporting for the
                                              neighborTabList. */
    uint8_t u8NeighborTabListCount;      /**< Number of Neighbor Table entries
                                              included within neighborTabList. */
    ts_NeighborTable asNeighborTable[8]; /**< A list of descriptors, beginning with
                                              the startIdx element and continuing
                                              for neighborTabListCount. */
} ts_MsgMgmtLqiRspPayload;

typedef struct ts_BindTabList
{
    uint64_t u64SrcAddr;    /*!< the device who build the binding table */
    uint8_t  u8SrcEndpoint; /*!< The source endpoint for the binding entry */
    uint8_t  u16ClusterId;  /*!< The identifier of the cluster on the source
                                device that is bound to the destination */
    uint8_t u8DstAddrMode;  /*!< destination address mode
                                 0x01 - 16-bit group address for dstAddr and dstEp not present
                                 0x03 - 64-bit extAddr for dstAddr and estEp present */
    union {
        struct {
            uint64_t u64DstExtAddr;
            uint8_t  u8DstEndpoint;
        } sDstExtAddr;
        uint16_t u16DstGroupAddr;
    } uDstAddr;                      /*!< The destination address for the binding entry */
} ts_BindTabList;


typedef struct ts_MsgMgmtBindRspPayload
{
    uint8_t        u8SeqNum;              /**< ZDP transaction sequence number. */
    uint8_t        u8Status;              /**< The status of the request command. */
    uint8_t        u8BindingTabEntries;   /**< Total number of Binding Table entries within the Remote Device. */
    uint8_t        u8StartIdx;            /**< Starting index within the Binding Table to begin reporting for the bindingTabList. */
    uint8_t        u8BindingTabListCount; /**< Number of Binding Table entries included within bindingTabList. */
    ts_BindTabList asBindingTabList[4];   /**< A list of descriptors, beginning with the startIdx element and continuing for bindingTabListCount. */
} ts_MsgMgmtBindRspPayload;

typedef struct ts_MsgMgmtLeaveRspPayload
{
    uint8_t u8SeqNum; /**< ZDP transaction sequence number. */
    uint8_t u8Status; /**< The status of the request command. */
} ts_MsgMgmtLeaveRspPayload;

typedef struct ts_MsgMgmtPermitJoinRspPayload
{
    uint8_t u8SeqNum; /**< ZDP transaction sequence number. */
    uint8_t u8Status; /**< The status of the request command. */
} ts_MsgMgmtPermitJoinRspPayload;

typedef struct ts_MsgNodesJoinedGetRspPayload
{
    uint16_t u16TotalCnt;        /**< The total count of the joined nodes. */
    uint16_t u16StartIdx;        /**< Starting index within the mac address list. */
    uint8_t  u8ListCount;        /**< The count of the MAC address list in the current packet. */
    uint8_t  u8Status;           /**< The status of the request command. */
    uint64_t au64MacAddrList[6]; /**< The MAC address list in the current packet. */
    uint16_t au16ShortAddrList[6]; /**< The MAC address list in the current packet. */
} ts_MsgNodesJoinedGetRspPayload;

typedef struct ts_MsgTxRxPerformceTestRspPayload
{
	uint16_t u16DstAddr;
	uint16_t u16SendCnt;
	uint16_t u16AckCnt;
} ts_MsgTxRxPerformceTestRspPayload;

typedef struct ts_MsgNodesDevAnnceRspPayload
{
    uint16_t u16NwkAddr;   /**< NWK address of the joined device. */
    uint64_t u64IEEEAddr;  /**< IEEE address of the joined device. */
    uint8_t  u8Capability; /**< Capability of the joined device. */
} ts_MsgNodesDevAnnceRspPayload;

typedef struct ts_MsgAfDataSendTestRspPayload
{
    uint16_t u16SrcAddr;
    uint8_t  u8SrcEp;
    uint8_t  u8DstEp;
    uint16_t u16ClusterId;
    uint16_t u16DataLen;
    uint8_t  au8Payload[256];
} ts_MsgAfDataSendTestRspPayload;

typedef struct ts_MsgLeaveIndicationPayload
{
    uint64_t u64MacAddr;
    uint8_t  u8Rejoin;
} ts_MsgLeaveIndicationPayload;

typedef struct ts_AttrRead
{
    uint16_t   u16AttrID;
    uint8_t    u8Status;
    uint8_t    u8DataType;
    uint16_t   u16DataLen;
    t_AttrData uAttrData;
} ts_AttrRead;

typedef struct ts_MsgZclAttrReadRspPayload
{
    uint8_t     u8SeqNum;
    uint16_t    u16SrcAddr;
    uint8_t     u8SrcEp;
    uint16_t    u16ClusterId;
    uint8_t     u8AttrNum;           /**< The number of attributes to be read. */
    ts_AttrRead asAttrReadList[8]; /**< The list of the attributes to be read. */
} ts_MsgZclAttrReadRspPayload;

typedef struct ts_AttrWrite
{
    uint8_t  u8Status;
    uint16_t u16AttrID;
} ts_AttrWrite;

typedef struct ts_MsgZclAttrWriteRspPayload
{
    uint8_t      u8SeqNum;
    uint16_t     u16SrcAddr;
    uint8_t      u8SrcEp;
    uint16_t     u16ClusterId;
    uint8_t      u8AttrNum;              /**< The number of attributes to be written. */
    ts_AttrWrite asAttrWriteList[256]; /**< The list of the attributes to be written. */
} ts_MsgZclAttrWriteRspPayload;

typedef struct ts_AttrConfigReport
{
    uint8_t  u8Status;
    uint8_t  u8ReportDirection;
    uint16_t u16AttrID;
} ts_AttrConfigReport;

typedef struct ts_MsgZclConfigReportRspPayload
{
    uint8_t             u8SeqNum;
    uint16_t            u16SrcAddr;
    uint8_t             u8SrcEp;
    uint16_t            u16ClusterId;
    uint8_t             u8AttrNum;                  /**< TThe number of attributes' reporting to be configured. */
    ts_AttrConfigReport asAttrConfigReportList[16]; /**< The list of the attributes to be configured. */
} ts_MsgZclConfigReportRspPayload;

typedef struct ts_AttrReadConfigReport
{
    uint8_t  u8Status;
    uint8_t  u8ReportDirection;
    uint16_t u16AttrID;
    uint8_t  u8DataType;
    uint16_t u16MinRepInterval;
    uint16_t u16MaxRepInterval;
    uint8_t  au8ReportableChange[128];
    uint16_t u16TimeoutPeriod;
} ts_AttrReadConfigReport;

typedef struct ts_MsgZclReadReportCfgRspPayload
{
    uint8_t                 u8SeqNum;
    uint16_t                u16SrcAddr;      /**< The source address of the reporting message. */
    uint8_t                 u8SrcEp;         /**< The source endpoint of the reporting message. */
    uint16_t                u16ClusterId;
    uint8_t                 u8AttrNum;       /**< The number of attributes' reporting to be read. */
    ts_AttrReadConfigReport asAttrList[8];   /**< The list of the attributes to be read. */
} ts_MsgZclReadReportCfgRspPayload;

typedef struct ts_MsgZclReportMsgRcvPayload
{
    uint8_t     u8SeqNum;
    uint16_t    u16SrcAddr;      /**< The source address of the reporting
                                      message. */
    uint8_t     u8SrcEp;         /**< The source endpoint of the reporting
                                      message. */
    uint8_t     u8DstEp;
    uint16_t    u16ClusterId;
    uint8_t     u8AttrNum;       /**< The number of attributes' reporting
                                      message to be received. */
    ts_AttrList asAttrList[4]; /**< The list of the attributes' reporting
                                      message to be received. */
} ts_MsgZclReportMsgRcvPayload;

typedef struct ts_MsgZclGroupAddRspPayload
{
    uint8_t u8Status;    /**< The status field is set to SUCCESS,
                              DUPLICATE_EXISTS, or INSUFFICIENT_SPACE as
                              appropriate. */
    uint16_t u16GroupId; /**< Group identifier. */
} ts_MsgZclGroupAddRspPayload;


typedef struct ts_MsgZclGroupViewRspPayload
{
    uint8_t  u8Status;          /**< The status field is set to SUCCESS,
                                     DUPLICATE_EXISTS, or INSUFFICIENT_SPACE as
                                     appropriate. */
    uint16_t u16GroupId;        /**< Group identifier. */
    uint8_t  u8GroupNameLength; /**< */
    uint8_t  au8GroupName[256]; /**< Group name, character string. */
} ts_MsgZclGroupViewRspPayload;


typedef struct ts_MsgZclGroupGetMembershipRspPayload
{
    uint8_t u8Capability;    /**< The remaining capability of the group table of the device. */
    uint8_t u8GroupCount;    /**< The number of groups contained in the group list field. */
    uint16_t au16GroupId[32]; /**< The list of groupId in the group list field. */
} ts_MsgZclGroupGetMembershipRspPayload;

typedef struct ts_MsgZclGroupRemoveRspPayload
{
    uint8_t u8Status;    /**< The status field is set to SUCCESS,
                              DUPLICATE_EXISTS, or INSUFFICIENT_SPACE as
                              appropriate. */
    uint16_t u16GroupId; /**< Group identifier. */
} ts_MsgZclGroupRemoveRspPayload;

typedef struct ts_MsgZclIdentifyQueryRspPayload
{
    uint16_t u16ShortAddr; /**< The short address of the device. */
    uint8_t  u8SrcEp;      /**< The source endpoint of the device. */
    uint16_t u16Timeout;   /**< The remaining time. */
} ts_MsgZclIdentifyQueryRspPayload;

typedef struct ts_MsgZclOnOffCmdRcvPayload
{
    uint8_t  u8SrcEp;
    uint8_t  u8DstEp;
    uint16_t u16ClusterId;
    uint8_t  u8CmdId;
} ts_MsgZclOnOffCmdRcvPayload;

typedef struct ts_MsgZclSceneAddRspPayload
{
    uint8_t  u8Status;   /**< SUCCESS, INSUFFICIENT_SPACE or INVALID_FIELD (the
                              group is not present in the group table). */
    uint16_t u16GroupId; /**< The group ID for which this scene applies. */
    uint8_t  u8SceneId;  /**< The identifier, unique within this group, which is
                              used to identify this scene. */
} ts_MsgZclSceneAddRspPayload;

typedef struct ts_MsgZclSceneViewRspPayload
{
    uint8_t  u8Status;       /**< SUCCESS, INSUFFICIENT_SPACE or INVALID_FIELD (the
                                  group is not present in the group table). */
    uint16_t u16GroupId;     /**< The group ID for which this scene applies. */
    uint8_t  u8SceneId;      /**< The identifier, unique within this group, which is
                                  used to identify this scene. */
    uint16_t u16TransTime;   /**< Transition time copied from scene table entry. */
    uint8_t  u8SceneNameLength;
    uint8_t  au8SceneName[32];    /**< Scene name copied from scene table entry. First
                                  byte is the length of the scene name. */
    uint8_t  extFieldLength;
    uint8_t  au8ExtFieldSets[32]; /**< Extension field sets copied from scene table
                                  entry. First byte is the length of the
                                  extension field sets. */
} ts_MsgZclSceneViewRspPayload;

typedef struct ts_MsgZclSceneRemoveRspPayload
{
    uint8_t  u8Status;   /**< SUCCESS, INSUFFICIENT_SPACE or INVALID_FIELD (the
                              group is not present in the group table). */
    uint16_t u16GroupId; /**< The group ID for which this scene applies. */
    uint8_t  u8SceneId;  /**< The identifier, unique within this group, which is
                              used to identify this scene. */
} ts_MsgZclSceneRemoveRspPayload;

typedef struct ts_MsgZclSceneRemoveAllRspPayload
{
    uint8_t  u8Status;   /**< SUCCESS, INSUFFICIENT_SPACE or INVALID_FIELD (the
                              group is not present in the group table). */
    uint16_t u16GroupId; /**< The group ID for which this scene applies. */
} ts_MsgZclSceneRemoveAllRspPayload;

typedef struct ts_MsgZclSceneStoreRspPayload
{
    uint8_t  u8Status;   /**< SUCCESS, INSUFFICIENT_SPACE or INVALID_FIELD (the
                              group is not present in the group table). */
    uint16_t u16GroupId; /**< The group ID for which this scene applies. */
    uint8_t  u8SceneId;  /**< The identifier, unique within this group, which is
                              used to identify this scene. */
} ts_MsgZclSceneStoreRspPayload;

typedef struct ts_MsgZclSceneGetMenbershipRspPayload
{
    uint8_t  u8Status;           /**< SUCCESS or INVALID_FIELD (the group is not
                                      present in the group table). */
    uint8_t  u8Capability;       /**< Contain the remaining capacity of the
                                      scene table of the device. */
    uint16_t u16GroupId;         /**< The group ID for which this scene applies. */
    uint8_t  u8SceneCnt;         /**< The number of scenes contained in the
                                      scene list field. */
    uint8_t  au8SceneList[256];  /**< Contain the identifiers of all the scenes
                                      in the scene table with the corresponding
                                      Group ID. */
} ts_MsgZclSceneGetMenbershipRspPayload;

typedef struct ts_MsgDataConfirmPayload
{
    uint8_t u8DstAddrMode;
    uint8_t u8SrcEndpoint;
    uint8_t u8DstEndpoint;
    struct {
        uint64_t u64DstExtAddr;
        uint16_t u16DstShortAddr;
    } sDstExtAddr;
    uint16_t u16ClusterId;
    uint8_t u8Status;
    uint8_t u8ApsCnt;
} ts_MsgDataConfirmPayload;

typedef struct ts_MsgMacAddrIndPayload
{
    uint64_t u64DeviceExtAddr;
} ts_MsgMacAddrIndPayload;

typedef struct ts_MsgNodeLeaveIndPayload
{
    uint16_t u16TotalCnt;
    uint64_t u64MacAddr;
} ts_MsgNodeLeaveIndPayload;

typedef struct ts_HciMsg
{
    uint16_t u16MsgType;
    uint16_t u16MsgLength;
    union
    {
        ts_MsgAckPayload                       sAckPayload;
        ts_MsgBdbCommissionFormationRspPayload sBdbCommissionFormationRspPayload;
        ts_MsgNetworkStateRspPayload           sNetworkStateRspPayloasd;
        ts_MsgDiscoveryNwkAddrRspPayload       sDiscoveryNwkAddrRspPayload;
        ts_MsgDiscoveryIEEEAddrRspPayload      sDiscoveryIEEEAddrRspPayload;
        ts_MsgDiscoveryNodeDescRspPayload      sDiscoveryNodeDescRspPayload;
        ts_MsgDiscoverySimpleDescRspPayload    sDiscoverySimpleDescRspPayload;
        ts_MsgDiscoveryMatchDescRspPayload     sDiscoveryMatchDescRspPayload;
        ts_MsgDiscoveryActiveEpRspPayload      sDiscoveryActiveEpRspPayload;
        ts_MsgBindRspPayload                   sBindRspPayload;
        ts_MsgUnbindRspPayload                 sUnbindRspPayload;
        ts_NeighborTable                       sghborTable;
        ts_MsgMgmtLqiRspPayload                sMgmtLqiRspPayload;
        ts_MsgMgmtBindRspPayload               sMgmtBindRspPayload;
        ts_MsgMgmtLeaveRspPayload              sMgmtLeaveRspPayload;
        ts_MsgMgmtPermitJoinRspPayload         sMgmtPermitJoinRspPayload;
        ts_MsgNodesJoinedGetRspPayload         sNodesJoinedGetRspPayload;
        ts_MsgTxRxPerformceTestRspPayload      sTxRxPerformceTestRspPayload;
        ts_MsgNodesDevAnnceRspPayload          sNodesDevAnnceRspPayload;
        ts_MsgAfDataSendTestRspPayload         sAfDataSendTestRspPayload;
        ts_MsgLeaveIndicationPayload           sLeaveIndicationPayload;
        ts_MsgZclAttrReadRspPayload            sZclAttrReadRspPayload;
        ts_MsgZclAttrWriteRspPayload           sZclAttrWriteRspPayload;
        ts_MsgZclConfigReportRspPayload        sZclConfigReportRspPayload;
        ts_MsgZclReadReportCfgRspPayload       sZclReadReportCfgRspPayload;
        ts_MsgZclReportMsgRcvPayload           sZclReportMsgRcvPayload;
        ts_MsgZclGroupAddRspPayload            sZclGroupAddRspPayload;
        ts_MsgZclGroupViewRspPayload           sZclGroupViewRspPayload;
        ts_MsgZclGroupGetMembershipRspPayload  sZclGroupGetMembershipRspPayload;
        ts_MsgZclGroupRemoveRspPayload         sZclGroupRemoveRspPayload;
        ts_MsgZclIdentifyQueryRspPayload       sZclIdentifyQueryRspPayload;
        ts_MsgZclOnOffCmdRcvPayload            sZclOnOffCmdRcvPayload;
        ts_MsgZclSceneAddRspPayload            sZclSceneAddRspPayload;
        ts_MsgZclSceneViewRspPayload           sZclSceneViewRspPayload;
        ts_MsgZclSceneRemoveRspPayload         sZclSceneRemoveRspPayload;
        ts_MsgZclSceneRemoveAllRspPayload      sZclSceneRemoveAllRspPayload;
        ts_MsgZclSceneStoreRspPayload          sZclSceneStoreRspPayload;
        ts_MsgZclSceneGetMenbershipRspPayload  sZclSceneGetMenbershipRspPayload;
        ts_MsgDataConfirmPayload               sDataConfirmPayload;
        ts_MsgMacAddrIndPayload                sMacAddrIndPayload;
        ts_MsgNodeLeaveIndPayload              sNodeLeaveIndPayload;
    } uPayload;
} ts_HciMsg;

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/