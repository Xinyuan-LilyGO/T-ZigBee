
#ifndef ZBHCI_H
#define ZBHCI_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include "zbhci_commom.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

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

/**
 * @addtogroup zbhci_fountions zigbee hci APIs
 *  @{
 */

void zbhci_Init(QueueHandle_t queue);

/**
 * @brief start network formation.
 */
void zbhci_BdbCommissionFormation(void);

/**
 * @brief classic join start.
 *
 * @note Only Router and EndDevice are valid
 */
void zbhci_BdbCommissionSteer(void);

/**
 * @brief touch link network formation or touch link join start.
 * 
 * @param eRole initiator or target @see te_MsgBdbCommissionTouchlinkRole
 */
void zbhci_BdbCommissionTouchlink(te_MsgBdbCommissionTouchlinkRole eRole);

/**
 * @brief find and bind touch link node.
 * 
 * @param eRole initiator or target @see te_MsgBdbCommissionTouchlinkRole
 */
void zbhci_BdbCommissionFindbind(te_MsgBdbCommissionFindbindRole eRole);

/**
 * @brief Reset device to factory new.
 *
 * @note This interface could be called by Coordinator, Router or End-Device.
 *       If coordinator: \n
 *         It will erase all NV information and reset all layer settings. \n
 *       if router or end device: \n
 *         If it is a not factory new device, it will broadcast a Leave Command
 *         before factory new reset.
 */
void zbhci_BdbFactoryReset(void);

/**
 * @brief Add pre-install code to NV.
 * 
 * @param u64DevAddr       the ieee address of the device using unique link key join
 * @param pu8UniqueLinkKey the pointer of install code
 */
void zbhci_BdbPreInstallCode(uint64_t u64DevAddr, uint8_t *pu8UniqueLinkKey);

/**
 * @brief Set channel mask to APS_IB.
 * 
 * @param u8ChannelIdx channel: 11 - 26
 */
void zbhci_BdbChannelSet(uint8_t u8ChannelIdx);

/**
 * @brief Set the working mode of the coordinator
 * 
 * @param eMode work mode @see te_MsgBdbDongleWorkingMode
 */
void zbhci_BdbDongleWorkingModeSet(te_MsgBdbDongleWorkingMode eMode);

/**
 * @brief Interface for forgetting all information about the specified device.
 * 
 * @param u64DevAddr ieee address of the specified device
 */
void zbhci_BdbNodeDelete(uint64_t u64DevAddr);

/**
 * @brief Set specified transmitted power.
 * 
 * @param eLevel Specified power @see te_BdbTxPowerLevel.
 */
void zbhci_BdbTxPowerSet(te_BdbTxPowerLevel eLevel);

/**
 * @brief Get the network information of the coordinator node
 */
void zbhci_NetworkStateReq(void);

/**
 * @brief Send address request to target device for short address.
 *
 * @param[in] u16DstAddr  The destination address to which this command will be
 *                        sent, shall be unicast or broadcast to all devices
 *                        for which macRxOnWhenIdle = TRUE.
 * @param[in] u64IEEEAddr The IEEE address to be matched by the remote device.
 * @param[in] u8ReqType   Request type for this command: \n
 *                          0x00 – Single device response; \n
 *                          0x01 – Extended response; \n
 *                          0x02 ~ 0xFF – Reserved.
 * @param[in] u8StartIdx  If the request type for this command is extended
 *                        response, the startIdx provides the starting index for
 *                        the requested elements of the associated device list.
 */
void zbhci_DiscoveryNwkAddrReq(uint16_t u16DstAddr,
                               uint64_t u64IEEEAddr,
                               uint8_t  u8ReqType,
                               uint8_t  u8StartIdx);

/**
 * @brief Send address request to target device for IEEE address.
 *
 * @param[in] u16DstAddr           The destination address to which this command
 *                                 will be sent, shall be unicast or broadcast
 *                                 to all devices for which macRxOnWhenIdle = TRUE.
 * @param[in] u16NwkAddrOfInterest The NWK address that is used for IEEE address
 *                                 mapping.
 * @param[in] u8ReqType            Request type for this command: \n
 *                                   0x00 – Single device response; \n
 *                                   0x01 – Extended response; \n
 *                                   0x02 ~ 0xFF – Reserved.
 * @param[in] u8StartIdx           If the request type for this command is
 *                                 extended response, the startIdx provides the
 *                                 starting index for the requested elements of
 *                                 the associated device list.
*/
void zbhci_DiscoveryIeeeAddrReq(uint16_t u16DstAddr,
                                uint16_t u16NwkAddrOfInterest,
                                uint8_t  u8ReqType,
                                uint8_t  u8StartIdx);

/**
 * @brief Send node descriptor request.
 *
 * @param[in] u16DstAddr           The destination address to which this command
 *                                 will be sent, shall be unicast or broadcast
 *                                 to all devices for which macRxOnWhenIdle = TRUE.
 * @param[in] u16NwkAddrOfInterest NWK address of the target.
 */
void zbhci_DiscoveryNodeDescReq(uint16_t u16DstAddr,
                                uint16_t u16NwkAddrOfInterest);

/**
 * @brief Send simple descriptor request.
 *
 * @param[in] u16DstAddr           The destination address to which this command
 *                                 will be sent, shall be unicast or broadcast
 *                                 to all devices for which macRxOnWhenIdle = TRUE.
 * @param[in] u16NwkAddrOfInterest NWK address of the target.
 * @param[in] u8Endpoint           The endpoint on the destination.
 */
void zbhci_DiscoverySimpleDescReq(uint16_t u16DstAddr,
                                  uint16_t u16NwkAddrOfInterest,
                                  uint8_t  u8Endpoint);

/**
 * @brief Send match descriptor request.
 *
 * @param[in] u16DstAddr           The destination address to which this command
 *                                 will be sent, shall be unicast or broadcast
 *                                 to all devices for which macRxOnWhenIdle = TRUE.
 * @param[in] u16NwkAddrOfInterest NWK address of the target.
 * @param[in] u16ProfileID         Profile ID to be matched at the destination.
 * @param[in] u8NumInClusters      Profile ID to be matched at the destination.
 * @param[in] u8NumOutClusters     Profile ID to be matched at the destination.
 * @param[in] pu16ClusterList      inClusterList + outClusterList.
 *                                 List of input cluster IDs to be used for
 *                                 matching, the inClusterList is the desired
 *                                 list to be matched by the Remote
 *                                 Device (the elements of the inClusterList
 *                                 are the supported output clusters of the
 *                                 Local Device).
 *                                 List of output cluster IDs to be used for
 *                                 matching, the outClusterList is the desired
 *                                 list to be matched by the Remote Device (the
 *                                 elements of the outClusterList are the
 *                                 supported input clusters of the Local Device)
 */
void zbhci_DiscoveryMatchDescReq(uint16_t u16DstAddr,
                                 uint16_t u16NwkAddrOfInterest,
                                 uint16_t u16ProfileID,
                                 uint8_t  u8NumInClusters,
                                 uint8_t  u8NumOutClusters,
                                 uint16_t *pu16ClusterList);

/**
 * @brief Send active endpoint request.
 *
 * @param[in] u16DstAddr           The destination address to which this command
 *                                 will be sent, shall be unicast or broadcast
 *                                 to all devices for which macRxOnWhenIdle = TRUE.
 * @param[in] u16NwkAddrOfInterest NWK address of the target.
 */
void zbhci_DiscoveryActiveEpReq(uint16_t u16DstAddr,
                                uint16_t u16NwkAddrOfInterest);

/**
 * @brief Send the command of mgmt_leave_req.
 *
 * @param[in] u64DevAddr       The IEEE address of the device to be removed or
 *                             NULL if the device removes itself.
 * @param[in] u8Rejoin         TRUE if the device is being asked to leave from
 *                             the current parent and requested to rejoin the
 *                             network. Otherwise, the parameter has a value
 *                             of FALSE.
 * @param[in] u8RemoveChildren TRUE if the device being asked to leave the
 *                             network is also being asked to remove its child
 *                             device, if any. Otherwise, it has a value
 *                             of FALSE.
 */
void zbhci_DiscoveryLeaveReq(uint64_t u64DevAddr,
                             uint8_t  u8Rejoin,
                             uint8_t  u8RemoveChildren);

/**
 * @brief Send bind request, destination address is based on the srcAddr in the request command
 *
 * @param[in] u64SrcIEEEAddr The IEEE address for the source.
 * @param[in] u8SrcEndpoint  The source endpoint for the binding entry.
 * @param[in] u16ClusterID   The identifier of the cluster on the source device
 *                           that is bound to the destination.
 * @param[in] u8DstAddrMode  The addressing mode for the destination address
 *                           used in this command.\n
 *                             0x00 – Reserved;\n
 *                             0x01 – 16-bit group address for dstAddr and
 *                                    dstEndpoint not present;\n
 *                             0x02 – Reserved;\n
 *                             0x03 – 64-bit extended address for dstAddr and
 *                                    dstEndpoint present;\n
 *                             0x04 ~ 0xFF – Reserved.
 * @param[in] sDstAddr       The destination address for the binding entry.
 * @param[in] u8DstEndpoint  Shall be present only if the dstAddrMode field has
 *                           a value of 0x03 and, if present, shall be the
 *                           destination endpoint for the binding entry.
 */
void zbhci_BindingReq(uint64_t   u64SrcIEEEAddr,
                      uint8_t    u8SrcEndpoint,
                      uint16_t   u16ClusterID,
                      uint8_t    u8DstAddrMode,
                      ts_DstAddr sDstAddr,
                      uint8_t    u8DstEndpoint);

/**
 * @brief Send unbind request, destination address is based on the srcAddr in the request command
 *
 * @param[in] u64SrcIEEEAddr The IEEE address for the source.
 * @param[in] u8SrcEndpoint  The source endpoint for the binding entry.
 * @param[in] u16ClusterID   The identifier of the cluster on the source device
 *                           that is bound to the destination.
 * @param[in] u8DstAddrMode  The addressing mode for the destination address
 *                           used in this command.\n
 *                             0x00 – Reserved;\n
 *                             0x01 – 16-bit group address for dstAddr and
 *                                    dstEndpoint not present;\n
 *                             0x02 – Reserved;\n
 *                             0x03 – 64-bit extended address for dstAddr and
 *                                    dstEndpoint present;\n
 *                             0x04 ~ 0xFF – Reserved.
 * @param[in] sDstAddr       The destination address for the binding entry.
 * @param[in] u8DstEndpoint  Shall be present only if the dstAddrMode field has
 *                           a value of 0x03 and, if present, shall be the
 *                           destination endpoint for the binding entry.
 */
void zbhci_UnbindingReq(uint64_t   u64SrcIEEEAddr,
                        uint8_t    u8SrcEndpoint,
                        uint16_t   u16ClusterID,
                        uint8_t    u8DstAddrMode,
                        ts_DstAddr sDstAddr,
                        uint8_t    u8DstEndpoint);

/**
 * @brief Send the command of mgmt_lqi_req.
 *
 * @param[in] u16DstAddr The destination address to which this command will be
 *                       sent, shall be unicast.
 * @param[in] u8StartIdx Starting index for the requested elements of the
 *                       neighbor table.
 */
void zbhci_MgmtLqiReq(uint16_t u16DstAddr, uint8_t u8StartIdx);

/**
 * @brief Send the command mgmt_bind_req.
 *
 * @param[in] u16DstAddr The destination address to which this command will be
 *                       sent, shall be unicast.
 * @param[in] u8StartIdx Starting index for the requested elements of the
 *                       binding table.
 */
void zbhci_MgmtBindReq(uint16_t u16DstAddr, uint8_t u8StartIdx);

/**
 * @brief Send the command of mgmt_leave_req.
 *
 * @param[in] u16DstAddr       The destination address to which this command
 *                             will be sent, shall be unicast.
 * @param[in] u64DevAddr       The IEEE address of the device to be removed or
 *                             NULL if the device removes itself.
 * @param[in] u8Rejoin         TRUE if the device is being asked to leave from
 *                             the current parent and requested to rejoin the
 *                             network. Otherwise, the parameter has a value
 *                             of FALSE.
 * @param[in] u8RemoveChildren TRUE if the device being asked to leave the
 *                             network is also being asked to remove its child
 *                             device, if any. Otherwise, it has a value of FALSE.
 */
void zbhci_MgmtLeaveReq(uint16_t u16DstAddr,
                        uint64_t u64DevAddr,
                        uint8_t  u8Rejoin,
                        uint8_t  u8RemoveChildren);

/**
 * @brief None
 */
void zbhci_MgmtDirectJoinReq(void);


/**
 * @brief Send the command of mgmt_permitJoin_req.
 *
 * @param[in] u16DstAddr       The destination address to which this command
 *                             will be sent.
 * @param[in] u8PermitDuration The length of time in seconds during which the
 *                             coordinator or router will allow associations.
 *                             The value 0x00 and 0xff indicate that permission
 *                             is disabled or enabled, respectively, without a
 *                             specified limit.
 * @param[in] u8TCSignificance This field shall always have a value of 1,
 *                             indicating a request to change the Trust Center
 *                             policy. If a frame is received with a value of 0,
 *                             it shall be treated as having a value of 1.
 */
void zbhci_MgmtPermitJoinReq(uint16_t u16DstAddr,
                             uint8_t  u8PermitDuration,
                             uint8_t  u8TCSignificance);

/**
 * @brief Send the command of mgmt_network_update_req.
 *
 * @param[in] u16DstAddr        The destination address to which this command
 *                              will be sent.
 * @param[in] u16NwkManagerAddr This field shall be present only if the
 *                              scanDuration is set to 0xff, and, when present,
 *                              it indicates the NWK address for the device with
 *                              the Network Manager bit set in its Node Descriptor.
 * @param[in] u32ScanChannels   Channel bit mask, 32-bit field structure.
 * @param[in] u8ScanDuration    A value used to calculate the length of time to
 *                              spend on scanning each channel. 0x00 ~ 0x05 or
 *                              0xfe or 0xff.
 * @param[in] u8ScanCount       This field represents the number of energy scans
 *                              to be conducted and reported. This field shall
 *                              be present only if the scanDuration is within
 *                              the range of 0x00 to 0x05.
 * @param[in] u8NwkUpdateId     The value of the nwkUpdateId contained in this
 *                              request. This value is set by the Network
 *                              Channel Manager prior to sending the message.
 *                              This field shall only be present if the
 *                              scanDuration is 0xfe or 0xff. If the
 *                              scanDuration is 0xff, then the value in the
 *                              nwkUpdateId shall be ignored.
 */
void zbhci_MgmtNwkUpdateReq(uint16_t u16DstAddr,
                            uint16_t u16NwkManagerAddr,
                            uint32_t u32ScanChannels,
                            uint8_t  u8ScanDuration,
                            uint8_t  u8ScanCount,
                            uint8_t  u8NwkUpdateId);

/**
 * @brief get the MAC address form key pair table so as to get the information
 *        of the node which have joined the network
 *
 * @param[in] u8StartIdx Starting index for the requested elements of the joined
 *                       node list.
 */
void zbhci_NodesJoinedGetReq(uint8_t u8StartIdx);

/**
 * @brief send ON or OFF command in ONOFF cluster
 *
 * @param[in] u8OnOff         On/off command.
 * @param[in] u8TimerInterval The timer interval of the data transmission.
 *                            Unit: millisecond.
 */
void zbhci_NodesTogleTestReq(uint8_t u8OnOff, uint8_t u8TimerInterval);

/**
 * @brief send Tx Rx Performance test Req.
 *
 * @param u16DstAddr   The destination address to which this command will be sent.
 * @param u8SrcEp      The source endpoint for the binding entry.
 * @param u8DstEp      Destination endpoint
 * @param u16SendCnt   Unknown
 * @param u8Interval   Unknown
 * @param u8TxPowerSet Unused
 * @param pu8Payload   Unused
 */
void zbhci_TxRxPerformanceTestReq(uint16_t u16DstAddr,
                                  uint8_t  u8SrcEp,
                                  uint8_t  u8DstEp,
                                  uint16_t u16SendCnt,
                                  uint8_t  u8Interval,    //unit: 10ms
                                  uint8_t  u8TxPowerSet,
                                  uint8_t *pu8Payload);

/**
 * @brief Send AF data
 *
 * @param u16DstAddr   The destination address to which this command will be sent.
 * @param u8SrcEp      The source endpoint for the binding entry.
 * @param u8DstEp      Destination endpoint
 * @param u16ClusterId cluster identifier
 * @param u16DataLen   data length
 * @param pu8Payload   data payload
 */
void zbhci_AfDataSendTestReq(uint16_t u16DstAddr,
                             uint8_t  u8SrcEp,
                             uint8_t  u8DstEp,
                             uint16_t u16ClusterId,
                             uint16_t u16DataLen,
                             uint8_t *pu8Payload);

/**
 * @brief Send a Read Attribute command.
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u8Direction   direction of the command: \n
 *                            0 – Client to server; \n
 *                            1 – Server to client.
 * @param[in] u16ClusterID  Cluster identifier.
 * @param[in] u8AttrNum     The number of attributes to be read.
 * @param[in] pu16AttrList  The list of the attribute IDs to be read.
 */
void zbhci_ZclAttrRead(uint8_t    u8DstAddrMode,
                       ts_DstAddr sDstAddr,
                       uint8_t    u8SrcEp,
                       uint8_t    u8DstEp,
                       uint8_t    u8Direction,
                       uint16_t   u16ClusterID,
                       uint8_t    u8AttrNum,
                       uint16_t  *pu16AttrList);

/**
 * @brief Send a Write Attribute command
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                          0 – without destination address and endpoint, for binding; \n
 *                          1 – with group address; \n
 *                          2 – with destination short address and endpoint; \n
 *                          3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u8Direction   direction of the command: \n
 *                            0 – Client to server; \n
 *                            1 – Server to client.
 * @param[in] u16ClusterID  Cluster identifier.
 * @param[in] u8AttrNum     The number of attributes to be written.
 * @param[in] pu16AttrList  The list of the attributes to be written.
 */
void zbhci_ZclAttrWrite(uint8_t     u8DstAddrMode,
                       ts_DstAddr   sDstAddr,
                       uint8_t      u8SrcEp,
                       uint8_t      u8DstEp,
                       uint8_t      u8Direction,
                       uint16_t     u16ClusterID,
                       uint8_t      u8AttrNum,
                       ts_AttrList *psAttrList);

/**
 * @brief API to send ZCL configure report command
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u8Direction   specified the command direction: \n
 *                            0 – Client to server; \n
 *                            1 – Server to client.
 * @param[in] u16ClusterID  Cluster identifier.
 * @param[in] u8AttrNum     The number of attributes to be configured.
 * @param[in] pu16AttrList  The list of the attributes to be configured.
 */
void zbhci_ZclConfigReport(uint8_t     u8DstAddrMode,
                           ts_DstAddr   sDstAddr,
                           uint8_t      u8SrcEp,
                           uint8_t      u8DstEp,
                           uint8_t      u8Direction,
                           uint16_t     u16ClusterID,
                           uint8_t      u8AttrNum,
                           ts_AttrList *psAttrList);

/**
 * @brief This function can be used on a cluster client to send
 *        a ‘read reporting configuration’ command to a cluster server, in order
 *        to request the attribute reporting configuration data for a set of
 *        attributes.
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u8Direction   specified the command direction: \n
 *                            0 – Client to server;\n
 *                            1 – Server to client.
 * @param[in] u16ClusterID  Cluster identifier.
 * @param[in] u8AttrNum     The number of attributes’ configuration to be read.
 * @param[in] pu16AttrList  The list of the attributes to be read.
 */
void zbhci_ZclReadReportCfg(uint8_t    u8DstAddrMode,
                            ts_DstAddr sDstAddr,
                            uint8_t    u8SrcEp,
                            uint8_t    u8DstEp,
                            uint8_t    u8Direction,
                            uint16_t   u16ClusterID,
                            uint8_t    u8AttrNum,
                            uint16_t  *pu16AttrList);

/**
 * @brief API to send Basic Reset to factory default command
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 */
void zbhci_ZclBasicReset(uint8_t    u8DstAddrMode,
                         ts_DstAddr sDstAddr,
                         uint8_t    u8SrcEp,
                         uint8_t    u8DstEp);

/**
 * @brief API to send Add command in Group cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u16GroupId    Group identifier.
 * @param[in] pu8GroupName  Group name, character string.
 */
void zbhci_ZclGroupAdd(uint8_t    u8DstAddrMode,
                       ts_DstAddr sDstAddr,
                       uint8_t    u8SrcEp,
                       uint8_t    u8DstEp,
                       uint16_t   u16GroupId,
                       uint8_t   *pu8GroupName);

/**
 * @brief API to send View command in Group cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u16GroupId    Group identifier.
 */
void zbhci_ZclGroupView(uint8_t    u8DstAddrMode,
                        ts_DstAddr sDstAddr,
                        uint8_t    u8SrcEp,
                        uint8_t    u8DstEp,
                        uint16_t   u16GroupId);

/**
 * @brief API to send Get Membership command in Group cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u16GroupCount Group count.
 * @param[in] pu16GroupList Group list.
 */
void zbhci_ZclGroupGetMembership(uint8_t    u8DstAddrMode,
                                 ts_DstAddr sDstAddr,
                                 uint8_t    u8SrcEp,
                                 uint8_t    u8DstEp,
                                 uint8_t    u8GroupCount,
                                 uint16_t  *pu16GroupList);

/**
 * @brief API to send Remove command in Group cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u16GroupId    Group identifier.
 */
void zbhci_ZclGroupRemove(uint8_t    u8DstAddrMode,
                          ts_DstAddr sDstAddr,
                          uint8_t    u8SrcEp,
                          uint8_t    u8DstEp,
                          uint16_t   u16GroupId);

/**
 * @brief API to send Remove All command in Group cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 */
void zbhci_ZclGroupRemoveAll(uint8_t    u8DstAddrMode,
                             ts_DstAddr sDstAddr,
                             uint8_t    u8SrcEp,
                             uint8_t    u8DstEp);

/**
 * @brief API to send View command in Group cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u16GroupId    Group identifier.
 * @param[in] pu8GroupName  Group name, character string.
 */
void zbhci_ZclGroupAddIfIdentify(uint8_t    u8DstAddrMode,
                                 ts_DstAddr sDstAddr,
                                 uint8_t    u8SrcEp,
                                 uint8_t    u8DstEp,
                                 uint16_t   u16GroupId,
                                 uint8_t   *pu8GroupName);

/**
 * @brief API to send Identify Query command in IDENTIFY cluster.
 *
 * @param[in] u8DstAddrMode   Destination address mode: \n
 *                              0 – without destination address and endpoint, for binding; \n
 *                              1 – with group address; \n
 *                              2 – with destination short address and endpoint; \n
 *                              3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr        Destination address for this command.
 * @param[in] u8SrcEp         Source endpoint.
 * @param[in] u8DstEp         Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u16IdentifyTime Unsigned 16-bit integer.
 */
void zbhci_ZclIdentifyQuery(uint8_t    u8DstAddrMode,
                            ts_DstAddr sDstAddr,
                            uint8_t    u8SrcEp,
                            uint8_t    u8DstEp,
                            uint16_t   u16IdentifyTime);

/**
 * @brief API to send ON command in ONOFF cluster
 *
 * @param[in] u8DstAddrMode   Destination address mode: \n
 *                              0 – without destination address and endpoint, for binding; \n
 *                              1 – with group address; \n
 *                              2 – with destination short address and endpoint; \n
 *                              3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr        Destination address for this command.
 * @param[in] u8SrcEp         Source endpoint.
 * @param[in] u8DstEp         Destination endpoint if dstAddrMode is 2 or 3.
 */
void zbhci_ZclOnoffOn(uint8_t    u8DstAddrMode,
                      ts_DstAddr sDstAddr,
                      uint8_t    u8SrcEp,
                      uint8_t    u8DstEp);

/**
 * @brief API to send OFF command in ONOFF cluster
 *
 * @param[in] u8DstAddrMode   Destination address mode: \n
 *                              0 – without destination address and endpoint, for binding; \n
 *                              1 – with group address; \n
 *                              2 – with destination short address and endpoint; \n
 *                              3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr        Destination address for this command.
 * @param[in] u8SrcEp         Source endpoint.
 * @param[in] u8DstEp         Destination endpoint if dstAddrMode is 2 or 3.
 */
void zbhci_ZclOnoffOff(uint8_t    u8DstAddrMode,
                       ts_DstAddr sDstAddr,
                       uint8_t    u8SrcEp,
                       uint8_t    u8DstEp);

/**
 * @brief API to send TOGGLE command in ONOFF cluster
 *
 * @param[in] u8DstAddrMode   Destination address mode: \n
 *                              0 – without destination address and endpoint, for binding; \n
 *                              1 – with group address; \n
 *                              2 – with destination short address and endpoint; \n
 *                              3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr        Destination address for this command.
 * @param[in] u8SrcEp         Source endpoint.
 * @param[in] u8DstEp         Destination endpoint if dstAddrMode is 2 or 3.
 */
void zbhci_ZclOnoffToggle(uint8_t    u8DstAddrMode,
                          ts_DstAddr sDstAddr,
                          uint8_t    u8SrcEp,
                          uint8_t    u8DstEp);

/**
 * @brief API to send Move To Level command in Level cluster.
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u8Level       Level.
 * @param[in] u16TransTime  Transition time, 1/10ths of a second.
 */
void zbhci_ZclLevelMove2level(uint8_t    u8DstAddrMode,
                              ts_DstAddr sDstAddr,
                              uint8_t    u8SrcEp,
                              uint8_t    u8DstEp,
                              uint8_t    u8Level,
                              uint16_t   u16TransTime);

/**
 * @brief API to send Move command in Level cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u8Mode        Move mode. \n
 *                            0x00 – Up; \n
 *                            0x01 – Down.
 * @param[in] u8Rate        The rate field specifies the rate of movement in
 *                          units per second.
 */
void zbhci_ZclLevelMove(uint8_t    u8DstAddrMode,
                        ts_DstAddr sDstAddr,
                        uint8_t    u8SrcEp,
                        uint8_t    u8DstEp,
                        uint8_t    u8Mode,
                        uint8_t    u8Rate);

/**
 * @brief API to send Step command in LEVEL cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u8Mode        Move mode. \n
 *                          0x00 – Up; \n
 *                          0x01 – Down.
 * @param[in] u8StepSize    A step is a change in the current level of
 *                          'step size' units.
 * @param[in] u16TransTime  The transition time field specifies the time that
 *                          shall be taken to perform the step, in 1/10ths of a
 *                          second.
 */
void zbhci_ZclLevelStep(uint8_t    u8DstAddrMode,
                        ts_DstAddr sDstAddr,
                        uint8_t    u8SrcEp,
                        uint8_t    u8DstEp,
                        uint8_t    u8Mode,
                        uint8_t    u8StepSize,
                        uint16_t   u16TransTime);

/**
 * @brief API to send Stop command in Level cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 */
void zbhci_ZclLevelStop(uint8_t    u8DstAddrMode,
                        ts_DstAddr sDstAddr,
                        uint8_t    u8SrcEp,
                        uint8_t    u8DstEp);

/**
 * @brief API to send Move To Level command in Level cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u8Level       Level.
 * @param[in] u16TransTime  Transition time, 1/10ths of a second.
 */
void zbhci_ZclLevelMove2levelWithonoff(uint8_t    u8DstAddrMode,
                                       ts_DstAddr sDstAddr,
                                       uint8_t    u8SrcEp,
                                       uint8_t    u8DstEp,
                                       uint8_t    u8Level,
                                       uint16_t   u16TransTime);

/**
 * @brief API to send Move command in Level cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u8Mode        Move mode. \n
 *                            0x00 – Up; \n
 *                            0x01 – Down.
 * @param[in] u8Rate        The rate field specifies the rate of movement in
 *                          units per second.
 */
void zbhci_ZclLevelMoveWithonoff(uint8_t    u8DstAddrMode,
                                 ts_DstAddr sDstAddr,
                                 uint8_t    u8SrcEp,
                                 uint8_t    u8DstEp,
                                 uint8_t    u8Mode,
                                 uint8_t    u8Rate);

/**
 * @brief API to send Step command in LEVEL cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u8Mode        Move mode. \n
 *                          0x00 – Up; \n
 *                          0x01 – Down.
 * @param[in] u8StepSize    A step is a change in the current level of
 *                          'step size' units.
 * @param[in] u16TransTime  The transition time field specifies the time that
 *                          shall be taken to perform the step, in 1/10ths of a
 *                          second.
 */
void zbhci_ZclLevelStepWithonoff(uint8_t    u8DstAddrMode,
                                 ts_DstAddr sDstAddr,
                                 uint8_t    u8SrcEp,
                                 uint8_t    u8DstEp,
                                 uint8_t    u8Mode,
                                 uint8_t    u8StepSize,
                                 uint16_t   u16TransTime);

/**
 * @brief API to send Stop command in Level cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 */
void zbhci_ZclLevelStopWithonoff(uint8_t    u8DstAddrMode,
                                 ts_DstAddr sDstAddr,
                                 uint8_t    u8SrcEp,
                                 uint8_t    u8DstEp);

/**
 * @brief API to send add scene command in SCENE cluster
 *
 * @param[in] u8DstAddrMode   Destination address mode: \n
 *                              0 – without destination address and endpoint, for binding; \n
 *                              1 – with group address; \n
 *                              2 – with destination short address and endpoint; \n
 *                              3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr        Destination address for this command.
 * @param[in] u8SrcEp         Source endpoint.
 * @param[in] u8DstEp         Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u16GroupId      The group ID for which this scene applies.
 * @param[in] u8SceneId       The identifier, unique within this group, which is
 *                            used to identify this scene.
 * @param[in] u16TransTime    The amount of time, in seconds, it will take for
 *                            the device to change from its current state to the
 *                            requested scene.
 * @param[in] u8SceneNameLen  Length of scene name.
 * @param[in] pu8SceneName    Scene name, char string.
 * @param[in] u8ExtFieldLen   Length of extFieldSets field.
 * @param[in] pu8ExtFieldSets The sum of all such defines a scene.
 */
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
                       uint8_t   *pu8ExtFieldSets);

/**
 * @brief API to send view scene command in SCENE cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u16GroupId    The group ID for which this scene applies.
 * @param[in] u8SceneId     The identifier, unique within this group, which is
 *                          used to identify this scene.
 */
void zbhci_ZclSceneView(uint8_t    u8DstAddrMode,
                        ts_DstAddr sDstAddr,
                        uint8_t    u8SrcEp,
                        uint8_t    u8DstEp,
                        uint16_t   u16GroupId,
                        uint8_t    u8SceneId);

/**
 * @brief API to send remove scene command in SCENE cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u16GroupId    The group ID for which this scene applies.
 * @param[in] u8SceneId     The identifier, unique within this group, which is
 *                          used to identify this scene.
 */
void zbhci_ZclSceneRemove(uint8_t    u8DstAddrMode,
                          ts_DstAddr sDstAddr,
                          uint8_t    u8SrcEp,
                          uint8_t    u8DstEp,
                          uint16_t   u16GroupId,
                          uint8_t    u8SceneId);

/**
 * @brief API to send remove all scene command in SCENE cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u16GroupId    The group ID for which this scene applies.
 */
void zbhci_ZclSceneRemoveAll(uint8_t    u8DstAddrMode,
                             ts_DstAddr sDstAddr,
                             uint8_t    u8SrcEp,
                             uint8_t    u8DstEp,
                             uint16_t   u16GroupId);

/**
 * @brief API to send store scene command in SCENE cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u16GroupId    The group ID for which this scene applies.
 * @param[in] u8SceneId     The identifier, unique within this group, which is
 *                          used to identify this scene.
 */
void zbhci_ZclSceneStore(uint8_t    u8DstAddrMode,
                         ts_DstAddr sDstAddr,
                         uint8_t    u8SrcEp,
                         uint8_t    u8DstEp,
                         uint16_t   u16GroupId,
                         uint8_t    u8SceneId);

/**
 * @brief API to send recall scene command in SCENE cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u16GroupId    The group ID for which this scene applies.
 * @param[in] u8SceneId     The identifier, unique within this group, which is
 *                          used to identify this scene.
 */
void zbhci_ZclSceneRecall(uint8_t    u8DstAddrMode,
                          ts_DstAddr sDstAddr,
                          uint8_t    u8SrcEp,
                          uint8_t    u8DstEp,
                          uint16_t   u16GroupId,
                          uint8_t    u8SceneId);

/**
 * @brief API to send get scene membership command in SCENE cluster
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u16GroupId    The group ID for which this scene applies.
 */
void zbhci_ZclSceneGetMembership(uint8_t    u8DstAddrMode,
                                 ts_DstAddr sDstAddr,
                                 uint8_t    u8SrcEp,
                                 uint8_t    u8DstEp,
                                 uint16_t   u16GroupId);


/**
 * @brief This function sends a Move to Hue command to instruct a device to move
 *        its ‘current hue’ attribute to a target hue value in a continuous
 *        manner within a given time.
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u8Hue         the target hue value.
 * @param[in] u8Direction   the direction/path of the change in hue
 * @param[in] u16TransTime  the time period, in tenths of a second, over which
 *                          the change in hue should be implemented.
 */
void zbhci_ZclColorMove2hue(uint8_t    u8DstAddrMode,
                            ts_DstAddr sDstAddr,
                            uint8_t    u8SrcEp,
                            uint8_t    u8DstEp,
                            uint8_t    u8Hue,
                            uint8_t    u8Direction,
                            uint16_t   u16TransTime);

/**
 * @brief This function sends a Move to Colour command to instruct a device to
 *        move its ‘current x’ and ‘current y’ attributes to target values in a
 *        continuous manner within a given time (where x and y are the
 *        chromaticities from the CIE xyY colour space).
 *
 * @param u8DstAddrMode Destination address mode: \n
 *                        0 – without destination address and endpoint, for binding; \n
 *                        1 – with group address; \n
 *                        2 – with destination short address and endpoint; \n
 *                        3 – with destination IEEE address and endpoint.
 * @param sDstAddr      Destination address for this command.
 * @param u8SrcEp       Source endpoint.
 * @param u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param u16ColorX     the target x-chromaticity in the CIE xyY colour space
 * @param u16ColorY     the target y-chromaticity in the CIE xyY colour space
 * @param u16TransTime  the time period, in tenths of a second, over which the
 *                      colour change should be implemented.
 */
void zbhci_ZclColorMove2Color(uint8_t    u8DstAddrMode,
                              ts_DstAddr sDstAddr,
                              uint8_t    u8SrcEp,
                              uint8_t    u8DstEp,
                              uint16_t   u16ColorX,
                              uint16_t   u16ColorY,
                              uint16_t   u16TransTime);

/**
 * @brief This function sends a Move to Saturation command to instruct a device
 *        to move its ‘current saturation’ attribute to a target saturation
 *        value in a continuous manner within a given time.
 *
 * @param u8DstAddrMode Destination address mode: \n
 *                        0 – without destination address and endpoint, for binding; \n
 *                        1 – with group address; \n
 *                        2 – with destination short address and endpoint; \n
 *                        3 – with destination IEEE address and endpoint.
 * @param sDstAddr      Destination address for this command.
 * @param u8SrcEp       Source endpoint.
 * @param u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param u8Saturation  the target saturation value.
 * @param u16TransTime  the time period, in tenths of a second, over which the
 *                      change in saturation should be implemented.
 */
void zbhci_ZclColorMove2sat(uint8_t    u8DstAddrMode,
                            ts_DstAddr sDstAddr,
                            uint8_t    u8SrcEp,
                            uint8_t    u8DstEp,
                            uint8_t    u8Saturation,
                            uint16_t   u16TransTime);

/**
 * @brief This function sends a Move to Colour Temperature command to instruct a
 *        device to move its ‘mired colour temperature’ attribute to a target
 *        value in a continuous manner within a given time.
 *
 * @param u8DstAddrMode       Destination address mode: \n
 *                              0 – without destination address and endpoint, for binding; \n
 *                              1 – with group address; \n
 *                              2 – with destination short address and endpoint; \n
 *                              3 – with destination IEEE address and endpoint.
 * @param sDstAddr            Destination address for this command.
 * @param u8SrcEp             Source endpoint.
 * @param u8DstEp             Destination endpoint if dstAddrMode is 2 or 3.
 * @param u16ColorTemperature the target value of colour temperature
 * @param u16TransTime        the time period, in tenths of a second, over which
 *                            the change in colour temperature should be implemented.
 */
void zbhci_ZclColorMove2temp(uint8_t    u8DstAddrMode,
                             ts_DstAddr sDstAddr,
                             uint8_t    u8SrcEp,
                             uint8_t    u8DstEp,
                             uint16_t   u16ColorTemperature,
                             uint16_t   u16TransTime);

/**
 * @brief This function issues an Image Notify message to one or more clients to
 *        indicate that a new application image is available for download.
 *
 * @param[in] u8DstAddrMode Destination address mode: \n
 *                            0 – without destination address and endpoint, for binding; \n
 *                            1 – with group address; \n
 *                            2 – with destination short address and endpoint; \n
 *                            3 – with destination IEEE address and endpoint.
 * @param[in] sDstAddr      Destination address for this command.
 * @param[in] u8SrcEp       Source endpoint.
 * @param[in] u8DstEp       Destination endpoint if dstAddrMode is 2 or 3.
 * @param[in] u8PayloadType 0x00 – Query jitter; \n
 *                          0x01 – Query jitter and manufacturer code; \n
 *                          0x02 – Query jitter, manufacturer code, and image type; \n
 *                          0x03 - Query jitter, manufacturer code, image type,
 *                                 and new file version.
 * @param[in] queryJitter   By using the parameter, it prevents a single
 *                          notification of a new OTA upgrade image from
 *                          flooding the upgrade server with requests from clients.
 */
void zbhci_ZclOtaImageNotify(uint8_t    u8DstAddrMode,
                             ts_DstAddr sDstAddr,
                             uint8_t    u8SrcEp,
                             uint8_t    u8DstEp,
                             uint8_t    u8PayloadType,
                             uint8_t    u8QueryJitter);

/** @} end of group zbhci_fountions */

#ifdef __cplusplus
}
#endif

#endif
/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/