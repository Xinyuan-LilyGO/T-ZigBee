
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

#include "app_mqtt.h"
#include "zbhci.h"
#include "app_db.h"
#include "device.h"
#include "app_config.h"
#include "web.h"

QueueHandle_t msg_queue;

String sta_status = "n/a";
String sta_ssid = "";
String sta_pwd = "";
String ap_status = "n/a";
String ap_ssid = "";
char mqtt_server[64];
uint32_t mqtt_port = 0;

#define CONFIG_USR_BUTTON_PIN 2
#define CONFIG_BLUE_LIGHT_PIN 3

/**
 * Initialize a new OneButton instance for a button
 * connected to digital pin 4 and GND, which is active low
 * and uses the internal pull-up resistor.
 */
OneButton btn = OneButton(CONFIG_USR_BUTTON_PIN, /** Input pin for the button */
                          true,                  /** Button is active LOW */
                          false);                 /** Enable internal pull-up resistor */

void setup()
{
    Serial.begin(115200);

    if(!LittleFS.begin()) {
        Serial.printf("An Error has occurred while mounting LittleFS\n");
        return;
    }

    pinMode(CONFIG_BLUE_LIGHT_PIN, OUTPUT);
    digitalWrite(CONFIG_BLUE_LIGHT_PIN, LOW);

    load_db();
    power_init();
    app_nvs_init();
    wifi_init_sta();
    web_init();
    mqtt_app_start();
    app_db_init();

    msg_queue = xQueueCreate(10, sizeof(ts_HciMsg));

    // Power on the zigbee chip:
    //   power_ctl(true);
    // Power off the zigbee chip:
    //   power_ctl(false);
    //   zbhci_Deinit();
    power_ctl(true);

    zbhci_Init(msg_queue);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(zbhciTask,
                            "zbhci",   /** A name just for humans */
                            4096,      /** This stack size can be checked & adjusted by reading the Stack Highwater */
                            NULL,
                            5,         /** Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest. */
                            NULL ,
                            ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(ledTask,
                            "led",   /** A name just for humans */
                            2048,      /** This stack size can be checked & adjusted by reading the Stack Highwater */
                            NULL,
                            4,         /** Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest. */
                            NULL ,
                            ARDUINO_RUNNING_CORE);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    zbhci_NetworkStateReq();
    btn.attachClick(handleClick);
    btn.attachDoubleClick(handleDoubleClick);
}


void loop()
{
    btn.tick();
    delay(10);
}


void ledTask(void *pvParameters)
{
    while (1)
    {
        if (sta_status == "n/a")
        {
            digitalWrite(CONFIG_BLUE_LIGHT_PIN, LOW);
            delay(1000);
            digitalWrite(CONFIG_BLUE_LIGHT_PIN, HIGH);
            delay(1000);
        }
        else if (sta_status == "running" && !get_mqtt_status() )
        {
            digitalWrite(CONFIG_BLUE_LIGHT_PIN, LOW);
            delay(3000);
            digitalWrite(CONFIG_BLUE_LIGHT_PIN, HIGH);
            delay(3000);
        }
        else if (sta_status == "running" && get_mqtt_status())
        {
            digitalWrite(CONFIG_BLUE_LIGHT_PIN, HIGH);
            delay(100);
        }
    }
}


void zbhciTask(void *pvParameters)
{
    ts_HciMsg sHciMsg;
    device_node_t *device = NULL;

    while (1)
    {
        bzero(&sHciMsg, sizeof(sHciMsg));
        if (xQueueReceive(msg_queue, &sHciMsg, portMAX_DELAY))
        {
            switch (sHciMsg.u16MsgType)
            {
                case ZBHCI_CMD_BDB_COMMISSION_FORMATION_RSP:
                {
                    zbhci_MgmtPermitJoinReq(0xFFFC, 0xFF, 1);
                }
                break;

                case ZBHCI_CMD_NETWORK_STATE_RSP:
                    if (sHciMsg.uPayload.sNetworkStateRspPayloasd.u16NwkAddr != 0x0000)
                    {
                        zbhci_BdbChannelSet(25);
                        vTaskDelay(100 / portTICK_PERIOD_MS);
                        zbhci_BdbCommissionFormation();
                    }
                    else
                    {
                        brige_node.nwk_addr  = sHciMsg.uPayload.sNetworkStateRspPayloasd.u16NwkAddr;
                        brige_node.mac_addr  = sHciMsg.uPayload.sNetworkStateRspPayloasd.u64IeeeAddr;
                        brige_node.pan_id    = sHciMsg.uPayload.sNetworkStateRspPayloasd.u16PanId;
                        brige_node.ex_pan_id = sHciMsg.uPayload.sNetworkStateRspPayloasd.u64extPanId;
                        brige_node.channel   = sHciMsg.uPayload.sNetworkStateRspPayloasd.u8Channel;
                        app_db_recover();
                        zbhci_MgmtPermitJoinReq(0xFFFC, 0xFF, 1);
                        digitalWrite(CONFIG_BLUE_LIGHT_PIN, HIGH);
                    }
                break;

                case ZBHCI_CMD_NODES_DEV_ANNCE_IND:
                {
                    device = get_empty_device();
                    if (!device) continue;
                    device->u16NwkAddr  = sHciMsg.uPayload.sNodesDevAnnceRspPayload.u16NwkAddr;
                    device->u64IeeeAddr = sHciMsg.uPayload.sNodesDevAnnceRspPayload.u64IEEEAddr;
                    device->u8Type      = sHciMsg.uPayload.sNodesDevAnnceRspPayload.u8Capability;
                }
                break;

                case ZBHCI_CMD_LEAVE_INDICATION:
                {
                    device = find_device_by_ieeeaddr(sHciMsg.uPayload.sLeaveIndicationPayload.u64MacAddr);
                    if (!sHciMsg.uPayload.sLeaveIndicationPayload.u8Rejoin)
                    {
                        if (!strncmp((const char *)device->au8ModelId,
                                        "lumi.sensor_motion.aq2",
                                        strlen("lumi.sensor_motion.aq2")))
                        {
                            rtcgq11lm_delete(device->u64IeeeAddr);
                        }
                        else if (!strncmp((const char *)device->au8ModelId,
                                            "lumi.weather",
                                            strlen("lumi.weather")))
                        {
                            wsdcgq11lm_delete(device->u64IeeeAddr);
                        }
                        else if (!strncmp((const char *)device->au8ModelId,
                                            "LILYGO.Light",
                                            strlen("LILYGO.Light")))
                        {
                            lilygo_light_delete(device->u64IeeeAddr);
                        }
                        else if (!strncmp((const char *)device->au8ModelId,
                                            "LILYGO.Sensor",
                                            strlen("LILYGO.Sensor")))
                        {
                            lilygo_sensor_delete(device->u64IeeeAddr);
                        }
                        memset(device, 0, sizeof(device_node_t));
                        app_db_save();
                    }
                }
                break;

                case ZBHCI_CMD_ZCL_REPORT_MSG_RCV:
                {
                    switch (sHciMsg.uPayload.sZclReportMsgRcvPayload.u16ClusterId)
                    {
                        case 0x0000: /* Basic Cluster */
                            for (size_t i = 0; i < sHciMsg.uPayload.sZclReportMsgRcvPayload.u8AttrNum; i++)
                            {
                                if (sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].u16AttrID == 0x0005)
                                {
                                    // lumi.sensor_motion.aq2
                                    device = find_device_by_nwkaddr(sHciMsg.uPayload.sZclReportMsgRcvPayload.u16SrcAddr);
                                    if (!device) continue;
                                    memcpy(device->au8ModelId, \
                                           sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].uAttrData.au8AttrData, \
                                           sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].u16DataLen);
                                    if (!strncmp((const char *)sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].uAttrData.au8AttrData,
                                                 "lumi.sensor_motion.aq2",
                                                 strlen("lumi.sensor_motion.aq2")))
                                    {
                                        app_db_save();
                                        rtcgq11lm_add(device->u64IeeeAddr);
                                    }
                                    else if (!strncmp((const char *)sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].uAttrData.au8AttrData,
                                                      "lumi.weather",
                                                      strlen("lumi.weather")))
                                    {
                                        app_db_save();
                                        wsdcgq11lm_add(device->u64IeeeAddr);
                                    }
                                    else if (!strncmp((const char *)sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].uAttrData.au8AttrData,
                                                      "LILYGO.Light",
                                                      strlen("LILYGO.Light")))
                                    {
                                        app_db_save();
                                        lilygo_light_add(device->u64IeeeAddr);
                                    }
                                    else if (!strncmp((const char *)sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].uAttrData.au8AttrData,
                                                      "LILYGO.Sensor",
                                                      strlen("LILYGO.Sensor")))
                                    {
                                        app_db_save();
                                        lilygo_sensor_add(device->u64IeeeAddr);
                                    }
                                }
                            }
                        break;

                        case 0x0006:
                        {
                            device = find_device_by_nwkaddr(sHciMsg.uPayload.sZclReportMsgRcvPayload.u16SrcAddr);
                            if (!device) continue;
                            for (size_t i = 0; i < sHciMsg.uPayload.sZclReportMsgRcvPayload.u8AttrNum; i++)
                            {
                                if (sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].u16AttrID == 0x0000)
                                {
                                    device->device_data.light.u8State = sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].uAttrData.u8AttrData;
                                    if (!strncmp((const char *)device->au8ModelId,
                                                "LILYGO.Light",
                                                strlen("LILYGO.Light")))
                                    {
                                        printf("Light\n");
                                        lilygo_light_report(device->u64IeeeAddr, device->device_data.light.u8State);
                                    }
                                }
                            }
                        }
                        break;

                        case 0x0400: /* Illuminance Measurement Cluster */
                        {
                            device = find_device_by_nwkaddr(sHciMsg.uPayload.sZclReportMsgRcvPayload.u16SrcAddr);
                            if (!device) continue;
                            for (size_t i = 0; i < sHciMsg.uPayload.sZclReportMsgRcvPayload.u8AttrNum; i++)
                            {
                                if (sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].u16AttrID == 0x0000)
                                {
                                    device->device_data.rtcgq11lm.u16Illuminance = sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].uAttrData.u16AttrData;
                                    if (!strncmp((const char *)device->au8ModelId,
                                                "lumi.sensor_motion.aq2",
                                                strlen("lumi.sensor_motion.aq2")))
                                    {
                                        rtcgq11lm_report(device->u64IeeeAddr,
                                                         device->device_data.rtcgq11lm.u8Occupancy,
                                                         device->device_data.rtcgq11lm.u16Illuminance);
                                    }
                                }
                            }
                        }
                        break;

                        case 0x0402: /* Temperature Measurement Cluster */
                        {
                            device = find_device_by_nwkaddr(sHciMsg.uPayload.sZclReportMsgRcvPayload.u16SrcAddr);
                            if (!device) continue;
                            for (size_t i = 0; i < sHciMsg.uPayload.sZclReportMsgRcvPayload.u8AttrNum; i++)
                            {
                                if (sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].u16AttrID == 0x0000)
                                {
                                    device->device_data.wsdcgq11lm.i16Temperature = (int16_t)sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].uAttrData.u16AttrData;
                                    if (!strncmp((const char *)device->au8ModelId,
                                                "lumi.weather",
                                                strlen("lumi.weather")))
                                    {
                                            wsdcgq11lm_report(device->u64IeeeAddr,
                                                              device->device_data.wsdcgq11lm.i16Temperature,
                                                              device->device_data.wsdcgq11lm.i16Humidity,
                                                              device->device_data.wsdcgq11lm.i16Pressure);
                                    }
                                    else if (!strncmp((const char *)device->au8ModelId,
                                                     "LILYGO.Sensor",
                                                      strlen("LILYGO.Sensor")))
                                    {
                                            lilygo_sensor_report(device->u64IeeeAddr,
                                                                 device->device_data.sensor.i16Temperature,
                                                                 device->device_data.sensor.i16Humidity);
                                    }
                                }
                            }
                        }
                        break;

                        case 0x0403:/* Pressure Measurement Cluster */
                        {
                            device = find_device_by_nwkaddr(sHciMsg.uPayload.sZclReportMsgRcvPayload.u16SrcAddr);
                            if (!device) continue;
                            for (size_t i = 0; i < sHciMsg.uPayload.sZclReportMsgRcvPayload.u8AttrNum; i++)
                            {
                                if (sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].u16AttrID == 0x0000)
                                {
                                    device->device_data.wsdcgq11lm.i16Pressure = (int16_t)sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].uAttrData.u16AttrData;
                                    if (!strncmp((const char *)device->au8ModelId,
                                                "lumi.weather",
                                                strlen("lumi.weather")))
                                    {
                                            wsdcgq11lm_report(device->u64IeeeAddr,
                                                              device->device_data.wsdcgq11lm.i16Temperature,
                                                              device->device_data.wsdcgq11lm.i16Humidity,
                                                              device->device_data.wsdcgq11lm.i16Pressure);
                                    }
                                }
                            }
                        }
                        break;

                        case 0x0405: /* Relative Humidity Measurement Cluster */
                        {
                            device = find_device_by_nwkaddr(sHciMsg.uPayload.sZclReportMsgRcvPayload.u16SrcAddr);
                            if (!device) continue;
                            for (size_t i = 0; i < sHciMsg.uPayload.sZclReportMsgRcvPayload.u8AttrNum; i++)
                            {
                                if (sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].u16AttrID == 0x0000)
                                {
                                    device->device_data.wsdcgq11lm.i16Humidity = (int16_t)sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].uAttrData.u16AttrData;
                                    if (!strncmp((const char *)device->au8ModelId,
                                                "lumi.weather",
                                                strlen("lumi.weather")))
                                    {
                                            wsdcgq11lm_report(device->u64IeeeAddr,
                                                              device->device_data.wsdcgq11lm.i16Temperature,
                                                              device->device_data.wsdcgq11lm.i16Humidity,
                                                              device->device_data.wsdcgq11lm.i16Pressure);
                                    }
                                    else if (!strncmp((const char *)device->au8ModelId,
                                                     "LILYGO.Sensor",
                                                     strlen("LILYGO.Sensor")))
                                    {
                                            lilygo_sensor_report(device->u64IeeeAddr,
                                                                 device->device_data.sensor.i16Temperature,
                                                                 device->device_data.sensor.i16Humidity);
                                    }
                                }
                            }
                        }
                        break;

                        case 0x0406: /* Occupancy Sensing Cluster */
                        {
                            device = find_device_by_nwkaddr(sHciMsg.uPayload.sZclReportMsgRcvPayload.u16SrcAddr);
                            if (!device) continue;
                            for (size_t i = 0; i < sHciMsg.uPayload.sZclReportMsgRcvPayload.u8AttrNum; i++)
                            {
                                if (sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].u16AttrID == 0x0000)
                                {
                                    device->device_data.rtcgq11lm.u8Occupancy = sHciMsg.uPayload.sZclReportMsgRcvPayload.asAttrList[i].uAttrData.u8AttrData;
                                    if (!strncmp((const char *)device->au8ModelId,
                                                "lumi.sensor_motion.aq2",
                                                strlen("lumi.sensor_motion.aq2")))
                                    {
                                        rtcgq11lm_report(device->u64IeeeAddr,
                                                         device->device_data.rtcgq11lm.u8Occupancy,
                                                         device->device_data.rtcgq11lm.u16Illuminance);
                                    }
                                }
                            }
                        }
                        break;

                        default:
                        break;
                    }

                }
                break;

                default:
                break;
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}


void handleClick(void)
{
    // digitalWrite(CONFIG_BLUE_LIGHT_PIN, LOW);
    zbhci_MgmtPermitJoinReq(0xFFFC, 0x00, 1);
}


void handleDoubleClick(void)
{
    // digitalWrite(CONFIG_BLUE_LIGHT_PIN, HIGH);
    zbhci_MgmtPermitJoinReq(0xFFFC, 0xFF, 1);
}


void load_db(void)
{
    StaticJsonDocument<1024> doc;

    File configfile = LittleFS.open("/db.json", "r");
    DeserializationError error = deserializeJson(doc, configfile);
    if (error)
        Serial.println(F("Failed to read file, using default configuration"));
    configfile.close();

    sta_ssid = (const char *)doc["sta"]["ssid"];
    sta_pwd = (const char *)doc["sta"]["pwd"];
    const char *server = (const char *)doc["mqtt"]["server"];
    memcpy(mqtt_server, server, strlen(server));
    mqtt_port = doc["mqtt"]["port"].as<uint32_t>();
}


void WiFiEvent(WiFiEvent_t event)
{
    Serial.printf("[WiFi-event] event: %d\n", event);

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
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            sta_status = "running";
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
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

void wifi_init_sta(void)
{
    uint8_t mac[6];
    char ssid[32];

    WiFi.disconnect(true);
    delay(1000);

    WiFi.mode(WIFI_AP_STA);
    // WiFi.mode(WIFI_AP);
    Serial.printf("WiFi: Set mode to WIFI_AP_STA\n");

    WiFi.onEvent(WiFiEvent);

    WiFi.begin(sta_ssid.c_str(), sta_pwd.c_str());

    //Wait for WiFi to connect to AP
    Serial.println("Waiting for WiFi");

    WiFi.macAddress(mac);
    sprintf(ssid, "LilyGo-%02X%02X", mac[4], mac[5]);
    if (WiFi.softAP(ssid) != true)
    {
        Serial.printf("WiFi: failed to create softAP\n");
        return ;
    }
    ap_status = "running";
    ap_ssid = ssid;
    Serial.printf("WiFi: softAP has been established\n");
    Serial.printf("WiFi: please connect to the %s\n", ssid);
}


void app_nvs_init(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}


void power_init()
{
    pinMode(0, OUTPUT);
}


void power_ctl(bool active)
{
    if (active)
    {
        digitalWrite(0, HIGH);
    }
    else
    {
        digitalWrite(0, LOW);
    }
}
