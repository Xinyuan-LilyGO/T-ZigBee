
#include "WiFi.h"
#include <OneButton.h>

#include <stdio.h>
#include <stdbool.h>
// #include <string.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <string.h>
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_system.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "LittleFS.h"
#include "ArduinoJson.h"
#include <cJSON.h>

#include "app_mqtt.h"
#include "zbhci.h"
#include "app_db.h"
#include "device.h"
#include "web.h"

#define CONFIG_USR_BUTTON_PIN 2
#define CONFIG_BLUE_LIGHT_PIN 3

QueueHandle_t msg_queue;

String staStatus = "n/a";
String staSSID = "";
String staPassword = "";
String apStatus = "n/a";
String apSSID = "";
String mqttServer = "";
uint32_t mqttPort = 0;
String mqttUsername = "";
String mqttPassword = "";

// MQTTClient mqtt;

OneButton btn = OneButton(
    CONFIG_USR_BUTTON_PIN, /** Input pin for the button */
    true,                  /** Button is active LOW */
    false                  /** Enable internal pull-up resistor */
);

void setup() {
    Serial.begin(115200);

    if(!LittleFS.begin()) {
        Serial.printf("An Error has occurred while mounting LittleFS\n");
        delay(1000);
        return;
    }

    pinMode(CONFIG_BLUE_LIGHT_PIN, OUTPUT);
    digitalWrite(CONFIG_BLUE_LIGHT_PIN, LOW);

    appLoadDateBase();
    appPowerInit();
    appNVSInit();
    appWiFiInit();
    webInit();
    mqtt.attachConnectedEvent(appDataBaseRecover);
    mqtt.subscribe("zigbee2mqtt/bridge/request/+", appHandlrMqttPermitJoin);
    appDataBaseInit();

    msg_queue = xQueueCreate(10, sizeof(ts_HciMsg));

    // Power on the zigbee chip:
    //   appPowerOn(true);
    // Power off the zigbee chip:
    //   appPowerOn(false);
    //   zbhci_Deinit();
    appPowerOn(true);

    zbhci_Init(msg_queue);
    delay(100);
    xTaskCreatePinnedToCore(
        zbhciTask,
        "zbhciTask",
        4096,
        NULL,
        5,
        NULL ,
        ARDUINO_RUNNING_CORE
    );
    xTaskCreatePinnedToCore(
        ledTask,
        "ledTask",
        2048,
        NULL,
        4,
        NULL ,
        ARDUINO_RUNNING_CORE
    );
    xTaskCreatePinnedToCore(
        buttonTask,
        "buttonTask",
        2048,
        NULL,
        4,
        NULL ,
        ARDUINO_RUNNING_CORE
    );

    delay(100);
    zbhci_NetworkStateReq();
    delay(100);
    zbhci_NodesJoinedGetReq(0);
    btn.attachClick(handleClick);
    btn.attachDoubleClick(handleDoubleClick);
}


void loop() {
    webLoop();
}


void buttonTask(void *pvParameters) {
    while (1) {
        btn.tick();
        delay(10);
    }
    vTaskDelete(NULL);
}

void ledTask(void *pvParameters) {
    while (1) {
        if (staStatus == "n/a") {
            digitalWrite(CONFIG_BLUE_LIGHT_PIN, LOW);
            delay(1000);
            digitalWrite(CONFIG_BLUE_LIGHT_PIN, HIGH);
            delay(1000);
        } else if (staStatus == "running" && !mqtt.connected()) {
            digitalWrite(CONFIG_BLUE_LIGHT_PIN, LOW);
            delay(3000);
            digitalWrite(CONFIG_BLUE_LIGHT_PIN, HIGH);
            delay(3000);
        } else if (staStatus == "running" && mqtt.connected()) {
            digitalWrite(CONFIG_BLUE_LIGHT_PIN, HIGH);
            delay(100);
        }
    }
    vTaskDelete(NULL);
}


void zbhciTask(void *pvParameters) {
    ts_HciMsg sHciMsg;

    while (1) {
        bzero(&sHciMsg, sizeof(sHciMsg));
        if (xQueueReceive(msg_queue, &sHciMsg, portMAX_DELAY)) {
            switch (sHciMsg.u16MsgType) {
                case ZBHCI_CMD_BDB_COMMISSION_FORMATION_RSP:
                    zbhci_MgmtPermitJoinReq(0xFFFC, 0xFF, 1);
                break;

                case ZBHCI_CMD_NETWORK_STATE_RSP:
                    appHandleNetworkStateRsp(&sHciMsg.uPayload.sNetworkStateRspPayloasd);
                break;

                case ZBHCI_CMD_NODES_DEV_ANNCE_IND:
                    appHandleDeviceAnnouncementIndication(&sHciMsg.uPayload.sNodesDevAnnceRspPayload);
                break;

                case ZBHCI_CMD_LEAVE_INDICATION:
                    appHandleLeaveIndication(&sHciMsg.uPayload.sLeaveIndicationPayload);
                break;

                case ZBHCI_CMD_ZCL_REPORT_MSG_RCV:
                    appHandleZCLreportMsgRcv(&sHciMsg.uPayload.sZclReportMsgRcvPayload);
                break;

                case ZBHCI_CMD_ZCL_ATTR_READ_RSP:
                    appHandleZCLReadResponse(&sHciMsg.uPayload.sZclAttrReadRspPayload);
                break;

                default:
                break;
            }
        }
        delay(100);
    }
    vTaskDelete(NULL);
}


void handleClick(void) {
    // digitalWrite(CONFIG_BLUE_LIGHT_PIN, LOW);
    zbhci_MgmtPermitJoinReq(0xFFFC, 0x00, 1);
}


void handleDoubleClick(void) {
    // digitalWrite(CONFIG_BLUE_LIGHT_PIN, HIGH);
    zbhci_MgmtPermitJoinReq(0xFFFC, 0xFF, 1);
}


void appLoadDateBase(void) {
    StaticJsonDocument<1024> doc;

    File configfile = LittleFS.open("/db.json", "r");
    DeserializationError error = deserializeJson(doc, configfile);
    if (error)
        Serial.println(F("Failed to read file, using default configuration"));
    configfile.close();

    staSSID = doc["sta"]["ssid"].as<const char *>();
    staPassword = doc["sta"]["pwd"].as<const char *>();
    mqttServer = doc["mqtt"]["server"].as<const char *>();
    mqttPort = doc["mqtt"]["port"].as<uint32_t>();
    mqttUsername = doc["mqtt"]["username"].as<const char *>();
    mqttPassword = doc["mqtt"]["password"].as<const char *>();
}


void onWiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\r\n", event);

    switch (event) {
        case ARDUINO_EVENT_WIFI_READY:
            Serial.println("WiFi interface ready");
            break;
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            Serial.println("Completed scan for access points");
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("WiFi client started");
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            Serial.println("WiFi clients stopped");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("Connected to access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            WiFi.begin(staSSID.c_str(), staPassword.c_str());
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            staStatus = "running";
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
            mqtt.connect(mqttServer.c_str(), mqttPort);
            break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            Serial.println("Lost IP address and IP address is reset to 0");
            break;
        case ARDUINO_EVENT_WPS_ER_SUCCESS:
            Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_FAILED:
            Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_TIMEOUT:
            Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_PIN:
            Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("WiFi access point started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            Serial.println("WiFi access point  stopped");
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            Serial.println("Client connected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            Serial.println("Client disconnected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            Serial.println("Assigned IP address to client");
            break;
        case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
            Serial.println("Received probe request");
            break;
        case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
            Serial.println("AP IPv6 is preferred");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            Serial.println("STA IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP6:
            Serial.println("Ethernet IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_START:
            Serial.println("Ethernet started");
            break;
        case ARDUINO_EVENT_ETH_STOP:
            Serial.println("Ethernet stopped");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            Serial.println("Ethernet connected");
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println("Ethernet disconnected");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.println("Obtained IP address");
            break;
        default: break;
    }
}


void appWiFiInit(void) {
    uint8_t mac[6];
    char ssid[32];

    WiFi.disconnect(true);
    delay(1000);

    Serial.println("WiFi: Set mode to WIFI_AP_STA");
    WiFi.mode(WIFI_AP_STA);
    WiFi.onEvent(onWiFiEvent);

    WiFi.begin(staSSID.c_str(), staPassword.c_str());

    //Wait for WiFi to connect to AP
    Serial.println("Waiting for WiFi");

    WiFi.softAPmacAddress(mac);
    sprintf(ssid, "LILYGO-%02X%02X", mac[4], mac[5]);
    if (WiFi.softAP(ssid) != true) {
        Serial.println("WiFi: failed to create softAP");
        return ;
    }
    apStatus = "running";
    apSSID = ssid;
    Serial.println("WiFi: softAP has been established");
    Serial.printf("WiFi: please connect to the %s\r\n", ssid);
}


void appNVSInit(void) {
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || \
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}


void appPowerInit() {
    pinMode(0, OUTPUT);
}


void appPowerOn(bool active) {
    if (active) {
        digitalWrite(0, HIGH);
    } else {
        digitalWrite(0, LOW);
    }
}


void appHandleNetworkStateRsp(ts_MsgNetworkStateRspPayload *payload) {
    if (payload->u16NwkAddr != 0x0000) {
        zbhci_BdbChannelSet(25);
        delay(100);
        zbhci_BdbCommissionFormation();
    } else {
        brigeNode.nwkAddr  = payload->u16NwkAddr;
        brigeNode.macAddr  = payload->u64IeeeAddr;
        brigeNode.panId    = payload->u16PanId;
        brigeNode.exPanId = payload->u64extPanId;
        brigeNode.channel   = payload->u8Channel;
        appDataBaseRecover();
        zbhci_MgmtPermitJoinReq(0xFFFC, 0xFF, 1);
        digitalWrite(CONFIG_BLUE_LIGHT_PIN, HIGH);
    }
}


void appHandleDeviceAnnouncementIndication(ts_MsgNodesDevAnnceRspPayload *payload) {
    DeviceNode *device = NULL;
    device = getEmptyDevice();
    if (!device) return;
    device->u16NwkAddr  = payload->u16NwkAddr;
    device->u64IeeeAddr = payload->u64IEEEAddr;
    device->u8Type      = payload->u8Capability;
    // When the device is connected to the network, read its model id
    uint16_t au16AttrList[1] = {0x0005};
    zbhci_ZclAttrRead(0x02, (ts_DstAddr) {.u16DstAddr = payload->u16NwkAddr}, 1, 1, 0, 0x0000, 1, au16AttrList);
}


void appHandleLeaveIndication(ts_MsgLeaveIndicationPayload *payload) {
    DeviceNode *device = NULL;

    device = findDeviceByIeeeaddr(payload->u64MacAddr);
    if (device == NULL) {
        return ;
    }

    if (!payload->u8Rejoin) {
        if (!strncmp((const char *)device->au8ModelId,
                        "lumi.sensor_motion.aq2",
                        strlen("lumi.sensor_motion.aq2")))
        {
            rtcgq11lmDelete(device->u64IeeeAddr);
        }
        else if (!strncmp((const char *)device->au8ModelId,
                            "lumi.weather",
                            strlen("lumi.weather")))
        {
            wsdcgq11lmDelete(device->u64IeeeAddr);
        }
        else if (!strncmp((const char *)device->au8ModelId,
                            "LILYGO.Light",
                            strlen("LILYGO.Light")))
        {
            lilygoLightDelete(device->u64IeeeAddr);
        }
        else if (!strncmp((const char *)device->au8ModelId,
                            "LILYGO.Sensor",
                            strlen("LILYGO.Sensor")))
        {
            lilygoSensorDelete(device->u64IeeeAddr);
        }
        else if (!strncmp((const char *)device->au8ModelId,
                            "ESP32C6.Light",
                            strlen("ESP32C6.Light")))
        {
            espressifLightDelete(device->u64IeeeAddr);
        }
        memset(device, 0, sizeof(DeviceNode));
        appDataBaseSave();
    }
}


void appHandleZCLreportMsgRcv(ts_MsgZclReportMsgRcvPayload *payload) {
    DeviceNode *device = NULL;
    device = findDeviceByNwkaddr(payload->u16SrcAddr);
    if (device == NULL) {
        return ;
    }
    switch (payload->u16ClusterId) {
        case 0x0000: /* Basic Cluster */
            for (size_t i = 0; i < payload->u8AttrNum; i++) {
                if (payload->asAttrList[i].u16AttrID == 0x0005) {
                    memcpy(
                        device->au8ModelId,
                        payload->asAttrList[i].uAttrData.au8AttrData,
                        payload->asAttrList[i].u16DataLen
                    );
                    if (!strncmp((const char *)payload->asAttrList[i].uAttrData.au8AttrData,
                                 "lumi.sensor_motion.aq2",
                                 strlen("lumi.sensor_motion.aq2")))
                    {
                        appDataBaseSave();
                        rtcgq11lmAdd(device->u64IeeeAddr);
                    }
                    else if (!strncmp((const char *)payload->asAttrList[i].uAttrData.au8AttrData,
                                      "lumi.weather",
                                      strlen("lumi.weather")))
                    {
                        appDataBaseSave();
                        wsdcgq11lmAdd(device->u64IeeeAddr);
                    }
                    else if (!strncmp((const char *)payload->asAttrList[i].uAttrData.au8AttrData,
                                        "LILYGO.Light",
                                        strlen("LILYGO.Light")))
                    {
                        appDataBaseSave();
                        lilygoLightAdd(device->u64IeeeAddr);
                    }
                    else if (!strncmp((const char *)payload->asAttrList[i].uAttrData.au8AttrData,
                                      "LILYGO.Sensor",
                                      strlen("LILYGO.Sensor")))
                    {
                        appDataBaseSave();
                        lilygoSensorAdd(device->u64IeeeAddr);
                    }
                    else if (!strncmp((const char *)payload->asAttrList[i].uAttrData.au8AttrData,
                                       "ESP32C6.Light",
                                       strlen("ESP32C6.Light")))
                    {
                        appDataBaseSave();
                        espressifLightAdd(device->u64IeeeAddr);
                        // The configure method below is needed to make the device reports on/off state changes
                        // when the device is controlled manually through the button on it.
                        zbhci_BindingReq(
                            device->u64IeeeAddr,
                            1,
                            0x0006,
                            0x03,
                            (ts_DstAddr) {
                                .u64DstAddr = brigeNode.macAddr
                            },
                            1
                        );
                    }
                }
            }
        break;

        case 0x0006:
        {
            for (size_t i = 0; i < payload->u8AttrNum; i++)
            {
                if (payload->asAttrList[i].u16AttrID == 0x0000)
                {
                    device->deviceData.light.u8State = payload->asAttrList[i].uAttrData.u8AttrData;
                    if (!strncmp((const char *)device->au8ModelId, "LILYGO.Light", strlen("LILYGO.Light")) || \
                        !strncmp((const char *)device->au8ModelId, "ESP32C6.Light", strlen("ESP32C6.Light")))
                    {
                        lilygoLightReport(
                            device->u64IeeeAddr,
                            device->deviceData.light.u8State
                        );
                    }
                }
            }
        }
        break;

        case 0x0400: /* Illuminance Measurement Cluster */
        {
            for (size_t i = 0; i < payload->u8AttrNum; i++)
            {
                if (payload->asAttrList[i].u16AttrID == 0x0000)
                {
                    device->deviceData.rtcgq11lm.u16Illuminance = payload->asAttrList[i].uAttrData.u16AttrData;
                    if (!strncmp((const char *)device->au8ModelId,
                                 "lumi.sensor_motion.aq2",
                                 strlen("lumi.sensor_motion.aq2")))
                    {
                        rtcgq11lmReport(
                            device->u64IeeeAddr,
                            device->deviceData.rtcgq11lm.u8Occupancy,
                            device->deviceData.rtcgq11lm.u16Illuminance
                        );
                    }
                }
            }
        }
        break;

        case 0x0402: /* Temperature Measurement Cluster */
        {
            for (size_t i = 0; i < payload->u8AttrNum; i++)
            {
                if (payload->asAttrList[i].u16AttrID == 0x0000)
                {
                    device->deviceData.wsdcgq11lm.i16Temperature = (int16_t)payload->asAttrList[i].uAttrData.u16AttrData;
                    if (!strncmp((const char *)device->au8ModelId,
                                 "lumi.weather",
                                 strlen("lumi.weather")))
                    {
                        wsdcgq11lmReport(
                            device->u64IeeeAddr,
                            device->deviceData.wsdcgq11lm.i16Temperature,
                            device->deviceData.wsdcgq11lm.i16Humidity,
                            device->deviceData.wsdcgq11lm.i16Pressure
                        );
                    }
                    else if (!strncmp((const char *)device->au8ModelId,
                                      "LILYGO.Sensor",
                                      strlen("LILYGO.Sensor")))
                    {
                        lilygoSensorReport(
                            device->u64IeeeAddr,
                            device->deviceData.sensor.i16Temperature,
                            device->deviceData.sensor.i16Humidity
                        );
                    }
                }
            }
        }
        break;

        case 0x0403:/* Pressure Measurement Cluster */
        {
            for (size_t i = 0; i < payload->u8AttrNum; i++)
            {
                if (payload->asAttrList[i].u16AttrID == 0x0000)
                {
                    device->deviceData.wsdcgq11lm.i16Pressure = (int16_t)payload->asAttrList[i].uAttrData.u16AttrData;
                    if (!strncmp((const char *)device->au8ModelId,
                                 "lumi.weather",
                                 strlen("lumi.weather")))
                    {
                        wsdcgq11lmReport(
                            device->u64IeeeAddr,
                            device->deviceData.wsdcgq11lm.i16Temperature,
                            device->deviceData.wsdcgq11lm.i16Humidity,
                            device->deviceData.wsdcgq11lm.i16Pressure
                        );
                    }
                }
            }
        }
        break;

        case 0x0405: /* Relative Humidity Measurement Cluster */
        {
            for (size_t i = 0; i < payload->u8AttrNum; i++)
            {
                if (payload->asAttrList[i].u16AttrID == 0x0000)
                {
                    device->deviceData.wsdcgq11lm.i16Humidity = (int16_t)payload->asAttrList[i].uAttrData.u16AttrData;
                    if (!strncmp((const char *)device->au8ModelId,
                                 "lumi.weather",
                                 strlen("lumi.weather")))
                    {
                        wsdcgq11lmReport(
                            device->u64IeeeAddr,
                            device->deviceData.wsdcgq11lm.i16Temperature,
                            device->deviceData.wsdcgq11lm.i16Humidity,
                            device->deviceData.wsdcgq11lm.i16Pressure
                        );
                    }
                    else if (!strncmp((const char *)device->au8ModelId,
                                      "LILYGO.Sensor",
                                      strlen("LILYGO.Sensor")))
                    {
                        lilygoSensorReport(
                            device->u64IeeeAddr,
                            device->deviceData.sensor.i16Temperature,
                            device->deviceData.sensor.i16Humidity
                        );
                    }
                }
            }
        }
        break;

        case 0x0406: /* Occupancy Sensing Cluster */
        {
            for (size_t i = 0; i < payload->u8AttrNum; i++)
            {
                if (payload->asAttrList[i].u16AttrID == 0x0000) {
                    device->deviceData.rtcgq11lm.u8Occupancy = payload->asAttrList[i].uAttrData.u8AttrData;
                    if (!strncmp((const char *)device->au8ModelId,
                                 "lumi.sensor_motion.aq2",
                                 strlen("lumi.sensor_motion.aq2")))
                    {
                        rtcgq11lmReport(
                            device->u64IeeeAddr,
                            device->deviceData.rtcgq11lm.u8Occupancy,
                            device->deviceData.rtcgq11lm.u16Illuminance
                        );
                    }
                }
            }
        }
        break;

        default:
        break;
    }
}


void appHandleZCLReadResponse(ts_MsgZclAttrReadRspPayload *payload) {
    DeviceNode *device = NULL;
    device = findDeviceByNwkaddr(payload->u16SrcAddr);
    if (device == NULL) {
        return ;
    }
    switch (payload->u16ClusterId) {
        case 0x0000: /* Basic Cluster */
            for (size_t i = 0; i < payload->u8AttrNum; i++) {
                if (payload->asAttrReadList[i].u16AttrID == 0x0005) {
                    printf("attr\n");
                    memcpy(
                        device->au8ModelId,
                        payload->asAttrReadList[i].uAttrData.au8AttrData,
                        payload->asAttrReadList[i].u16DataLen
                    );
                    if (!strncmp((const char *)payload->asAttrReadList[i].uAttrData.au8AttrData,
                                 "lumi.sensor_motion.aq2",
                                 strlen("lumi.sensor_motion.aq2")))
                    {
                        appDataBaseSave();
                        rtcgq11lmAdd(device->u64IeeeAddr);
                    }
                    else if (!strncmp((const char *)payload->asAttrReadList[i].uAttrData.au8AttrData,
                                      "lumi.weather",
                                      strlen("lumi.weather")))
                    {
                        appDataBaseSave();
                        wsdcgq11lmAdd(device->u64IeeeAddr);
                    }
                    else if (!strncmp((const char *)payload->asAttrReadList[i].uAttrData.au8AttrData,
                                        "LILYGO.Light",
                                        strlen("LILYGO.Light")))
                    {
                        appDataBaseSave();
                        lilygoLightAdd(device->u64IeeeAddr);
                    }
                    else if (!strncmp((const char *)payload->asAttrReadList[i].uAttrData.au8AttrData,
                                      "LILYGO.Sensor",
                                      strlen("LILYGO.Sensor")))
                    {
                        appDataBaseSave();
                        lilygoSensorAdd(device->u64IeeeAddr);
                    }
                   else if (!strncmp((const char *)payload->asAttrReadList[i].uAttrData.au8AttrData,
                                      "ESP32C6.Light",
                                      strlen("ESP32C6.Light")))
                    {
                        appDataBaseSave();
                        espressifLightAdd(device->u64IeeeAddr);
                        // The configure method below is needed to make the device reports on/off state changes
                        // when the device is controlled manually through the button on it.
                        zbhci_BindingReq(
                            device->u64IeeeAddr,
                            1,
                            0x0006,
                            0x03,
                            (ts_DstAddr) {
                                .u64DstAddr = brigeNode.macAddr
                            },
                            1
                        );
                    }
                }
            }
        break;

        case 0x0006:
        {
            for (size_t i = 0; i < payload->u8AttrNum; i++)
            {
                if (payload->asAttrReadList[i].u16AttrID == 0x0000)
                {
                    device->deviceData.light.u8State = payload->asAttrReadList[i].uAttrData.u8AttrData;
                    if (!strncmp((const char *)device->au8ModelId, "LILYGO.Light", strlen("LILYGO.Light")) || \
                        !strncmp((const char *)device->au8ModelId, "ESP32C6.Light", strlen("ESP32C6.Light")))
                    {
                        lilygoLightReport(
                            device->u64IeeeAddr,
                            device->deviceData.light.u8State
                        );
                    }
                }
            }
        }
        break;

        case 0x0400: /* Illuminance Measurement Cluster */
        {
            for (size_t i = 0; i < payload->u8AttrNum; i++)
            {
                if (payload->asAttrReadList[i].u16AttrID == 0x0000)
                {
                    device->deviceData.rtcgq11lm.u16Illuminance = payload->asAttrReadList[i].uAttrData.u16AttrData;
                    if (!strncmp((const char *)device->au8ModelId,
                                 "lumi.sensor_motion.aq2",
                                 strlen("lumi.sensor_motion.aq2")))
                    {
                        rtcgq11lmReport(
                            device->u64IeeeAddr,
                            device->deviceData.rtcgq11lm.u8Occupancy,
                            device->deviceData.rtcgq11lm.u16Illuminance
                        );
                    }
                }
            }
        }
        break;

        case 0x0402: /* Temperature Measurement Cluster */
        {
            for (size_t i = 0; i < payload->u8AttrNum; i++)
            {
                if (payload->asAttrReadList[i].u16AttrID == 0x0000)
                {
                    device->deviceData.wsdcgq11lm.i16Temperature = (int16_t)payload->asAttrReadList[i].uAttrData.u16AttrData;
                    if (!strncmp((const char *)device->au8ModelId,
                                 "lumi.weather",
                                 strlen("lumi.weather")))
                    {
                        wsdcgq11lmReport(
                            device->u64IeeeAddr,
                            device->deviceData.wsdcgq11lm.i16Temperature,
                            device->deviceData.wsdcgq11lm.i16Humidity,
                            device->deviceData.wsdcgq11lm.i16Pressure
                        );
                    }
                    else if (!strncmp((const char *)device->au8ModelId,
                                      "LILYGO.Sensor",
                                      strlen("LILYGO.Sensor")))
                    {
                        lilygoSensorReport(
                            device->u64IeeeAddr,
                            device->deviceData.sensor.i16Temperature,
                            device->deviceData.sensor.i16Humidity
                        );
                    }
                }
            }
        }
        break;

        case 0x0403:/* Pressure Measurement Cluster */
        {
            for (size_t i = 0; i < payload->u8AttrNum; i++)
            {
                if (payload->asAttrReadList[i].u16AttrID == 0x0000)
                {
                    device->deviceData.wsdcgq11lm.i16Pressure = (int16_t)payload->asAttrReadList[i].uAttrData.u16AttrData;
                    if (!strncmp((const char *)device->au8ModelId,
                                 "lumi.weather",
                                 strlen("lumi.weather")))
                    {
                        wsdcgq11lmReport(
                            device->u64IeeeAddr,
                            device->deviceData.wsdcgq11lm.i16Temperature,
                            device->deviceData.wsdcgq11lm.i16Humidity,
                            device->deviceData.wsdcgq11lm.i16Pressure
                        );
                    }
                }
            }
        }
        break;

        case 0x0405: /* Relative Humidity Measurement Cluster */
        {
            for (size_t i = 0; i < payload->u8AttrNum; i++)
            {
                if (payload->asAttrReadList[i].u16AttrID == 0x0000)
                {
                    device->deviceData.wsdcgq11lm.i16Humidity = (int16_t)payload->asAttrReadList[i].uAttrData.u16AttrData;
                    if (!strncmp((const char *)device->au8ModelId,
                                 "lumi.weather",
                                 strlen("lumi.weather")))
                    {
                        wsdcgq11lmReport(
                            device->u64IeeeAddr,
                            device->deviceData.wsdcgq11lm.i16Temperature,
                            device->deviceData.wsdcgq11lm.i16Humidity,
                            device->deviceData.wsdcgq11lm.i16Pressure
                        );
                    }
                    else if (!strncmp((const char *)device->au8ModelId,
                                      "LILYGO.Sensor",
                                      strlen("LILYGO.Sensor")))
                    {
                        lilygoSensorReport(
                            device->u64IeeeAddr,
                            device->deviceData.sensor.i16Temperature,
                            device->deviceData.sensor.i16Humidity
                        );
                    }
                }
            }
        }
        break;

        case 0x0406: /* Occupancy Sensing Cluster */
        {
            for (size_t i = 0; i < payload->u8AttrNum; i++)
            {
                if (payload->asAttrReadList[i].u16AttrID == 0x0000) {
                    device->deviceData.rtcgq11lm.u8Occupancy = payload->asAttrReadList[i].uAttrData.u8AttrData;
                    if (!strncmp((const char *)device->au8ModelId,
                                 "lumi.sensor_motion.aq2",
                                 strlen("lumi.sensor_motion.aq2")))
                    {
                        rtcgq11lmReport(
                            device->u64IeeeAddr,
                            device->deviceData.rtcgq11lm.u8Occupancy,
                            device->deviceData.rtcgq11lm.u16Illuminance
                        );
                    }
                }
            }
        }
        break;

        default:
        break;
    }
}


static void appHandlrMqttPermitJoin(const char *topic, const char *data) {
    if (!data) return ;

    cJSON *json = cJSON_Parse(data);
    if (!json) {
        Serial.println("json error\n");
        return ;
    }

    cJSON *value = cJSON_GetObjectItem(json, "value");
    cJSON *time  = cJSON_GetObjectItem(json, "time");
    if (value && time) {
        if (value->valueint && time->valueint > 0x00 && time->valueint < 0xFF) {
            zbhci_MgmtPermitJoinReq(0xFFFC, time->valueint, 1);
        } else if (value->valueint && time->valueint <= 0) {
            zbhci_MgmtPermitJoinReq(0xFFFC, 0x00, 1);
        } else if (value->valueint && time->valueint >= 0xFF) {
            zbhci_MgmtPermitJoinReq(0xFFFC, 0xFF, 1);
        }
    } else if (value && !time) {
        if (value->valueint) {
            zbhci_MgmtPermitJoinReq(0xFFFC, 0xFF, 1);
        } else {
            zbhci_MgmtPermitJoinReq(0xFFFC, 0x00, 1);
        }
    }

    cJSON_Delete(json);
}