#ifndef HCI_DISPLAY_H
#define HCI_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include "zbhci_commom.h"

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

void displayAcknowledg(ts_MsgAckPayload *psMsgAckPayload);

void displayBdbCommissionFormationRsp(ts_MsgBdbCommissionFormationRspPayload *psPayload);

void displayNetworkStateRsp(ts_MsgNetworkStateRspPayload *psPayload);

void displayDiscoveryNwkAddrRsp(ts_MsgDiscoveryNwkAddrRspPayload *psPayload);

void displayDiscoveryIEEEAddrRsp(ts_MsgDiscoveryIEEEAddrRspPayload *psPayload);

void displayDiscoveryNodeDescRsp(ts_MsgDiscoveryNodeDescRspPayload *psPayload);

void displayDiscoverySimpleDescRsp(ts_MsgDiscoverySimpleDescRspPayload *psPayload);

void displayDiscoveryMatchDescRsp(ts_MsgDiscoveryMatchDescRspPayload *psPayload);

void displayDiscoveryActiveEpRsp(ts_MsgDiscoveryActiveEpRspPayload *psPayload);

void displayBindingRsp(ts_MsgBindRspPayload *psPayload);

void displayUnbindingRsp(ts_MsgUnbindRspPayload *psPayload);

void displayMgmtLqiRsp(ts_MsgMgmtLqiRspPayload *psPayload);

void displayMgmtBindRsp(ts_MsgMgmtBindRspPayload *psPayload);

void displayMgmtLeaveRsp(ts_MsgMgmtLeaveRspPayload *psPayload);

void displayMgmtDirectJoinRsp();

void displayMgmtPermitJoinRsp(ts_MsgMgmtPermitJoinRspPayload *psPayload);

void displayMgmtNwkUpdateRsp();

void displayNodesJoinedGetRsp(ts_MsgNodesJoinedGetRspPayload *psPayload);

void displayNodesTogleTestRsp();

void displayTxRxPerformanceTestRsp(ts_MsgTxRxPerformceTestRspPayload *psPayload);

void displayNodesDevAnnceInd(ts_MsgNodesDevAnnceRspPayload *psPayload);

void displayAfDataSendTestRsp(ts_MsgAfDataSendTestRspPayload *psPayload);

void displayLeaveIndication(ts_MsgLeaveIndicationPayload *psPayload);

void displayZclAttrReadRsp(ts_MsgZclAttrReadRspPayload *psPayload);

void displayZclAttrWriteRsp(ts_MsgZclAttrWriteRspPayload *psPayload);

void displayZclConfigReportRsp(ts_MsgZclConfigReportRspPayload *psPayload);

void displayZclReadReportCfgRsp(ts_MsgZclReadReportCfgRspPayload *psPayload);

void displayZclReportMsgRcv(ts_MsgZclReportMsgRcvPayload *psPayload);

void displayZclGroupAddRsp(ts_MsgZclGroupAddRspPayload *psPayload);

void displayZclGroupViewRsp(ts_MsgZclGroupViewRspPayload *psPayload);

void displayZclGroupGetMembershipRsp(ts_MsgZclGroupGetMembershipRspPayload *psPayload);

void displayZclGroupRemoveRsp(ts_MsgZclGroupRemoveRspPayload *psPayload);

void displayZclIdentifyQueryRsp(ts_MsgZclIdentifyQueryRspPayload *psPayload);

void displayZclSceneAddRsp(ts_MsgZclSceneAddRspPayload *psPayload);

void displayZclSceneViewRsp(ts_MsgZclSceneViewRspPayload *psPayload);

void displayZclSceneRemoveRsp(ts_MsgZclSceneRemoveRspPayload *psPayload);

void displayZclSceneRemoveAllRsp(ts_MsgZclSceneRemoveAllRspPayload *psPayload);

void displayZclSceneStoreRsp(ts_MsgZclSceneStoreRspPayload *psPayload);

void displayZclSceneGetMembershipRsp(ts_MsgZclSceneGetMenbershipRspPayload *psPayload);

void displayDataConfirm(ts_MsgDataConfirmPayload *psPayload);

void displayMacAddrInd(ts_MsgMacAddrIndPayload *psPayload);

void displayNodeLeaveInd(ts_MsgNodeLeaveIndPayload *psPayload);

#ifdef __cplusplus
}
#endif

#endif
/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/