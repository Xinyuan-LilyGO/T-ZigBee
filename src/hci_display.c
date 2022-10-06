
/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include "hci_display.h"
#include "esp32-hal-log.h"
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

#ifndef ARDUINO
#define log_i(format, args...) ESP_LOGI("zbhci", format, ##args)
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
    log_i("Type: %#04x", ZBHCI_CMD_ACKNOWLEDGE);
    log_i("  (Status)");
    log_i("  Status:  %#02x", psPayload->eStatus);
    log_i("  Message: %#04x", psPayload->u16MsgType);
}


void displayBdbCommissionFormationRsp(ts_MsgBdbCommissionFormationRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_BDB_COMMISSION_FORMATION_RSP);
    log_i("  (Bdb Commission Formation Rsp)");
    log_i("  Status: %#02x", psPayload->u8Status);
}


void displayNetworkStateRsp(ts_MsgNetworkStateRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_NETWORK_STATE_RSP);
    log_i("  (Network State Rsp)");
    log_i("  Nwk Addr:   %#04x",    psPayload->u16NwkAddr);
    log_i("  IEEE Addr:  %#016llx", psPayload->u64IeeeAddr);
    log_i("  Pan Id:     %#04x",    psPayload->u16PanId);
    log_i("  Ext Pan Id: %#016llx", psPayload->u64extPanId);
    log_i("  Channel:    %#02x",    psPayload->u8Channel);
}


void displayDiscoveryNwkAddrRsp(ts_MsgDiscoveryNwkAddrRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_DISCOVERY_NWK_ADDR_RSP);
    log_i("  (Nwk Address Rsp)");
    log_i("  SQN:                    %#02x",    psPayload->u8SeqNum);
    log_i("  Status:                 %#02x",    psPayload->u8Status);
    log_i("  IEEE Address:           %#016llx", psPayload->u64IEEEAddr);
    log_i("  Nwk Address:            %#04x",    psPayload->u16NwkAddr);
    if (psPayload->u8NumAssocDev)
    {
        log_i("  Num Associated Devices: %#02x", psPayload->u8NumAssocDev);
        log_i("  Start Index:            %#02x", psPayload->u8StartIdx);
        log_i("  Associated Devices List:");
        for (size_t i = 0; i < psPayload->u8NumAssocDev; i++)
        {
            log_i("    Devices %d Nwk Address: %#02x", i, psPayload->au16AssocDevList[i]);
        }
    }
}


void displayDiscoveryIEEEAddrRsp(ts_MsgDiscoveryIEEEAddrRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_DISCOVERY_IEEE_ADDR_RSP);
    log_i("  (IEEE Address Rsp)");
    log_i("  SQN:                    %#02x",    psPayload->u8SeqNum);
    log_i("  Status:                 %#02x",    psPayload->u8Status);
    log_i("  IEEE Address:           %#016llx", psPayload->u64IEEEAddr);
    log_i("  Nwk Address:            %#04x",    psPayload->u16NwkAddr);
    if (psPayload->u8NumAssocDev)
    {
        log_i("  Num Associated Devices: %#02x", psPayload->u8NumAssocDev);
        log_i("  Start Index:            %#02x", psPayload->u8StartIdx);
        log_i("  Associated Devices List:");
        for (size_t i = 0; i < psPayload->u8NumAssocDev; i++)
        {
            log_i("    Devices %d Nwk Address: %#02x", i, psPayload->au16AssocDevList[i]);
        }
    }
}


void displayDiscoveryNodeDescRsp(ts_MsgDiscoveryNodeDescRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_DISCOVERY_NODE_DESC_RSP);
    log_i("  (Node Desc Rsp)");
    log_i("  SQN:          %#02x", psPayload->u8SeqNum);
    log_i("  Status:       %#02x", psPayload->u8Status);
    log_i("  IEEE Address: %#04x", psPayload->u16NwkAddrOfInterest);
    log_i("  Node Desc:    %s",   psPayload->au8NodeDesc);
}


void displayDiscoverySimpleDescRsp(ts_MsgDiscoverySimpleDescRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_DISCOVERY_SIMPLE_DESC_RSP);
    log_i("  (Simple Desc Rsp)");
    log_i("  SQN:          %#02x", psPayload->u8SeqNum);
    log_i("  Status:       %#02x", psPayload->u8Status);
    log_i("  Nwk Address:  %#04x", psPayload->u16NwkAddrOfInterest);
    log_i("  Desc Len:     %#02x", psPayload->u8Length);
    log_i("  Simple Desc:  %s",   psPayload->au8SimpleDesc);
}


void displayDiscoveryMatchDescRsp(ts_MsgDiscoveryMatchDescRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_DISCOVERY_MATCH_DESC_RSP);
    log_i("  (Match Desc Rsp)");
    log_i("  SQN:         %#02x", psPayload->u8SeqNum);
    log_i("  Status:      %#02x", psPayload->u8Status);
    log_i("  Nwk Address: %#04x", psPayload->u16NwkAddrOfInterest);
    log_i("  Match Len:   %#02x", psPayload->u8MatchLen);
    if (psPayload->u8MatchLen)
    {
        log_i("  Match Ep List:");
        for (size_t i = 0; i < psPayload->u8MatchLen; i++)
        {
            log_i("    Match Ep:  %#02x", psPayload->au8MatchList[i]);
        }
    }
}


void displayDiscoveryActiveEpRsp(ts_MsgDiscoveryActiveEpRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_DISCOVERY_ACTIVE_EP_RSP);
    log_i("  (Active Ep Rsp)");
    log_i("  SQN:             %#02x", psPayload->u8SeqNum);
    log_i("  Status:          %#02x", psPayload->u8Status);
    log_i("  Nwk Address:     %#04x", psPayload->u16NwkAddrOfInterest);
    log_i("  Active Ep Count: %#02x", psPayload->u8ActiveEpCount);
    if (psPayload->u8ActiveEpCount)
    {
        log_i("  Active Ep List:");
        for (size_t i = 0; i < psPayload->u8ActiveEpCount; i++)
        {
            log_i("    Active Ep %d:  %#02x", i, psPayload->au8EpList[i]);
        }
    }
}


void displayBindingRsp(ts_MsgBindRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_BINDING_RSP);
    log_i("  (Bind Rsp)");
    log_i("  SQN:    %#02x", psPayload->u8SeqNum);
    log_i("  Status: %#02x", psPayload->u8Status);
}


void displayUnbindingRsp(ts_MsgUnbindRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_UNBINDING_RSP);
    log_i("  (Unbind Rsp)");
    log_i("  SQN:    %#02x", psPayload->u8SeqNum);
    log_i("  Status: %#02x", psPayload->u8Status);
}


void displayMgmtLqiRsp(ts_MsgMgmtLqiRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_MGMT_LQI_RSP);
    log_i("  (MGMT LQI Rsp)");
    log_i("  SQN:                     %#02x", psPayload->u8SeqNum);
    log_i("  Status:                  %#02x", psPayload->u8Status);
    log_i("  Neighbor Tab Entries:    %#02x", psPayload->u8NeighborTabEntries);
    log_i("  Start Index:             %#02x", psPayload->u8StartIdx);
    log_i("  Neighbor Tab List Count: %#02x", psPayload->u8NeighborTabListCount);
    if (psPayload->u8NeighborTabListCount)
    {
        log_i("  Neighbor Tab List:");
        for (size_t i = 0; i < psPayload->u8NeighborTabListCount; i++)
        {
            log_i("    Neighbor %d:", i);
            log_i("      Ext Pan ID:      %#016llx", psPayload->asNeighborTable[i].ext_pan_id);
            log_i("      Ext Address:     %#016llx", psPayload->asNeighborTable[i].ext_addr);
            log_i("      Nwk Address:     %#04x",    psPayload->asNeighborTable[i].network_addr);
            log_i("      Device Type:     %#02x",    psPayload->asNeighborTable[i].deviceType);
            log_i("      Rx On When Idle: %#02x",    psPayload->asNeighborTable[i].rxOnWhenIdle);
            log_i("      Relationship:    %#02x",    psPayload->asNeighborTable[i].relationship);
            log_i("      Permit Joining:  %#02x",    psPayload->asNeighborTable[i].permitJoining);
            log_i("      Depth:           %#02x",    psPayload->asNeighborTable[i].depth);
            log_i("      LQI:             %#02x",    psPayload->asNeighborTable[i].lqi);
        }
    }
}


void displayMgmtBindRsp(ts_MsgMgmtBindRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_MGMT_BIND_RSP);
    log_i("  (MGMT Bind Rsp)");
    log_i("  SQN:                    %#02x", psPayload->u8SeqNum);
    log_i("  Status:                 %#02x", psPayload->u8Status);
    log_i("  Binding Tab Entries:    %#02x", psPayload->u8BindingTabEntries);
    log_i("  Start Idx:              %#02x", psPayload->u8StartIdx);
    log_i("  Binding Tab List Count: %#02x", psPayload->u8BindingTabListCount);
    if (psPayload->u8BindingTabListCount)
    {
        log_i("  Binding Tab List:");
        for (size_t i = 0; i < psPayload->u8BindingTabListCount; i++)
        {
            log_i("    Src Addr:       %#016llx", psPayload->asBindingTabList[i].u64SrcAddr);
            log_i("    Src Ep:         %#02x", psPayload->asBindingTabList[i].u8SrcEndpoint);
            log_i("    Cluster ID:     %#04x", psPayload->asBindingTabList[i].u16ClusterId);
            log_i("    Dst Addr Mode:  %#02x", psPayload->asBindingTabList[i].u8DstAddrMode);
            if (psPayload->asBindingTabList[i].u8DstAddrMode == 0x01)
            {
                log_i("    Dst Group Addr: %#04x", psPayload->asBindingTabList[i].uDstAddr.u16DstGroupAddr);
            }
            else if (psPayload->asBindingTabList[i].u8DstAddrMode == 0x03)
            {
                log_i("    Dst Ext Addr:   %#016llx", psPayload->asBindingTabList[i].uDstAddr.sDstExtAddr.u64DstExtAddr);
                log_i("    Dst Ep:         %#02x",    psPayload->asBindingTabList[i].uDstAddr.sDstExtAddr.u8DstEndpoint);
            }
        }
    }
}


void displayMgmtLeaveRsp(ts_MsgMgmtLeaveRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_MGMT_LEAVE_RSP);
    log_i("  (MGMT Leave Rsp)");
    log_i("  SQN:    %#02x", psPayload->u8SeqNum);
    log_i("  Status: %#02x", psPayload->u8Status);
}


void displayMgmtDirectJoinRsp()
{
    /** None */
}


void displayMgmtPermitJoinRsp(ts_MsgMgmtPermitJoinRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_MGMT_PERMIT_JOIN_RSP);
    log_i("  (MGMT Permit Join Rsp)");
    log_i("  SQN:    %#02x", psPayload->u8SeqNum);
    log_i("  Status: %#02x", psPayload->u8Status);
}


void displayMgmtNwkUpdateRsp()
{
    /** None */
}


void displayNodesJoinedGetRsp(ts_MsgNodesJoinedGetRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_NODES_JOINED_GET_RSP);
    log_i("  (Nodes Joined Get Rsp)");
    log_i("  Total:      %#04x", psPayload->u16TotalCnt);
    log_i("  Start Idx:  %#04x", psPayload->u16StartIdx);
    log_i("  List Count: %#02x", psPayload->u8ListCount);
    log_i("  Status:     %#02x", psPayload->u8Status);
    if (psPayload->u8ListCount)
    {
        log_i("  Mac Addr List:");
        for (size_t i = 0; i < psPayload->u8ListCount; i++)
        {
            log_i("    Node %d: %#016llx", i, psPayload->au64MacAddrList[i]);
            log_i("    Node %d: %#04x", i, psPayload->au16ShortAddrList[i]);
        }
    }
}


void displayNodesTogleTestRsp()
{
    /** None */
}


void displayTxRxPerformanceTestRsp(ts_MsgTxRxPerformceTestRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_TXRX_PERFORMANCE_TEST_RSP);
    log_i("  (TxRx Performce Test Rsp)");
    log_i("  Dst Addr: %#04x", psPayload->u16DstAddr);
    log_i("  Send Cnt: %#04x", psPayload->u16SendCnt);
    log_i("  Ack Cnt:  %#04x", psPayload->u16AckCnt);
}


void displayNodesDevAnnceInd(ts_MsgNodesDevAnnceRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_NODES_DEV_ANNCE_IND);
    log_i("  (Nodes Dev Annce Rsp)");
    log_i("  Nwk Addr:   %#04x",    psPayload->u16NwkAddr);
    log_i("  IEEE Addr:  %#016llx", psPayload->u64IEEEAddr);
    // log_i("  Capability: %#02x",    psPayload->u8Capability);
    log_i("  Capability:");
    log_i("    Coordinator:           %#02x", psPayload->u8Capability & 0b00000001);
    log_i("    Devive Type:           %#02x", psPayload->u8Capability & 0b00000010);
    log_i("    Power Source:          %#02x", psPayload->u8Capability & 0b00000100);
    log_i("    Receiver on when idle: %#02x", psPayload->u8Capability & 0b00001000);
    log_i("    Security capability:   %#02x", psPayload->u8Capability & 0b01000000);
    log_i("    Allocate address:      %#02x", psPayload->u8Capability & 0b10000000);
}


void displayAfDataSendTestRsp(ts_MsgAfDataSendTestRspPayload *psPayload)
{
    char str[256] = { 0 };
    uint8_t size = 0;

    log_i("Type: %#04x", ZBHCI_CMD_AF_DATA_SEND_TEST_RSP);
    log_i("  (AF DATA SEND TEST Rsp)");
    log_i("  Src Addr:   %#04x", psPayload->u16SrcAddr);
    log_i("  Src Ep:     %#02x", psPayload->u8SrcEp);
    log_i("  Dst Ep:     %#02x", psPayload->u8DstEp);
    log_i("  Cluster ID: %#04x", psPayload->u16ClusterId);
    log_i("  Data Len:   %#04x", psPayload->u16DataLen);
    for (size_t i = 0; i < psPayload->u16DataLen; i++)
    {
        size += snprintf(&str[size], sizeof(str) - 1, "%#02x", psPayload->au8Payload[i]);
    }
    log_i("  Payload:    %s",   str);
}


void displayLeaveIndication(ts_MsgLeaveIndicationPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_LEAVE_INDICATION);
    log_i("  (Leave Indication)");
    log_i("  Mac Addr: %#04x", psPayload->u64MacAddr);
    log_i("  Rejoin:   %#02x", psPayload->u8Rejoin);
}


void displayZclAttrReadRsp(ts_MsgZclAttrReadRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_ATTR_READ_RSP);
    log_i("  (ZCL Attr Read Rsp)");
    log_i("  SQN:        %#02x", psPayload->u8SeqNum);
    log_i("  Src Addr:   %#04x", psPayload->u16SrcAddr);
    log_i("  Src Ep:     %#02x", psPayload->u8SrcEp);
    log_i("  Cluster ID: %#04x", psPayload->u16ClusterId);
    log_i("  Attr Num:   %#04x", psPayload->u8AttrNum);
    if (psPayload->u8AttrNum)
    {
        log_i("  Attr List:");
        for (size_t i = 0; i < psPayload->u8AttrNum; i++)
        {
            log_i("    Attr %d:", i);
            log_i("      Attr ID:   %#04x", psPayload->asAttrReadList[i].u16AttrID);
            log_i("      Data Type: %#02x", psPayload->asAttrReadList[i].u8DataType);
            log_i("      Data Len:  %#04x", psPayload->asAttrReadList[i].u16DataLen);
            if((psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_LONG_CHAR_STR)  || \
               (psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_LONG_OCTET_STR) || \
               (psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_CHAR_STR)       || \
               (psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_OCTET_STR))
            {
                log_i("      Attr Data: %s", psPayload->asAttrReadList[i].uAttrData.au8AttrData);
            }
            else if(psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_BOOLEAN || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_DATA8   || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_BITMAP8 || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_UINT8   || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_INT8    || \
                    psPayload->asAttrReadList[i].u8DataType == ZCL_DATA_TYPE_ENUM8)
            {
                log_i("      Attr Data: %#02x", psPayload->asAttrReadList[i].uAttrData.u8AttrData);
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
                log_i("      Attr Data: %#04x", psPayload->asAttrReadList[i].uAttrData.u16AttrData);
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
                log_i("      Attr Data: %#08x", psPayload->asAttrReadList[i].uAttrData.u32AttrData);
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
                log_i("      Attr Data: %#016llx", psPayload->asAttrReadList[i].uAttrData.u64AttrData);
            }
        }
    }
}


void displayZclAttrWriteRsp(ts_MsgZclAttrWriteRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_CONFIG_REPORT_RSP);
    log_i("  (ZCL Attr Write Rsp)");
    log_i("  SQN:        %#02x", psPayload->u8SeqNum);
    log_i("  Src Addr:   %#04x", psPayload->u16SrcAddr);
    log_i("  Src Ep:     %#02x", psPayload->u8SrcEp);
    log_i("  Cluster ID: %#04x", psPayload->u16ClusterId);
    log_i("  Attr Num:   %#04x", psPayload->u8AttrNum);
    if (psPayload->u8AttrNum)
    {
        log_i("  Attr List:");
        for (size_t i = 0; i < psPayload->u8AttrNum; i++)
        {
            log_i("    Attr %d:", i);
            log_i("      Status:  %#02x", psPayload->asAttrWriteList[i].u8Status);
            log_i("      Attr ID: %#04x", psPayload->asAttrWriteList[i].u16AttrID);
        }
    }
}


void displayZclConfigReportRsp(ts_MsgZclConfigReportRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_CONFIG_REPORT_RSP);
    log_i("  (ZCL Config Report Rsp)");
    log_i("  SQN:        %#02x", psPayload->u8SeqNum);
    log_i("  Src Addr:   %#04x", psPayload->u16SrcAddr);
    log_i("  Src Ep:     %#02x", psPayload->u8SrcEp);
    log_i("  Cluster ID: %#04x", psPayload->u16ClusterId);
    log_i("  Attr Num:   %#04x", psPayload->u8AttrNum);
    if (psPayload->u8AttrNum)
    {
        log_i("  Attr List:");
        for (size_t i = 0; i < psPayload->u8AttrNum; i++)
        {
            log_i("    Attr %d:", i);
            log_i("      Status:           %#02x", psPayload->asAttrConfigReportList[i].u8Status);
            log_i("      Report Direction: %#02x", psPayload->asAttrConfigReportList[i].u8ReportDirection);
            log_i("      Attr ID:          %#04x", psPayload->asAttrConfigReportList[i].u16AttrID);
        }
    }
}


void displayZclReadReportCfgRsp(ts_MsgZclReadReportCfgRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_READ_REPORT_CFG_RSP);
    log_i("  (ZCL Config Report Rsp)");
    log_i("  SQN:        %#02x", psPayload->u8SeqNum);
    log_i("  Src Addr:   %#04x", psPayload->u16SrcAddr);
    log_i("  Src Ep:     %#02x", psPayload->u8SrcEp);
    log_i("  Cluster ID: %#04x", psPayload->u16ClusterId);
    log_i("  Attr Num:   %#04x", psPayload->u8AttrNum);
    if (psPayload->u8AttrNum)
    {
        log_i("  Attr List:");
        for (size_t i = 0; i < psPayload->u8AttrNum; i++)
        {
            log_i("    Attr %d:", i);
            log_i("      Status:           %#02x", psPayload->asAttrList[i].u8Status);
            log_i("      Report Direction: %#02x", psPayload->asAttrList[i].u8ReportDirection);
            log_i("      Attr ID:          %#04x", psPayload->asAttrList[i].u16AttrID);

            if (psPayload->asAttrList[i].u8ReportDirection == 0x00)
            {
                log_i("      Data Type:        %#02x", psPayload->asAttrList[i].u8DataType);
                log_i("      Min Rep Interval: %#04x", psPayload->asAttrList[i].u16MinRepInterval);
                log_i("      Max Rep Interval: %#04x", psPayload->asAttrList[i].u16MaxRepInterval);
            }
            else
            {
                log_i("      Timeout Period:   %#04x", psPayload->asAttrList[i].u16TimeoutPeriod);
            }
        }
    }
}


void displayZclReportMsgRcv(ts_MsgZclReportMsgRcvPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_REPORT_MSG_RCV);
    log_i("  (ZCL Attr Report)");
    log_i("  SQN:        %#02x", psPayload->u8SeqNum);
    log_i("  Src Addr:   %#04x", psPayload->u16SrcAddr);
    log_i("  Src Ep:     %#02x", psPayload->u8SrcEp);
    log_i("  Cluster ID: %#04x", psPayload->u16ClusterId);
    log_i("  Attr Num:   %#04x", psPayload->u8AttrNum);
    if (psPayload->u8AttrNum)
    {
        log_i("  Attr List:");
        for (size_t i = 0; i < psPayload->u8AttrNum; i++)
        {
            log_i("    Attr %d:", i);
            log_i("      Attr ID:   %#04x", psPayload->asAttrList[i].u16AttrID);
            log_i("      Data Type: %#02x", psPayload->asAttrList[i].u8DataType);
            log_i("      Data Len:  %#04x", psPayload->asAttrList[i].u16DataLen);
            if((psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_LONG_CHAR_STR)  || \
               (psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_LONG_OCTET_STR) || \
               (psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_CHAR_STR)       || \
               (psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_OCTET_STR))
            {
                log_i("      Attr Data: %s", psPayload->asAttrList[i].uAttrData.au8AttrData);
            }
            else if(psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_BOOLEAN || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_DATA8   || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_BITMAP8 || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_UINT8   || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_INT8    || \
                    psPayload->asAttrList[i].u8DataType == ZCL_DATA_TYPE_ENUM8)
            {
                log_i("      Attr Data: %#02x", psPayload->asAttrList[i].uAttrData.u8AttrData);
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
                log_i("      Attr Data: %#04x", psPayload->asAttrList[i].uAttrData.u16AttrData);
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
                log_i("      Attr Data: %#08x", psPayload->asAttrList[i].uAttrData.u32AttrData);
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
                log_i("      Attr Data: %#016llx", psPayload->asAttrList[i].uAttrData.u64AttrData);
            }
        }
    }
}


void displayZclGroupAddRsp(ts_MsgZclGroupAddRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_GROUP_ADD_RSP);
    log_i("  (ZCL Group Add Rsp)");
    log_i("  Status:   %#02x", psPayload->u8Status);
    log_i("  Group ID: %#04x", psPayload->u16GroupId);
}


void displayZclGroupViewRsp(ts_MsgZclGroupViewRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_GROUP_VIEW_RSP);
    log_i("  (ZCL Group View Rsp)");
    log_i("  Status:     %#02x", psPayload->u8Status);
    log_i("  Group ID:   %#04x", psPayload->u16GroupId);
    log_i("  Length:     %#02x", psPayload->u8GroupNameLength);
    log_i("  Group Name: %s",   psPayload->u8GroupNameLength);
}


void displayZclGroupGetMembershipRsp(ts_MsgZclGroupGetMembershipRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_GROUP_GET_MEMBERSHIP_RSP);
    log_i("  (ZCL Group Get Membership Rsp)");
    log_i("  Group Capability: %#02x", psPayload->u8Capability);
    log_i("  Group Count:      %#04x", psPayload->u8GroupCount);

    if (psPayload->u8GroupCount) 
    {
        log_i("  Membership List:");
        for (size_t i = 0; i < psPayload->u8GroupCount; i++)
        {
            log_i(    "Membership %d: %#04x", i, psPayload->au16GroupId[i]);
        }
    }
}


void displayZclGroupRemoveRsp(ts_MsgZclGroupRemoveRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_GROUP_REMOVE_RSP);
    log_i("  (ZCL Group Remove Rsp)");
    log_i("  Status:   %#02x", psPayload->u8Status);
    log_i("  Group ID: %#04x", psPayload->u16GroupId);
}


void displayZclIdentifyQueryRsp(ts_MsgZclIdentifyQueryRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_IDENTIFY_QUERY_RSP);
    log_i("  (ZCL Identify Query Rsp)");
    log_i("  Nwk Addr: %#04x", psPayload->u16ShortAddr);
    log_i("  Time Out: %#04x", psPayload->u16Timeout);
}


void displayZclOnOffCmdRcv(ts_MsgZclOnOffCmdRcvPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_ONOFF_CMD_RCV);
    log_i("  (ZCL OnOff Cmd Rcv)");
    log_i("  Src Ep: %#02x",     psPayload->u8SrcEp);
    log_i("  Dst Ep: %#02x",     psPayload->u8DstEp);
    log_i("  Cluster ID: %#04x", psPayload->u16ClusterId);
    log_i("  Cmd ID: %#02x",     psPayload->u8CmdId);
}


void displayZclSceneAddRsp(ts_MsgZclSceneAddRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_SCENE_ADD_RSP);
    log_i("  (ZCL Scene Add Rsp)");
    log_i("  Status:   %#02x", psPayload->u8Status);
    log_i("  Group ID: %#04x", psPayload->u16GroupId);
    log_i("  Scene ID: %#02x", psPayload->u8SceneId);
}


void displayZclSceneViewRsp(ts_MsgZclSceneViewRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_SCENE_VIEW_RSP);
    log_i("  (ZCL Scene View Rsp)");
    log_i("  Status:            %#02x", psPayload->u8Status);
    log_i("  Group ID:          %#04x", psPayload->u16GroupId);
    log_i("  Scene ID:          %#02x", psPayload->u8SceneId);
    log_i("  Trans Time:        %#04x", psPayload->u16TransTime);
    log_i("  Scene Name Length: %#02x", psPayload->u8SceneNameLength);
    log_i("  Scene Name:        %s", psPayload->au8SceneName);
    log_i("  Ext Field Length:  %#02x", psPayload->extFieldLength);
    log_i("  Ext Field Sets:    %s", psPayload->au8ExtFieldSets);
}


void displayZclSceneRemoveRsp(ts_MsgZclSceneRemoveRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_SCENE_REMOVE_RSP);
    log_i("  (ZCL Scene Remove Rsp)");
    log_i("  Status:   %#02x", psPayload->u8Status);
    log_i("  Group ID: %#04x", psPayload->u16GroupId);
    log_i("  Scene ID: %#02x", psPayload->u8SceneId);
}


void displayZclSceneRemoveAllRsp(ts_MsgZclSceneRemoveAllRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_SCENE_REMOVE_ALL_RSP);
    log_i("  (ZCL Scene Remove All Rsp)");
    log_i("  Status:   %#02x", psPayload->u8Status);
    log_i("  Group ID: %#04x", psPayload->u16GroupId);
}


void displayZclSceneStoreRsp(ts_MsgZclSceneStoreRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_SCENE_STORE_RSP);
    log_i("  (ZCL Scene Store Rsp)");
    log_i("  Status:   %#02x", psPayload->u8Status);
    log_i("  Group ID: %#04x", psPayload->u16GroupId);
    log_i("  Scene ID: %#02x", psPayload->u8SceneId);
}


void displayZclSceneGetMembershipRsp(ts_MsgZclSceneGetMenbershipRspPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_ZCL_SCENE_GET_MEMBERSHIP_RSP);
    log_i("  (ZCL Scene Get Menbership Rsp)");
    log_i("  Status:     %#02x", psPayload->u8Status);
    log_i("  Group ID:   %#04x", psPayload->u16GroupId);
    log_i("  Capability: %#02x", psPayload->u8Capability);
    log_i("  Scene Cnt:  %#02x", psPayload->u8SceneCnt);
    if (psPayload->u8SceneCnt)
    {
        log_i("  Scene List: ");
        for (size_t i = 0; i < psPayload->u8SceneCnt; i++)
        {
            log_i("    Scene ID: %#02x", psPayload->au8SceneList);
        }
    }
}


void displayDataConfirm(ts_MsgDataConfirmPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_DATA_CONFIRM);
    log_i("  (Data Confirm)");
    log_i("  Status:  %#02x", psPayload->u8Status);
    log_i("  Src Ep:  %#02x", psPayload->u8SrcEndpoint);
    log_i("  Aps Cnt: %#02x", psPayload->u8ApsCnt);
}


void displayMacAddrInd(ts_MsgMacAddrIndPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_MAC_ADDR_IND);
    log_i("  (Mac Addr Ind)");
    log_i("  Device Ext Addr: %#016llx", psPayload->u64DeviceExtAddr);
}


void displayNodeLeaveInd(ts_MsgNodeLeaveIndPayload *psPayload)
{
    log_i("Type: %#04x", ZBHCI_CMD_NODE_LEAVE_IND);
    log_i("  (Node Leave Ind)");
    log_i("  Total Cnt: %#04x",    psPayload->u16TotalCnt);
    log_i("  MacAddr:   %#016llx", psPayload->u64MacAddr);
}

/******************************************************************************/
/***        local functions                                                 ***/
/******************************************************************************/


/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/