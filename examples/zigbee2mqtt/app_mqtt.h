#ifndef APP_MQTT_H
#define APP_MQTT_H

/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include <mqtt_client.h>

#include <WString.h>
#include <IPAddress.h>

#include <vector>
#include <functional>

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

typedef std::function<void(const char *topic, const char *data)> HandlerFunction;
typedef std::function<void(void)> EventFunction;

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

class SubscribeHandler {
public:
    SubscribeHandler(const char *topic, HandlerFunction fn, uint8_t qos = 0):
        _topic(topic),
        _fn(fn),
        _qos(qos) {}
    SubscribeHandler(const String& topic, HandlerFunction fn, uint8_t qos = 0):
        _topic(topic),
        _fn(fn),
        _qos(qos) {}
    ~SubscribeHandler();

    bool handle(const char *topic, const char *data) {
        if (String(topic) != _topic) {
            return false;
        }
        _fn(topic, data);
        return true;
    }

public:
    String _topic;
    uint8_t _qos;

private:
    HandlerFunction _fn;
};


class MQTTClient {
public:
    MQTTClient();
    ~MQTTClient();

    int connect(IPAddress ip, uint16_t port = 1883);
    int connect(const char *host, uint16_t port = 1883);

    void setUsernamePassword(const char* username, const char* password);
    void setUsernamePassword(const String& username, const String& password);
    void setCleanSession(bool cleanSession);

    // void setKeepAliveInterval(unsigned long interval);
    // void setConnectionTimeout(unsigned long timeout);
    // void setTxPayloadSize(unsigned short size);

    int subscribe(const char* topic, uint8_t qos = 0);
    int subscribe(const String& topic, uint8_t qos = 0);
    int subscribe(const char* topic, HandlerFunction fn, uint8_t qos = 0);
    int subscribe(const String& topic, HandlerFunction fn, uint8_t qos = 0);
    int unsubscribe(const char* topic);
    int unsubscribe(const String& topic);

    int publish(const char *topic, const char *data);
    int publish(const String& topic, const String& data);

    bool connected();
    void stop();
    void attachConnectedEvent(EventFunction fn);
    void attachDisconnectedEvent();

private:
    std::vector<SubscribeHandler *> _subscribeHandler;
    EventFunction _connectedEventFn;
    String _username;
    String _password;
    bool _cleanSession;
    esp_mqtt_client_handle_t _client;
    bool _connected;

private:
    static void onEvent(
        void *handler_args,
        esp_event_base_t base,
        int32_t event_id,
        void *event_data
    );
    void _onDataEvent(
        const char *topic,
        int32_t topic_len,
        const char *data,
        int32_t data_len
    );
    void _subscribeAllTopic();
};

extern MQTTClient mqtt;

#endif
/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/