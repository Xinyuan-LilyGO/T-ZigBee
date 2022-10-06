#include "app_db.h"
#include "app_mqtt.h"

#include <WebServer.h>
#include <ArduinoJson.h>
#include "LittleFS.h"
#include "Esp.h"

static WebServer server(80);

static void handle_index();
static void handle_css();
static void handleSettings();
static void get_status();
static void get_config();
static void handle_config();
static void handleNotFound();
static void webTask(void *args);

extern String sta_status;
extern String ap_status;
extern String ap_ssid;
extern String sta_ssid;
extern String sta_pwd;
extern String mqtt_server;
extern uint32_t mqtt_port;

void web_init(void)
{
    server.on("/", handle_index);
    server.on("/settings", HTTP_GET, handleSettings);
    server.on("/settings", HTTP_POST, handle_config);
    server.on("/style.css", handle_css);
    server.on("/status", HTTP_GET, get_status);


    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");

    TaskHandle_t t1;
    xTaskCreatePinnedToCore((void (*)(void *))webTask, "webTask", 8192, NULL, 10, &t1, 0);
}

static void handle_index()
{
    File file = LittleFS.open("/index.html", "r");
    if (!file)
    {
        Serial.println("file error");
    }
    server.streamFile(file, "text/html");
    file.close();
}


static void handle_css()
{
    File file = LittleFS.open("/style.css", "r");
    if (!file)
    {
        Serial.println("file error");
    }
    server.streamFile(file, "text/css");
    file.close();
}


static void handleSettings()
{
    File file = LittleFS.open("/settings.html", "r");
    if (!file)
    {
        Serial.println("file error");
    }
    server.streamFile(file, "text/html");
    file.close();
}


static void get_status()
{
    String s;
    StaticJsonDocument<768> rsp;

    JsonObject sys_obj = rsp.createNestedObject("sys");
    sys_obj["model"] = "LiLyGo T-ZigBee";
    JsonObject arch_obj = sys_obj.createNestedObject("arch");
    arch_obj["mfr"] = "Espressif";
    arch_obj["model"] = ESP.getChipModel();
    arch_obj["revision"] = ESP.getChipRevision();
    if (!strncmp(ESP.getChipModel(), "ESP32-S3", strlen("ESP32-S3")))
    {
        arch_obj["cpu"] = "XTensa® dual-core LX7";
    }
    else if (!strncmp(ESP.getChipModel(), "ESP32-S2", strlen("ESP32-S2")))
    {
        arch_obj["cpu"] = "XTensa® single-core LX7";
    }
    else if (!strncmp(ESP.getChipModel(), "ESP32-C3", strlen("ESP32-C3")))
    {
        arch_obj["cpu"] = "RISC-V";
    }
    else if (!strncmp(ESP.getChipModel(), "ESP32", strlen("ESP32")))
    {
        arch_obj["cpu"] = "XTensa® dual-core LX6";
    }
    arch_obj["freq"] = ESP.getCpuFreqMHz();
    sys_obj["fw"] = "v0.1.1";
    sys_obj["sdk"] = ESP.getSdkVersion();
    sys_obj["uptime"] = millis();

    JsonObject mem_obj = rsp.createNestedObject("mem");
    mem_obj["total"] = ESP.getHeapSize();
    mem_obj["free"] = ESP.getFreeHeap();

    JsonObject fs_obj = rsp.createNestedObject("fs");
    fs_obj["total"] = LittleFS.totalBytes();
    fs_obj["used"] = LittleFS.usedBytes();
    fs_obj["free"] = LittleFS.totalBytes() - LittleFS.usedBytes();

    JsonObject softap_obj = rsp.createNestedObject("softap");
    softap_obj["status"] = ap_status;
    softap_obj["ip"] = "192.168.4.1";
    softap_obj["ssid"] = ap_ssid;

    JsonObject sta_obj = rsp.createNestedObject("sta");
    sta_obj["status"] = sta_status;
    sta_obj["ip"] = WiFi.localIP();
    sta_obj["ssid"] = sta_ssid;
    sta_obj["pwd"] = sta_pwd;

    JsonObject zigbee_obj = rsp.createNestedObject("zigbee");
    if (brige_node.nwk_addr == 0x0000)
    {
        zigbee_obj["status"] = "runing";
    }
    else 
    {
        zigbee_obj["status"] = "n/a";
    }
    zigbee_obj["nwk_addr"] = brige_node.nwk_addr;
    zigbee_obj["ieee_addr"] = brige_node.mac_addr;
    zigbee_obj["pan_id"] = brige_node.pan_id;
    zigbee_obj["ext_pan"] = brige_node.ex_pan_id;
    zigbee_obj["channel"] = brige_node.channel;

    JsonObject mqtt_obj = rsp.createNestedObject("mqtt");
    mqtt_obj["status"] = get_mqtt_status() ? "connected" : "disconnect";
    mqtt_obj["server"] = mqtt_server;
    mqtt_obj["port"] = mqtt_port;

    serializeJson(rsp, s);

    server.send(200, "application/json", s);
}


static void get_config()
{
    String s;
    StaticJsonDocument<512> rsp;

    JsonObject wlan_obj = rsp.createNestedObject("wlan");
    wlan_obj["ssid"] = sta_ssid;
    wlan_obj["pwd"] = sta_pwd;
    JsonObject zigbee_obj = rsp.createNestedObject("zigbee");
    zigbee_obj["channel"] = 25;
    JsonObject mqtt_obj = rsp.createNestedObject("mqtt");
    mqtt_obj["server"] = mqtt_server;
    mqtt_obj["port"] = mqtt_port;
    serializeJson(rsp, s);

    server.send(200, "application/json", s);
}


static void handle_config()
{
    StaticJsonDocument<1024> doc;

    bool sta_flag = false;
    bool mqtt_flag = false;

    File configfile = LittleFS.open("/db.json", "r");
    DeserializationError error = deserializeJson(doc, configfile);
    if (error)
        Serial.println(F("Failed to read file, using default configuration"));
    configfile.close();

    for ( uint8_t i = 0; i < server.args(); i++ )
    {
        Serial.printf("name: %s\n", server.argName(i));
        Serial.printf("args: %s\n", server.arg(i));
        if (server.argName(i).equals("ssid"))
        {
            if (doc["wlan"]["ssid"] != server.arg(i))
            {
                doc["wlan"]["ssid"] = server.arg(i);
                sta_ssid = server.arg(i);
                sta_flag = true;
            }
        }
        else if (server.argName(i).equals("password"))
        {
            if (doc["wlan"]["pwd"] != server.arg(i))
            {
                doc["wlan"]["pwd"] = server.arg(i);
                sta_pwd = server.arg(i);
                sta_flag = true;
            }
        }
        else if (server.argName(i).equals("channel"))
        {
            doc["zigbee"]["channel"] = server.arg(i).toInt();
        }
        else if (server.argName(i).equals("server"))
        {
            if (doc["mqtt"]["server"] != server.arg(i))
            {
                doc["mqtt"]["server"] = server.arg(i);
                mqtt_server = server.arg(i);
                mqtt_flag = true;
            }
        }
        else if (server.argName(i).equals("port"))
        {
            if (doc["mqtt"]["port"] != server.arg(i))
            {
                doc["mqtt"]["port"] = server.arg(i);
                mqtt_port = server.arg(i).toInt();
                mqtt_flag = true;
            }
        }
    }

    configfile = LittleFS.open("/db.json", "w");
    if (!configfile)
    {
        Serial.println("file error");
    }
    if (serializeJson(doc, configfile) == 0)
    {
        Serial.println(F("Failed to write to file"));
    }
    configfile.close();

    File htmlfile = LittleFS.open("/settings.html", "r");
    if (!htmlfile)
    {
        Serial.println("file error");
    }
    server.streamFile(htmlfile, "text/html");
    htmlfile.close();

    if (sta_flag)
    {
        WiFi.disconnect();
        delay(1000);
        WiFi.begin(sta_ssid.c_str(), sta_pwd.c_str());
    }
    if (mqtt_flag)
    {
        mqtt_app_stop();
        mqtt_app_start();
    }
}


static void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}


static void webTask(void *args)
{
    while (1)
    {
        server.handleClient();
        delay(10); //allow the cpu to switch to other tasks
    }
    vTaskDelete(NULL);
}
