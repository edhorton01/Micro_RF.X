/* 
 * File:   utility.h
 * Author: ed
 *
 * Created on February 18, 2017, 1:42 PM
 */

#ifndef UTILITY_H
#define	UTILITY_H

/**
  Section: Included Files
*/

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

/**
  Section: Data Types Definitions
*/

/**
 *  result size of an A/D conversion
 */

typedef uint16_t adc_result_t;

/**
 *  result type of a Double ADC conversion
 */
typedef struct
{
    adc_result_t adcResult1;
    adc_result_t adcResult2;
} adc_sync_double_result_t;

/** ADC Channel Definition

 @Summary
   Defines the channels available for conversion.

 @Description
   This routine defines the channels that are available for the module to use.

 Remarks:
   None
 */

typedef enum
{
    channel_AN0 =  0x0,
    channel_AN1 =  0x1,
    channel_AN14 =  0xE,
    channel_CTMU =  0x1D,
    channel_DAC =  0x1E,
    channel_FVRBuf2 =  0x1F
} adc_channel_t;

void ADC_Initialize(void);
void ADC_SelectChannel(adc_channel_t channel);
void ADC_StartConversion();
bool ADC_IsConversionDone();
adc_result_t ADC_GetConversionResult(void);
adc_result_t ADC_GetConversion(adc_channel_t channel);
void ADC_ISR(void);

void PWM2_Initialize(void);
void PWM2_LoadDutyValue(uint16_t dutyValue);

void TMR0_Initialize(void);
void TMR0_StartTimer(void);
void TMR0_StopTimer(void);
uint16_t TMR0_ReadTimer(void);
void TMR0_WriteTimer(uint16_t timerVal);
void TMR0_Reload(void);
void TMR0_ISR(void);

void TMR1_Initialize(void);
void TMR1_StartTimer(void);
void TMR1_StopTimer(void);
void TMR1_ISR(void);

void TMR3_Initialize(void);
void TMR3_StartTimer(void);
void TMR3_StopTimer(void);
void TMR3_ISR(void);

void TMR2_Initialize(void);
void TMR2_StartTimer(void);
void TMR2_StopTimer(void);
uint8_t TMR2_ReadTimer(void);
void TMR2_WriteTimer(uint8_t timerVal);
void TMR2_LoadPeriodRegister(uint8_t periodVal);
bool TMR2_HasOverflowOccured(void);

void IOC_Initialize(void);
void IOC_ISR(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UTILITY_H */

