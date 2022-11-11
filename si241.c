#include "main.h"
#include "si241.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/include/port.h"
#include "mcc_generated_files/include/spi0.h"

extern uint8_t SPI_Bout[16];
extern uint8_t SPI_Bin[16];
extern uint8_t RX_Payload[16];
extern uint8_t TX_Payload[16];
extern uint8_t RX_PL_Len;
extern uint8_t TX_PL_Len;
extern uint8_t TX_Address[6];
extern uint8_t RX_Address[6];
extern uint16_t si24_on_timer;

extern volatile TmrDelay TimerD;
extern volatile KEYstateControl Key;
extern volatile uint8_t Si24_Status;

void SI241_PwrOn(void)
{
    uint8_t work;
    if(!IO_PA5_PWR_GetValue())
    {
        PORTA.PIN4CTRL = 0x00;
        IO_PA5_PWR_SetHigh();
        PORTC_set_pin_level(1, true);      // TP3

        IO_PA6_CSN_SetHigh();
        IO_PA7_CE_SetLow();
        TimerD._delaycnt = 0;
        TimerD._delaytarget = 0x0a;     // Set to 100mSec delay
        TimerD._finished = 0;
        TimerD._active = 1;
        while (!TimerD._finished);
        PORTA.DIR = 0xEA;               // Set MISO as INPUT
        PORTMUX.SPIROUTEA = 0x00;       // Map SPI standard mappings to PORTA functions 
        SPI0_Initialize();              // Enable SPI after power up        
        work = SPI0.INTFLAGS;
        if(work && 0x80)
        {
            work = SPI0.DATA;
        }
        SPI0.INTCTRL = 0x01;            // enable SPI Interrupts 
        PORTA.PIN4CTRL = 0x02;          // enable IRQ interrupts
    }
    si24_on_timer = 500;            // set to 500 * 0.01 = 5 Seconds
}

void SI241_PwrOff(void)
{
    if(IO_PA5_PWR_GetValue())
    {
        PORTA.DIR = 0xEE;               // Set MISO as OUTPUT
        PORTMUX.SPIROUTEA = 0x03;       // Un-map SPI standard mappings to PORTA functions 
        PORTA.OUT = 0x00;               // All lines Low
        SPI0_Disable();                 // Disable SPI after power dn       
    }
    IO_PA5_PWR_SetLow(); 
    IO_PA6_CSN_SetLow();
    IO_PA7_CE_SetLow();
}

void SI241_SetupRx(void)
{
    SI241_PwrOn();

//    SI241_LoadTxAddress();
//    IO_PA6_CSN_SetLow();    
//    SPI0_WriteBlock(&TX_Address, 6);
//    while(!SPI0_StatusDone());
//    IO_PA6_CSN_SetHigh();    

    SI241_LoadTxAddress();
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&TX_Address, 6);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();    

    SI241_LoadRxAddress();
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&RX_Address, 6);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();    

    SPI_Bout[0] = SETUP_AW;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0x03;
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();        
    
    SPI_Bout[0] = EN_RXADDR;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0x01;
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();        

    SPI_Bout[0] = RF_CH;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;    
    SPI_Bout[1] = 0x40;
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();        

    IO_PA6_CSN_SetLow();        
    SPI_Bout[0] = RX_PW_P0
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;    
    SPI_Bout[1] = Payload_len;
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();        

    SPI_Bout[0] = RF_SETUP;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;    
    SPI_Bout[1] = 0x08;
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();        

    SPI_Bout[0] = CONFIG;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;    
    SPI_Bout[1] = 0x0F;
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh(); 
    
    IO_PA6_CSN_SetLow();
    SPI0_ReadBlockCmd(&SPI_Bin, 6, 0x0A);       // RX_ADDR_P0
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();     
}


void SI241_SetRx(void)
{
    Si24_Status = 0x80;
    IO_PA7_CE_SetHigh();
}

void SI241_SetStandby(void)
{
    Si24_Status = 0x40;
    IO_PA7_CE_SetLow();
    SPI_Bout[0] = STATUS;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0b01110000;       // clear interrupt flags
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();            
    
}

uint8_t SI241_Status(void)
{
    IO_PA6_CSN_SetLow();
    SPI0_ReadBlockCmd(&SPI_Bin, 2, 0x00);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();
    return SPI_Bin[0];
}

uint8_t SI241_RX0_BC(void)
{
    IO_PA6_CSN_SetLow();
    SPI0_ReadBlockCmd(&SPI_Bin, 2, 0x11);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();
    return SPI_Bin[1];
}

uint8_t SI241_RSSI(void)
{
    IO_PA6_CSN_SetLow();
    SPI0_ReadBlockCmd(&SPI_Bin, 2, 0x09);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();
    return SPI_Bin[1];
}

void SI241_RX0_Payload(uint8_t rx_bc)
{
    if(rx_bc > Payload_len)
    {
        rx_bc = Payload_len;
    }
    rx_bc++;
    IO_PA6_CSN_SetLow();
    SPI0_ReadBlockCmd(&RX_Payload, rx_bc, 0x61);       // R_RX_Payload
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();
}

void SI241_RX0_ClearInt(void)
{
    SPI_Bout[0] = STATUS;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0b01000000;       // clear RX interrupt flags
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();
}

void SI241_LoadRxAddress(void)
{
    RX_Address[0] = RX_ADDR_P0;
    RX_Address[0] = RX_Address[0] | W_REGISTER;
    RX_Address[1] = 0x01;
    RX_Address[2] = 0x00;
    RX_Address[3] = 0x00;
    RX_Address[4] = 0x00;
    RX_Address[5] = 0x01;    
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
