
/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include "app_mqtt.h"
#include "app_db.h"
#include "zbhci.h"

#include "mqtt_client.h"
#include "esp_err.h"
#include "esp_log.h"
#include "cJSON.h"

#include <string.h>
#include <Arduino.h>

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

#define TAG "APP_MQTT"

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

/******************************************************************************/
/***        local function prototypes                                       ***/
/******************************************************************************/

static void log_error_if_nonzero(const char * message, int error_code);

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        local variables                                                 ***/
/******************************************************************************/

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

MQTTClient::MQTTClient() {
}


MQTTClient::~MQTTClient() {
}


int MQTTClient::connect(IPAddress ip, uint16_t port) {
    return connect(ip.toString().c_str(), port);
}


int MQTTClient::connect(const char *host, uint16_t port) {
    char uri[128] = { 0 };
    sprintf(uri, "mqtt://%s:%d", host, port);
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = uri,
        .disable_clean_session = _cleanSession
    };
    if (_username != "") {
        mqtt_cfg.username = _username.c_str();
    }
    if (_password != "") {
        mqtt_cfg.password = _password.c_str();
    }

    _client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(
        _client,
        (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID,
        onEvent,
        this
    );
    return esp_mqtt_client_start(_client);
}


void MQTTClient::setUsernamePassword(const char* username, const char* password) {
    _username = username;
    _password = password;
}


void MQTTClient::setUsernamePassword(const String& username, const String& password) {
    _username = username;
    _password = password;
}


void MQTTClient::setCleanSession(bool cleanSession) {
    _cleanSession = cleanSession;
}


int MQTTClient::subscribe(const char* topic, uint8_t qos) {
    int ret = esp_mqtt_client_subscribe(_client, topic, qos);
    _subscribeHandler.push_back(new SubscribeHandler(topic, nullptr, qos));
    return ret;
}


int MQTTClient::subscribe(const String& topic, uint8_t qos) {
    int ret = esp_mqtt_client_subscribe(_client, topic.c_str(), qos);
    _subscribeHandler.push_back(new SubscribeHandler(topic, nullptr, qos));
    return ret;
}


int MQTTClient::subscribe(const char* topic, HandlerFunction fn, uint8_t qos) {
    int ret = esp_mqtt_client_subscribe(_client, topic, qos);
    _subscribeHandler.push_back(new SubscribeHandler(topic, fn, qos));
    return ret;
}


int MQTTClient::subscribe(const String& topic, HandlerFunction fn, uint8_t qos) {
    int ret = esp_mqtt_client_subscribe(_client, topic.c_str(), qos);
    _subscribeHandler.push_back(new SubscribeHandler(topic, fn, qos));
    return ret;
}


int MQTTClient::unsubscribe(const char* topic) {
    return esp_mqtt_client_unsubscribe(_client, topic);
}


int MQTTClient::unsubscribe(const String& topic) {
    return esp_mqtt_client_unsubscribe(_client, topic.c_str());
}


int MQTTClient::publish(const char *topic, const char *data) {
    if (!_connected) {
        return -1;
    }
    ESP_LOGI(TAG, "MQTT subscribe: topic \'%s\', payload \'%s\'\r\n", topic, data);
    return esp_mqtt_client_publish(_client, topic, data, 0, 1, 0);
}


int MQTTClient::publish(const String& topic, const String& data) {
    if (!_connected) {
        return -1;
    }
    ESP_LOGI(TAG, "MQTT subscribe: topic \'%s\', payload \'%s\'\r\n", topic, data);
    return esp_mqtt_client_publish(_client, topic.c_str(), data.c_str(), 0, 1, 0);
}


bool MQTTClient::connected() {
    return _connected;
}


void MQTTClient::stop() {
    _connected = false;
    esp_mqtt_client_stop(_client);
    esp_mqtt_client_destroy(_client);
}


void MQTTClient::attachConnectedEvent(EventFunction fn) {
    _connectedEventFn = fn;
}


void MQTTClient::onEvent(
    void *handler_args,
    esp_event_base_t base,
    int32_t event_id,
    void *event_data
) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;
    MQTTClient *mqtt = (MQTTClient *)handler_args;
    int msg_id = -1;

    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            mqtt->_connected = true;
            mqtt->_subscribeAllTopic();
            if (mqtt->_connectedEventFn) {
                mqtt->_connectedEventFn();
            }
        break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            mqtt->_connected = false;
        break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            ESP_LOGI(TAG, "MQTT subscribe: topic '%.*s', payload '%.*s'\r\n",
                event->topic_len,
                event->topic,
                event->data_len,
                event->data
            );
            mqtt->_onDataEvent(event->topic, event->topic_len, event->data, event->data_len);
        break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
        break;

        default:
            ESP_LOGI(TAG, "Other event id: %d", event->event_id);
        break;
    }
}


void MQTTClient::_onDataEvent(
        const char *topic,
        int32_t topic_len,
        const char *data,
        int32_t data_len
) {
    char *topic_new = (char *)calloc(1, topic_len + 2);
    char *data_new = (char *)calloc(1, data_len + 2);
    memcpy(topic_new, topic, topic_len);
    memcpy(data_new, data, data_len);

    for (size_t i = 0; i < _subscribeHandler.size(); i++) {
        if (_subscribeHandler[i]->handle(topic_new, data_new)) {
            break;
        }
    }
    free(topic_new);
    free(data_new);
}


void MQTTClient::_subscribeAllTopic() {
    for (size_t i = 0; i < _subscribeHandler.size(); i++) {
        esp_mqtt_client_subscribe(
            _client,
            _subscribeHandler[i]->_topic.c_str(),
            _subscribeHandler[i]->_qos);
    }
}


MQTTClient mqtt;


/******************************************************************************/
/***        local functions                                                 ***/
/******************************************************************************/

static void log_error_if_nonzero(const char * message, int error_code) {
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/
