
/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include "app_config.h"
#include "zbhci.h"

#include "mqtt_client.h"
#include "esp_err.h"
#include "esp_log.h"
#include "cJSON.h"

#include <string.h>

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

#define TAG "APP_MQTT"

#define PERMIT_JOIN_TOPIC "zigbee2mqtt/bridge/request/permit_join"

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

/******************************************************************************/
/***        local function prototypes                                       ***/
/******************************************************************************/

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);

static void log_error_if_nonzero(const char * message, int error_code);

static void msg_handler(const char *topic, const char *data);

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        local variables                                                 ***/
/******************************************************************************/

static esp_mqtt_client_handle_t client;

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

void app_mqtt_client_subscribe(const char *topic, int qos)
{
    if (!topic) return ;

    esp_mqtt_client_subscribe(client, topic, qos);
}


void app_mqtt_client_publish(const char *topic, const char *data)
{
    if (!topic) return ;

    ESP_LOGI(TAG, "MQTT subscribe: topic \'%s\', payload \'%s\'\r\n", topic, data);

    esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
}


void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_BROKER_URL,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

/******************************************************************************/
/***        local functions                                                 ***/
/******************************************************************************/

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}


static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch (event->event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, "zigbee2mqtt/bridge/request/+", 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
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
                          event->topic_len, event->topic,
                          event->data_len, event->data);
            msg_handler(event->topic, event->data);
        break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
            {
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
    return ESP_OK;
}


static void log_error_if_nonzero(const char * message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}


static void msg_handler(const char *topic, const char *data)
{
    if (!topic || !data) return ;

    cJSON *json = cJSON_Parse(data);
    if (!json)
    {
        ESP_LOGE(TAG, "json error\n");
        return ;
    }

    if (!strncmp(topic, PERMIT_JOIN_TOPIC, strlen(topic) > strlen(PERMIT_JOIN_TOPIC) ? strlen(PERMIT_JOIN_TOPIC) : strlen(topic)))
    {
        cJSON *value = cJSON_GetObjectItem(json, "value");
        cJSON *time  = cJSON_GetObjectItem(json, "time");
        if (value && time)
        {
            if (value->valueint && time->valueint > 0x00 && time->valueint < 0xFF)
            {
                zbhci_MgmtPermitJoinReq(0xFFFC, time->valueint, 1);
            }
            else if (value->valueint && time->valueint <= 0)
            {
                zbhci_MgmtPermitJoinReq(0xFFFC, 0x00, 1);
            }
            else if (value->valueint && time->valueint >= 0xFF)
            {
                zbhci_MgmtPermitJoinReq(0xFFFC, 0xFF, 1);
            }
        }
        else if (value && !time)
        {
            if (value->valueint)
            {
                zbhci_MgmtPermitJoinReq(0xFFFC, 0xFF, 1);
            }
            else
            {
                zbhci_MgmtPermitJoinReq(0xFFFC, 0x00, 1);
            }
        }
    }

    cJSON_Delete(json);
}


/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/