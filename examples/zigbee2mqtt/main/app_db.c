#include "app_db.h"
#include "device.h"

#include "nvs_flash.h"
#include "nvs.h"
#include "cJSON.h"

#include <string.h>

#define DB_NAME "zigbee"

brige_node_t brige_node;
device_node_t device_list[10];

void hex2str(uint64_t u64number, char *str, int len);
/* convert str into hex */
uint64_t str2hex(const char *str);


device_node_t * find_device_by_nwkaddr(uint16_t u16NwkAddr)
{
    for (size_t i = 0; i < sizeof(device_list)/sizeof(device_node_t); i++)
    {
        if (device_list[i].u16NwkAddr == u16NwkAddr)
        {
            return &device_list[i];
        }
    }
    return NULL;
}


device_node_t * find_device_by_ieeeaddr(uint64_t u64IeeeAddr)
{
    for (size_t i = 0; i < sizeof(device_list)/sizeof(device_node_t); i++)
    {
        if (device_list[i].u64IeeeAddr == u64IeeeAddr)
        {
            return &device_list[i];
        }
    }
    return NULL;
}


device_node_t * get_empty_device(void)
{
    for (size_t i = 0; i < sizeof(device_list)/sizeof(device_node_t); i++)
    {
        if (device_list[i].u64IeeeAddr == 0)
        {
            return &device_list[i];
        }
    }
    return NULL;
}



void app_db_init(void)
{
    nvs_handle_t db_handle;

    memset(device_list, 0, sizeof(device_list));

    esp_err_t err = nvs_open(DB_NAME, NVS_READWRITE, &db_handle);
    if (err != ESP_OK)
    {
        printf("db open err\n");
    }

    size_t required_size = 0;
    err = nvs_get_blob(db_handle, "device_list", NULL, &required_size);

    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return ;

    if (required_size == 0)
    {
        printf("Nothing saved yet!\n");
    }
    else
    {
        uint8_t* str = malloc(required_size);
        err = nvs_get_blob(db_handle, "device_list", str, &required_size);
        if (err != ESP_OK)
        {
            free(str);
            goto OUT;
        }
        // printf("str: %s\n", str);
        cJSON *json = cJSON_Parse((const char *)str);
        if (!json)
        {
            printf("json error\n");
        }
        else
        {
            cJSON *bridge = cJSON_GetObjectItem(json, "bridge");
            if (bridge)
            {
                cJSON *nwk_addr = cJSON_GetObjectItem(bridge, "nwk_addr");
                cJSON *mac_addr = cJSON_GetObjectItem(bridge, "mac_addr");
                cJSON *pan_id = cJSON_GetObjectItem(bridge, "pan_id");
                cJSON *ex_pan_id = cJSON_GetObjectItem(bridge, "ex_pan_id");
                cJSON *channel = cJSON_GetObjectItem(bridge, "channel");
                if (nwk_addr)
                    brige_node.nwk_addr = nwk_addr->valueint;
                if (mac_addr)
                    brige_node.mac_addr = str2hex(mac_addr->string);
                if (pan_id)
                    brige_node.pan_id = pan_id->valueint;
                if (ex_pan_id)
                    brige_node.ex_pan_id = str2hex(ex_pan_id->string);
                if (channel)
                    brige_node.channel = channel->valueint;
            }
            cJSON *devices = cJSON_GetObjectItem(json, "devices");
            cJSON *device = NULL;
            for (size_t i = 0; i < cJSON_GetArraySize(devices); i++)
            {
                device = cJSON_GetArrayItem(devices, i);
                cJSON *IeeeAddr = cJSON_GetObjectItem(device, "ieee_addr");
                cJSON *NwkAddr  = cJSON_GetObjectItem(device, "nwk_addr");
                cJSON *Type     = cJSON_GetObjectItem(device, "type");
                cJSON *ModelId  = cJSON_GetObjectItem(device, "modele_id");
                cJSON *VendorId = cJSON_GetObjectItem(device, "vendor_id");
                device_node_t *device_node = get_empty_device();
                device_node->u64IeeeAddr = str2hex(IeeeAddr->valuestring);
                device_node->u16NwkAddr = NwkAddr->valueint;
                device_node->u8Type = Type->valueint;
                memcpy(device_node->au8ModelId, ModelId->valuestring, strlen(ModelId->valuestring));
                memcpy(device_node->au8VendorId, VendorId->valuestring, strlen(VendorId->valuestring));
            }
            cJSON_Delete(json);
        }
        free(str);
    }

OUT:
    // Close
    nvs_close(db_handle);
}


void app_db_recover(void)
{
    for (size_t i = 0; i < sizeof(device_list)/sizeof(device_node_t); i++)
    {
        if (device_list[i].u64IeeeAddr != 0)
        {
            if (!strncmp((const char *)device_list[i].au8ModelId,
                         "lumi.weather",
                         strlen("lumi.weather")))
            {
                wsdcgq11lm_add(device_list[i].u64IeeeAddr);
            }
            else if (!strncmp((const char *)device_list[i].au8ModelId,
                     "lumi.sensor_motion.aq2",
                     strlen("lumi.sensor_motion.aq2")))
            {
                rtcgq11lm_add(device_list[i].u64IeeeAddr);
            }
        }
    }
}


void app_db_save(void)
{
    nvs_handle_t db_handle;

    esp_err_t err = nvs_open(DB_NAME, NVS_READWRITE, &db_handle);
    if (err != ESP_OK)
    {
        printf("db open err\n");
    }

    cJSON *json = NULL;
    cJSON *bridge = NULL;
    cJSON *devices = NULL;

    json = cJSON_CreateObject();
    bridge = cJSON_CreateObject();
    devices = cJSON_CreateArray();

    cJSON_AddNumberToObject(bridge, "nwk_addr", brige_node.nwk_addr);
    char string[32] = { 0 };
    hex2str(brige_node.mac_addr, string, sizeof(string));
    cJSON_AddStringToObject(bridge, "ieee_addr", string);
    cJSON_AddNumberToObject(bridge, "pan_id", brige_node.pan_id);
    memset(string, 0, sizeof(string));
    hex2str(brige_node.ex_pan_id, string, sizeof(string));
    cJSON_AddStringToObject(bridge, "ex_pan_id", string);
    cJSON_AddNumberToObject(bridge, "channel", brige_node.channel);
    cJSON_AddItemToObject(json, "bridge", bridge);
    for (size_t i = 0; i < sizeof(device_list)/sizeof(device_node_t); i++)
    {
        if (device_list[i].u64IeeeAddr != 0)
        {
            cJSON *device = cJSON_CreateObject();
            memset(string, 0, sizeof(string));
            hex2str(device_list[i].u64IeeeAddr, string, sizeof(string));
            cJSON_AddStringToObject(device, "ieee_addr", string);
            cJSON_AddNumberToObject(device, "nwk_addr" , device_list[i].u16NwkAddr);
            cJSON_AddNumberToObject(device, "type",      device_list[i].u8Type);
            cJSON_AddStringToObject(device, "modele_id", (const char *)device_list[i].au8ModelId);
            cJSON_AddStringToObject(device, "vendor_id", (const char *)device_list[i].au8VendorId);
            cJSON_AddItemToArray(devices, device);
        }
    }
    cJSON_AddItemToObject(json, "devices", devices);

    char *str = cJSON_Print(json);
    nvs_set_blob(db_handle, "device_list", str, strlen(str));

    cJSON_Delete(json);

    // Close
    nvs_close(db_handle);
}


void hex2str(uint64_t u64number, char *str, int len)
{
    uint8_t temp = 0;
    uint8_t i = 0, j = 0;

    if (NULL == str) return;
    while (u64number) {
        temp = u64number % 0x10;
        if (temp <= 9) {
            str[i] = temp + '0';
        } else if (temp >= 0x0A && temp <= 0x0F) {
            str[i] = temp - 10 + 'a';
        }
        i++;
        if (i > len) return;
        u64number >>= 4;
    }
    i--;
    for (; j < i; j++, i--) {
        temp = str[j];
        str[j] = str[i];
        str[i] = temp;
    }
}

/* convert str into hex */
uint64_t str2hex(const char *str)
{
    uint64_t u64Result = 0;
    if (NULL == str)
        return 0;
    if (!strncasecmp(str, "0x", 2))
        str += 2;
    while (*str) {
        if (*str >= '0' && *str <= '9')
            u64Result = (u64Result << 4) + *str - '0';
        else if (*str >= 'a' && *str <= 'f')
            u64Result = (u64Result << 4) + *str - 'a' + 10;
        else if (*str >= 'A' && *str <= 'F')
            u64Result = (u64Result << 4) + *str - 'A' + 10;
        else {
            return 0;
        }
        str++;
    }
    return u64Result;
}

