#ifndef DEVICE_H
#define DEVICE_H

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

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

void wsdcgq11lmAdd(uint64_t u64IeeeAddr);

void wsdcgq11lmReport(
    uint64_t u64IeeeAddr,
    int16_t  i16Temperature,
    int16_t  i16Humidity,
    int16_t  i16Pressure
);

void wsdcgq11lmDelete(uint64_t u64IeeeAddr);

void rtcgq11lmAdd(uint64_t u64IeeeAddr);

void rtcgq11lmReport(
    uint64_t u64IeeeAddr,
    int8_t   u8Occupancy,
    uint16_t u16Illuminance
);

void rtcgq11lmDelete(uint64_t u64IeeeAddr);

void lilygoLightAdd(uint64_t u64IeeeAddr);

void lilygoLightDelete(uint64_t u64IeeeAddr);

void lilygoLightReport(uint64_t u64IeeeAddr, uint8_t u8OnOff);

void lilygoSensorAdd(uint64_t u64IeeeAddr);

void lilygoSensorReport(
    uint64_t u64IeeeAddr,
    int16_t  i16Temperature,
    int16_t  i16Humidity
);

void lilygoSensorDelete(uint64_t u64IeeeAddr);

#endif
/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/
