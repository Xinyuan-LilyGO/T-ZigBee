
/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include "hci_display.h"

#ifdef ARDUINO
#include <Arduino.h>
#else
#include "esp_log.h"
#endif

#include <stdio.h>
#include <stddef.h>

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

#ifdef ARDUINO
#define LOGI(format, args...) printf(format"\n", ##args)
#else
#define LOGI(format, args...) ESP_LOGI("zbhci", format, ##args)
#endif

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

/******************************************************************************/
/***        local function prototypes                                       ***/
/******************************************************************************/

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        local variables                                                 ***/
/******************************************************************************/

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/


void displayAcknowledg(ts_MsgAckPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ACKNOWLEDGE);
    LOGI("  (Status)");
    LOGI("  Status:  %#02x", psPayload->eStatus);
    LOGI("  Message: %#04x", psPayload->u16MsgType);
}


void displayBdbCommissionFormationRsp(ts_MsgBdbCommissionFormationRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_BDB_COMMISSION_FORMATION_RSP);
    LOGI("  (Bdb Commission Formation Rsp)");
    LOGI("  Status: %#02x", psPayload->u8Status);
}


void displayNetworkStateRsp(ts_MsgNetworkStateRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_NETWORK_STATE_RSP);
    LOGI("  (Network State Rsp)");
    LOGI("  Nwk Addr:   %#04x",    psPayload->u16NwkAddr);
    LOGI("  IEEE Addr:  %#016llx", psPayload->u64IeeeAddr);
    LOGI("  Pan Id:     %#04x",    psPayload->u16PanId);
    LOGI("  Ext Pan Id: %#016llx", psPayload->u64extPanId);
    LOGI("  Channel:    %#02x",    psPayload->u8Channel);
}


void displayDiscoveryNwkAddrRsp(ts_MsgDiscoveryNwkAddrRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_DISCOVERY_NWK_ADDR_RSP);
    LOGI("  (Nwk Address Rsp)");
    LOGI("  SQN:                    %#02x",    psPayload->u8SeqNum);
    LOGI("  Status:                 %#02x",    psPayload->u8Status);
    LOGI("  IEEE Address:           %#016llx", psPayload->u64IEEEAddr);
    LOGI("  Nwk Address:            %#04x",    psPayload->u16NwkAddr);
    if (psPayload->u8NumAssocDev)
    {
        LOGI("  Num Associated Devices: %#02x", psPayload->u8NumAssocDev);
        LOGI("  Start Index:            %#02x", psPayload->u8StartIdx);
        LOGI("  Associated Devices List:");
        for (size_t i = 0; i < psPayload->u8NumAssocDev; i++)
        {
            LOGI("    Devices %d Nwk Address: %#02x", i, psPayload->au16AssocDevList[i]);
        }
    }
}


void displayDiscoveryIEEEAddrRsp(ts_MsgDiscoveryIEEEAddrRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_DISCOVERY_IEEE_ADDR_RSP);
    LOGI("  (IEEE Address Rsp)");
    LOGI("  SQN:                    %#02x",    psPayload->u8SeqNum);
    LOGI("  Status:                 %#02x",    psPayload->u8Status);
    LOGI("  IEEE Address:           %#016llx", psPayload->u64IEEEAddr);
    LOGI("  Nwk Address:            %#04x",    psPayload->u16NwkAddr);
    if (psPayload->u8NumAssocDev)
    {
        LOGI("  Num Associated Devices: %#02x", psPayload->u8NumAssocDev);
        LOGI("  Start Index:            %#02x", psPayload->u8StartIdx);
        LOGI("  Associated Devices List:");
        for (size_t i = 0; i < psPayload->u8NumAssocDev; i++)
        {
            LOGI("    Devices %d Nwk Address: %#02x", i, psPayload->au16AssocDevList[i]);
        }
    }
}


void displayDiscoveryNodeDescRsp(ts_MsgDiscoveryNodeDescRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_DISCOVERY_NODE_DESC_RSP);
    LOGI("  (Node Desc Rsp)");
    LOGI("  SQN:          %#02x", psPayload->u8SeqNum);
    LOGI("  Status:       %#02x", psPayload->u8Status);
    LOGI("  IEEE Address: %#04x", psPayload->u16NwkAddrOfInterest);
    LOGI("  Node Desc:    %s",   psPayload->au8NodeDesc);
}


void displayDiscoverySimpleDescRsp(ts_MsgDiscoverySimpleDescRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_DISCOVERY_SIMPLE_DESC_RSP);
    LOGI("  (Simple Desc Rsp)");
    LOGI("  SQN:          %#02x", psPayload->u8SeqNum);
    LOGI("  Status:       %#02x", psPayload->u8Status);
    LOGI("  Nwk Address:  %#04x", psPayload->u16NwkAddrOfInterest);
    LOGI("  Desc Len:     %#02x", psPayload->u8Length);
    LOGI("  Simple Desc:  %s",   psPayload->au8SimpleDesc);
}


void displayDiscoveryMatchDescRsp(ts_MsgDiscoveryMatchDescRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_DISCOVERY_MATCH_DESC_RSP);
    LOGI("  (Match Desc Rsp)");
    LOGI("  SQN:         %#02x", psPayload->u8SeqNum);
    LOGI("  Status:      %#02x", psPayload->u8Status);
    LOGI("  Nwk Address: %#04x", psPayload->u16NwkAddrOfInterest);
    LOGI("  Match Len:   %#02x", psPayload->u8MatchLen);
    if (psPayload->u8MatchLen)
    {
        LOGI("  Match Ep List:");
        for (size_t i = 0; i < psPayload->u8MatchLen; i++)
        {
            LOGI("    Match Ep:  %#02x", psPayload->au8MatchList[i]);
        }
    }
}


void displayDiscoveryActiveEpRsp(ts_MsgDiscoveryActiveEpRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_DISCOVERY_ACTIVE_EP_RSP);
    LOGI("  (Active Ep Rsp)");
    LOGI("  SQN:             %#02x", psPayload->u8SeqNum);
    LOGI("  Status:          %#02x", psPayload->u8Status);
    LOGI("  Nwk Address:     %#04x", psPayload->u16NwkAddrOfInterest);
    LOGI("  Active Ep Count: %#02x", psPayload->u8ActiveEpCount);
    if (psPayload->u8ActiveEpCount)
    {
        LOGI("  Active Ep List:");
        for (size_t i = 0; i < psPayload->u8ActiveEpCount; i++)
        {
            LOGI("    Active Ep %d:  %#02x", i, psPayload->au8EpList[i]);
        }
    }
}


void displayBindingRsp(ts_MsgBindRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_BINDING_RSP);
    LOGI("  (Bind Rsp)");
    LOGI("  SQN:    %#02x", psPayload->u8SeqNum);
    LOGI("  Status: %#02x", psPayload->u8Status);
}


void displayUnbindingRsp(ts_MsgUnbindRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_UNBINDING_RSP);
    LOGI("  (Unbind Rsp)");
    LOGI("  SQN:    %#02x", psPayload->u8SeqNum);
    LOGI("  Status: %#02x", psPayload->u8Status);
}


void displayMgmtLqiRsp(ts_MsgMgmtLqiRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_MGMT_LQI_RSP);
    LOGI("  (MGMT LQI Rsp)");
    LOGI("  SQN:                     %#02x", psPayload->u8SeqNum);
    LOGI("  Status:                  %#02x", psPayload->u8Status);
    LOGI("  Neighbor Tab Entries:    %#02x", psPayload->u8NeighborTabEntries);
    LOGI("  Start Index:             %#02x", psPayload->u8StartIdx);
    LOGI("  Neighbor Tab List Count: %#02x", psPayload->u8NeighborTabListCount);
    if (psPayload->u8NeighborTabListCount)
    {
        LOGI("  Neighbor Tab List:");
        for (size_t i = 0; i < psPayload->u8NeighborTabListCount; i++)
        {
            LOGI("    Neighbor %d:", i);
            LOGI("      Ext Pan ID:      %#016llx", psPayload->asNeighborTable[i].ext_pan_id);
            LOGI("      Ext Address:     %#016llx", psPayload->asNeighborTable[i].ext_addr);
            LOGI("      Nwk Address:     %#04x",    psPayload->asNeighborTable[i].network_addr);
            LOGI("      Device Type:     %#02x",    psPayload->asNeighborTable[i].deviceType);
            LOGI("      Rx On When Idle: %#02x",    psPayload->asNeighborTable[i].rxOnWhenIdle);
            LOGI("      Relationship:    %#02x",    psPayload->asNeighborTable[i].relationship);
            LOGI("      Permit Joining:  %#02x",    psPayload->asNeighborTable[i].permitJoining);
            LOGI("      Depth:           %#02x",    psPayload->asNeighborTable[i].depth);
            LOGI("      LQI:             %#02x",    psPayload->asNeighborTable[i].lqi);
        }
    }
}


void displayMgmtBindRsp(ts_MsgMgmtBindRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_MGMT_BIND_RSP);
    LOGI("  (MGMT Bind Rsp)");
    LOGI("  SQN:                    %#02x", psPayload->u8SeqNum);
    LOGI("  Status:                 %#02x", psPayload->u8Status);
    LOGI("  Binding Tab Entries:    %#02x", psPayload->u8BindingTabEntries);
    LOGI("  Start Idx:              %#02x", psPayload->u8StartIdx);
    LOGI("  Binding Tab List Count: %#02x", psPayload->u8BindingTabListCount);
    if (psPayload->u8BindingTabListCount)
    {
        LOGI("  Binding Tab List:");
        for (size_t i = 0; i < psPayload->u8BindingTabListCount; i++)
        {
            LOGI("    Src Addr:       %#016llx", psPayload->asBindingTabList[i].u64SrcAddr);
            LOGI("    Src Ep:         %#02x", psPayload->asBindingTabList[i].u8SrcEndpoint);
            LOGI("    Cluster ID:     %#04x", psPayload->asBindingTabList[i].u16ClusterId);
            LOGI("    Dst Addr Mode:  %#02x", psPayload->asBindingTabList[i].u8DstAddrMode);
            if (psPayload->asBindingTabList[i].u8DstAddrMode == 0x01)
            {
                LOGI("    Dst Group Addr: %#04x", psPayload->asBindingTabList[i].uDstAddr.u16DstGroupAddr);
            }
            else if (psPayload->asBindingTabList[i].u8DstAddrMode == 0x03)
            {
                LOGI("    Dst Ext Addr:   %#016llx", psPayload->asBindingTabList[i].uDstAddr.sDstExtAddr.u64DstExtAddr);
                LOGI("    Dst Ep:         %#02x",    psPayload->asBindingTabList[i].uDstAddr.sDstExtAddr.u8DstEndpoint);
            }
        }
    }
}


void displayMgmtLeaveRsp(ts_MsgMgmtLeaveRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_MGMT_LEAVE_RSP);
    LOGI("  (MGMT Leave Rsp)");
    LOGI("  SQN:    %#02x", psPayload->u8SeqNum);
    LOGI("  Status: %#02x", psPayload->u8Status);
}


void displayMgmtDirectJoinRsp()
{
    /** None */
}


void displayMgmtPermitJoinRsp(ts_MsgMgmtPermitJoinRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_MGMT_PERMIT_JOIN_RSP);
    LOGI("  (MGMT Permit Join Rsp)");
    LOGI("  SQN:    %#02x", psPayload->u8SeqNum);
    LOGI("  Status: %#02x", psPayload->u8Status);
}


void displayMgmtNwkUpdateRsp()
{
    /** None */
}


void displayNodesJoinedGetRsp(ts_MsgNodesJoinedGetRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_NODES_JOINED_GET_RSP);
    LOGI("  (Nodes Joined Get Rsp)");
    LOGI("  Status:     %#02x", psPayload->u8Status);
    LOGI("  Total:      %#02x", psPayload->u8TotalCnt);
    LOGI("  Start Idx:  %#02x", psPayload->u8StartIdx);
    LOGI("  List Count: %#02x", psPayload->u8ListCount);
    if (psPayload->u8ListCount)
    {
        LOGI("  Mac Addr List:");
        for (size_t i = 0; i < psPayload->u8ListCount; i++)
        {
            LOGI("    Node %d: %#016llx", i, psPayload->au64MacAddrList[i]);
        }
    }
}


void displayNodesTogleTestRsp()
{
    /** None */
}


void displayTxRxPerformanceTestRsp(ts_MsgTxRxPerformceTestRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_TXRX_PERFORMANCE_TEST_RSP);
    LOGI("  (TxRx Performce Test Rsp)");
    LOGI("  Dst Addr: %#04x", psPayload->u16DstAddr);
    LOGI("  Send Cnt: %#04x", psPayload->u16SendCnt);
    LOGI("  Ack Cnt:  %#04x", psPayload->u16AckCnt);
}


void displayNodesDevAnnceInd(ts_MsgNodesDevAnnceRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_NODES_DEV_ANNCE_IND);
    LOGI("  (Nodes Dev Annce Rsp)");
    LOGI("  Nwk Addr:   %#04x",    psPayload->u16NwkAddr);
    LOGI("  IEEE Addr:  %#016llx", psPayload->u64IEEEAddr);
    // LOGI("  Capability: %#02x",    psPayload->u8Capability);
    LOGI("  Capability:");
    LOGI("    Coordinator:           %#02x", psPayload->u8Capability & 0b00000001);
    LOGI("    Devive Type:           %#02x", psPayload->u8Capability & 0b00000010);
    LOGI("    Power Source:          %#02x", psPayload->u8Capability & 0b00000100);
    LOGI("    Receiver on when idle: %#02x", psPayload->u8Capability & 0b00001000);
    LOGI("    Security capability:   %#02x", psPayload->u8Capability & 0b01000000);
    LOGI("    Allocate address:      %#02x", psPayload->u8Capability & 0b10000000);
}


void displayAfDataSendTestRsp(ts_MsgAfDataSendTestRspPayload *psPayload)
{
    char str[256] = { 0 };
    uint8_t size = 0;

    LOGI("Type: %#04x", ZBHCI_CMD_AF_DATA_SEND_TEST_RSP);
    LOGI("  (AF DATA SEND TEST Rsp)");
    LOGI("  Src Addr:   %#04x", psPayload->u16SrcAddr);
    LOGI("  Src Ep:     %#02x", psPayload->u8SrcEp);
    LOGI("  Dst Ep:     %#02x", psPayload->u8DstEp);
    LOGI("  Cluster ID: %#04x", psPayload->u16ClusterId);
    LOGI("  Data Len:   %#04x", psPayload->u16DataLen);
    for (size_t i = 0; i < psPayload->u16DataLen; i++)
    {
        size += snprintf(&str[size], sizeof(str) - 1, "%#02x", psPayload->au8Payload[i]);
    }
    LOGI("  Payload:    %s",   str);
}


void displayLeaveIndication(ts_MsgLeaveIndicationPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_LEAVE_INDICATION);
    LOGI("  (Leave Indication)");
    LOGI("  Mac Addr: %#04x", psPayload->u64MacAddr);
    LOGI("  Rejoin:   %#02x", psPayload->u8Rejoin);
}


void displayZclAttrReadRsp(ts_MsgZclAttrReadRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_ATTR_READ_RSP);
    LOGI("  (ZCL Attr Read Rsp)");
    LOGI("  SQN:        %#02x", psPayload->u8SeqNum);
    LOGI("  Src Addr:   %#04x", psPayload->u16SrcAddr);
    LOGI("  Src Ep:     %#02x", psPayload->u8SrcEp);
    LOGI("  Cluster ID: %#04x", psPayload->u16ClusterId);
    LOGI("  Attr Num:   %#04x", psPayload->u8AttrNum);
    if (psPayload->u8AttrNum)
    {
        LOGI("  Attr List:");
        for (size_t i = 0; i < psPayload->u8AttrNum; i++)
        {
            LOGI("    Attr %d:", i);
            LOGI("      Attr ID:   %#04x", psPayload->asAttrReadList[i].u16AttrID);
            LOGI("      Data Type: %#02x", psPayload->asAttrReadList[i].u8DataType);
            LOGI("      Data Len:  %#04x", psPayload->asAttrReadList[i].u16DataLen);
            if((psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_LONG_CHAR_STR)  || \
               (psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_LONG_OCTET_STR) || \
               (psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_CHAR_STR)       || \
               (psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_OCTET_STR))
            {
                LOGI("      Attr Data: %s", psPayload->asAttrReadList[i].uAttrData.au8AttrData);
            }
            else if(psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_BOOLEAN || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_DATA8   || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_BITMAP8 || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_UINT8   || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_INT8    || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_ENUM8)
            {
                LOGI("      Attr Data: %#02x", psPayload->asAttrReadList[i].uAttrData.u8AttrData);
            }
            else if(psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_DATA16     || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_BITMAP16   || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_UINT16     || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_INT16      || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_ENUM16     || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_SEMI_PREC  || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_CLUSTER_ID || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_ATTR_ID)
            {
                LOGI("      Attr Data: %#04x", psPayload->asAttrReadList[i].uAttrData.u16AttrData);
            }
            else if(psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_DATA24      || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_BITMAP24    || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_UINT24      || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_INT24       || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_DATA32      || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_BITMAP32    || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_UINT32      || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_INT32       || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_SINGLE_PREC || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_UTC         || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_DATE        || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_BAC_OID)
            {
                LOGI("      Attr Data: %#08x", psPayload->asAttrReadList[i].uAttrData.u32AttrData);
            }
            else if(psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_DATA40      || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_BITMAP40    || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_UINT40      || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_INT40       || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_DATA48      || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_BITMAP48    || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_UINT48      || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_INT48       || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_DATA56      || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_BITMAP56    || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_UINT56      || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_INT56       ||\
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_DATA64      || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_BITMAP64    || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_UINT64      || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_INT64       || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_DOUBLE_PREC || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_IEEE_ADDR)
            {
                LOGI("      Attr Data: %#016llx", psPayload->asAttrReadList[i].uAttrData.u64AttrData);
            }
        }
    }
}


void displayZclAttrWriteRsp(ts_MsgZclAttrWriteRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_CONFIG_REPORT_RSP);
    LOGI("  (ZCL Attr Write Rsp)");
    LOGI("  SQN:        %#02x", psPayload->u8SeqNum);
    LOGI("  Src Addr:   %#04x", psPayload->u16SrcAddr);
    LOGI("  Src Ep:     %#02x", psPayload->u8SrcEp);
    LOGI("  Cluster ID: %#04x", psPayload->u16ClusterId);
    LOGI("  Attr Num:   %#04x", psPayload->u8AttrNum);
    if (psPayload->u8AttrNum)
    {
        LOGI("  Attr List:");
        for (size_t i = 0; i < psPayload->u8AttrNum; i++)
        {
            LOGI("    Attr %d:", i);
            LOGI("      Status:  %#02x", psPayload->asAttrWriteList[i].u8Status);
            LOGI("      Attr ID: %#04x", psPayload->asAttrWriteList[i].u16AttrID);
        }
    }
}


void displayZclConfigReportRsp(ts_MsgZclConfigReportRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_CONFIG_REPORT_RSP);
    LOGI("  (ZCL Config Report Rsp)");
    LOGI("  SQN:        %#02x", psPayload->u8SeqNum);
    LOGI("  Src Addr:   %#04x", psPayload->u16SrcAddr);
    LOGI("  Src Ep:     %#02x", psPayload->u8SrcEp);
    LOGI("  Cluster ID: %#04x", psPayload->u16ClusterId);
    LOGI("  Attr Num:   %#04x", psPayload->u8AttrNum);
    if (psPayload->u8AttrNum)
    {
        LOGI("  Attr List:");
        for (size_t i = 0; i < psPayload->u8AttrNum; i++)
        {
            LOGI("    Attr %d:", i);
            LOGI("      Status:           %#02x", psPayload->asAttrConfigReportList[i].u8Status);
            LOGI("      Report Direction: %#02x", psPayload->asAttrConfigReportList[i].u8ReportDirection);
            LOGI("      Attr ID:          %#04x", psPayload->asAttrConfigReportList[i].u16AttrID);
        }
    }
}


void displayZclReadReportCfgRsp(ts_MsgZclReadReportCfgRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_READ_REPORT_CFG_RSP);
    LOGI("  (ZCL Config Report Rsp)");
    LOGI("  SQN:        %#02x", psPayload->u8SeqNum);
    LOGI("  Src Addr:   %#04x", psPayload->u16SrcAddr);
    LOGI("  Src Ep:     %#02x", psPayload->u8SrcEp);
    LOGI("  Cluster ID: %#04x", psPayload->u16ClusterId);
    LOGI("  Attr Num:   %#04x", psPayload->u8AttrNum);
    if (psPayload->u8AttrNum)
    {
        LOGI("  Attr List:");
        for (size_t i = 0; i < psPayload->u8AttrNum; i++)
        {
            LOGI("    Attr %d:", i);
            LOGI("      Status:           %#02x", psPayload->asAttrList[i].u8Status);
            LOGI("      Report Direction: %#02x", psPayload->asAttrList[i].u8ReportDirection);
            LOGI("      Attr ID:          %#04x", psPayload->asAttrList[i].u16AttrID);

            if (psPayload->asAttrList[i].u8ReportDirection == 0x00)
            {
                LOGI("      Data Type:        %#02x", psPayload->asAttrList[i].u8DataType);
                LOGI("      Min Rep Interval: %#04x", psPayload->asAttrList[i].u16MinRepInterval);
                LOGI("      Max Rep Interval: %#04x", psPayload->asAttrList[i].u16MaxRepInterval);
            }
            else
            {
                LOGI("      Timeout Period:   %#04x", psPayload->asAttrList[i].u16TimeoutPeriod);
            }
        }
    }
}


void displayZclReportMsgRcv(ts_MsgZclReportMsgRcvPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_REPORT_MSG_RCV);
    LOGI("  (ZCL Attr Report)");
    LOGI("  SQN:        %#02x", psPayload->u8SeqNum);
    LOGI("  Src Addr:   %#04x", psPayload->u16SrcAddr);
    LOGI("  Src Ep:     %#02x", psPayload->u8SrcEp);
    LOGI("  Cluster ID: %#04x", psPayload->u16ClusterId);
    LOGI("  Attr Num:   %#04x", psPayload->u8AttrNum);
    if (psPayload->u8AttrNum)
    {
        LOGI("  Attr List:");
        for (size_t i = 0; i < psPayload->u8AttrNum; i++)
        {
            LOGI("    Attr %d:", i);
            LOGI("      Attr ID:   %#04x", psPayload->asAttrList[i].u16AttrID);
            LOGI("      Data Type: %#02x", psPayload->asAttrList[i].u8DataType);
            LOGI("      Data Len:  %#04x", psPayload->asAttrList[i].u16DataLen);
            if((psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_LONG_CHAR_STR)  || \
               (psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_LONG_OCTET_STR) || \
               (psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_CHAR_STR)       || \
               (psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_OCTET_STR))
            {
                LOGI("      Attr Data: %s", psPayload->asAttrList[i].uAttrData.au8AttrData);
            }
            else if(psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_BOOLEAN || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_DATA8   || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_BITMAP8 || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_UINT8   || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_INT8    || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_ENUM8)
            {
                LOGI("      Attr Data: %#02x", psPayload->asAttrList[i].uAttrData.u8AttrData);
            }
            else if(psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_DATA16     || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_BITMAP16   || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_UINT16     || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_INT16      || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_ENUM16     || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_SEMI_PREC  || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_CLUSTER_ID || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_ATTR_ID)
            {
                LOGI("      Attr Data: %#04x", psPayload->asAttrList[i].uAttrData.u16AttrData);
            }
            else if(psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_DATA24      || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_BITMAP24    || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_UINT24      || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_INT24       || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_DATA32      || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_BITMAP32    || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_UINT32      || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_INT32       || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_SINGLE_PREC || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_UTC         || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_DATE        || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_BAC_OID)
            {
                LOGI("      Attr Data: %#08x", psPayload->asAttrList[i].uAttrData.u32AttrData);
            }
            else if(psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_DATA40      || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_BITMAP40    || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_UINT40      || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_INT40       || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_DATA48      || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_BITMAP48    || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_UINT48      || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_INT48       || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_DATA56      || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_BITMAP56    || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_UINT56      || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_INT56       ||\
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_DATA64      || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_BITMAP64    || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_UINT64      || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_INT64       || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_DOUBLE_PREC || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_IEEE_ADDR)
            {
                LOGI("      Attr Data: %#016llx", psPayload->asAttrList[i].uAttrData.u64AttrData);
            }
        }
    }
}


void displayZclGroupAddRsp(ts_MsgZclGroupAddRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_GROUP_ADD_RSP);
    LOGI("  (ZCL Group Add Rsp)");
    LOGI("  Status:   %#02x", psPayload->u8Status);
    LOGI("  Group ID: %#04x", psPayload->u16GroupId);
}


void displayZclGroupViewRsp(ts_MsgZclGroupViewRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_GROUP_VIEW_RSP);
    LOGI("  (ZCL Group View Rsp)");
    LOGI("  Status:     %#02x", psPayload->u8Status);
    LOGI("  Group ID:   %#04x", psPayload->u16GroupId);
    LOGI("  Length:     %#02x", psPayload->u8GroupNameLength);
    LOGI("  Group Name: %s",   psPayload->u8GroupNameLength);
}


void displayZclGroupGetMembershipRsp(ts_MsgZclGroupGetMembershipRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_GROUP_GET_MEMBERSHIP_RSP);
    LOGI("  (ZCL Group Get Membership Rsp)");
    LOGI("  Group Capability: %#02x", psPayload->u8Capability);
    LOGI("  Group Count:      %#04x", psPayload->u8GroupCount);

    if (psPayload->u8GroupCount) 
    {
        LOGI("  Membership List:");
        for (size_t i = 0; i < psPayload->u8GroupCount; i++)
        {
            LOGI(    "Membership %d: %#04x", i, psPayload->au16GroupId[i]);
        }
    }
}


void displayZclGroupRemoveRsp(ts_MsgZclGroupRemoveRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_GROUP_REMOVE_RSP);
    LOGI("  (ZCL Group Remove Rsp)");
    LOGI("  Status:   %#02x", psPayload->u8Status);
    LOGI("  Group ID: %#04x", psPayload->u16GroupId);
}


void displayZclIdentifyQueryRsp(ts_MsgZclIdentifyQueryRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_IDENTIFY_QUERY_RSP);
    LOGI("  (ZCL Identify Query Rsp)");
    LOGI("  Nwk Addr: %#04x", psPayload->u16ShortAddr);
    LOGI("  Time Out: %#04x", psPayload->u16Timeout);
}


void displayZclOnOffCmdRcv(ts_MsgZclOnOffCmdRcvPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_ONOFF_CMD_RCV);
    LOGI("  (ZCL OnOff Cmd Rcv)");
    LOGI("  Src Ep: %#02x",     psPayload->u8SrcEp);
    LOGI("  Dst Ep: %#02x",     psPayload->u8DstEp);
    LOGI("  Cluster ID: %#04x", psPayload->u16ClusterId);
    LOGI("  Cmd ID: %#02x",     psPayload->u8CmdId);
}


void displayZclSceneAddRsp(ts_MsgZclSceneAddRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_SCENE_ADD_RSP);
    LOGI("  (ZCL Scene Add Rsp)");
    LOGI("  Status:   %#02x", psPayload->u8Status);
    LOGI("  Group ID: %#04x", psPayload->u16GroupId);
    LOGI("  Scene ID: %#02x", psPayload->u8SceneId);
}


void displayZclSceneViewRsp(ts_MsgZclSceneViewRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_SCENE_VIEW_RSP);
    LOGI("  (ZCL Scene View Rsp)");
    LOGI("  Status:            %#02x", psPayload->u8Status);
    LOGI("  Group ID:          %#04x", psPayload->u16GroupId);
    LOGI("  Scene ID:          %#02x", psPayload->u8SceneId);
    LOGI("  Trans Time:        %#04x", psPayload->u16TransTime);
    LOGI("  Scene Name Length: %#02x", psPayload->u8SceneNameLength);
    LOGI("  Scene Name:        %s", psPayload->au8SceneName);
    LOGI("  Ext Field Length:  %#02x", psPayload->extFieldLength);
    LOGI("  Ext Field Sets:    %s", psPayload->au8ExtFieldSets);
}


void displayZclSceneRemoveRsp(ts_MsgZclSceneRemoveRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_SCENE_REMOVE_RSP);
    LOGI("  (ZCL Scene Remove Rsp)");
    LOGI("  Status:   %#02x", psPayload->u8Status);
    LOGI("  Group ID: %#04x", psPayload->u16GroupId);
    LOGI("  Scene ID: %#02x", psPayload->u8SceneId);
}


void displayZclSceneRemoveAllRsp(ts_MsgZclSceneRemoveAllRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_SCENE_REMOVE_ALL_RSP);
    LOGI("  (ZCL Scene Remove All Rsp)");
    LOGI("  Status:   %#02x", psPayload->u8Status);
    LOGI("  Group ID: %#04x", psPayload->u16GroupId);
}


void displayZclSceneStoreRsp(ts_MsgZclSceneStoreRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_SCENE_STORE_RSP);
    LOGI("  (ZCL Scene Store Rsp)");
    LOGI("  Status:   %#02x", psPayload->u8Status);
    LOGI("  Group ID: %#04x", psPayload->u16GroupId);
    LOGI("  Scene ID: %#02x", psPayload->u8SceneId);
}


void displayZclSceneGetMembershipRsp(ts_MsgZclSceneGetMenbershipRspPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_ZCL_SCENE_GET_MEMBERSHIP_RSP);
    LOGI("  (ZCL Scene Get Menbership Rsp)");
    LOGI("  Status:     %#02x", psPayload->u8Status);
    LOGI("  Group ID:   %#04x", psPayload->u16GroupId);
    LOGI("  Capability: %#02x", psPayload->u8Capability);
    LOGI("  Scene Cnt:  %#02x", psPayload->u8SceneCnt);
    if (psPayload->u8SceneCnt)
    {
        LOGI("  Scene List: ");
        for (size_t i = 0; i < psPayload->u8SceneCnt; i++)
        {
            LOGI("    Scene ID: %#02x", psPayload->au8SceneList);
        }
    }
}


void displayDataConfirm(ts_MsgDataConfirmPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_DATA_CONFIRM);
    LOGI("  (Data Confirm)");
    LOGI("  Status:  %#02x", psPayload->u8Status);
    LOGI("  Src Ep:  %#02x", psPayload->u8SrcEndpoint);
    LOGI("  Aps Cnt: %#02x", psPayload->u8ApsCnt);
}


void displayMacAddrInd(ts_MsgMacAddrIndPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_MAC_ADDR_IND);
    LOGI("  (Mac Addr Ind)");
    LOGI("  Device Ext Addr: %#016llx", psPayload->u64DeviceExtAddr);
}


void displayNodeLeaveInd(ts_MsgNodeLeaveIndPayload *psPayload)
{
    LOGI("Type: %#04x", ZBHCI_CMD_NODE_LEAVE_IND);
    LOGI("  (Node Leave Ind)");
    LOGI("  Total Cnt: %#04x",    psPayload->u16TotalCnt);
    LOGI("  MacAddr:   %#016llx", psPayload->u64MacAddr);
}

/******************************************************************************/
/***        local functions                                                 ***/
/******************************************************************************/


/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/