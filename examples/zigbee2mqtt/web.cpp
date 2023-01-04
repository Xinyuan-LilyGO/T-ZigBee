#include "app_db.h"
#include "app_mqtt.h"

#include <WebServer.h>
#include <ArduinoJson.h>
#include "LittleFS.h"
#include "Esp.h"

static WebServer server(80);

static void handleIndex();
static void handleCSS();
static void handleSettings();
static void getStatus();
static void getConfig();
static void handleConfig();
static void handleNotFound();
static void webTask(void *args);

extern String staStatus;
extern String apStatus;
extern String apSSID;
extern String staSSID;
extern String staPassword;
extern String mqttServer;
extern uint32_t mqttPort;
extern String mqttUsername;
extern String mqttPassword;

void webInit(void) {
    server.on("/", handleIndex);
    server.on("/settings", HTTP_GET, handleSettings);
    server.on("/settings", HTTP_POST, handleConfig);
    server.on("/style.css", handleCSS);
    server.on("/status", HTTP_GET, getStatus);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");

    // TaskHandle_t t1;
    // xTaskCreatePinnedToCore((void (*)(void *))webTask, "webTask", 8192, NULL, 10, &t1, 0);
}


static void handleIndex() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        Serial.println("file error");
    }
    server.streamFile(file, "text/html");
    file.close();
}


static void handleCSS() {
    File file = LittleFS.open("/style.css", "r");
    if (!file)
    {
        Serial.println("file error");
    }
    server.streamFile(file, "text/css");
    file.close();
}


static void handleSettings() {
    File file = LittleFS.open("/settings.html", "r");
    if (!file) {
        Serial.println("file error");
    }
    server.streamFile(file, "text/html");
    file.close();
}


static void getStatus() {
    String s;
    StaticJsonDocument<768> rsp;

    JsonObject sys_obj = rsp.createNestedObject("sys");
    sys_obj["model"] = "LILYGO T-ZigBee";
    JsonObject archObj = sys_obj.createNestedObject("arch");
    archObj["mfr"] = "Espressif";
    archObj["model"] = ESP.getChipModel();
    archObj["revision"] = ESP.getChipRevision();
    if (!strncmp(ESP.getChipModel(), "ESP32-S3", strlen("ESP32-S3"))) {
        archObj["cpu"] = "XTensa® dual-core LX7";
    } else if (!strncmp(ESP.getChipModel(), "ESP32-S2", strlen("ESP32-S2"))) {
        archObj["cpu"] = "XTensa® single-core LX7";
    } else if (!strncmp(ESP.getChipModel(), "ESP32-C3", strlen("ESP32-C3"))) {
        archObj["cpu"] = "RISC-V";
    } else if (!strncmp(ESP.getChipModel(), "ESP32", strlen("ESP32"))) {
        archObj["cpu"] = "XTensa® dual-core LX6";
    }
    archObj["freq"] = ESP.getCpuFreqMHz();
    sys_obj["fw"] = "v0.1.1";
    sys_obj["sdk"] = ESP.getSdkVersion();
    sys_obj["uptime"] = millis();

    JsonObject memObj = rsp.createNestedObject("mem");
    memObj["total"] = ESP.getHeapSize();
    memObj["free"] = ESP.getFreeHeap();

    JsonObject fsObj = rsp.createNestedObject("fs");
    fsObj["total"] = LittleFS.totalBytes();
    fsObj["used"] = LittleFS.usedBytes();
    fsObj["free"] = LittleFS.totalBytes() - LittleFS.usedBytes();

    JsonObject softapObj = rsp.createNestedObject("softap");
    softapObj["status"] = apStatus;
    softapObj["ip"] = "192.168.4.1";
    softapObj["ssid"] = apSSID;

    JsonObject staObj = rsp.createNestedObject("sta");
    staObj["status"] = staStatus;
    staObj["ip"] = WiFi.localIP();
    staObj["ssid"] = staSSID;
    staObj["pwd"] = staPassword;

    JsonObject zigbeeObj = rsp.createNestedObject("zigbee");
    if (brigeNode.nwkAddr == 0x0000) {
        zigbeeObj["status"] = "runing";
    } else {
        zigbeeObj["status"] = "n/a";
    }
    zigbeeObj["nwk_addr"] = brigeNode.nwkAddr;
    zigbeeObj["ieee_addr"] = brigeNode.macAddr;
    zigbeeObj["pan_id"] = brigeNode.panId;
    zigbeeObj["ext_pan"] = brigeNode.exPanId;
    zigbeeObj["channel"] = brigeNode.channel;

    JsonObject mqttObj = rsp.createNestedObject("mqtt");
    mqttObj["status"] = mqtt.connected() ? "connected" : "disconnect";
    mqttObj["server"] = mqttServer;
    mqttObj["port"] = mqttPort;
    mqttObj["username"] = mqttUsername;
    mqttObj["password"] = mqttPassword;

    serializeJson(rsp, s);

    server.send(200, "application/json", s);
}


static void getConfig() {
    String s;
    StaticJsonDocument<512> rsp;

    JsonObject wlanObj = rsp.createNestedObject("sta");
    wlanObj["ssid"] = staSSID;
    wlanObj["pwd"] = staPassword;
    JsonObject zigbeeObj = rsp.createNestedObject("zigbee");
    zigbeeObj["channel"] = 25;
    JsonObject mqttObj = rsp.createNestedObject("mqtt");
    mqttObj["server"] = mqttServer;
    mqttObj["port"] = mqttPort;
    serializeJson(rsp, s);

    server.send(200, "application/json", s);
}


static void handleConfig() {
    StaticJsonDocument<1024> doc;

    bool staFlag = false;
    bool mqttFlag = false;

    File configfile = LittleFS.open("/db.json", "r");
    DeserializationError error = deserializeJson(doc, configfile);
    if (error) {
        Serial.println(F("Failed to read file, using default configuration"));
    }
    configfile.close();

    for ( uint8_t i = 0; i < server.args(); i++ ) {
        Serial.printf("name: %s\n", server.argName(i));
        Serial.printf("args: %s\n", server.arg(i));
        if (server.argName(i).equals("ssid")) {
            if (doc["sta"]["ssid"] != server.arg(i)) {
                doc["sta"]["ssid"] = server.arg(i);
                staSSID = server.arg(i);
                staFlag = true;
            }
        } else if (server.argName(i).equals("password")) {
            if (doc["sta"]["pwd"] != server.arg(i)) {
                doc["sta"]["pwd"] = server.arg(i);
                staPassword = server.arg(i);
                staFlag = true;
            }
        } else if (server.argName(i).equals("channel")) {
            doc["zigbee"]["channel"] = server.arg(i).toInt();
        } else if (server.argName(i).equals("server")) {
            if (doc["mqtt"]["server"] != server.arg(i)) {
                doc["mqtt"]["server"] = server.arg(i);
                mqttServer = server.arg(i);
                mqttFlag = true;
            }
        } else if (server.argName(i).equals("port")) {
            if (doc["mqtt"]["port"] != server.arg(i)) {
                doc["mqtt"]["port"] = server.arg(i);
                mqttPort = server.arg(i).toInt();
                mqttFlag = true;
            }
        } else if (server.argName(i).equals("mqtt_username")) {
            if (doc["mqtt"]["username"] != server.arg(i)) {
                doc["mqtt"]["username"] = server.arg(i);
                mqttUsername = server.arg(i);
                mqttFlag = true;
            }
        } else if (server.argName(i).equals("mqtt_password")) {
            if (doc["mqtt"]["password"] != server.arg(i)) {
                doc["mqtt"]["password"] = server.arg(i);
                mqttPassword = server.arg(i);
                mqttFlag = true;
            }
        }
    }

    configfile = LittleFS.open("/db.json", "w");
    if (!configfile) {
        Serial.println("file error");
    }
    if (serializeJson(doc, configfile) == 0) {
        Serial.println(F("Failed to write to file"));
    }
    configfile.close();

    File htmlfile = LittleFS.open("/settings.html", "r");
    if (!htmlfile) {
        Serial.println("file error");
    }
    server.streamFile(htmlfile, "text/html");
    htmlfile.close();

    if (staFlag) {
        WiFi.disconnect();
        delay(1000);
        WiFi.begin(staSSID.c_str(), staPassword.c_str());
    }
    if (mqttFlag) {
        mqtt.stop();
        mqtt.connect(mqttServer.c_str(), mqttPort);
    }
}


static void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}


static void webTask(void *args) {
    while (1) {
        server.handleClient();
        delay(10); //allow the cpu to switch to other tasks
    }
    vTaskDelete(NULL);
}

void webLoop() {
    while (1) {
        server.handleClient();
        delay(10); //allow the cpu to switch to other tasks
    }
}
