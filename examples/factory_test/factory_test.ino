/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <zbhci.h>

#include "esp_task_wdt.h"

#define CONFIG_TIMEOUT 10000
#define CONFIG_RSSI    -60

const char* product = "T-ZigBee";

// Please enter the ssid and password of the wifi
const char* ssid     = "";
const char* password = "";

const char* host = "www.baidu.com";
const char* streamId   = "....................";
const char* privateKey = "....................";

bool apScanTestResult = false;
bool connectTestResult = false;
bool httpTestResult = false;
bool zigbeeTestResult = false;

#define COLOR_NONE "" // "\033[0m"
#define FONT_COLOR_RED "" // "\033[0;31m"
#define FONT_COLOR_GREEN "" // "\033[1;32m"
#define BACKGROUND_COLOR_RED "" // "\033[41m"
#define BACKGROUND_COLOR_GREEN "" //"\033[42m"

void apScanTest()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.println("WIFI Scan Test");
    Serial.print("Results: ");

    int n = WiFi.scanNetworks();

    if (n == 0) {
        Serial.print(BACKGROUND_COLOR_RED);
        Serial.print("Failed");
        Serial.print(COLOR_NONE);
        Serial.println(" - (no networks found)");
    } else {
        for (int i = 0; i < n; ++i) {
            if ( WiFi.SSID(i).equals(ssid) && WiFi.RSSI(i) >= CONFIG_RSSI)
            {
                Serial.print(BACKGROUND_COLOR_GREEN);
                Serial.print("Pass");
                Serial.print(COLOR_NONE);
                Serial.print(" - ""(RSSI: ");
                Serial.print(WiFi.RSSI(i));
                Serial.print(")");
                Serial.println();
                apScanTestResult = true;
                break ;
            }
            else if (WiFi.SSID(i).equals(ssid) && WiFi.RSSI(i) < CONFIG_RSSI)
            {
                Serial.print(BACKGROUND_COLOR_RED);
                Serial.print("Failed");
                Serial.print(COLOR_NONE);
                Serial.print(" - ""(RSSI: ");
                Serial.print(WiFi.RSSI(i));
                Serial.print(")");
                Serial.println();
                apScanTestResult = false;
                break ;
            }
        }
    }
    Serial.println();
}


void connectTest()
{
    int count = 0;

    WiFi.begin(ssid, password);

    Serial.println("WIFI Connect Test");
    Serial.print("Results: ");

    while (WiFi.status() != WL_CONNECTED && count * 500 < CONFIG_TIMEOUT)
    {
        esp_task_wdt_reset();
        delay(500);
        count++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print(BACKGROUND_COLOR_GREEN);
        Serial.print("Pass");
        Serial.print(COLOR_NONE);
        Serial.print(" - ""(IP: ");
        Serial.print(WiFi.localIP());
        Serial.print(")");
        Serial.println();
        connectTestResult = true;
    }
    else
    {
        Serial.print(BACKGROUND_COLOR_RED);
        Serial.print("Failed");
        Serial.print(COLOR_NONE);
        Serial.print(" - ""(Connect Timeout)");
        Serial.println();
        connectTestResult = false;
    }
    Serial.println();
}


void httpTest()
{
    HTTPClient http;

    Serial.println("Http Test");
    Serial.print("Results: ");
    http.begin("http://www.baidu.com");

    int httpCode = http.GET();
    // httpCode will be negative on error
    if(httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        // USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if(httpCode == HTTP_CODE_OK) {
            // String payload = http.getString();
            Serial.print(BACKGROUND_COLOR_GREEN);
            Serial.print("Pass");
            Serial.print(COLOR_NONE);
            Serial.println();
            httpTestResult = true;
        }
    } else {
        Serial.print(BACKGROUND_COLOR_RED);
        Serial.print("Failed");
        Serial.print(COLOR_NONE);
        Serial.print(" - ""(");
        Serial.print(http.errorToString(httpCode).c_str());
        Serial.print(")");
        Serial.println();
        httpTestResult = false;
    }
    esp_task_wdt_reset();
    http.end();
    Serial.println();
}

QueueHandle_t msg_queue;

void zbhciTask(void *pvParameters)
{
    ts_HciMsg sHciMsg;

    while (1)
    {
        bzero(&sHciMsg, sizeof(sHciMsg));
        // Serial.println("hahahahahahaha");
        if (xQueueReceive(msg_queue, &sHciMsg, portMAX_DELAY))
        {
            // Serial.printf("u16MsgType %d\n", sHciMsg.u16MsgType);
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
                        zbhci_MgmtPermitJoinReq(0xFFFC, 0xFF, 1);
                    }
                break;

                case ZBHCI_CMD_NODES_DEV_ANNCE_IND:
                {
                    zbhci_MgmtLeaveReq(sHciMsg.uPayload.sNodesDevAnnceRspPayload.u16NwkAddr,
                                       sHciMsg.uPayload.sNodesDevAnnceRspPayload.u64IEEEAddr,
                                       0x00,
                                       0x01);
                    if (!zigbeeTestResult)
                    {
                        Serial.println("ZigBee Test");
                        Serial.print("Results: ");
                        Serial.print(BACKGROUND_COLOR_GREEN);
                        Serial.print("Pass");
                        Serial.print(COLOR_NONE);
                        Serial.println();
                        Serial.println();
                        zigbeeTestResult = true;
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

void test(void *pvParameters)
{
    bool flag = false;

    while (1)
    {
        esp_task_wdt_reset();
        if (!apScanTestResult)
        {
            /* code */
            apScanTest();
            esp_task_wdt_reset();
        }
        else if (!connectTestResult)
        {
            /* code */
            connectTest();
            esp_task_wdt_reset();
        }
        else if (!httpTestResult)
        {
            /* code */
            httpTest();
            esp_task_wdt_reset();
        }
        else if (!zigbeeTestResult)
        {
            delay(100);
        }
        else if (!flag)
        {
            Serial.print(BACKGROUND_COLOR_GREEN);
            Serial.printf(" All functional tests passed, please unplug the device!!!\r");
            Serial.print(COLOR_NONE);
            esp_task_wdt_reset();
            flag = true;
        }

        delay(100);
    }
}



void setup()
{
    Serial.begin(115200);
    delay(10);

    Serial.println("");
    Serial.printf("Build Time: %s\r\n", __TIME__);
    Serial.println("Factory Test");
    Serial.print("Product: ");
    Serial.println(product);
    Serial.println();

    pinMode(0, OUTPUT);
    digitalWrite(0, HIGH);
    delay(500);

    msg_queue = xQueueCreate(10, sizeof(ts_HciMsg));
    zbhci_Init(msg_queue);
    delay(100);
    zbhci_NetworkStateReq();
    // Now set up two tasks to run independently.
    xTaskCreatePinnedToCore(
        zbhciTask,
        "zbhci",   // A name just for humans
        4096,          // This stack size can be checked & adjusted by reading the Stack Highwater
        NULL,
        5,             // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        NULL ,
        ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        test,
        "test",   // A name just for humans
        4096,          // This stack size can be checked & adjusted by reading the Stack Highwater
        NULL,
        2,             // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        NULL ,
        ARDUINO_RUNNING_CORE);
}



void loop()
{

}
