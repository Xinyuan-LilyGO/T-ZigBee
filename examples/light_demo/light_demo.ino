/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <Arduino.h>
#include <zbhci.h>
#include <hci_display.h>
#include <OneButton.h>
#include "esp_task_wdt.h"

#define CONFIG_ZIGBEE_MODULE_PIN 0
#define CONFIG_USR_BUTTON_PIN 2
#define CONFIG_BLUE_LIGHT_PIN 3

const uint8_t au8ManufacturerName[] = {12,'L','I','L','Y','G','O', '.', 'L', 'i', 'g', 'h', 't'};

QueueHandle_t msg_queue;

/**
 * Initialize a new OneButton instance for a button
 * connected to digital pin 4 and GND, which is active low
 * and uses the internal pull-up resistor.
 */
OneButton btn = OneButton(CONFIG_USR_BUTTON_PIN,     // Input pin for the button
                          true,  // Button is active LOW
                          true); // Enable internal pull-up resistor

void setup()
{
    Serial.begin(115200);
    delay(10);
    Serial.printf("Init\n");

    pinMode(CONFIG_ZIGBEE_MODULE_PIN, OUTPUT);
    digitalWrite(CONFIG_ZIGBEE_MODULE_PIN, HIGH);
    delay(500);

    pinMode(CONFIG_BLUE_LIGHT_PIN, OUTPUT);
    digitalWrite(CONFIG_BLUE_LIGHT_PIN, LOW);

    btn.attachClick(handleClick);

    msg_queue = xQueueCreate(10, sizeof(ts_HciMsg));
    zbhci_Init(msg_queue);

    xTaskCreatePinnedToCore(
        zbhciTask,
        "zbhci",   // A name just for humans
        4096,          // This stack size can be checked & adjusted by reading the Stack Highwater
        NULL,
        5,             // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        NULL ,
        ARDUINO_RUNNING_CORE);

    // zbhci_BdbFactoryReset();
    delay(100);
    zbhci_NetworkStateReq();
}


ts_DstAddr sDstAddr;



void loop()
{
    btn.tick();
}

uint8_t ledState = 0;

void handleClick(void)
{
    ledState = !ledState;
    sDstAddr.u16DstAddr = 0x0000;
    zbhci_ZclSendReportCmd(0x02, sDstAddr, 1, 1, 0, 1, 0x0006, 0x0000, ZCL_DATA_TYPE_BOOLEAN, 1, &ledState);
    delay(100);
    digitalWrite(CONFIG_BLUE_LIGHT_PIN, ledState);
}

void zbhciTask(void *pvParameters)
{
    ts_HciMsg sHciMsg;
    ts_DstAddr sDstAddr;

    while (1)
    {
        bzero(&sHciMsg, sizeof(sHciMsg));
        if (xQueueReceive(msg_queue, &sHciMsg, portMAX_DELAY))
        {
            switch (sHciMsg.u16MsgType)
            {
                case ZBHCI_CMD_ACKNOWLEDGE:
                    // displayAcknowledg(&sHciMsg.uPayload.sAckPayload);
                break;

                case ZBHCI_CMD_NETWORK_STATE_RSP:
                    if (sHciMsg.uPayload.sNetworkStateRspPayloasd.u16NwkAddr == 0xFFFF)
                    {
                        zbhci_BdbCommissionSteer();
                        vTaskDelay(100 / portTICK_PERIOD_MS);
                    }
                break;

                case ZBHCI_CMD_NETWORK_STATE_REPORT:
                    sDstAddr.u16DstAddr = 0x0000;
                    zbhci_ZclSendReportCmd(0x02, sDstAddr, 1, 1, 0, 1, 0x0000, 0x0005, ZCL_DATA_TYPE_CHAR_STR, sizeof(au8ManufacturerName), (uint8_t *)&au8ManufacturerName);
                break;

                case ZBHCI_CMD_ZCL_ONOFF_CMD_RCV:
                    if (sHciMsg.uPayload.sZclOnOffCmdRcvPayload.u8CmdId == 0)
                    {
                        digitalWrite(CONFIG_BLUE_LIGHT_PIN, LOW);
                        ledState = 0;
                    }
                    else if (sHciMsg.uPayload.sZclOnOffCmdRcvPayload.u8CmdId == 1)
                    {
                        digitalWrite(CONFIG_BLUE_LIGHT_PIN, HIGH);
                        ledState = 1;
                    }
                    else if (sHciMsg.uPayload.sZclOnOffCmdRcvPayload.u8CmdId == 2)
                    {
                        ledState = !ledState;
                        digitalWrite(CONFIG_BLUE_LIGHT_PIN, ledState);
                    }
                    zbhci_ZclSendReportCmd(0x02, sDstAddr, 1, 1, 0, 1, 0x0006, 0x0000, ZCL_DATA_TYPE_BOOLEAN, 1, &ledState);
                break;

                default:
                    Serial.printf("u16MsgType %d\n", sHciMsg.u16MsgType);
                break;
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
