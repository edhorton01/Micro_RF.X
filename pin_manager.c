
#include <xc.h>
#include "pin_manager.h"
#include "stdbool.h"

void PIN_MANAGER_Initialize(void)
{
    /**
    LATx registers
     */
    LATE = 0x03;
    LATD = 0x00;
    LATA = 0x03;
    LATB = 0xf0;
    LATC = 0x03;

    /**
    TRISx registers
     */
    TRISE = 0x03;
    TRISD = 0x00; // all outputs
    TRISC = 0x03; // RC0, RC1 Input
    TRISB = 0xF1; // RB7, RB6, RB5, RB4, RB0 inputs
    TRISA = 0x03; // RA0, RA1 analog inputs
    /**
    ANSELx registers
     */
    ANSELA = 0x02;
    ANSELB = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE = 0x00;

    IO_RB4_SetPullup();
    IO_RB5_SetPullup();
    IO_RB6_SetPullup();
    IO_RB7_SetPullup();
    INTCON2bits.nRBPU = 0;
}

/**
 End of File
 */