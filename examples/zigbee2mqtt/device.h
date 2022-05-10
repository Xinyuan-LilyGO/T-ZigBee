#ifndef DEVICE_H
#define DEVICE_H

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

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

void wsdcgq11lm_add(uint64_t u64IeeeAddr);

void wsdcgq11lm_report(uint64_t u64IeeeAddr,
                       int16_t  i16Temperature,
                       int16_t  i16Humidity,
                       int16_t  i16Pressure);

void wsdcgq11lm_delete(uint64_t u64IeeeAddr);

void rtcgq11lm_add(uint64_t u64IeeeAddr);

void rtcgq11lm_report(uint64_t u64IeeeAddr,
                      int8_t   u8Occupancy,
                      uint16_t u16Illuminance);

void rtcgq11lm_delete(uint64_t u64IeeeAddr);

void lilygo_light_add(uint64_t u64IeeeAddr);

void lilygo_light_delete(uint64_t u64IeeeAddr);

void lilygo_light_report(uint64_t u64IeeeAddr, uint8_t u8OnOff);

#ifdef __cplusplus
}
#endif

#endif
/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/