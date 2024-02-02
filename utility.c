#include <xc.h>
#include "utility.h"
#include "main.h"
#include "types.h"
#include "pin_manager.h"
#include "si241.h"

extern KEYstateBITS KeyStatus;
extern LEDControl LEDState[3];
extern uint8_t TurnOff;
extern uint8_t TurnOffBlink;
extern DIMstateBITS DimB;
extern BLINKstateBITS BLINKState;
extern volatile TmrDelay TimerD;
extern volatile uint8_t rf_failsafe;

/**
  Section: ADC Module APIs
 */

void ADC_Initialize(void)
{
    // GO_nDONE stop; ADON enabled; CHS AN0;
    ADCON0bits.CHS = 0x0E; // Select AN14:PC2
    ADCON1 = 0b00000000; // TRIGSEL CCP2; NVCFG VSS; PVCFG VDD;
    ADCON2 = 0b10111110; // ADFM right; ACQT 20TAD; ADCS FOSC/64;

    // ADRESL 0;
    ADRESL = 0x00;
    // ADRESH 0;
    ADRESH = 0x00;

    // Enabling ADC interrupt.
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 0;
    ADCON0bits.ADON = 1; // Enable A/D
}

void ADC_SelectChannel(adc_channel_t channel)
{
    // select the A/D channel
    ADCON0bits.CHS = channel;
    // Turn on the ADC module
    PIR1bits.ADIF = 0;
    ADCON0bits.ADON = 1;
}

void ADC_StartConversion()
{
    // Start the conversion
    PIR1bits.ADIF = 0;
    ADCON0bits.GO_nDONE = 1;
}

bool ADC_IsConversionDone()
{
    // Start the conversion
    return (!ADCON0bits.GO_nDONE);
}

adc_result_t ADC_GetConversionResult(void)
{
    // Conversion finished, return the result
    return ((ADRESH << 8) + ADRESL);
}

/*
adc_result_t ADC_GetConversion(adc_channel_t channel)
{
    // select the A/D channel
    ADCON0bits.CHS = channel;

    // Turn on the ADC module
    ADCON0bits.ADON = 1;

    // Start the conversion
    ADCON0bits.GO_nDONE = 1;

    // Wait for the conversion to finish
    while (ADCON0bits.GO_nDONE)
    {
    }
    // Conversion finished, return the result
    return ((ADRESH << 8) + ADRESL);

}
 */
void ADC_ISR(void)
{
    // Clear the ADC interrupt flag
    PIR1bits.ADIF = 0;
}

/**
  Section: DAC APIs
 */

/*
void DAC_Initialize(void)
{
    VREFCON0 = 0b00100000;
    VREFCON0 = 0b10100000;
    while (!(VREFCON0 & 0b01000000))
    {
        Nop();
    }
    // DACOE enabled; DACEN enabled; DACLPS neg_ref; DACNSS VSS; DACPSS VDD;
    VREFCON1 = 0b10101000;
    // DACR 8;
    VREFCON2 = 0x00;
}

void DAC_SetOutput(uint8_t inputData)
{
    uint8_t work, HighRangeForce;
    extern DIMstateBITS DimB;
    extern uint8_t HighRange;

    if (inputData == 0)
    {
        VREFCON0 = 0b00100000;
        VREFCON1 = 0b10100000;
    }
        //    else if (inputData == 0x1f) {
    else if (DimB._MaxOut || DimB._SuperMax)
    {
        HighRangeForce = 0x10;
        if (inputData <= 0x1d)
        {
            //            inputData = inputData + 2;
            //            VREFCON0 = 0b10100000;
            //            VREFCON1 = 0b10101000;
            work = 0b10100000 + HighRangeForce;
            VREFCON0 = work;
            VREFCON1 = 0b10101000;
        }
        else
        {
            //            inputData = inputData - 10;
            //            VREFCON0 = 0b10100000;
            //            VREFCON1 = 0b10100000;
            work = 0b10100000 + HighRangeForce;
            VREFCON0 = work;
            VREFCON1 = 0b10101000;
        }
    }
    else
    {
        work = 0b10100000 + HighRange;
        VREFCON0 = work;
        VREFCON1 = 0b10101000;
    }
    VREFCON2 = inputData;
}
 */
/**
  Section: Macro Declarations
 */

#define PWM2_INITIALIZE_DUTY_VALUE    187

/**
  Section: PWM Module APIs
 */
/*
void PWM2_Initialize(void)
{
    // Set the PWM to the options selected in the MPLAB(c) Code Configurator

    // CCP2M PWM; DC2B 3;
    CCP2CON = 0x3C;

    // CCPR2L 46;
    CCPR2L = 0x2E;

    // CCPR2H 0;
    CCPR2H = 0x00;

}

void PWM2_LoadDutyValue(uint16_t dutyValue)
{
    // Writing to 8 MSBs of pwm duty cycle in CCPRL register
    CCPR2L = (uint8_t) ((dutyValue & 0x03FC) >> 2);

    // Writing to 2 LSBs of pwm duty cycle in CCPCON register
    CCP2CON = (uint8_t) ((CCP2CON & 0xCF) | ((dutyValue & 0x0003) << 4));
}

  Section: Global Variables Definitions
 */

void (*TMR0_InterruptHandler)(void);

volatile uint16_t timer0ReloadVal;
extern uint8_t timer0Cycle;

/**
  Section: TMR0 APIs
 */


void TMR0_Initialize(void)
{
    // Set TMR0 to the options selected in the User Interface

    // T0PS 1:4; T08BIT 16-bit; T0SE Increment_hi_lo; T0CS FOSC/4; TMR0ON off; PSA assigned;
    T0CON = 0x11;

    //    TMR0H = 0xF4; // load 3000 to give imSec period
    //    TMR0L = 0x4C;

    TMR0H = 0xFA; // load 1500 to give imSec  half period
    TMR0L = 0x24;

    // Load TMR0 value to the 16-bit reload variable
    //    timer0ReloadVal = 0xF44C;
    timer0ReloadVal = 0xFA24;

    // Clear Interrupt flag before enabling the interrupt
    INTCONbits.TMR0IF = 0;

    // Enabling TMR0 interrupt.
    INTCONbits.TMR0IE = 1;
    timer0Cycle = 0;

    // Start TMR0
    TMR0_StartTimer();
}

void TMR0_StartTimer(void)
{
    // Start the Timer by writing to TMR0ON bit
    T0CONbits.TMR0ON = 1;
}

void TMR0_StopTimer(void)
{
    // Stop the Timer by writing to TMR0ON bit
    T0CONbits.TMR0ON = 0;
}

/*
uint16_t TMR0_ReadTimer(void)
{
    uint16_t readVal;
    uint8_t readValLow;
    uint8_t readValHigh;

    readValLow = TMR0L;
    readValHigh = TMR0H;
    readVal = ((uint16_t) readValHigh << 8) + readValLow;

    return readVal;
}

void TMR0_WriteTimer(uint16_t timerVal)
{
    // Write to the Timer0 register
    TMR0H = timerVal >> 8;
    TMR0L = (uint8_t) timerVal;
}

void TMR0_Reload(void)
{
    // Write to the Timer0 register
    TMR0H = timer0ReloadVal >> 8;
    TMR0L = (uint8_t) timer0ReloadVal;
}
 */
void TMR0_ISR(void)
{
    //    RC2_Toggle();     // Test PointRC2_
    // clear the TMR0 interrupt flag
    INTCONbits.TMR0IF = 0;

    // reload TMR0
    // Write to the Timer0 register
    //    TMR0H =  (uint8_t) 0xF2;
    TMR0H = timer0ReloadVal >> 8;
    TMR0L = (uint8_t) timer0ReloadVal;

    timer0Cycle++;
    //    if (!(timer0Cycle & 0x01))
    //    {
    SendLEDs();
    //    }
    TimerD._window = 1;
    ServiceKeyPress();
    if (TimerD._RF_Active)
    {
        SI241_PwrOn();
        if (TimerD._finished)
        {
            if (!(PORTB & 0x04))
            {
                rf_failsafe++;
                if (rf_failsafe >= 3)
                {
                    rf_failsafe = rf_failsafe | 0x40;
                }
            }
            else
            {
                rf_failsafe = 0;
            }
        }
        if (TimerD._rx_on_pulse)
        {
            TimerD._pulsecnt++;
            if (TimerD._pulsecnt >= 5)
            {
                TimerD._pulsecnt = 0;
            }
            if (TimerD._pulsecnt == 0)
            {
                SI241_SetRx();
                RC2_SetHigh();
            }
            else
            {
                //                SI241_ClearRx();
                //                RC2_SetLow();
            }
        }
    }
}

void (*TMR1_InterruptHandler)(void);

extern uint16_t timer1ReloadVal;

void TMR1_Initialize(void)
{
    // Fosc/4; T1PS 1:4; SOSCEN = 0; TxSync = 1; T18-bit; T0SE Increment_hi_lo; Timer off
    T1CON = 0b00100100;
    T1GCON = 0b00000000;

    //    TMR1H = 0xF5; // load 3000 to give imSec period
    //    TMR1L = 0x00;

    // Load TMR0 value to the 16-bit reload variable
    //timer1ReloadVal = 0xFB00;

    // Clear Interrupt flag before enabling the interrupt
    PIR1bits.TMR1IF = 0;

    // Enabling TMR0 interrupt.
    PIE1bits.TMR1IE = 1;

    // Start TMR0
    //   TMR1_StartTimer();
}

void TMR1_StartTimer(void)
{
    // Start the Timer by writing to TMR0ON bit
    PIR1bits.TMR1IF = 0;
    TMR1H = (uint8_t) (timer1ReloadVal >> 8);
    TMR1L = (uint8_t) timer1ReloadVal;
    PIR1bits.TMR1IF = 0;
    Nop();
    Nop();
    Nop();
    PIE1bits.TMR1IE = 1;
    T1CONbits.TMR1ON = 1;
}

void TMR1_StopTimer(void)
{
    // Stop the Timer by writing to TMR0ON bit
    T1CONbits.TMR1ON = 0;
}

void TMR1_ISR(void)
{
    extern DIMstateBITS DimB;

    PIR1bits.TMR1IF = 0;
    TMR1_StopTimer();
    if (BLINKState._onIdle || BLINKState._onForce)
    {
        BLINKState._onIdle = 0;
        BLINKState._onForce = 0;
        switch (BLINKState._port)
        {
        case NO_PORT:
            break;
        case PORT_AM:
            TRISA = TRISA & ~BLINKState._bitmap;
            break;
        case PORT_DM:
#ifdef STANDARD
            if (BLINKState._bitmap == 0x40)
            {
                TRISD = TRISD & ~0x04;
                TRISA = TRISA & ~0x20;
            }
            else
            {
                TRISD = TRISD & ~BLINKState._bitmap;
            }
#else
            TRISD = TRISD & ~BLINKState._bitmap;
#endif
            break;
        }
    }
    if (!TurnOffBlink)
    {
        TRISA = TRISA & ~LEDState[0]._tris_a_contrib;
        TRISD = TRISD & ~LEDState[0]._tris_d_contrib & ~LEDState[2]._tris_d_contrib;
    }
    if (TimerD._RF_Active)
    {
        TRISB = TRISB & 0b11111101;
    }
    else
    {
        TRISB = TRISB & 0b11111000;
    }
}

void (*TMR1_InterruptHandler)(void);

extern uint16_t timer3ReloadVal;

void TMR3_Initialize(void)
{
    // Fosc/4; T1PS 1:4; SOSCEN = 0; TxSync = 1; T18-bit; T0SE Increment_hi_lo; Timer off
    T3CON = 0b00100100;
    T3GCON = 0b00000000;

    TMR3H = 0xF8; // load 3000 to give imSec period
    TMR3L = 0x00;

    // Load TMR0 value to the 16-bit reload variable
    //timer3ReloadVal = 0xFA00;

    // Clear Interrupt flag before enabling the interrupt
    PIR2bits.TMR3IF = 0;

    // Enabling TMR0 interrupt.
    PIE2bits.TMR3IE = 1;

    // Start TMR0
    //    TMR3_StartTimer();
}

void TMR3_StartTimer(void)
{
    // Start the Timer by writing to TMR0ON bit
    TMR3H = (uint8_t) (timer3ReloadVal >> 8);
    TMR3L = (uint8_t) timer3ReloadVal;
    PIR2bits.TMR3IF = 0;
    Nop();
    Nop();
    Nop();
    PIE2bits.TMR3IE = 1;
    T3CONbits.TMR3ON = 1;
}

void TMR3_StopTimer(void)
{
    // Stop the Timer by writing to TMR0ON bit
    T3CONbits.TMR3ON = 0;
}

void TMR3_ISR(void)
{
    PIR2bits.TMR3IF = 0;
    TMR3_StopTimer();
    if (!TurnOffBlink)
    {
        if (!DimB._MaxOutF && !DimB._MaxOutWL)
        {
            if (LEDState[1]._active && !LEDState[0]._active)
            {
                TRISA = TRISA & ~LEDState[1]._tris_a_contrib;
                TRISD = TRISD & ~LEDState[1]._tris_d_contrib;
            }
            else if (LEDState[0]._active && !LEDState[1]._active && !T1CONbits.TMR1ON)
            {
                TRISA = TRISA & ~LEDState[0]._tris_a_contrib;
                TRISD = TRISD & ~LEDState[0]._tris_d_contrib;
            }
            else
            {
                TRISA = TRISA & ~LEDState[1]._tris_a_contrib;
                TRISD = TRISD & ~LEDState[1]._tris_d_contrib;
            }

        }
    }
}

/**
  Section: Global Variables Definitions
 */

void (*TMR2_InterruptHandler)(void);

/**
  Section: TMR2 APIs
 */

void TMR2_Initialize(void)
{
    // Set TMR2 to the options selected in the User Interface

    // T2CKPS 1:16; T2OUTPS 1:8; TMR2ON off;
    T2CON = 0x3A;

    // PR2 93;
    PR2 = 0x5D;

    // TMR2 0;
    TMR2 = 0x00;

    // Clearing IF flag.
    PIR1bits.TMR2IF = 0;

    // Start TMR2
    TMR2_StartTimer();
}

void TMR2_StartTimer(void)
{
    // Start the Timer by writing to TMRxON bit
    T2CONbits.TMR2ON = 1;
}

void TMR2_StopTimer(void)
{
    // Stop the Timer by writing to TMRxON bit
    T2CONbits.TMR2ON = 0;
}

uint8_t TMR2_ReadTimer(void)
{
    uint8_t readVal;

    readVal = TMR2;

    return readVal;
}

void TMR2_WriteTimer(uint8_t timerVal)
{
    // Write to the Timer2 register
    TMR2 = timerVal;
}

void TMR2_LoadPeriodRegister(uint8_t periodVal)
{
    PR2 = periodVal;
}

bool TMR2_HasOverflowOccured(void)
{
    // check if  overflow has occurred by checking the TMRIF bit
    bool status = PIR1bits.TMR2IF;
    if (status)
    {
        // Clearing IF flag.
        PIR1bits.TMR2IF = 0;
    }
    return status;
}

void IOC_Initialize()
{
    uint8_t readVal;
    IOCB = 0xf0; // allow interrupt on change for B7:B4
    IOCC = 0x03; // allow interrupt on change for C0:C1
    readVal = PORTB;
    readVal = PORTC;
    Nop();
    Nop();
    INTCONbits.IOCIF = 0;
    INTCONbits.IOCIE = 1;
}

void IOC_ISR(void)
{
    uint8_t readVal;
    readVal = PORTB;
    readVal = PORTC;
    KeyStatus._pressed = 1;
    Nop();
    Nop();
    INTCONbits.IOCIF = 0;
}

