#ifndef APP_DB_H
#define APP_DB_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

typedef struct ts_wsdcgq11lm
{
    uint8_t u8Ep;
    int16_t i16Temperature;
    int16_t i16Humidity;
    int16_t i16Pressure;
} ts_wsdcgq11lm;

typedef struct ts_rtcgq11lm
{
    uint8_t  u8Ep;
    uint8_t  u8Occupancy;
    uint16_t u16Illuminance;
} ts_rtcgq11lm;

typedef struct ts_light
{
    uint8_t  u8Ep;
    uint8_t  u8State;
} ts_light;

typedef struct device_node_t
{
    uint64_t u64IeeeAddr;
    uint16_t u16NwkAddr;
    uint8_t  u8Type;
    uint8_t  au8ModelId[33];
    uint8_t  au8VendorId[33];
    union
    {
        ts_wsdcgq11lm wsdcgq11lm;
        ts_rtcgq11lm  rtcgq11lm;
        ts_light      light;
    } device_data;
} device_node_t;

typedef struct brige_node_t
{
    uint16_t nwk_addr;
    uint64_t mac_addr;
    uint16_t pan_id;
    uint64_t ex_pan_id;
    uint8_t channel;
} brige_node_t;

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

extern brige_node_t brige_node;

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

void app_db_init(void);

void app_db_save(void);

void app_db_recover(void);

device_node_t * find_device_by_nwkaddr(uint16_t u16NwkAddr);

device_node_t * find_device_by_ieeeaddr(uint64_t u64IeeeAddr);

device_node_t * get_empty_device(void);

#ifdef __cplusplus
}
#endif

#endif
/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/