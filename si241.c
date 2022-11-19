#include <pic18f45k50.h>
#include "mcc.h"
#include "types.h"
#include "pin_manager.h"
#include "si241.h"
#include "spi1.h"
#include "main.h"


extern uint8_t SPI_Bout[16];
extern uint8_t SPI_Bin[16];
extern uint8_t RX_Payload[16];
extern uint8_t TX_Payload[16];
extern uint8_t RX_PL_Len;
extern uint8_t TX_PL_Len;
extern uint8_t TX_Address[6];
extern uint8_t RX_Address[6];
extern uint8_t RX_Channel;
extern uint16_t si24_on_timer;

extern volatile TmrDelay TimerD;
extern volatile uint8_t Si24_Status;

void SI241_PwrOn(void)
{
    if (!IO_RC6_GetValue())
    {
        IO_RC6_SetHigh();
        RC2_SetDigitalOutput();
        //        RC2_SetHigh(); // TP

        IO_RA3_SetHigh(); // CSN
        RA2_SetLow(); // CE
        TimerD._delaycnt = 0;
        TimerD._delaytarget = 100; // Set to 100mSec delay
        TimerD._finished = 0;
        TimerD._active = 1;
        TimerD._RF_Active = 1;
        TimerD._prime_rx = 0;
    }
    else if (IO_RC6_GetValue() && !TimerD._finished && TimerD._active)
    {
        TimerD._delaycnt++;
        if (TimerD._delaycnt >= TimerD._delaytarget)
        {
            TimerD._finished = 1;
            TimerD._active = 0;
            IO_RB0_SetDigitalInput(); // Set MISO as INPUT
            IO_RB2_SetDigitalInput(); // Set IRQ to input
            SPI1_Initialize(); // Enable SPI after power up
            INTCON3bits.INT2IF = 0; // Enable INT from RF chip to Ext Int 2 : Clear Flag
            INTCON2bits.INTEDG2 = 0; // Set to falling edge
            INTCON3bits.INT2IE = 1; // Enable
            SSP1CON1bits.SSPEN = 1;
            si24_on_timer = 5000; // set to 5000 * 0.001 = 5 Seconds
        }
    }
    else if (IO_RC6_GetValue() && TimerD._finished && !TimerD._active)
    {
        if (si24_on_timer != 0)
        {
            si24_on_timer--;
        }
    }
}

void SI241_PwrOff(void)
{
    if (IO_RC6_GetValue())
    {
        IO_RC6_SetLow();
        RC2_SetDigitalOutput();
        RC2_SetLow(); // TP

        IO_RA3_SetLow(); // CSN
        RA2_SetLow(); // CE
        TimerD._finished = 0;
        TimerD._active = 0;
        TimerD._RF_Active = 0;
        TimerD._prime_rx = 0;

        SSP1CON1bits.SSPEN = 0;
        IO_RB0_SetDigitalOutput(); // Set MISO as Output
        INTCON3bits.INT2IF = 0; //Clear Flag
        INTCON3bits.INT2IE = 0; // Disable
        si24_on_timer = 0;
    }
}

void SI241_Interrupt(void)
{
    INTCON3bits.INT2IF = 0;
    TimerD._new_rx = 1;
    Nop();
    Nop();
    Nop();
}

void SI241_SetupRx(void)
{
    SI241_LoadTxAddress();
    IO_RA3_SetLow(); // CSN low
    SPI1_WriteBlock(&TX_Address, 6);
    IO_RA3_SetHigh(); // CSN high

    SI241_LoadRxAddress();
    IO_RA3_SetLow(); // CSN low
    SPI1_WriteBlock(&RX_Address, 6);
    IO_RA3_SetHigh(); // CSN high

    SPI_Bout[0] = SETUP_AW;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0x03;
    IO_RA3_SetLow(); // CSN low
    SPI1_WriteBlock(&SPI_Bout, 2);
    IO_RA3_SetHigh(); // CSN high

    SPI_Bout[0] = EN_RXADDR;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0x01;
    IO_RA3_SetLow(); // CSN low
    SPI1_WriteBlock(&SPI_Bout, 2);
    IO_RA3_SetHigh(); // CSN high

    SPI_Bout[0] = RF_CH;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = RX_Channel;
    IO_RA3_SetLow(); // CSN low
    SPI1_WriteBlock(&SPI_Bout, 2);
    IO_RA3_SetHigh(); // CSN high

    SPI_Bout[0] = RX_PW_P0;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = Payload_len;
    IO_RA3_SetLow(); // CSN low
    SPI1_WriteBlock(&SPI_Bout, 2);
    IO_RA3_SetHigh(); // CSN high

    SPI_Bout[0] = RF_SETUP;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0x08;
    IO_RA3_SetLow(); // CSN low
    SPI1_WriteBlock(&SPI_Bout, 2);
    IO_RA3_SetHigh(); // CSN high

    SPI_Bout[0] = CONFIG;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0x0F;
    IO_RA3_SetLow(); // CSN low
    SPI1_WriteBlock(&SPI_Bout, 2);
    IO_RA3_SetHigh(); // CSN high

    IO_RA3_SetLow(); // CSN low
    SPI1_ReadBlockCmd(&SPI_Bin, 5, 0x0a); // RX_ADDR_P0
    IO_RA3_SetHigh(); // CSN high
    Nop();
    Nop();
    Nop();
}

void SI241_SetRx(void)
{
    Si24_Status = 0x80;
    TimerD._new_rx = 0;
    RA2_SetHigh(); // CE
}

void SI241_SetStandby(void)
{
    Si24_Status = 0x40;
    RA2_SetLow(); // CE
    SPI_Bout[0] = STATUS;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0b01110000; // clear interrupt flags
    IO_RA3_SetLow(); // CSN low
    SPI1_WriteBlock(&SPI_Bout, 2);
    IO_RA3_SetHigh(); // CSN high
}

uint8_t SI241_Status(void)
{
    uint8_t work;
    IO_RA3_SetLow(); // CSN low
    work = SPI1_ExchangeByte(0);
    IO_RA3_SetHigh(); // CSN high
    return work;
}

uint8_t SI241_RX0_BC(void)
{
    IO_RA3_SetLow(); // CSN low
    SPI1_ReadBlockCmd(&SPI_Bin, 1, 0x11);
    IO_RA3_SetHigh(); // CSN high
    return SPI_Bin[0];
}

uint8_t SI241_RSSI(void)
{
    IO_RA3_SetLow(); // CSN low
    SPI1_ReadBlockCmd(&SPI_Bin, 1, 0x09);
    IO_RA3_SetHigh(); // CSN high
    return SPI_Bin[0];
}

void SI241_RX0_Payload(uint8_t rx_bc)
{
    if (rx_bc > Payload_len)
    {
        rx_bc = Payload_len;
    }
    IO_RA3_SetLow(); // CSN low
    SPI1_ReadBlockCmd(&RX_Payload, rx_bc, 0x61); // R_RX_Payload
    IO_RA3_SetHigh(); // CSN high
}

void SI241_RX0_ClearInt(void)
{
    SPI_Bout[0] = STATUS;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0b01000000; // clear RX interrupt flags
    IO_RA3_SetLow(); // CSN low
    SPI1_WriteBlock(&SPI_Bout, 2);
    IO_RA3_SetHigh(); // CSN high
}

void SI241_LoadRxAddress(void)
{
    uint8_t i, j, t1, t2, c1, c2;

    RX_Address[0] = RX_ADDR_P0;
    RX_Address[0] = RX_Address[0] | W_REGISTER;
    //    RX_Address[1] = 0x30;
    //    RX_Address[2] = 0x4b;
    //    RX_Address[3] = 0x4d;
    //    RX_Address[4] = 0x50;
    //    RX_Address[5] = 0x4f;

    i = 0;
    c1 = 0;
    c2 = 0;
    while (i < 6)
    {
        j = i + 8;
        t1 = EEPROM_Read(i);
        t2 = ~EEPROM_Read(j);
        i++;
        if (t1 == t2)
        {
            if (i != 6)
            {
                RX_Address[i] = t1;
            }
            else
            {
                RX_Channel = t1;
            }
        }
        else
        {
            c1 = 1;
            break;
        }
    }
    if (c1)
    {
        i = 16;
        while (i < 22)
        {
            j = i + 8;
            t1 = EEPROM_Read(i);
            t2 = ~EEPROM_Read(j);
            i++;
            if (t1 == t2)
            {
                if (i != 22)
                {
                    RX_Address[i - 16] = t1;
                }
                else
                {
                    RX_Channel = t1;
                }
            }
            else
            {
                c2 = 1;
                break;
            }
        }
    }
    if (c1 && c2)
    {
        RX_Address[1] = 0x01;
        RX_Address[2] = 0x00;
        RX_Address[3] = 0x00;
        RX_Address[4] = 0x00;
        RX_Address[5] = 0x01;
        RX_Channel = 0x40;
    }
}

void SI241_LoadTxAddress(void)
{
    TX_Address[0] = TX_ADDR;
    TX_Address[0] = TX_Address[0] | W_REGISTER;
    TX_Address[1] = 0x00;
    TX_Address[2] = 0x01;
    TX_Address[3] = 0x01;
    TX_Address[4] = 0x01;
    TX_Address[5] = 0x00;
}