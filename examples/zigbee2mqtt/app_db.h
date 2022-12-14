#ifndef APP_DB_H
#define APP_DB_H

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

typedef struct WSDCGQ11LM
{
    uint8_t u8Ep;
    int16_t i16Temperature;
    int16_t i16Humidity;
    int16_t i16Pressure;
} WSDCGQ11LM;

typedef struct RTCGQ11LM
{
    uint8_t  u8Ep;
    uint8_t  u8Occupancy;
    uint16_t u16Illuminance;
} RTCGQ11LM;

typedef struct Light
{
    uint8_t  u8Ep;
    uint8_t  u8State;
} Light;

typedef struct Sensor
{
    uint8_t u8Ep;
    int16_t i16Temperature;
    int16_t i16Humidity;
} Sensor;

typedef struct DeviceNode {
    uint64_t u64IeeeAddr;
    uint16_t u16NwkAddr;
    uint8_t  u8Type;
    uint8_t  au8ModelId[33];
    uint8_t  au8VendorId[33];
    union {
        WSDCGQ11LM wsdcgq11lm;
        RTCGQ11LM  rtcgq11lm;
        Light      light;
        Sensor     sensor;
    } deviceData;
} DeviceNode;

typedef struct BrigeNode {
    uint16_t nwkAddr;
    uint64_t macAddr;
    uint16_t panId;
    uint64_t exPanId;
    uint8_t channel;
} BrigeNode;

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

extern BrigeNode brigeNode;

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

void appDataBaseInit(void);

void appDataBaseSave(void);

void appDataBaseRecover(void);

DeviceNode * findDeviceByNwkaddr(uint16_t u16NwkAddr);

DeviceNode * findDeviceByIeeeaddr(uint64_t u64IeeeAddr);

DeviceNode * getEmptyDevice(void);

#endif
/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/
