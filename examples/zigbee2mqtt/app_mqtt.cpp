
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

#define PERMIT_JOIN_TOPIC "zigbee2mqtt/bridge/request/permit_join"

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

// typedef void (*sub_topic_handle_t)(const char *data);

typedef struct ts_sub_topic
{
    char *topic;
    sub_topic_handle_t handle;
} ts_sub_topic;

/******************************************************************************/
/***        local function prototypes                                       ***/
/******************************************************************************/

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

static void log_error_if_nonzero(const char * message, int error_code);

static void msg_handler(const char *topic, int32_t topic_len, const char *data, int32_t data_len);

static void permit_join_handler(const char *topic, const char *data);

static void topic_init(void);

static ts_sub_topic * get_unused_topic(void);

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

extern String mqtt_server;
extern uint32_t mqtt_port;

/******************************************************************************/
/***        local variables                                                 ***/
/******************************************************************************/

static esp_mqtt_client_handle_t client;

ts_sub_topic sub_topic[10];

static bool mqtt_status = false;

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

void app_mqtt_client_subscribe(const char *topic, int qos, sub_topic_handle_t handle)
{
    if (!topic) return ;

    ts_sub_topic *ps_sub_topic = get_unused_topic();
    if (!ps_sub_topic) return ;

    ps_sub_topic->topic = (char *)calloc(1, strlen(topic) + 1);
    memcpy(ps_sub_topic->topic, topic, strlen(topic));
    ps_sub_topic->handle = handle;

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
    char uri[128] = {0};
    sprintf(uri, "mqtt://%s:%d", mqtt_server.c_str(), mqtt_port);
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = uri,
        // .host = mqtt_server.c_str(),
        // .port = mqtt_port
    };
    topic_init();
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}


bool get_mqtt_status(void)
{
    return mqtt_status;
}


void mqtt_app_stop(void)
{
    esp_mqtt_client_stop(client);
    esp_mqtt_client_destroy(client);
}

/******************************************************************************/
/***        local functions                                                 ***/
/******************************************************************************/

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    ts_sub_topic *ps_sub_topic;

    switch (event->event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, "zigbee2mqtt/bridge/request/+", 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            ps_sub_topic = get_unused_topic();
            if (ps_sub_topic) 
            {
                ps_sub_topic->topic = (char *)calloc(1, strlen(PERMIT_JOIN_TOPIC) + 1);
                memcpy(ps_sub_topic->topic, PERMIT_JOIN_TOPIC, strlen(PERMIT_JOIN_TOPIC));
                ps_sub_topic->handle = permit_join_handler;
            }
            app_db_recover();
            mqtt_status = true;
        break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            mqtt_status = false;
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
            msg_handler(event->topic, event->topic_len, event->data, event->data_len);
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
    // return ESP_OK;
}


static void log_error_if_nonzero(const char * message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}


static void msg_handler(const char *topic, int32_t topic_len, const char *data, int32_t data_len)
{
    if (!topic || !data) return ;

    char *topic_new = (char *)calloc(1, topic_len + 2);
    char *data_new = (char *)calloc(1, data_len + 2);

    memcpy(topic_new, topic, topic_len);
    memcpy(data_new, data, data_len);
    // printf("topic: %s, data: %s\n", topic_new, data_new);

    for (size_t i = 0; i < 10; i++)
    {
        if (sub_topic[i].topic != NULL && \
            sub_topic[i].handle != NULL)
        {
            // printf("topic: %s\n", sub_topic[i].topic);
            if (!memcmp(sub_topic[i].topic, topic_new, strlen(sub_topic[i].topic)))
                sub_topic[i].handle(topic_new, data_new);
        }
    }

    free(topic_new);
    free(data_new);
}


static void permit_join_handler(const char *topic, const char *data)
{
    if (!data) return ;

    cJSON *json = cJSON_Parse(data);
    if (!json)
    {
        ESP_LOGE(TAG, "json error\n");
        return ;
    }

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

    cJSON_Delete(json);
}


static void topic_init(void)
{
    for (size_t i = 0; i < 10; i++)
    {
        sub_topic[i].topic = NULL;
        sub_topic[i].handle = NULL;
    }
}

static ts_sub_topic * get_unused_topic(void)
{
    for (size_t i = 0; i < 10; i++)
    {
        if (sub_topic[i].topic == NULL)
            return &sub_topic[i];
    }
    return NULL;
}


/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/
