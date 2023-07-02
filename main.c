
#include <pic18f45k50.h>
#include "mcc.h"
#include "types.h"
#include "main.h"
#include "si241.h"

/* Globals*/


DIMstateBITS DimB;
DIMstateBITS DimB_Hold;
KEYstateBITS KeyStatus;
KEYstateControl Key;
DoubleTap Double_Tap;

LEDControl LEDState[3];

uint8_t DA_Active;
uint8_t RunMode;
uint16_t ModeChangeCnt;
uint8_t TurnOff;
uint8_t TurnOffCnt;
uint8_t TurnOffBlink;
uint8_t TurnOffCntMul;

uint8_t PgmChangeCnt;
uint8_t PgmStat;
uint16_t PgmStatCnt;
uint8_t PgmStatDwell;

uint8_t MaybeOff;
uint8_t Charge;
uint8_t NeedCal;
uint16_t ChargeCnt;
uint16_t ChargeCntScale;
uint16_t NotifyCnt;
uint8_t NotifyState;

uint8_t WL_hold;
uint16_t WL_hold_cnt;
uint8_t WL_hold_trip;

uint8_t BRT_hold;
uint16_t BRT_hold_cnt;
uint8_t BRT_hold_trip;

uint8_t BACK_hold;
uint16_t BACK_hold_cnt;
uint8_t BACK_hold_trip;

uint8_t FRONT_hold;
uint16_t FRONT_hold_cnt;
uint8_t FRONT_hold_trip;

uint8_t BOOT_hold;
uint16_t BOOT_hold_cnt;
uint8_t BOOT_hold_trip;

//uint8_t DA_Val_Full;
//uint8_t DA_Val_Half;
//uint8_t DA_Val_WL_Full;
//uint8_t DA_Val_WL_Half;

uint16_t DA_Val_Avg[16];
uint8_t DA_Avg_pnt;
uint16_t Battery_res;

uint8_t HighRange;
uint8_t HighRangeHold;

uint16_t timer1ReloadVal;
uint16_t timer3ReloadVal;
uint8_t timer0Cycle;

uint16_t bat_ad_val;
uint8_t bat_loop, bat_hold;
uint8_t DoBattery;

uint16_t max_cal;
uint8_t NeedBatCal;
uint8_t LastChgState;

uint8_t DimCycle;
uint8_t KeyChangeLock;
uint8_t TopHold;
uint8_t ForceButton;
BLINKstateBITS BLINKState;

uint8_t SPI_Bout[16];
uint8_t SPI_Bin[16];
uint8_t RX_Payload[16];
uint8_t TX_Payload[16];
uint8_t RX_PL_Len;
uint8_t TX_PL_Len;
uint8_t TX_Address[6];
uint8_t RX_Address[6];
uint8_t RX_Channel;

uint16_t si24_on_timer;

volatile TmrDelay TimerD;
volatile uint8_t Si24_Status;
uint8_t si241_stat;
uint8_t rx_bc;
uint8_t rx_rssi;
RF_Cmd rf_action;
uint8_t remote_pair;
uint8_t remote_model;
uint8_t fcc_test;
uint8_t fcc_power;
uint8_t fcc_channel;

volatile uint8_t rf_failsafe;

extern const unsigned char userID[8] @ 0x200000;
uint8_t Serial_buffer[8], count;


__EEPROM_DATA(0x01, 0x00, 0x00, 0x00, 0x01, 0x40, 0xff, 0xff);
__EEPROM_DATA(0xfe, 0xff, 0xff, 0xff, 0xfe, 0xbf, 0x00, 0x00);
__EEPROM_DATA(0x01, 0x00, 0x00, 0x00, 0x01, 0x40, 0xff, 0xff);
__EEPROM_DATA(0xfe, 0xff, 0xff, 0xff, 0xfe, 0xbf, 0x00, 0x00);


// Table of led operation for each button and number of presses
// first integer is number of timer 0 tic's, an entry of 0x0 means always on
// next two integers are PortB, PortD, PortC on mask and LED On count for pattern, in MsByte to LsByte orientation
// mask bit 1 = 0n, 0 = off


#ifdef WHITEYELLOW
const uint16_t LedFront_FullPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedFront_HalfPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedRear_FullPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedRear_HalfPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
#elif WHITEWHITE
const uint16_t LedFront_FullPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedFront_HalfPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedRear_FullPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedRear_HalfPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
#elif WHITERED
const uint16_t LedFront_FullPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedFront_HalfPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedRear_FullPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedRear_HalfPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
#elif WHITEORANGE
const uint16_t LedFront_FullPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedFront_HalfPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedRear_FullPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedRear_HalfPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
#elif WHITEGREEN
const uint16_t LedFront_FullPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedFront_HalfPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedRear_FullPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedRear_HalfPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};

#elif YELLOWYELLOW
const uint16_t LedFront_FullPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedFront_HalfPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedRear_FullPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedRear_HalfPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
#elif GREENGREEN
const uint16_t LedFront_FullPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedFront_HalfPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedRear_FullPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedRear_HalfPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
    //    0xFA00, 0xFD80, 0xFF00, 0xFF80,
};
#elif REDRED
const uint16_t LedFront_FullPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedFront_HalfPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedRear_FullPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedRear_HalfPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
#elif ORANGEORANGE
const uint16_t LedFront_FullPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedFront_HalfPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedRear_FullPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedRear_HalfPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
#elif REDGREEN
const uint16_t LedFront_FullPWR[] = {
    0xFDF0, 0xFE80, 0xFFA0, 0xFFD0,
};
const uint16_t LedFront_HalfPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedRear_FullPWR[] = {
    0xF940, 0xFBf4, 0xFE34, 0xFF1A,
};
const uint16_t LedRear_HalfPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
#elif IR_LED
const uint16_t LedFront_FullPWR[] = {
    0xFB00, 0xFC80, 0xFD80, 0xFF00,
};
const uint16_t LedFront_HalfPWR[] = {
    0xFB00, 0xFC80, 0xFD80, 0xFF00,
};
const uint16_t LedRear_FullPWR[] = {
    0xFB00, 0xFC80, 0xFD80, 0xFF00,
};
const uint16_t LedRear_HalfPWR[] = {
    0xFB00, 0xFC80, 0xFD80, 0xFF00,
};
#elif IRGREEN
const uint16_t LedFront_FullPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedFront_HalfPWR[] = {
    0xF800, 0xFB00, 0xFD80, 0xFF00,
};
const uint16_t LedRear_FullPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
const uint16_t LedRear_HalfPWR[] = {
    0xF880, 0xFB80, 0xFE00, 0xFF00,
};
#endif

#ifdef NORMAL_VER
const uint16_t Led_B1P1[] = {
    0x0000, 0x03e0, 0x0005
};
const uint16_t Led_B1P2[] = {
    //    0x0036, 0x0220, 0x0002, 0x0036, 0x0360, 0x0004, 0x0036, 0x01c0, 0x0003, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
    0x0050, 0x01c0, 0x0003, 0x0038, 0x0360, 0x0004, 0x0020, 0x0220, 0x0002, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B1P3[] = {
    0x0036, 0x0200, 0x0001,
    0x0030, 0x0300, 0x0002,
    0x0028, 0x0380, 0x0003,
    0x0022, 0x03c0, 0x0004,
    0x001a, 0x03e0, 0x0005,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B2P1[] = {
    0x0000, 0x001F, 0x0005
};
const uint16_t Led_B2P2[] = {
    //    0x0036, 0x0011, 0x0002, 0x0036, 0x001b, 0x0004, 0x0036, 0x000e, 0x0003, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
    0x0050, 0x000e, 0x0003, 0x0038, 0x001b, 0x0004, 0x0020, 0x0011, 0x0002, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B2P3[] = {
    0x0036, 0x0001, 0x0001,
    0x0030, 0x0003, 0x0002,
    0x0028, 0x0007, 0x0003,
    0x0022, 0x000f, 0x0004,
    0x001a, 0x001f, 0x0005,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
#elif GERMAN_VER
const uint16_t Led_B1P1[] = {
    0x0000, 0x0360, 0x0004
};
const uint16_t Led_B1P2[] = {
    0x0000, 0x03e0, 0x0005
};
const uint16_t Led_B1P3[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B2P1[] = {
    0x0000, 0x001F, 0x0005
};
const uint16_t Led_B2P2[] = {
    0x0050, 0x000e, 0x0003, 0x0038, 0x001b, 0x0004, 0x0020, 0x0011, 0x0002, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B2P3[] = {
    0x0000, 0x0000, 0x0000
};
#elif NAVLIGHT_VER
const uint16_t Led_B1P1[] = {
    0x0000, 0x01e0, 0x0004
};
const uint16_t Led_B1P2[] = {
    //    0x0036, 0x0220, 0x0002, 0x0036, 0x0360, 0x0004, 0x0036, 0x01c0, 0x0003, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
    0x0038, 0x00c0, 0x0002, 0x0050, 0x01e0, 0x0004, 0x0020, 0x0120, 0x0002, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B1P3[] = {
    0x0036, 0x0100, 0x0001,
    0x0030, 0x0180, 0x0002,
    0x0028, 0x01c0, 0x0003,
    0x0022, 0x01e0, 0x0004,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B2P1[] = {
    0x0000, 0x000F, 0x0004
};
const uint16_t Led_B2P2[] = {
    //    0x0036, 0x0011, 0x0002, 0x0036, 0x001b, 0x0004, 0x0036, 0x000e, 0x0003, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
    0x0038, 0x0006, 0x0002, 0x0050, 0x000f, 0x0004, 0x0020, 0x0009, 0x0002, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B2P3[] = {
    0x0036, 0x0001, 0x0001,
    0x0030, 0x0003, 0x0002,
    0x0028, 0x0007, 0x0003,
    0x0022, 0x000f, 0x0004,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};

#endif

#ifdef NORMAL_VER
#ifdef WHITEWHITE
const uint16_t Led_B3P1[] = {
    0x0000, 0x03ff, 0x000A
};
const uint16_t Led_B3P2[] = {
    0x0018, 0x0400, 0x0001,
    0x0018, 0x02A0, 0x0003,
    0x0018, 0x0140, 0x0002,
    0x0018, 0x0015, 0x0003,
    0x0018, 0x000A, 0x0002,
    0x0018, 0x0400, 0x0001,
    0x0018, 0x02A0, 0x0003,
    0x0018, 0x0140, 0x0002,
    0x0018, 0x0015, 0x0003,
    0x0018, 0x000A, 0x0002,
};
const uint16_t Led_B3P3[] = {
    0x0000, 0x0400, 0x0001
};
#else
const uint16_t Led_B3P1[] = {
    0x0018, 0x0400, 0x0001,
    0x0018, 0x02A0, 0x0003,
    0x0018, 0x0140, 0x0002,
    0x0018, 0x0015, 0x0003,
    0x0018, 0x000A, 0x0002,
    0x0018, 0x0400, 0x0001,
    0x0018, 0x02A0, 0x0003,
    0x0018, 0x0140, 0x0002,
    0x0018, 0x0015, 0x0003,
    0x0018, 0x000A, 0x0002,
};
const uint16_t Led_B3P2[] = {
    0x0036, 0x0001, 0x0001,
    0x0030, 0x0003, 0x0002,
    0x0028, 0x0007, 0x0003,
    0x0022, 0x000f, 0x0004,
    0x001a, 0x001f, 0x0005,
    0x001a, 0x003e, 0x0005,
    0x001a, 0x007c, 0x0005,
    0x001a, 0x00f8, 0x0005,
    0x001a, 0x01f0, 0x0005,
    0x001a, 0x03e0, 0x0005,
    0x0022, 0x03c0, 0x0004,
    0x0028, 0x0380, 0x0003,
    0x0030, 0x0300, 0x0002,
    0x0036, 0x0200, 0x0001,
    0x0081, 0x0000, 0x0000,
};
const uint16_t Led_B3P3[] = {
    0x0036, 0x0200, 0x0001,
    0x0030, 0x0300, 0x0002,
    0x0028, 0x0380, 0x0003,
    0x0022, 0x03c0, 0x0004,
    0x001a, 0x03e0, 0x0005,
    0x001a, 0x01f0, 0x0005,
    0x001a, 0x00f8, 0x0005,
    0x001a, 0x007c, 0x0005,
    0x001a, 0x003e, 0x0005,
    0x001a, 0x001f, 0x0005,
    0x0022, 0x000f, 0x0004,
    0x0028, 0x0007, 0x0003,
    0x0030, 0x0003, 0x0002,
    0x0036, 0x0001, 0x0001,
    0x0081, 0x0000, 0x0000,
};
#endif

#elif NAVLIGHT_VER
const uint16_t Led_B3P1[] = {
    //    0x0018, 0x0400, 0x0001,
    0x0018, 0x02A0, 0x0003,
    0x0018, 0x0140, 0x0002,
    0x0018, 0x0015, 0x0003,
    0x0018, 0x000A, 0x0002,
    //    0x0018, 0x0400, 0x0001,
    0x0018, 0x02A0, 0x0003,
    0x0018, 0x0140, 0x0002,
    0x0018, 0x0015, 0x0003,
    0x0018, 0x000A, 0x0002,
};
const uint16_t Led_B3P2[] = {
    0x0018, 0x0400, 0x0001,
    0x0018, 0x0120, 0x0002,
    0x0018, 0x0006, 0x0002,
    0x0018, 0x00c0, 0x0002,
    0x0018, 0x0009, 0x0002,

    //    0x0018, 0x0015, 0x0003,
    //0x0018, 0x000A, 0x0002,
    //    0x0018, 0x0400, 0x0001,
    //    0x0018, 0x02A0, 0x0003,
    //    0x0018, 0x0140, 0x0002,
    //    0x0018, 0x0015, 0x0003,
    //    0x0018, 0x000A, 0x0002,
};
//const uint16_t Led_B3P2[] = {
//    0x0000, 0x0400, 0x0001
//};
const uint16_t Led_B3P3[] = {
    0x0036, 0x0100, 0x0001,
    0x0030, 0x0180, 0x0002,
    0x0028, 0x01c0, 0x0003,
    0x0022, 0x01e0, 0x0004,
    0x0022, 0x00e8, 0x0004,
    0x0022, 0x006c, 0x0004,
    0x0022, 0x002e, 0x0004,
    0x0022, 0x000f, 0x0004,
    0x0028, 0x0007, 0x0003,
    0x0030, 0x0003, 0x0002,
    0x0036, 0x0001, 0x0001,
    0x0081, 0x0000, 0x0000,
};

#elif GERMAN_VER
const uint16_t Led_B3P1[] = {
    0x0000, 0x077f, 0x000B
};
const uint16_t Led_B3P2[] = {
    0x0000, 0x0400, 0x0001
};
const uint16_t Led_B3P3[] = {
    0x0000, 0x0000, 0x0000
};
#endif

const uint16_t Led_B4P1[] = {
    0x0000, 0x0200, 0x0001
};
const uint16_t Led_B4P2[] = {
    0x0000, 0x0400, 0x0001
};
const uint16_t Led_B4P3[] = {
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000,
    0x0050, 0x0129, 0x0004,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000,
    0x0050, 0x0129, 0x0004,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000,
    0x0050, 0x0129, 0x0004,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000,
};

const uint16_t Led_B7P1[] = {
    0x0036, 0x0008, 0x0001,
    0x0082, 0x0008, 0x0001,
    0x0082, 0x0008, 0x0001,
    0x0082, 0x0008, 0x0001,
    0x0082, 0x0008, 0x0001,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B7P2[] = {
    0x0036, 0x0008, 0x0001,
    0x0036, 0x000c, 0x0002,
    0x0082, 0x000c, 0x0002,
    0x0082, 0x000c, 0x0002,
    0x0082, 0x000c, 0x0002,
    0x0082, 0x000c, 0x0002,
    0x0081, 0x0000, 0x0002, 0x0081, 0x0000, 0x0002
    //    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B7P3[] = {
    0x0036, 0x0008, 0x0001,
    0x0036, 0x000c, 0x0002,
    0x0036, 0x000e, 0x0003,
    0x0082, 0x000e, 0x0003,
    0x0082, 0x000e, 0x0003,
    0x0082, 0x000e, 0x0003,
    0x0082, 0x000e, 0x0003,
    0x0081, 0x0000, 0x0002, 0x0081, 0x0000, 0x0002
    //    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B8P1[] = {
    0x0036, 0x0008, 0x0001,
    0x0036, 0x000c, 0x0002,
    0x0036, 0x000e, 0x0003,
    0x0036, 0x000f, 0x0004,
    0x0082, 0x000f, 0x0004,
    0x0082, 0x000f, 0x0004,
    0x0082, 0x000f, 0x0004,
    0x0082, 0x000f, 0x0004,
    0x0081, 0x0000, 0x0002, 0x0081, 0x0000, 0x0002
    //    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B8P2[] = {
    0x0036, 0x0008, 0x0001,
    0x0036, 0x000c, 0x0002,
    0x0036, 0x000e, 0x0003,
    0x0036, 0x000f, 0x0004,
    0x0082, 0x000f, 0x0004,
    0x0082, 0x000f, 0x0004,
    0x0082, 0x000f, 0x0004,
    0x0082, 0x000f, 0x0004, 0x0081, 0x0000, 0x0002, 0x0081, 0x0000, 0x0002
    //    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B8P3[] = {
    0x0036, 0x0008, 0x0001,
    0x0036, 0x000c, 0x0002,
    0x0036, 0x000e, 0x0003,
    0x0036, 0x000f, 0x0004,
    0x0082, 0x000f, 0x0004,
    0x0082, 0x000f, 0x0004,
    0x0082, 0x000f, 0x0004,
    0x0082, 0x000f, 0x0004, 0x0081, 0x0000, 0x0002, 0x0081, 0x0000, 0x0002
    //    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};

const uint16_t Led_B9P1[] = {
    0x003c, 0x07EF, 0x000A, 0x0081, 0x0000, 0x0000,
    0x003c, 0x07EF, 0x000A, 0x0081, 0x0000, 0x0000,
    0x003c, 0x07EF, 0x000A, 0x0081, 0x0000, 0x0000,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000,
    0x00A2, 0x07EF, 0x000A, 0x0081, 0x0000, 0x0000,
    0x00A2, 0x07EF, 0x000A, 0x0081, 0x0000, 0x0000,
    0x00A2, 0x07EF, 0x000A, 0x0081, 0x0000, 0x0000,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000,
    0x003c, 0x07EF, 0x000A, 0x0081, 0x0000, 0x0000,
    0x003c, 0x07EF, 0x000A, 0x0081, 0x0000, 0x0000,
    0x003c, 0x07EF, 0x000A, 0x0081, 0x0000, 0x0000,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B9P2[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B9P3[] = {
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000,
    0x0050, 0x0129, 0x0004,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000,
    0x0050, 0x0129, 0x0004,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000,
};


#if NAVLIGHT_VER
#if REDGREEN
const uint16_t Led_B10P1[] = {
    0x0000, 0x01e0, 0x0004
};
const uint16_t Led_B10P2[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B10P3[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B11P1[] = {
    0x0000, 0x000F, 0x0004
};
const uint16_t Led_B11P2[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B11P3[] = {
    0x0000, 0x0000, 0x0000
};
#elif WHITEWHITE
const uint16_t Led_B10P1[] = {
    0x0000, 0x0360, 0x0004
};
const uint16_t Led_B10P2[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B10P3[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B11P1[] = {
    0x0000, 0x001F, 0x0005
};
const uint16_t Led_B11P2[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B11P3[] = {
    0x0000, 0x0000, 0x0000
};
#endif
#elif AMAZON_VER
const uint16_t Led_B10P1[] = {
    0x0000, 0x03e0, 0x0005
};
const uint16_t Led_B10P2[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B10P3[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B11P1[] = {
    0x0030, 0x0003, 0x0002,
    0x0081, 0x0000, 0x0002, 0x0081, 0x0000, 0x0002,
    0x0030, 0x001c, 0x0003,
    0x0081, 0x0000, 0x0002, 0x0081, 0x0000, 0x0002,
    0x0081, 0x0000, 0x0002, 0x0081, 0x0000, 0x0002
};
const uint16_t Led_B11P2[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B11P3[] = {
    0x0000, 0x0000, 0x0000
};
#else
const uint16_t Led_B10P1[] = {
    0x0000, 0x03e0, 0x0005
};
const uint16_t Led_B10P2[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B10P3[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B11P1[] = {
    0x0030, 0x0003, 0x0002,
    0x0081, 0x0000, 0x0002, 0x0081, 0x0000, 0x0002,
    0x0030, 0x001c, 0x0003,
    0x0081, 0x0000, 0x0002, 0x0081, 0x0000, 0x0002,
    0x0081, 0x0000, 0x0002, 0x0081, 0x0000, 0x0002
};
const uint16_t Led_B11P2[] = {
    0x0000, 0x0000, 0x0000
};
const uint16_t Led_B11P3[] = {
    0x0000, 0x0000, 0x0000
};
#endif

#ifdef NORMAL_VER
#ifdef REDGREEN_HYBRID
const uint16_t Led_B12P1[] = {
    0x0018, 0x0000, 0x8081,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B12P2[] = {
    0x0000, 0x0000, 0x8081
};
const uint16_t Led_B12P3[] = {
    0x0000, 0x0000, 0x0000
};
#elif WHITERED_HYBRID
const uint16_t Led_B12P1[] = {
    0x0018, 0x0000, 0x8081,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B12P2[] = {
    0x0000, 0x0000, 0x8081
};
const uint16_t Led_B12P3[] = {
    0x0000, 0x0000, 0x0000
};
#else
const uint16_t Led_B12P1[] = {
    0x0000, 0x0400, 0x0001
};
const uint16_t Led_B12P2[] = {
    0x0018, 0x0400, 0x0001,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B12P3[] = {
    0x0000, 0x0000, 0x0000
};
#endif
#elif MULTIFUNCTION_VER
const uint16_t Led_B12P1[] = {
    0x0000, 0x0000, 0x8081
};
const uint16_t Led_B12P2[] = {
    0x0018, 0x0000, 0x8081,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B12P3[] = {
    0x0000, 0x0000, 0x0000
};
#elif NAVLIGHT_VER
const uint16_t Led_B12P1[] = {
    0x0000, 0x0400, 0x0001
};
const uint16_t Led_B12P2[] = {
    0x0018, 0x0400, 0x0001,
    0x0081, 0x0000, 0x0000, 0x0081, 0x0000, 0x0000
};
const uint16_t Led_B12P3[] = {
    0x0000, 0x0000, 0x0000
};
#endif


const uint16_t Led_size[] = {
    sizeof (Led_B1P1) / 6, sizeof (Led_B1P2) / 6, sizeof (Led_B1P3) / 6,
    sizeof (Led_B2P1) / 6, sizeof (Led_B2P2) / 6, sizeof (Led_B2P3) / 6,
    sizeof (Led_B3P1) / 6, sizeof (Led_B3P2) / 6, sizeof (Led_B3P3) / 6,
    sizeof (Led_B4P1) / 6, sizeof (Led_B4P2) / 6, sizeof (Led_B4P3) / 6,
    sizeof (Led_B7P1) / 6, sizeof (Led_B7P2) / 6, sizeof (Led_B7P3) / 6,
    sizeof (Led_B8P1) / 6, sizeof (Led_B8P2) / 6, sizeof (Led_B8P3) / 6,
    sizeof (Led_B9P1) / 6, sizeof (Led_B9P2) / 6, sizeof (Led_B9P3) / 6,
#if NAVLIGHT_VER
    sizeof (Led_B10P1) / 6, sizeof (Led_B10P2) / 6, sizeof (Led_B10P3) / 6,
    sizeof (Led_B11P1) / 6, sizeof (Led_B11P2) / 6, sizeof (Led_B11P3) / 6,
    sizeof (Led_B12P1) / 6, sizeof (Led_B12P2) / 6, sizeof (Led_B12P3) / 6
#elif AMAZON_VER
    sizeof (Led_B10P1) / 6, sizeof (Led_B10P2) / 6, sizeof (Led_B10P3) / 6,
    sizeof (Led_B11P1) / 6, sizeof (Led_B11P2) / 6, sizeof (Led_B11P3) / 6,
    sizeof (Led_B12P1) / 6, sizeof (Led_B12P2) / 6, sizeof (Led_B12P3) / 6
#else
    sizeof (Led_B10P1) / 6, sizeof (Led_B10P2) / 6, sizeof (Led_B10P3) / 6,
    sizeof (Led_B11P1) / 6, sizeof (Led_B11P2) / 6, sizeof (Led_B11P3) / 6,
    sizeof (Led_B12P1) / 6, sizeof (Led_B12P2) / 6, sizeof (Led_B12P3) / 6
#endif
};

const uint16_t * Led_Point[] = {
    //const uint16_t Led_Point[] = {
    Led_B1P1, Led_B1P2, Led_B1P3,
    Led_B2P1, Led_B2P2, Led_B2P3,
    Led_B3P1, Led_B3P2, Led_B3P3,
    Led_B4P1, Led_B4P2, Led_B4P3,
    Led_B7P1, Led_B7P2, Led_B7P3,
    Led_B8P1, Led_B8P2, Led_B8P3,
    Led_B9P1, Led_B9P2, Led_B9P3,
#if NAVLIGHT_VER
    Led_B10P1, Led_B10P2, Led_B10P3,
    Led_B11P1, Led_B11P2, Led_B11P3,
    Led_B12P1, Led_B12P2, Led_B12P3,
#elif AMAZON_VER
    Led_B10P1, Led_B10P2, Led_B10P3,
    Led_B11P1, Led_B11P2, Led_B11P3,
    Led_B12P1, Led_B12P2, Led_B12P3,
#else
    Led_B10P1, Led_B10P2, Led_B10P3,
    Led_B11P1, Led_B11P2, Led_B11P3,
    Led_B12P1, Led_B12P2, Led_B12P3,
#endif
};

#ifdef NORMAL_VER
#ifdef WHITEWHITE
const uint16_t Button_Press[] = {
    0x0003, 0x0003, 0x0002, 0x0001, 0x0002, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001
};
#elif AMAZON_VER
const uint16_t Button_Press[] = {
    0x0003, 0x0003, 0x0001, 0x0001, 0x0002, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001
};
#else
const uint16_t Button_Press[] = {
    0x0003, 0x0003, 0x0001, 0x0001, 0x0002, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001
};
#endif

#elif NAVLIGHT_VER
const uint16_t Button_Press[] = {
    0x0003, 0x0003, 0x0001, 0x0001, 0x0002, 0x0001, 0x0001, 0x0001, 0x0001, 0x0002, 0x0001, 0x0001
};

#elif GERMAN_VER
const uint16_t Button_Press[] = {
    0x0001, 0x0001, 0x0001, 0x0001, 0x0002, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001
};
#endif

/*
                         Main application
 */
void main(void)
{
    uint8_t work, work1, work2;
    uint8_t eeval1, eeval2;
    uint8_t readVal;
    uint16_t notify_test;
    uint16_t* l_pnt;
    uint8_t ind;

    // Initialize the device
    RunMode = 0;
    NeedCal = 0;
    rf_failsafe = 0;

    count = 8;
    while (count--)
    {
        Serial_buffer[count] = userID[count];
    }

    NeedBatCal = 0;
    LastChgState = 0;
    eeval1 = EEPROM_Read(250);
    work = ~EEPROM_Read(251);
    if (eeval1 != work)
    {
        NeedBatCal = 1;
    }
    eeval2 = EEPROM_Read(252);
    work = ~EEPROM_Read(253);
    if (eeval2 != work)
    {
        NeedBatCal = 1;
    }
    if (NeedBatCal)
    {
        max_cal = 0x3f0;
    }
    else
    {
        max_cal = (eeval2 << 8) + eeval1;
    }

    work = RCONbits.TO;
    ClrWdt();
    if (!work)
    {
        Charge = PORTAbits.RA0;
        if (Charge)
        {
            RunMode = 1;
        }
        else
        {
            RunMode = 0;
            //            RunMode = 1;
        }
    }

    SYSTEM_Initialize();
    INTCONbits.TMR0IE = 0;
    TurnOff = 0;
    TurnOffBlink = 0;
    ModeChangeCnt = 0;
    NotifyCnt = 0;
    NotifyState = 0;
    //    DA_Val_Full = 0x0c;
    WL_hold = 0;
    WL_hold_cnt = 0;
    WL_hold_trip = 0;
    BRT_hold = 0;
    BRT_hold_cnt = 0;
    BRT_hold_trip = 0;
    BACK_hold = 0;
    BACK_hold_cnt = 0;
    BACK_hold_trip = 0;
    FRONT_hold = 0;
    FRONT_hold_cnt = 0;
    FRONT_hold_trip = 0;

    BOOT_hold = 0;
    BOOT_hold_cnt = 0;
    BOOT_hold_trip = 0;
    HighRange = 0;
    DoBattery = 0;
    rf_action._int = 0;
    remote_model = 0;
    fcc_test = 0;

    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    OSCCONbits.IRCF = 0x02;
    OSCCON2bits.PLLEN = 0x01;
    VREFCON0 = 0b00100000; // off voltage reference for now

    while (!RunMode)
    {
        readVal = PORTB & 0xf0; // mask off buttons
        readVal = readVal | ((PORTC & 0x03) << 2);
        if (readVal == 0xd8)
        {
            if (INTCONbits.TMR0IF)
            {
                INTCONbits.TMR0IF = 0;
                TMR0H = 0xF4; // load 3000 to give imSec period
                TMR0L = 0x4c;

                if (ModeChangeCnt >= 0x10)
                {
                    RunMode = 1;
                    Key._bounce = readVal;
                    KeyStatus._scan_end = 1;
                    Key._cmd = Key._bounce;
                }
                else
                {
                    ModeChangeCnt++;
                }
            }
        }
        else
        {
            TMR0H = 0xF4; // load 3000 to give imSec period
            TMR0L = 0x4c;
            ModeChangeCnt = 0;
            OSCCONbits.IDLEN = 0;
            Sleep();
            work = RCONbits.TO;
            if (!work)
            {
                Charge = PORTAbits.RA0;
                ClrWdt();
                if (Charge)
                {
                    RunMode = 1;
                }
                else
                {
                    RunMode = 0;
                }
            }
        }
    }

    INTCONbits.TMR0IE = 1;
    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 0;
    ClrWdt();

    IO_RC6_SetLow();
    IO_RC6_SetDigitalOutput();

    //    IO_RE2_SetHigh();
    //    CalibrateCurrent();
    NeedCal = 0;
    DimB._cmd = 0;
    DimB._DimCnt = 3;

    l_pnt = (uint16_t*) LedRear_FullPWR;
    ind = 3 - DimB._DimCnt;
    LEDState[0]._timer_timeout = l_pnt[ind];

    l_pnt = (uint16_t*) LedRear_FullPWR;
    ind = 3 - DimB._DimCnt;
    LEDState[1]._timer_timeout = l_pnt[ind];

    Charge = 0;
    ChargeCnt = 0;

    LEDState[0]._active = 0;
    LEDState[1]._active = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    TMR1_Initialize();
    TMR3_Initialize();
    Charge = PORTAbits.RA0;
    if (!Charge)
    {
#ifdef	    ALLIR_VER
        WL_hold = 0x42;
        init_button((uint8_t) 4, (uint8_t) 0); // start out of sleep with top light on
        WL_hold = 0;
#else
        init_button((uint8_t) 4, (uint8_t) 0); // start out of sleep with work light on
        remote_pair = 0;
        SI241_LoadRxAddress();
        TimerD._flags = 0; // init flag
        if (remote_pair)
        {
            TimerD._RF_Active = 1; // Enable RF
        }
#endif
    }
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable high priority global interrupts
    //INTERRUPT_GlobalInterruptHighEnable();

    // Enable low priority global interrupts.
    //INTERRUPT_GlobalInterruptLowEnable();

    // Disable high priority global interrupts
    //INTERRUPT_GlobalInterruptHighDisable();

    // Disable low priority global interrupts.
    //INTERRUPT_GlobalInterruptLowDisable();

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();Micro

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    while (1)
    {


        ClrWdt();

        Charge = PORTAbits.RA0;
        if (Charge && !LastChgState)
        {
            remote_pair = 0;
            SI241_LoadRxAddress();
            TimerD._flags = 0; // init flag

            if (remote_pair)
            {
                SI241_PwrOff(0x01);
                TimerD._RF_Active = 1;
                SI241_PwrOn();
            }
        }

        if (KeyStatus._startCycle)
        {
            ServiceCmd();
            KeyStatus._startCycle = 0;
            KeyStatus._interrupt = 0;
            if (!TurnOff)
            {
                if (PgmStat == 0x81)
                {
                    notify_test = 800;
                    PgmStatCnt++;
                    if (PgmStatCnt >= notify_test)
                    {
                        PgmStatCnt = 0;
                        PgmStatDwell = 0;
                        KeyChangeLock = 0;
                        //                        IO_RE2_SetHigh();
                        //                        NotifyState = 1;
                    }
                }
#ifdef ALLIR_VER
                NotifyCnt++;
                notify_test = 0;
                if (LEDState[0]._active)
                {
                    if (WL_hold_trip)
                    {
                        notify_test = 5000;
                    }
                    else if (LEDState[0]._button == 1)
                    {
                        notify_test = 5000;
                    }
                    else if (BACK_hold_trip)
                    {
                        notify_test = 5000;
                    }
                    else if ((LEDState[0]._button == 4) && (LEDState[0]._button_press == 0))
                    {
                        notify_test = 5000;
                    }
                }
                else if (LEDState[1]._active)
                {
                    if (LEDState[1]._button == 2)
                    {
                        notify_test = 5000;
                    }
                }
                if (notify_test != 0)
                {
                    if (!NotifyState)
                    {
                        if (NotifyCnt >= notify_test)
                        {
                            NotifyCnt = 0;
                            NotifyState = 1;
                        }
                    }
                    else
                    {
                        if (NotifyCnt >= 2)
                        {
                            Nop();
                            Nop();
                            NotifyCnt = 0;
                            NotifyState = 0;
                        }
                    }
                }
                else
                {
                    NotifyCnt = 0;
                    NotifyState = 0;
                }
#endif
                ServiceLEDs((uint8_t) 0);
                ServiceTopLED((uint8_t) 2);
                //		ServiceLEDs((uint8_t) 1);
            }
        }
        else if (KeyStatus._interrupt)
        {
            KeyStatus._interrupt = 0;
            if (!TurnOff)
            {
                ServiceLEDs((uint8_t) 1);
            }
        }

        if (TimerD._RF_Active && TimerD._finished && (rf_failsafe & 0x40))
        {
            rf_failsafe = 0;
            SI241_PwrOff(0);
            TimerD._RF_Active = 1;
        }

        if (TimerD._RF_Active && TimerD._finished && !TimerD._active && !TimerD._prime_rx)
        {
            TimerD._prime_rx = 1;
            if (rf_action._pair_mode)
            {
                rf_action._pair_active = 1;
            }
            Nop();
            Nop();
            Nop();
            SI241_SetupRx();
            TimerD._statuscnt = 0;
            SI241_RX0_ClearInt();
            SI241_SetRx();
        }
        else if (TimerD._RF_Active && TimerD._finished && !TimerD._active && TimerD._prime_rx && TimerD._new_rx)
        {
            TimerD._new_rx = 0;
            rx_bc = SI241_RX0_BC();
            Nop();
            Nop();
            Nop();
            SI241_RX0_Payload(rx_bc);
            if (rx_bc == 2)
            {
                RX_Payload[1] = ~RX_Payload[1];
                if (RX_Payload[0] == RX_Payload[1])
                {
                    rf_action._RF_cmd = RX_Payload[0];
                    rf_action._new_cmd = 1;
                }
                rx_rssi = SI241_RSSI();
                Nop();
                Nop();
                Nop();
                SI241_ClearRxFifo();
                SI241_RX0_ClearInt();
            }
            else if ((rx_bc == 7) && rf_action._pair_active)
            {
                RX_Payload[1] = ~RX_Payload[1];
                if (RX_Payload[0] == RX_Payload[1])
                {
                    SI241_SaveRxAddress();
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                    LEDState[2]._active = 0;
                    clean_up();
                    IO_RE2_SetLow();
                    PgmStat = 0;
                    ForceButton = 0x82;
                    init_button((uint8_t) 4, (uint8_t) 0);
                    DimB._Bat_Status = 1;
                    SI241_ClearRxFifo();
                    SI241_SetStandby();
                    si24_on_timer = 50; // set to 50 * 0.001 = to use as delay
                    while (si24_on_timer > 0)
                    {
                        if (TimerD._window)
                        {
                            Nop();
                            Nop();
                            Nop();
                            TimerD._window = 0;
                            si24_on_timer--;
                        }
                    }
                    SI241_SetuptxResp();
                    SI241_SetTxResp();

                    while (!TimerD._new_tx)
                    {
                        Nop();
                        Nop();
                        Nop();
                    }

                    TimerD._new_tx = 0;
                    SI241_SetStandby();

                    SI241_PwrOff(0);
                    TimerD._RF_Active = 1;
                    SI241_PwrOn();
                    rf_action._pair_mode = 0;
                    rf_action._pair_active = 0;
                    rf_action._RF_cmd = RX_Payload[0];
                    rf_action._new_cmd = 1;
                    rf_action._just_paired = 1;
                }
            }
        }
        else if (TimerD._RF_Active && TimerD._finished && !TimerD._active && TimerD._prime_rx && (TimerD._rx_on_full || TimerD._rx_on_pulse) && TimerD._window)
        {
            TimerD._window = 0;
            TimerD._statuscnt++;
            if (TimerD._statuscnt >= 200)
            {
                TimerD._statuscnt = 0;
                if (TimerD._rx_on_full)
                {
                    RC2_Toggle(); // Test Point RC2
                }
                if (si24_on_timer != 0)
                {
                    si24_on_timer--;
                    if (si24_on_timer == 0)
                    {
                        if (TimerD._rx_on_full)
                        {
                            TimerD._rx_on_full = 0;
                            TimerD._rx_on_pulse = 1;
                            TimerD._pulsecnt = 0;
                            SI241_PwrOff(0);
                            TimerD._RF_Active = 1;
                            SI241_PwrOn();
                            rf_action._pair_mode = 0;
                            rf_action._pair_active = 0;
                            si24_on_timer = si24_on_timer_val; // 10 Minutes
                        }
                        else if (TimerD._rx_on_pulse)
                        {
                            TimerD._rx_on_pulse = 0;
                            SI241_PwrOff(0);
                        }
                    }
                }
                //                si241_stat = SI241_Status();
            }
        }
        ServiceRFCmd();

        OSCCONbits.SCS = 0; // PRI_IDLE
        if (LEDState[0]._active || LEDState[1]._active || LEDState[2]._active || (PgmStat == 0x81) || (fcc_test & 0x80))
        {
            if (PgmStat != 0x81)
            {
                if (remote_pair)
                {
                    TimerD._RF_Active = 1; // Enable RF
                    if (TimerD._rx_on_pulse)
                    {
                        TimerD._rx_on_pulse = 0;
                        TimerD._prime_rx = 0;
                        SI241_SetRx();
                    }
                    TimerD._rx_on_full = 1;
                    si24_on_timer = si24_on_timer_val; // 10 Minutes
                }
            }
            OSCCONbits.IDLEN = 1;
            OSCCONbits.IRCF = 0x07;
            IO_RE2_SetHigh();
        }
        else
        {
            if (!TurnOff && !PgmStat)
            {
                clean_up();
            }
            MaybeOff = 1;
            if (Charge)
            {
                if (!WL_hold && !BOOT_hold && !BRT_hold && !BACK_hold && !FRONT_hold)
                {
                    ChargeCnt++;
                }
                else
                {
                    ChargeCnt = 0;
                }
                ChargeCntScale = ChargeCnt;
                if (OSCCONbits.IRCF != 0x02)
                {
                    ChargeCntScale = ChargeCntScale >> 7;
                }
                if (!LastChgState)
                {
                    ChargeCntScale = 0xff;
                }
                if (ChargeCntScale > 0xff)
                {
                    ChargeCnt = 0;
                    ChargeCntScale = 0;
                }
                work = PORTE & 0x03;
                if (work == 0x02)
                {
                    LastChgState = work;
                    if (ChargeCntScale >= 0xff)
                    {
                        MaybeOff = 0;
#ifdef ALLIR_VER
                        BLINKState._port = PORT_DM;
                        BLINKState._bitmap = 0x80;
                        //                        TRISDbits.TRISD7 = 1;
#else
                        BLINKState._port = PORT_DM;
                        BLINKState._bitmap = 0x40;
                        //                        TRISDbits.TRISD6 = 1;
#endif
                        BLINKState._onForce = 1;
                        PIR1bits.TMR1IF = 0;
                        TMR1_StopTimer();

                        if (!WL_hold && !BOOT_hold && !TimerD._RF_Active)
                        {
                            OSCCONbits.IRCF = 0x02;
                        }
                        //                        IO_RE2_SetHigh();
                    }
                }
                else if (work == 0x01)
                {
                    if (LastChgState == 0x02)
                    {
                        LastChgState = work;
                        if (NeedBatCal)
                        {
                            NeedBatCal = 0;
                            clean_up();
                            IO_RE2_SetHigh();
                            //			    TRISCbits.RC1 = 0; // disable current test function
                            ReadBattery();
                            IO_RE2_SetLow();
                            work1 = (uint8_t) (Battery_res & 0xff);
                            work2 = ~work1;
                            EEPROM_Write(250, work1);
                            EEPROM_Write(251, work2);
                            work1 = (uint8_t) (Battery_res >> 8);
                            work2 = ~work1;
                            EEPROM_Write(252, work1);
                            EEPROM_Write(253, work2);
                        }
                    }
                    //		    if ((ChargeCnt == 0x1d) || (ChargeCnt == 0x1f))

                    if ((ChargeCntScale == 0xfa) || (ChargeCntScale == 0xff))
                    {
                        MaybeOff = 0;
#ifdef ALLIR_VER
                        BLINKState._port = PORT_DM;
                        BLINKState._bitmap = 0x80;
                        //                        TRISDbits.TRISD7 = 1;
#else
                        BLINKState._port = PORT_DM;
                        BLINKState._bitmap = 0x40;
                        //                        TRISDbits.TRISD6 = 1;
#endif
                        BLINKState._onForce = 1;
                        PIR1bits.TMR1IF = 0;
                        TMR1_StopTimer();

                        if (!WL_hold && !BOOT_hold && !BRT_hold && !BACK_hold && !FRONT_hold && !TimerD._RF_Active)
                        {
                            OSCCONbits.IRCF = 0x02;
                        }
                        //                        IO_RE2_SetHigh();
                    }
                }
            }
            else
            {
                ChargeCnt = 0;
            }
            if (MaybeOff)
            {
                if (!WL_hold && !BOOT_hold && !TimerD._RF_Active)
                {
                    OSCCONbits.IRCF = 0x02;
                }
                if (!TurnOff)
                {
                    if (TimerD._RF_Active)
                    {
                        TRISB = 0xf5; // lower outputs except for MISO and INT
                    }
                    else
                    {
                        TRISB = 0xf0; // lower outputs
                    }
                    TRISD = 0x00; // all outputs
                    IO_RE2_SetLow();
                }
            }

            if (KeyStatus._pressed || Key._DB_cnt || Charge || TurnOff || WL_hold || BOOT_hold || BRT_hold || BACK_hold || FRONT_hold || TimerD._RF_Active)
            {
                OSCCONbits.IDLEN = 1;
            }
            else
            {
                OSCCONbits.IDLEN = 0;
            }
        }
        Sleep();
        /*
                if (DoBattery)
                {
                    DoBattery = 0;
                    ReadBattery();
                    TRISCbits.RC1 = 0; // disable current test function
                    IO_RE2_SetLow();
                }
         */
    }
}

#define debounce 40
#define debounce2p 90

#ifdef SS_VER
#define debounce_idle 18
#else
#define debounce_idle 1
#endif

void ServiceKeyPress(void)
{
    uint8_t kb, kb2;
    uint8_t bb, ind;
    uint16_t test;
    uint16_t* l_pnt;

    KeyStatus._interrupt = 1; // flag this interrupt happened
    /*
        if (Double_Tap._front_dt_inprog)
        {
            Double_Tap._front_counter++;
        }
        if (Double_Tap._rear_dt_inprog)
        {
            Double_Tap._rear_counter++;
        }
     */
    if (!(timer0Cycle & 0x01))
    {
        KeyStatus._startCycle = 1; // flag start of a two window cycle
        kb = PORTB & 0xf0; // mask off buttons
        kb2 = (PORTC & 0x03) << 2;
        kb = kb | kb2;
        if (kb == Key._bounce)
        {
            if (!KeyStatus._scan_end)
            {
                if (!KeyStatus._scan_st)
                {
                    KeyStatus._scan_st = 1;
                    KeyStatus._scan_end = 0;
                    Key._two_press_cnt = 0;
                    if (LEDState[0]._active || LEDState[1]._active || LEDState[2]._active || TimerD._RF_Active)
                    {
                        Key._DB_cnt = debounce;
                    }
                    else
                    {
                        Key._DB_cnt = debounce_idle;
                    }
                }
                else
                {
                    Key._DB_cnt--;
                    if (Key._DB_cnt == 0)
                    {
                        Key._two_press_cnt = debounce2p;
                        KeyStatus._scan_end = 1;
                        if (Key._cmd != Key._bounce)
                        {
                            if (KeyChangeLock == 0x40)
                            {
                                if ((Key._cmd == 0x6c) && ((Key._bounce == 0xbc) || (Key._bounce == 0x7c)))
                                {
                                    KeyChangeLock = 0;
                                    Key._cmd = Key._bounce;
                                    KeyStatus._new_cmd = 0;
                                }
                            }
                            else if (KeyChangeLock == 0x20)
                            {
                                if ((Key._cmd == 0x78) && ((Key._bounce == 0xf8) || (Key._bounce == 0x7c)))
                                {
                                    KeyChangeLock = 0;
                                    KeyStatus._new_cmd = 0;
                                }
                            }
                            else if (KeyChangeLock == 0x10)
                            {
                                if ((Key._cmd == 0x78) && ((Key._bounce == 0xdc) || (Key._bounce == 0x7c)))
                                {
                                    KeyChangeLock = 0;
                                    KeyStatus._new_cmd = 0;
                                }
                            }
                            else if (KeyChangeLock == 0x80)
                            {
                                Key._cmd = Key._bounce;
                                KeyStatus._new_cmd = 0;
                                KeyChangeLock = 0;
                            }
                            else
                            {
                                KeyChangeLock = 0;
                                Key._cmd = Key._bounce;
                                KeyStatus._new_cmd = 1;
                            }

                        }
                    }
                }
            }
        }
        else
        {
            switch (Key._bounce)
            {

            case 0x6c:
            {
                if ((kb == 0xec) || (kb == 0x7c))
                {
                    if (Key._two_press_cnt == 0)
                    {
                        Nop();
                        Nop();
                        Nop();
                        Key._bounce = kb;
                        Key._DB_cnt = debounce;
                        KeyStatus._scan_end = 0;
                        KeyStatus._scan_st = 0;
                        if (!(TurnOff & 0x80))
                        {
                            TurnOff = 0;
                            KeyChangeLock == 0;
                        }
                    }
                    else
                    {
                        Key._two_press_cnt--;
                    }
                }
                else
                {
                    Key._bounce = kb;
                    Key._DB_cnt = debounce;
                    KeyStatus._scan_end = 0;
                    KeyStatus._scan_st = 0;
                    if (!(TurnOff & 0x80))
                    {
                        TurnOff = 0;
                        KeyChangeLock == 0;
                    }
                }
                break;
            }

            default:
            {
                Key._bounce = kb;
                Key._DB_cnt = debounce;
                KeyStatus._scan_end = 0;
                KeyStatus._scan_st = 0;
                if (!(TurnOff & 0x80))
                {
                    TurnOff = 0;
                    KeyChangeLock == 0;
                }
                break;
            }

            }
        }
        //        if (kb != 0xfc || kb == 0x98 || kb == 0xF0)
        if (kb != 0xfc)
        {
            KeyStatus._pressed = 1;
            DimB._MaxOutWL = 0;
            if (PgmStat == 0x80)
            {
                test = 0x08;
                if (TimerD._RF_Active)
                {
                    test = 0x400;
                }
                PgmStatCnt++;
                if (PgmStatCnt >= test)
                {
                    Nop();
                    Nop();
                    Nop();
                    PgmStat = 0x81;
                    PgmStatCnt = 0;
                    PgmStatDwell = 0;
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                    RestoreNavDim();
                    SI241_PwrOff(0);

                    TimerD._RF_Active = 1;
                    remote_pair = 0x01;
                    SI241_PwrOn();
                    rf_action._pair_mode = 1;
                    rf_action._pair_active = 0;

                    DimB_Hold = DimB;
                    DimB._Nav_Dim = 1;
                    DimB._DimCnt = 0;
                    l_pnt = (uint16_t*) LedRear_FullPWR;
                    ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
                    LEDState[0]._timer_timeout = l_pnt[ind];
                    if (remote_model == 0x80)
                    {
                        ForceButton = 0x81;
                        init_button((uint8_t) 3, (uint8_t) 0);
                    }
                    else
                    {
                        ForceButton = 0x82;
                        init_button((uint8_t) 3, (uint8_t) 0);
                    }
                    KeyChangeLock = 0x80;
                }
            }

            else if (WL_hold == 0x80)
            {
#ifndef NO_WL_HOLD
                WL_hold_cnt++;
#endif
                if (WL_hold_cnt >= 0x300)
                {
                    /*
                                        if (!DimB._DA_Dim)
                                        {
                                            DA_Active = DA_Val_Full;
                                        }
                                        else
                                        {
                                            DA_Active = DA_Val_Half;
                                        }
                     */
                    WL_hold_trip = 0x80;
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                    RestoreNavDim();
#ifdef NORMAL_VER
                    WL_hold = 0x00;
                    init_button((uint8_t) 1, (uint8_t) 0);
                    WL_hold = 0x40;
                    init_button((uint8_t) 2, (uint8_t) 1);
                    WL_hold = 0x00;
#elif GERMAN_VER
                    WL_hold = 0x43;
                    init_button((uint8_t) 1, (uint8_t) 0);
                    WL_hold = 0x43;
                    init_button((uint8_t) 2, (uint8_t) 1);
                    WL_hold = 0x00;
                    LEDState[1]._holdoff = 0;
#else
                    WL_hold = 0x00;
                    init_button((uint8_t) 1, (uint8_t) 0);
                    WL_hold = 0x40;
                    init_button((uint8_t) 2, (uint8_t) 1);
                    WL_hold = 0;
#endif
                }
                else
                {
                    //		    if ((LEDState[0]._active == 0) && (LEDState[1]._active == 0))
                    //		    {
                    //		    DimB._MaxOutWL = 0x01;
                    //		    }
                }
            }
            else if (BRT_hold == 0x80)
            {
                BRT_hold_cnt++;
                test = 8;
                if (BRT_hold_trip || (LEDState[0]._active != 0) || (LEDState[1]._active != 0) || TimerD._RF_Active)
                {
                    test = 2048;
                }
                if (BRT_hold_cnt >= test)
                {
                    if (DimB._SOS_Mode)
                    {
                        LEDState[0]._active = 0;
                        LEDState[1]._active = 0;
                        DimB._SOS_Mode = 0;
                        clean_up();
                        BRT_hold_trip = 0;
                        BRT_hold_cnt = 0;
                        BRT_hold = 0;
                    }
                    else
                    {
                        RestoreNavDim();
                        BRT_hold = 0x40;
                        BRT_hold_cnt = 0;
                        BRT_hold_trip = 0x80;
                        DimB._MaxOutD = 0x01;
                        BACK_hold_trip = 0;
                        FRONT_hold_trip = 0;
                        BOOT_hold_trip = 0;
                        WL_hold_trip = 0;
                        LEDState[0]._active = 0;
                        LEDState[1]._active = 0;

#ifdef GERMAN_VER
                        if (DimB._German_IR)
                        {
                            BACK_hold = 0;
                            BRT_hold = 0;
                            BRT_hold_cnt = 0;
                            BRT_hold_trip = 0;
                            DimB._German_IR = 0;
                        }
                        else
                        {
                            DimB._DimCnt = 3;
                            BACK_hold = 0x21;
                            init_button((uint8_t) 3, (uint8_t) 0);
                            BACK_hold = 0;
                            BRT_hold = 0;
                            DimB._German_IR = 1;
                        }
#elif LE_HYBRID_VER
                        BACK_hold = 0x21;
                        init_button((uint8_t) 3, (uint8_t) 0);
                        BACK_hold = 0;
                        BRT_hold = 0;
                        DimB._IR_Mode = 1;
#else
                        DimB._SOS_Mode = 1;
#ifdef LE_EXT_VER
                        bb = 8;
#elif BUTTON_EXTEND
                        bb = 9;
#else
                        bb = 7;
#endif

                        l_pnt = (uint16_t*) LedRear_FullPWR;
                        ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
                        if (ind == 0)
                        {
                            LEDState[0]._timer_timeout = l_pnt[ind] + 0x100;
                        }
                        else
                        {
                            LEDState[0]._timer_timeout = l_pnt[ind];
                        }

                        l_pnt = (uint16_t*) LedFront_FullPWR;
                        ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
                        if (ind == 0)
                        {
                            LEDState[1]._timer_timeout = l_pnt[ind] + 0x100;
                        }
                        else
                        {
                            LEDState[1]._timer_timeout = l_pnt[ind];
                        }

                        BRT_hold = 0x20;
                        BACK_hold = 0x20;
                        init_button((uint8_t) bb, (uint8_t) 0);
                        BACK_hold = 0;
                        BRT_hold = 0;
#endif
                    }
                }
            }

            else if (BOOT_hold == 0x80)
            {
                test = 0x300;
                if (VBUS_PORT)
                {
                    test = 0x900;
                }
                BOOT_hold_cnt++;
                if (BOOT_hold_cnt >= test)
                {
                    BOOT_hold = 0x40;
                    BOOT_hold_cnt = 0;

                    if (VBUS_PORT)
                    {
                        SI241_PwrOff(0x01);
                        LEDState[0]._active = 0;
                        LEDState[1]._active = 0;
                        BOOT_hold = 0;
                        INTCONbits.GIE = 0;
                        OSCCONbits.IRCF = 0x07;
                        TRISB = 0xF0; // lower outputs
                        LATB = 0xF0;
                        TRISD = 0x00; // all outputs
                        LATD = 0x00;
                        IO_RE2_SetHigh();
#ifdef ALLIR_VER
                        TRISDbits.TRISD7 = 1;
#else
                        TRISDbits.TRISD6 = 1;
#endif
                        OSCCONbits.IDLEN = 1;
                        TMR0_StopTimer();
                        //		    TMR2_StopTimer();
                        asm("goto 0x001C");
                    }
#ifdef GERMAN_VER
                    else
                    {
                        BOOT_hold_cnt = 0;
                    }
#elif NAVLIGHT_VER
                    else
                    {
                        RestoreNavDim();
                        BOOT_hold_trip = 0x80;
                        LEDState[0]._active = 0;
                        LEDState[1]._active = 0;
                        WL_hold = 0x44;
                        init_button((uint8_t) 3, (uint8_t) 0);
                        WL_hold = 0;
                        BOOT_hold = 0;
                        BOOT_hold_cnt = 0;
                    }
#elif AMAZON_VER
                    else
                    {
                        RestoreNavDim();
                        BOOT_hold_trip = 0x80;
                        LEDState[0]._active = 0;
                        LEDState[1]._active = 0;
                        WL_hold = 0;
                        init_button((uint8_t) 3, (uint8_t) 0);
                        BOOT_hold = 0;
                        BOOT_hold_cnt = 0;
                    }
#else
                    else
                    {
                        BOOT_hold_trip = 0x80;
                        LEDState[0]._active = 0;
                        LEDState[1]._active = 0;
                        WL_hold = 0x41;
                        init_button((uint8_t) 1, (uint8_t) 0);
                        WL_hold = 0x41;
                        init_button((uint8_t) 2, (uint8_t) 1);
                        WL_hold = 0;
                        BOOT_hold = 0;
                        BOOT_hold_cnt = 0;
                    }
#endif
                }
            }

            else if (BACK_hold == 0x80)
            {
                BACK_hold_cnt++;
                if (BACK_hold_cnt >= 0x900)
                {
                    BACK_hold = 0x40;
                    BACK_hold_cnt = 0;
                    BACK_hold_trip = 0x80;
                    DimB._MaxOutR = 0x01;
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                    clean_up();
                    IO_RE2_SetHigh();
                    TRISCbits.RC1 = 1; // enable current test function
                    //		    DoBattery = 1;
                    ReadBattery();
                    TRISCbits.RC1 = 0; // disable current test function
                    IO_RE2_SetLow();
#ifdef LE_EXT_VER
                    bb = 6;
#elif BUTTON_EXTEND
                    bb = 7;
#else
                    bb = 5;
#endif
                    BACK_hold = 0x20;
                    BRT_hold = 0;
                    Battery_res = Battery_res + (0x400 - max_cal);
                    if (Battery_res >= 0x3a1)
                    {
                        bb++;
                        if (Battery_res >= 0x3d4)
                        {
                            BACK_hold = 0x22;
                        }
                        else if (Battery_res >= 0x3ba)
                        {
                            BACK_hold = 0x21;
                        }
                    }
                    else
                    {
                        if (Battery_res >= 0x387)
                        {
                            BACK_hold = 0x22;
                        }
                        else if (Battery_res >= 0x36e)
                        {
                            BACK_hold = 0x21;
                        }
                    }
                    DimB_Hold = DimB;
                    DimB._Bat_Status = 1;
                    init_button((uint8_t) bb, (uint8_t) 1);
                    BACK_hold = 0;
                }
            }

            else if (FRONT_hold == 0x80)
            {
#ifndef NO_FRONTHOLD
                FRONT_hold_cnt++;
#endif
                if (FRONT_hold_cnt >= 0x300)
                {
                    FRONT_hold_trip = 0x80;
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                    RestoreNavDim();
#ifdef NORMAL_VER
#ifdef WHITEWHITE
                    DimB._MaxOutF = 0x01;
                    FRONT_hold = 0x42;
                    init_button((uint8_t) 3, (uint8_t) 0);
                    FRONT_hold = 0x00;
#else
                    FRONT_hold = 0x41;
                    init_button((uint8_t) 3, (uint8_t) 0);
                    FRONT_hold = 0x00;
#endif
#elif NAVLIGHT_VER
                    FRONT_hold = 0x42;
                    init_button((uint8_t) 3, (uint8_t) 0);
                    FRONT_hold = 0x00;
#else
                    WL_hold = 0x40;
                    init_button((uint8_t) 1, (uint8_t) 0);
                    WL_hold = 0x00;
                    init_button((uint8_t) 2, (uint8_t) 1);
                    WL_hold = 0;
#endif
                }
            }
        }
        else
        {
            KeyStatus._pressed = 0;
            WL_hold = 0;
            BOOT_hold = 0;
            BRT_hold = 0;
            BACK_hold = 0;
            Key._two_press_cnt = 0;
            KeyChangeLock = 0;
        }
        if (TurnOff)
        {
            clean_up();
            LEDState[0]._LEDactive = 0;
            LEDState[1]._LEDactive = 0;
            LEDState[2]._LEDactive = 0;

            if (!(TurnOff & 0x80))
            {
                test = 0x20;
                if (BRT_hold_trip || (LEDState[0]._active != 0) || (LEDState[1]._active != 0) || TimerD._RF_Active)
                {
                    test = 2048;
                }
                ModeChangeCnt++;
                if (ModeChangeCnt >= test)
                {
                    TurnOff = 0x81;
                    TurnOffCnt = 0;
                    TurnOffCntMul = 0;
                    Nop();
                    Nop();
                }
            }
            else
            {
                Nop();
                Nop();
                if (BRT_hold_trip || (LEDState[0]._active != 0) || (LEDState[1]._active != 0) || TimerD._RF_Active)
                {
                    TurnOffCntMul++;
                    if (TurnOffCntMul >= 0x40)
                    {
                        TurnOffCntMul = 0;
                        TurnOffCnt++;
                    }
                }
                else
                {
                    TurnOffCnt++;
                }
                TurnOffBlink = 0;
                if ((TurnOffCnt >= 0x01) && TurnOffCnt <= 0x08)
                {
                    TurnOffBlink = 1;
#ifdef ALLIR_VER
                    TRISDbits.TRISD7 = 1;
#else
                    TRISDbits.TRISD6 = 1;
#endif
                    IO_RE2_SetHigh();
                }
                else if ((TurnOffCnt >= 0x09) && TurnOffCnt <= 0x10)
                {
                    TurnOffBlink = 0;
#ifdef ALLIR_VER
                    TRISDbits.TRISD7 = 0;
#else
                    TRISDbits.TRISD6 = 0;
#endif
                    IO_RE2_SetHigh();
                }
                else if ((TurnOffCnt >= 0x11) && TurnOffCnt <= 0x18)
                {
                    TurnOffBlink = 1;
#ifdef ALLIR_VER
                    TRISDbits.TRISD7 = 1;
#else
                    TRISDbits.TRISD6 = 1;
#endif
                    IO_RE2_SetHigh();
                }
                else if ((TurnOffCnt >= 0x19) && TurnOffCnt <= 0x20)
                {
                    TurnOffBlink = 0;
#ifdef ALLIR_VER
                    TRISDbits.TRISD7 = 0;
#else
                    TRISDbits.TRISD6 = 0;
#endif
                    IO_RE2_SetHigh();
                }
                else if ((TurnOffCnt >= 0x21) && TurnOffCnt <= 0x28)
                {
                    TurnOffBlink = 1;
#ifdef ALLIR_VER
                    TRISDbits.TRISD7 = 1;
#else
                    TRISDbits.TRISD6 = 1;
#endif
                    IO_RE2_SetHigh();
                }
                else if (TurnOffCnt >= 0x29)
                {
#ifdef ALLIR_VER
                    TRISDbits.TRISD7 = 0;
#else
                    TRISDbits.TRISD6 = 0;
#endif
                    IO_RE2_SetLow();
                    kb = 0xd8;
                    while (kb == 0xd8)
                    {
                        kb = PORTB & 0xf0; // mask off buttons
                        kb = kb | ((PORTC & 0x03) << 2);
                    }
                    Nop();
                    Nop();
                    Nop();
                    Nop();
                    Reset();
                }
            }
        }
    }
}

void RestoreNavDim(void)
{
    uint16_t* l_pnt;
    uint8_t ind;

    if (DimB._Nav_Dim == 1)
    {
        DimCycle = 0;
        DimB._Nav_Dim = 0;
        DimB = DimB_Hold;
        l_pnt = (uint16_t*) LedRear_FullPWR;
        ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
        LEDState[0]._timer_timeout = l_pnt[ind];
        l_pnt = (uint16_t*) LedFront_FullPWR;
        ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
        LEDState[1]._timer_timeout = l_pnt[ind];
    }
}

void ServiceRFCmd(void)
{
    uint16_t* l_pnt, test;
    uint8_t ind, bb;

#ifdef  NAVLIGHT_VER
    if (rf_action._new_cmd)
    {
        rf_action._new_cmd = 0;
        WL_hold = 0;
        BOOT_hold = 0;
        BACK_hold = 0;
        FRONT_hold = 0;

        if (rf_action._just_paired)
        {
            rf_action._just_paired = 0;
            rf_action._RF_cmd = 0xff;
        }

        switch (rf_action._RF_cmd)
        {

#ifdef REDGREEN
        case 0x37:
        case 0x3d:
#else
        case 0x37:
        case 0x1f:
#endif
        {
            DimB._IR_Mode = 0;
            DimB._Cust_Mode = 0;
            DimB._SOS_Mode = 0;
            WL_hold_trip = 0;
            BRT_hold_trip = 0;
            BOOT_hold = 0x80;
            BOOT_hold_cnt = 0;
            DimB._SOS_Mode = 0;

            clean_up();
            DimB._GA_Mode = 0;
            LEDState[0]._active = 0;
            LEDState[1]._active = 0;
            LEDState[2]._active = 0;
            break;
        }

#ifdef REDGREEN
        case 0xb7:
        case 0xbd:
#else
        case 0xb7:
        case 0x9f:
#endif
        {
            DimB._IR_Mode = 0;
            DimB._Cust_Mode = 0;
            DimB._SOS_Mode = 0;
            WL_hold_trip = 0;
            BRT_hold_trip = 0;
            BOOT_hold = 0x80;
            BOOT_hold_cnt = 0;
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 1;

            //#ifdef REDGREEN
            if (!DimB._Nav_Dim)
            {
                DimB_Hold = DimB;
                DimB._Nav_Dim = 1;
            }
            DimB._DimCnt = 0;
            l_pnt = (uint16_t*) LedRear_FullPWR;
            ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
            LEDState[0]._timer_timeout = l_pnt[ind];
            l_pnt = (uint16_t*) LedFront_FullPWR;
            ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
            LEDState[1]._timer_timeout = l_pnt[ind];
            //#endif
            ForceButton = 0x80;
            init_button((uint8_t) 8, (uint8_t) 0);
            ForceButton = 0x80;
            init_button((uint8_t) 9, (uint8_t) 1);
            break;
        }

            /*
                    case 0x1f:
                    {
                        DimB._IR_Mode = 0;
                        DimB._Cust_Mode = 0;
                        if (DimB._SOS_Mode || DimB._GA_Mode)
                        {
                            LEDState[0]._active = 0;
                            LEDState[1]._active = 0;
                            DimB._SOS_Mode = 0;
                            DimB._GA_Mode = 0;
                            clean_up();

                        }
                        if (FRONT_hold_trip == 0x80)
                        {
                            BRT_hold_trip = 0;
                            FRONT_hold_trip = 0;
                            LEDState[0]._active = 0;
                            LEDState[1]._active = 0;
                        }
                        else
                        {
                            WL_hold_trip = 0;
                            BRT_hold_trip = 0;
                            BOOT_hold_trip = 0;
                            FRONT_hold_trip = 0;
                            DimB._SOS_Mode = 0;
                            DimB._IR_Mode = 0;
                            DimB._Cust_Mode = 0;
                            if ((LEDState[0]._button == 3) || (LEDState[0]._button == 5) || (LEDState[0]._button == 7))
                            {
                                DimB._GA_Mode = 0;
                                LEDState[0]._active = 0;
                                LEDState[0]._button = 0;
                                clean_up();
                            }
                            RestoreNavDim();
                            DimB._GA_Mode = 0;
                            init_button((uint8_t) 1, (uint8_t) 0);
                        }
                        break;
                    }
             */
            /*
                    case 0x3d:
                    {
                        DimB._IR_Mode = 0;
                        DimB._Cust_Mode = 0;
                        if (DimB._SOS_Mode || DimB._GA_Mode)
                        {
                            LEDState[0]._active = 0;
                            LEDState[1]._active = 0;
                            DimB._SOS_Mode = 0;
                            DimB._GA_Mode = 0;
                            clean_up();

                        }
                        if (BACK_hold_trip == 0x80)
                        {
                            BRT_hold_trip = 0;
                            BACK_hold_trip = 0;
                            LEDState[0]._active = 0;
                            LEDState[1]._active = 0;
                        }
                        else
                        {
                            WL_hold_trip = 0;
                            BRT_hold_trip = 0;
                            BOOT_hold_trip = 0;
                            BACK_hold_trip = 0;
                            DimB._SOS_Mode = 0;
                            DimB._IR_Mode = 0;
                            DimB._Cust_Mode = 0;

                            if ((LEDState[0]._button == 3) || (LEDState[0]._button == 5))
                            {
                                LEDState[0]._active = 0;
                                LEDState[1]._active = 0;
                                LEDState[0]._button = 0;
                                clean_up();
                            }
                            RestoreNavDim();
                            DimB._GA_Mode = 0;
                            init_button((uint8_t) 2, (uint8_t) 1);
                        }
                        break;
                    }
             */
        case 0x3e:
        {
            LEDState[2]._active = 0;
            TopHold = 0;
            if (((LEDState[0]._button == 3) || (LEDState[0]._button == 9)) && LEDState[0]._active)
            {
                TopHold = 1;
            }
            break;
        }

        case 0xbe:
        {
            DimB._IR_Mode = 0;
            DimB._Cust_Mode = 0;
            if (DimB._SOS_Mode)
            {
                DimB._SOS_Mode = 0;
                LEDState[1]._active = 0;
            }
            if (!LEDState[0]._active && !LEDState[1]._active)
            {
                clean_up();
            }
            if (((LEDState[0]._button == 8) || (LEDState[0]._button == 9)) && LEDState[0]._active)
            {
                TopHold = 1;
            }
            else
            {
                RestoreNavDim();
            }
            ForceButton = 0x80;
            init_button((uint8_t) 10, (uint8_t) 2);
            break;
        }

            /*
                    case 0x3b:
                    case 0xbb:
                    {
                        BRT_hold = 0x80;
                        BRT_hold_cnt = 0;
                        if ((LEDState[0]._active != 0) || (LEDState[1]._active != 0) || (LEDState[2]._active != 0))
                        {
                            BRT_hold = 0x80;
                            BRT_hold_cnt = 0;
                            RestoreNavDim();
                            if (!DimB._SOS_Mode)
                            {
                                BRT_hold_trip = 0;
                            }
                            DimB._DimCnt--;
                            if (DimB._DimCnt == 0xff)
                            {
                                DimB._DimCnt = 3;
                            }
                            DimB._IR_Mode = 0;
                            if (DimB._SOS_Mode)
                            {
                                l_pnt = (uint16_t*) LedRear_FullPWR;
                                ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
                                if (ind == 0)
                                {
                                    LEDState[0]._timer_timeout = l_pnt[ind] + 0x100;
                                }
                                else
                                {
                                    LEDState[0]._timer_timeout = l_pnt[ind];
                                }

                                l_pnt = (uint16_t*) LedFront_FullPWR;
                                ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
                                if (ind == 0)
                                {
                                    LEDState[1]._timer_timeout = l_pnt[ind] + 0x100;
                                }
                                else
                                {
                                    LEDState[1]._timer_timeout = l_pnt[ind];
                                }
                            }
                            else
                            {
                                l_pnt = (uint16_t*) LedRear_FullPWR;
                                ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
                                LEDState[0]._timer_timeout = l_pnt[ind];
                                l_pnt = (uint16_t*) LedFront_FullPWR;
                                ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
                                LEDState[1]._timer_timeout = l_pnt[ind];
                            }
                        }
                        break;
                    }
             */
        case 0x3b:
        case 0xbb:
        case 0xac:
        case 0xad:
        {
            if (rf_action._RF_cmd == 0x3b)
            {
                DimB._DimCnt = 3;
            }
            else if (rf_action._RF_cmd == 0xbb)
            {
                DimB._DimCnt = 2;
            }
            else if (rf_action._RF_cmd == 0xac)
            {
                DimB._DimCnt = 1;
            }
            else if (rf_action._RF_cmd == 0xad)
            {
                DimB._DimCnt = 0;
            }
            DimB._IR_Mode = 0;
            if (DimB._SOS_Mode)
            {
                l_pnt = (uint16_t*) LedRear_FullPWR;
                ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
                if (ind == 0)
                {
                    LEDState[0]._timer_timeout = l_pnt[ind] + 0x100;
                }
                else
                {
                    LEDState[0]._timer_timeout = l_pnt[ind];
                }

                l_pnt = (uint16_t*) LedFront_FullPWR;
                ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
                if (ind == 0)
                {
                    LEDState[1]._timer_timeout = l_pnt[ind] + 0x100;
                }
                else
                {
                    LEDState[1]._timer_timeout = l_pnt[ind];
                }
            }
            else
            {
                l_pnt = (uint16_t*) LedRear_FullPWR;
                ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
                LEDState[0]._timer_timeout = l_pnt[ind];
                l_pnt = (uint16_t*) LedFront_FullPWR;
                ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
                LEDState[1]._timer_timeout = l_pnt[ind];
            }
            break;
        }

        case 0xc4:
        {
            if (DimB._SOS_Mode)
            {
                LEDState[0]._active = 0;
                LEDState[1]._active = 0;
                DimB._SOS_Mode = 0;
                clean_up();
                BRT_hold_trip = 0;
                BRT_hold_cnt = 0;
                BRT_hold = 0;
            }
            break;
        }

        case 0x44:
        {
            RestoreNavDim();
            BRT_hold = 0x40;
            BRT_hold_cnt = 0;
            BRT_hold_trip = 0x80;
            DimB._MaxOutD = 0x01;
            BACK_hold_trip = 0;
            FRONT_hold_trip = 0;
            BOOT_hold_trip = 0;
            WL_hold_trip = 0;
            LEDState[0]._active = 0;
            LEDState[1]._active = 0;
            DimB._SOS_Mode = 1;
#ifdef LE_EXT_VER
            bb = 8;
#elif BUTTON_EXTEND
            bb = 9;
#else
            bb = 7;
#endif
            l_pnt = (uint16_t*) LedRear_FullPWR;
            ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
            if (ind == 0)
            {
                LEDState[0]._timer_timeout = l_pnt[ind] + 0x100;
            }
            else
            {
                LEDState[0]._timer_timeout = l_pnt[ind];
            }

            l_pnt = (uint16_t*) LedFront_FullPWR;
            ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
            if (ind == 0)
            {
                LEDState[1]._timer_timeout = l_pnt[ind] + 0x100;
            }
            else
            {
                LEDState[1]._timer_timeout = l_pnt[ind];
            }

            BRT_hold = 0x20;
            BACK_hold = 0x20;
            init_button((uint8_t) bb, (uint8_t) 0);
            BACK_hold = 0;
            BRT_hold = 0;
            break;
        }

        case 0xc8:
        {
            DimB._SOS_Mode = 0;
            clean_up();
            BRT_hold_trip = 0;
            BRT_hold_cnt = 0;
            BRT_hold = 0;
            RestoreNavDim();
            BOOT_hold_trip = 0x80;
            LEDState[0]._active = 0;
            LEDState[1]._active = 0;
            WL_hold = 0x44;
            init_button((uint8_t) 3, (uint8_t) 0);
            WL_hold = 0;
            BOOT_hold = 0;
            BOOT_hold_cnt = 0;
            break;
        }

        default:
        {
            Nop();
            Nop();
            Nop();
        }

        }
    }

#else
    if (rf_action._new_cmd)
    {
        rf_action._new_cmd = 0;
        WL_hold = 0;
        BOOT_hold = 0;
        BACK_hold = 0;
        FRONT_hold = 0;

        switch (rf_action._RF_cmd)
        {

        case 0x3d:
        {
            if (BRT_hold_trip || WL_hold_trip)
            {
                LEDState[0]._active = 0;
                BRT_hold_trip = 0;
                BACK_hold_trip = 0;
            }
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            clean_up();
            LEDState[1]._active = 0;
            break;
        }
        case 0xbd:
        {
            if (BRT_hold_trip || WL_hold_trip)
            {
                LEDState[0]._active = 0;
                BRT_hold_trip = 0;
                BACK_hold_trip = 0;
            }
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            clean_up();
            DimB._GA_Mode = 0;
            ForceButton = 0;
            if ((LEDState[0]._button == 3) && LEDState[0]._active)
            {
                LEDState[0]._active = 0;
            }
            init_button((uint8_t) 2, (uint8_t) 1);
            break;
        }
        case 0xb5:
        {
            if (BRT_hold_trip || WL_hold_trip)
            {
                LEDState[0]._active = 0;
                BRT_hold_trip = 0;
                BACK_hold_trip = 0;
            }
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            clean_up();
            DimB._GA_Mode = 0;
            ForceButton = 0x81;
            init_button((uint8_t) 2, (uint8_t) 1);
            break;
        }
        case 0xb6:
        {
            if (BRT_hold_trip || WL_hold_trip)
            {
                LEDState[0]._active = 0;
                BRT_hold_trip = 0;
                BACK_hold_trip = 0;
            }
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            clean_up();
            DimB._GA_Mode = 0;
            ForceButton = 0x82;
            init_button((uint8_t) 2, (uint8_t) 1);
            break;
        }

        case 0x1f:
        {
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            clean_up();
            LEDState[0]._active = 0;
            break;
        }
        case 0x9f:
        {
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            clean_up();
            DimB._GA_Mode = 0;
            ForceButton = 0;
            init_button((uint8_t) 1, (uint8_t) 0);
            break;
        }
        case 0xa0:
        {
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            clean_up();
            DimB._GA_Mode = 0;
            ForceButton = 0x81;
            init_button((uint8_t) 1, (uint8_t) 0);
            break;
        }
        case 0xa1:
        {
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            clean_up();
            DimB._GA_Mode = 0;
            ForceButton = 0x82;
            init_button((uint8_t) 1, (uint8_t) 0);
            break;
        }

        case 0x3e:
        {
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            //            clean_up();
            LEDState[2]._active = 0;
            break;
        }
        case 0xbe:
        {
            DimB._IR_Mode = 0;
            DimB._Cust_Mode = 0;
            if (DimB._SOS_Mode)
            {
                DimB._SOS_Mode = 0;
                LEDState[1]._active = 0;
            }
            ForceButton = 0;
            if ((LEDState[0]._button == 3) && LEDState[0]._active)
            {
                TopHold = 1;
            }
            init_button((uint8_t) 10, (uint8_t) 2);
            break;
        }

        case 0x37:
        {
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            DimB._SOS_Mode = 0;
            clean_up();
            LEDState[0]._active = 0;
            LEDState[1]._active = 0;
            LEDState[2]._active = 0;
            break;
        }
        case 0xb7:
        {
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            DimB._SOS_Mode = 0;
            clean_up();
            LEDState[1]._active = 0;
            LEDState[2]._active = 0;
            ForceButton = 0;
            init_button((uint8_t) 3, (uint8_t) 0);
            break;
        }
        case 0xb8:
        {
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            DimB._SOS_Mode = 0;
            clean_up();
            ForceButton = 0x82;
            init_button((uint8_t) 1, (uint8_t) 0);
            ForceButton = 0x82;
            init_button((uint8_t) 2, (uint8_t) 1);
            break;
        }

        case 0x2f:
        {
            if (BRT_hold_trip || WL_hold_trip)
            {
                LEDState[1]._active = 0;
                BRT_hold_trip = 0;
                BACK_hold_trip = 0;
            }
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            DimB._SOS_Mode = 0;
            LEDState[0]._active = 0;
            break;
        }
        case 0xaf:
        {
            if (BRT_hold_trip || WL_hold_trip)
            {
                LEDState[1]._active = 0;
                BRT_hold_trip = 0;
                BACK_hold_trip = 0;
            }
            DimB._SOS_Mode = 0;
            DimB._GA_Mode = 0;
            DimB._SOS_Mode = 0;
            ForceButton = 0;
            init_button((uint8_t) 4, (uint8_t) 0);
            break;
        }

        case 0x3b:
        case 0xbb:
        case 0xac:
        case 0xad:
        {
            if (rf_action._RF_cmd == 0x3b)
            {
                DimB._DimCnt = 3;
            }
            else if (rf_action._RF_cmd == 0xbb)
            {
                DimB._DimCnt = 2;
            }
            else if (rf_action._RF_cmd == 0xac)
            {
                DimB._DimCnt = 1;
            }
            else if (rf_action._RF_cmd == 0xad)
            {
                DimB._DimCnt = 0;
            }
            DimB._IR_Mode = 0;
            if (DimB._SOS_Mode)
            {
                l_pnt = (uint16_t*) LedRear_FullPWR;
                ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
                if (ind == 0)
                {
                    LEDState[0]._timer_timeout = l_pnt[ind] + 0x100;
                }
                else
                {
                    LEDState[0]._timer_timeout = l_pnt[ind];
                }

                l_pnt = (uint16_t*) LedFront_FullPWR;
                ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
                if (ind == 0)
                {
                    LEDState[1]._timer_timeout = l_pnt[ind] + 0x100;
                }
                else
                {
                    LEDState[1]._timer_timeout = l_pnt[ind];
                }
            }
            else
            {
                l_pnt = (uint16_t*) LedRear_FullPWR;
                ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
                LEDState[0]._timer_timeout = l_pnt[ind];
                l_pnt = (uint16_t*) LedFront_FullPWR;
                ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
                LEDState[1]._timer_timeout = l_pnt[ind];
            }
            break;
        }

        case 0xc4:
        {
            if (DimB._SOS_Mode)
            {
                LEDState[0]._active = 0;
                LEDState[1]._active = 0;
                DimB._SOS_Mode = 0;
                clean_up();
                BRT_hold_trip = 0;
                BRT_hold_cnt = 0;
                BRT_hold = 0;
            }
            break;
        }

        case 0x44:
        {
            RestoreNavDim();
            BRT_hold = 0x40;
            BRT_hold_cnt = 0;
            BRT_hold_trip = 0x80;
            DimB._MaxOutD = 0x01;
            BACK_hold_trip = 0;
            FRONT_hold_trip = 0;
            BOOT_hold_trip = 0;
            WL_hold_trip = 0;
            LEDState[0]._active = 0;
            LEDState[1]._active = 0;
            DimB._SOS_Mode = 1;
#ifdef LE_EXT_VER
            bb = 8;
#elif BUTTON_EXTEND
            bb = 9;
#else
            bb = 7;
#endif
            l_pnt = (uint16_t*) LedRear_FullPWR;
            ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
            if (ind == 0)
            {
                LEDState[0]._timer_timeout = l_pnt[ind] + 0x100;
            }
            else
            {
                LEDState[0]._timer_timeout = l_pnt[ind];
            }

            l_pnt = (uint16_t*) LedFront_FullPWR;
            ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
            if (ind == 0)
            {
                LEDState[1]._timer_timeout = l_pnt[ind] + 0x100;
            }
            else
            {
                LEDState[1]._timer_timeout = l_pnt[ind];
            }

            BRT_hold = 0x20;
            BACK_hold = 0x20;
            init_button((uint8_t) bb, (uint8_t) 0);
            BACK_hold = 0;
            BRT_hold = 0;
            break;
        }

        case 0xd0:
        {
            RestoreNavDim();
            BRT_hold = 0;
            BRT_hold_cnt = 0;
            BRT_hold_trip = 0;
            BACK_hold_trip = 0;
            FRONT_hold_trip = 0;
            BOOT_hold_trip = 0;
            WL_hold_trip = 0x80;

            WL_hold = 0x00;
            init_button((uint8_t) 1, (uint8_t) 0);
            WL_hold = 0x40;
            init_button((uint8_t) 2, (uint8_t) 1);
            WL_hold = 0x00;
            break;
        }

        case 0xc0:
        {
            LEDState[0]._active = 0;
            LEDState[1]._active = 0;
            LEDState[2]._active = 0;
            clean_up();
            IO_RE2_SetLow();
            PgmStat = 0;
            ForceButton = 0x82;
            init_button((uint8_t) 7, (uint8_t) 0);
            DimB._Bat_Status = 1;
            break;
        }

        default:
        {
            Nop();
            Nop();
            Nop();
        }

        }
    }
#endif
}

void ServiceCmd(void)
{
    uint16_t* l_pnt, test;
    uint8_t bb, ind, work;

    if (KeyStatus._new_cmd)
    {
        KeyStatus._new_cmd = 0;
        WL_hold = 0;
        BOOT_hold = 0;
        BACK_hold = 0;
        FRONT_hold = 0;

        switch (Key._cmd)
        {
        case 0xD8:
        {
            if (!DimB._German_IR)
            {
                TurnOff = 1;
                //                KeyChangeLock = 0x40;
                ModeChangeCnt = 0;
                LEDState[0]._active = 0;
                LEDState[1]._active = 0;
                if (TimerD._RF_Active)
                {
                    TRISB = 0xf5; // lower outputs except MISO, INT
                }
                else
                {
                    TRISB = 0xf0; // lower outputs
                }
                TRISD = 0x00; // all outputs
            }
            break;
        }
#ifdef  NAVLIGHT_VER
        case 0x6c:
        {
            PgmStat = 0x80;
            PgmStatCnt = 0;
            LEDState[0]._active = 0;
            LEDState[1]._active = 0;
            if (TimerD._RF_Active)
            {
                TRISB = 0xf5; // lower outputs except MISO, INT
            }
            else
            {
                TRISB = 0xf0; // lower outputs
            }
            TRISD = 0x00; // all outputs
            break;
        }
#else
        case 0x78:
        {
            PgmStat = 0x80;
            PgmStatCnt = 0;
            remote_model = 0;
            LEDState[0]._active = 0;
            LEDState[1]._active = 0;
            if (TimerD._RF_Active)
            {
                TRISB = 0xf5; // lower outputs except MISO, INT
            }
            else
            {
                TRISB = 0xf0; // lower outputs
            }
            TRISD = 0x00; // all outputs
            KeyChangeLock = 0x20;
            break;
        }

        case 0x5c:
        {
            PgmStat = 0x80;
            PgmStatCnt = 0;
            remote_model = 0x80;
            LEDState[0]._active = 0;
            LEDState[1]._active = 0;
            if (TimerD._RF_Active)
            {
                TRISB = 0xf5; // lower outputs except MISO, INT
            }
            else
            {
                TRISB = 0xf0; // lower outputs
            }
            TRISD = 0x00; // all outputs
            KeyChangeLock = 0x10;
            break;
        }
#endif

        case 0xf8:
        {
#ifdef FCC_MODE
            if (!DimB._German_IR && !(fcc_test & 0x80))
#else
            if (!DimB._German_IR)
#endif
            {
                DimB._IR_Mode = 0;
                DimB._Cust_Mode = 0;
                if (DimB._SOS_Mode)
                {
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                    DimB._SOS_Mode = 0;
                    clean_up();
                }
                if (BACK_hold_trip == 0x80)
                {
                    BRT_hold_trip = 0;
                    BACK_hold_trip = 0;
                    DimB = DimB_Hold;
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                }
                else if (PgmStat != 0x81)
                {
                    if (!Double_Tap._rear_dt_inprog)
                    {
                        //			Double_Tap._rear_dt_inprog = 1;
                        Double_Tap._rear_dt_inprog = 0;
                        Double_Tap._rear_counter = 0;
                    }
                    else
                    {
                        if (Double_Tap._rear_counter <= 0x400)
                        {
                            Double_Tap._rear_counter = 0;
                            //				Double_Tap._rear_dt_fire = 1;
                        }
                        Double_Tap._rear_dt_inprog = 0;
                    }
                    WL_hold_trip = 0;
                    BRT_hold_trip = 0;
                    BOOT_hold_trip = 0;
                    FRONT_hold_trip = 0;
                    BACK_hold = 0x80;
                    BACK_hold_cnt = 0;
                    if (DimB._SOS_Mode || DimB._GA_Mode)
                    {
                        DimB._SOS_Mode = 0;
                        DimB._GA_Mode = 0;
                        clean_up();
                        LEDState[0]._active = 0;
                        LEDState[1]._active = 0;
                    }

                    if ((LEDState[0]._button == 3) || (LEDState[0]._button == 5))
                    {
                        LEDState[0]._active = 0;
                        LEDState[1]._active = 0;
                        LEDState[0]._button = 0;
                        clean_up();
                    }
                    if (Double_Tap._rear_dt_fire)
                    {
                        Double_Tap._rear_dt_fire = 0;
                        LEDState[1]._button_press = 1;

                    }
                    else
                    {
                        if ((LEDState[1]._button == 2) && (LEDState[1]._button_press == 2))
                        {
                            Double_Tap._rear_dt_inprog = 0;
                        }
                    }
                    RestoreNavDim();
                    DimB._GA_Mode = 0;
                    init_button((uint8_t) 2, (uint8_t) 1);
                }
            }
#ifdef FCC_MODE
            if (fcc_test & 0x80)
            {
                if (fcc_power > 0)
                {
                    fcc_power--;
                    ;
                }
                if (fcc_test & 0x01)
                {
                    fcc_test = 0x80;
                    SI241_SetuptxResp();
                    SI241_SetStandby();
                    fcc_test = 0x81;

                    si24_on_timer = 50; // set to 50 * 0.001 = to use as delay
                    fcc_channel = 0x40;
                    SI241_SetuptxResp();
                    SI241_SetTxResp();
                }

                bb = 5;
                work = fcc_power / 2;
                if (work >= 0x3)
                {
                    bb++;
                    work = work - 3;
                }
                DimB_Hold = DimB;
                DimB._Bat_Status = 1;
                ForceButton = work | 0x80;
                init_button((uint8_t) bb, (uint8_t) 1);
            }
#endif
            break;
        }

        case 0xdc:
        {
#ifdef FCC_MODE
            if (!DimB._German_IR && !(fcc_test & 0x80))
#else
            if (!DimB._German_IR)
#endif
            {
                DimB._IR_Mode = 0;
                DimB._Cust_Mode = 0;
                if (DimB._SOS_Mode)
                {
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                    DimB._SOS_Mode = 0;
                    clean_up();
                }
                if (FRONT_hold_trip == 0x80)
                {
                    BRT_hold_trip = 0;
                    FRONT_hold_trip = 0;
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                }
                else if (PgmStat != 0x81)
                {
                    if (!Double_Tap._front_dt_inprog)
                    {
                        //			Double_Tap._front_dt_inprog = 1;
                        Double_Tap._front_dt_inprog = 0;
                        Double_Tap._front_counter = 0;
                    }
                    else
                    {
                        if (Double_Tap._front_counter <= 0x400)
                        {
                            Double_Tap._front_counter = 0;
                            //				Double_Tap._front_dt_fire = 1;
                        }
                        Double_Tap._front_dt_inprog = 0;
                    }

                    WL_hold_trip = 0;
                    BRT_hold_trip = 0;
                    BOOT_hold_trip = 0;
                    FRONT_hold_trip = 0;
                    DimB._SOS_Mode = 0;
                    DimB._IR_Mode = 0;
                    DimB._Cust_Mode = 0;
                    FRONT_hold = 0x80;
                    FRONT_hold_cnt = 0;
                    if ((LEDState[0]._button == 3) || (LEDState[0]._button == 5) || (LEDState[0]._button == 7))
                    {
                        DimB._GA_Mode = 0;
                        LEDState[0]._active = 0;
                        LEDState[0]._button = 0;
                        clean_up();
                    }
                    if (Double_Tap._front_dt_fire)
                    {
                        Double_Tap._front_dt_fire = 0;
                        LEDState[0]._button_press = 1;
                        LEDState[0]._button = 1;
                        LEDState[0]._active = 1;
                    }
                    else
                    {
                        if ((LEDState[0]._button == 1) && (LEDState[0]._button_press == 2))
                        {
                            Double_Tap._front_dt_inprog = 0;
                        }
                    }
                    RestoreNavDim();
                    DimB._GA_Mode = 0;
                    init_button((uint8_t) 1, (uint8_t) 0);
                }
            }
#ifdef FCC_MODE
            if (fcc_test & 0x80)
            {
                fcc_power++;
                if (fcc_power >= 0x07)
                {
                    fcc_power = 0x07;
                }
                if (fcc_test & 0x01)
                {
                    fcc_test = 0x80;
                    SI241_SetuptxResp();
                    SI241_SetStandby();
                    fcc_test = 0x81;

                    si24_on_timer = 50; // set to 50 * 0.001 = to use as delay
                    fcc_channel = 0x40;
                    SI241_SetuptxResp();
                    SI241_SetTxResp();
                }

                bb = 5;
                work = fcc_power / 2;
                if (work >= 0x3)
                {
                    bb++;
                    work = work - 3;
                }
                DimB_Hold = DimB;
                DimB._Bat_Status = 1;
                ForceButton = work | 0x80;
                init_button((uint8_t) bb, (uint8_t) 1);
            }
#endif
            break;
        }
        case 0x7c: // GA Button Handler
        {
            DimB._IR_Mode = 0;
            DimB._Cust_Mode = 0;
            DimB._SOS_Mode = 0;
            if (BOOT_hold_trip == 0x80)
            {
                BOOT_hold_trip = 0;
                BRT_hold_trip = 0;
                LEDState[0]._active = 0;
                LEDState[1]._active = 0;
                LEDState[2]._active = 0;
            }
            else if ((PgmStat == 0x81) || (PgmStat == 0x80))
            {
                PgmStat = 0;
                DimB = DimB_Hold;
                LEDState[0]._active = 0;
                LEDState[1]._active = 0;
            }
            else
            {
                WL_hold_trip = 0;
                BRT_hold_trip = 0;
                BOOT_hold = 0x80;
                BOOT_hold_cnt = 0;
                DimB._SOS_Mode = 0;
#ifdef REDBLUE_SPEC_VER_ONE
                DimB._Cust_Mode = 1;
#endif
                if (DimB._IR_Mode)
                {
                    DimB._IR_Mode = 0;
                    LEDState[0]._active = 0;
                    LEDState[0]._button = 0;
                }
                if (DimB._German_IR)
                {
                    DimB._German_IR = 0;
                    LEDState[0]._active = 0;
                    LEDState[0]._button = 0;
                }
#ifdef BUTTON_EXTEND
                if (LEDState[0]._active == 0)
                {
                    init_button((uint8_t) 5, (uint8_t) 0);
                    init_button((uint8_t) 6, (uint8_t) 1);
                }
                else
                {
                    if (LEDState[0]._button != 0x05)
                    {
                        clean_up();
                        init_button((uint8_t) 5, (uint8_t) 0);
                        init_button((uint8_t) 6, (uint8_t) 1);
                        LEDState[0]._holdoff = 0;
                        LEDState[1]._holdoff = 0;
                    }
                    else
                    {
#ifdef WHITEREDWHITERED_SPEC
                        init_button((uint8_t) 5, (uint8_t) 0);
                        init_button((uint8_t) 6, (uint8_t) 1);
#elif WHITERED_SPEC
                        init_button((uint8_t) 5, (uint8_t) 0);
                        init_button((uint8_t) 6, (uint8_t) 1);
#elif YELLOWYELLOW_MNDOT
                        init_button((uint8_t) 5, (uint8_t) 0);
                        init_button((uint8_t) 6, (uint8_t) 1);
#else
                        clean_up();
                        init_button((uint8_t) 5, (uint8_t) 0);
                        LEDState[1]._active = 0; // off everything else if Hillary mode
#endif
                    }
                }
#else
#ifdef NAVLIGHT_VER
                if (((LEDState[0]._button == 8) && LEDState[0]._active) && ((LEDState[1]._button == 9) && LEDState[1]._active))
                {
                    clean_up();
                    DimB._GA_Mode = 1;
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                    RestoreNavDim();
                    WL_hold = 0x42;
                    init_button((uint8_t) 1, (uint8_t) 0);
                    WL_hold = 0x42;
                    init_button((uint8_t) 2, (uint8_t) 1);
                    WL_hold = 0;
                    //                    init_button((uint8_t) 3, (uint8_t) 0);
                }
                else if (((LEDState[0]._button == 1) && (LEDState[0]._button_press == 1) && LEDState[0]._active) && ((LEDState[1]._button == 2) && (LEDState[1]._button_press == 1) && LEDState[1]._active))
                {
                    clean_up();
                    DimB._GA_Mode = 0;
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                    LEDState[2]._active = 0;
                    //                    init_button((uint8_t) 3, (uint8_t) 0);
                }
                else
                {
                    DimB._GA_Mode = 1;
                    //#ifdef REDGREEN
                    DimB_Hold = DimB;
                    DimB._Nav_Dim = 1;
                    DimB._DimCnt = 0;
                    l_pnt = (uint16_t*) LedRear_FullPWR;
                    ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
                    LEDState[0]._timer_timeout = l_pnt[ind];
                    l_pnt = (uint16_t*) LedFront_FullPWR;
                    ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
                    LEDState[1]._timer_timeout = l_pnt[ind];
                    //#endif
                    init_button((uint8_t) 8, (uint8_t) 0);
                    init_button((uint8_t) 9, (uint8_t) 1);
                }
#elif AMAZON_VER
                if (((LEDState[0]._button == 8) && LEDState[0]._active) && ((LEDState[1]._button == 9) && LEDState[1]._active))
                {
                    clean_up();
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                    RestoreNavDim();
                    DimB._GA_Mode = 0;
                    //                    init_button((uint8_t) 3, (uint8_t) 0);
                }
                else
                {
                    DimB_Hold = DimB;
                    DimB._Nav_Dim = 1;
                    DimB._DimCnt = 3;
                    l_pnt = (uint16_t*) LedRear_FullPWR;
                    ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
                    LEDState[0]._timer_timeout = l_pnt[ind];
                    l_pnt = (uint16_t*) LedFront_FullPWR;
                    ind = 3 - DimB._DimCnt + LEDState[1]._Delay + 1;
                    LEDState[1]._timer_timeout = l_pnt[ind];
                    DimCycle = 0x80;
                    init_button((uint8_t) 8, (uint8_t) 0);
                    init_button((uint8_t) 9, (uint8_t) 1);
                }
#else
                LEDState[1]._active = 0; // off everything else if Hillary mode
                LEDState[2]._active = 0;
                init_button((uint8_t) 3, (uint8_t) 0);
#endif
#endif
            }

#ifdef FCC_MODE
            if (fcc_test == 0x80)
            {
                IO_RB0_SetDigitalInput(); // Set MISO as INPUT
                IO_RB2_SetDigitalInput(); // Set IRQ to input
                SPI1_Initialize(); // Enable SPI after power up
                INTCON3bits.INT2IF = 0; // Enable INT from RF chip to Ext Int 2 : Clear Flag
                INTCON2bits.INTEDG2 = 0; // Set to falling edge
                INTCON3bits.INT2IE = 1; // Enable
                SSP1CON1bits.SSPEN = 1;

                SI241_ClearRxFifo();
                SI241_SetStandby();
                si24_on_timer = 50; // set to 50 * 0.001 = to use as delay
                while (si24_on_timer > 0)
                {
                    if (TimerD._window)
                    {
                        Nop();
                        Nop();
                        Nop();
                        TimerD._window = 0;
                        si24_on_timer--;
                    }
                }
                fcc_channel = 0x40;
                SI241_SetuptxResp();
                SI241_SetTxResp();
                while (!TimerD._new_tx)
                {
                    Nop();
                    Nop();
                    Nop();
                }

                TimerD._new_tx = 0;
                SI241_SetStandby();
            }
#endif
            break;
        }

        case 0xbc:
        {
            BRT_hold = 0x80;
            BRT_hold_cnt = 0;
            if ((LEDState[0]._active != 0) || (LEDState[1]._active != 0) || (LEDState[2]._active != 0))
            {
                BRT_hold = 0x80;
                BRT_hold_cnt = 0;
                if (!DimB._SOS_Mode)
                {
                    BRT_hold_trip = 0;
                }
#ifdef GERMAN_VER
                if (DimB._DimCnt == 3)
                {
                    DimB._DimCnt = 0;
                }
                else if (DimB._DimCnt == 0)
                {
                    DimB._DimCnt = 1;
                }
                else
                {
                    DimB._DimCnt = 3;
                }
#else
                DimB._DimCnt--;
                if (DimB._DimCnt == 0xff)
                {
                    DimB._DimCnt = 3;
                }
#endif
                DimB._IR_Mode = 0;
                if (DimB._SOS_Mode)
                {
                    l_pnt = (uint16_t*) LedRear_FullPWR;
                    ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
                    if (ind == 0)
                    {
                        LEDState[0]._timer_timeout = l_pnt[ind] + 0x100;
                    }
                    else
                    {
                        LEDState[0]._timer_timeout = l_pnt[ind];
                    }

                    l_pnt = (uint16_t*) LedFront_FullPWR;
                    ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
                    if (ind == 0)
                    {
                        LEDState[1]._timer_timeout = l_pnt[ind] + 0x100;
                    }
                    else
                    {
                        LEDState[1]._timer_timeout = l_pnt[ind];
                    }
                }
                else
                {
                    l_pnt = (uint16_t*) LedRear_FullPWR;
                    ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
                    LEDState[0]._timer_timeout = l_pnt[ind];
                    l_pnt = (uint16_t*) LedFront_FullPWR;
                    ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
                    LEDState[1]._timer_timeout = l_pnt[ind];
                }
            }
            break;
        }
        case 0xec:
        {
            if (!DimB._German_IR)
            {

                DimB._IR_Mode = 0;
                DimB._Cust_Mode = 0;
                if (DimB._SOS_Mode)
                {
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                    DimB._SOS_Mode = 0;
                    clean_up();
                }
                if (WL_hold_trip == 0x80)
                {
                    WL_hold_trip = 0;
                    BRT_hold_trip = 0;
                    BOOT_hold_trip = 0;
                    FRONT_hold_trip = 0;
                    clean_up();
                    LEDState[0]._active = 0;
                    LEDState[1]._active = 0;
                }
#ifdef FCC_MODE
                else if ((PgmStat != 0x81) && (fcc_test == 0))
#else
                else if (PgmStat != 0x81)
#endif
                {
                    WL_hold = 0x80;
                    WL_hold_cnt = 0;
                    BRT_hold_trip = 0;
                    BOOT_hold_trip = 0;
                    FRONT_hold_trip = 0;
                    if (DimB._SOS_Mode)
                    {
                        DimB._SOS_Mode = 0;
                        clean_up();
                        LEDState[0]._active = 0;
                        LEDState[1]._active = 0;
                    }
                    DimB._SOS_Mode = 0;
                    DimB._IR_Mode = 0;
                    DimB._Cust_Mode = 0;
                    if (LEDState[0]._button == 3)
                    {
                        LEDState[0]._active = 0;
                        clean_up();
                    }
                    else if (LEDState[0]._button == 8)
                    {
                        LEDState[1]._active = 0;
                        clean_up();
                    }
                    else if (LEDState[1]._button == 2)
                    {
                        clean_up();
                    }
                    RestoreNavDim();
                    init_button((uint8_t) 4, (uint8_t) 0);
                }
#ifdef FCC_MODE
                if ((PgmStat == 0x81) || (fcc_test & 0x80))
                {
                    PgmStat = 0;
                    if (!(fcc_test & 0x01))
                    {
                        fcc_test = 0x81;
                        clean_up();
                        LEDState[0]._active = 0;
                        LEDState[1]._active = 0;
                        LEDState[2]._active = 0;
                        ForceButton = 0x81;
                        init_button((uint8_t) 10, (uint8_t) 2);
                        SI241_ClearRxFifo();
                        SI241_SetStandby();
                        si24_on_timer = 50; // set to 50 * 0.001 = to use as delay
                        while (si24_on_timer > 0)
                        {
                            if (TimerD._window)
                            {
                                Nop();
                                Nop();
                                Nop();
                                TimerD._window = 0;
                                si24_on_timer--;
                            }
                        }
                        fcc_power = 0;
                        fcc_channel = 0x40;
                        SI241_SetuptxResp();
                        SI241_SetTxResp();
                    }
                    else
                    {
                        fcc_test = 0x80;
                        clean_up();
                        LEDState[0]._active = 0;
                        LEDState[1]._active = 0;
                        LEDState[2]._active = 0;
                        SI241_SetuptxResp();
                        SI241_SetStandby();
                    }
                }
#endif
            }
            break;
        }

        case 0xf4:
        {
            DimB._IR_Mode = 0;
            DimB._Cust_Mode = 0;
            if (DimB._SOS_Mode)
            {
                DimB._SOS_Mode = 0;
                LEDState[1]._active = 0;
            }
            else if (PgmStat != 0x81)
            {
                if (!LEDState[0]._active && !LEDState[1]._active)
                {
                    clean_up();
                }
                if (((LEDState[0]._button == 3) || (LEDState[0]._button == 9)) && LEDState[0]._active)
                {
                    TopHold = 1;
                }
                init_button((uint8_t) 10, (uint8_t) 2);
            }
            break;
        }

        }
    }
}

void clean_up(void)
{
    LEDState[0]._tris_a_contrib = 0;
    LEDState[0]._tris_d_contrib = 0;
    LEDState[1]._tris_a_contrib = 0;
    LEDState[1]._tris_d_contrib = 0;
    LEDState[2]._tris_a_contrib = 0;
    LEDState[2]._tris_d_contrib = 0;
    DimB._MaxOutR = 0;
    DimB._MaxOutF = 0;
    DimB._MaxOutWL = 0;
    DimB._MaxOutD = 0;
    DimB._Bat_Status = 0;

    if (TimerD._RF_Active)
    {
        TRISB = 0xf5; // lower outputs except MISO, INT
    }
    else
    {
        TRISB = 0xf0; // lower outputs
    }

    if (!TurnOffBlink)
    {
        TRISD = 0;
    }
    LEDState[0]._holdoff = 0;
    LEDState[1]._holdoff = 0;
    LEDState[2]._holdoff = 0;
    DimCycle = 0;
    TopHold = 0;
}

void init_button(uint8_t button, uint8_t pos)
{
    uint16_t i, j, k;
    uint16_t* l_pnt;
    uint8_t ind;

    k = button - 1;
    j = Button_Press[k]; // get the number of functions per button press

    if (ForceButton & 0x80)
    {
        LEDState[pos]._button = button;
        LEDState[pos]._button_press = ForceButton & 0x3;
        LEDState[pos]._active = 1;
        ForceButton = 0;
    }
    else if (WL_hold == 0x40)
    {
        i = j - 2;
        LEDState[pos]._button_press = i;
        LEDState[pos]._button = button;
        LEDState[pos]._active = 1;
    }
    else if (WL_hold == 0x41)
    {
        LEDState[pos]._button_press = 2;
        LEDState[pos]._button = button;
        LEDState[pos]._active = 1;
    }
    else if (WL_hold == 0x42)
    {
        LEDState[pos]._button_press = 1;
        LEDState[pos]._button = button;
        LEDState[pos]._active = 1;
    }
    else if (WL_hold == 0x43)
    {
        LEDState[pos]._button_press = 1;
        LEDState[pos]._button = button;
        LEDState[pos]._active = 1;
    }
    else if (WL_hold == 0x44)
    {
        LEDState[pos]._button_press = 1;
        LEDState[pos]._button = button;
        LEDState[pos]._active = 1;
    }
    else if (WL_hold == 0x45)
    {
        LEDState[pos]._button_press = 2;
        LEDState[pos]._button = button;
        LEDState[pos]._active = 1;
    }
    else if (WL_hold == 0x46)
    {
        LEDState[pos]._button_press = 2;
        LEDState[pos]._button = button;
        LEDState[pos]._active = 1;
    }
    else if (BRT_hold == 0x41)
    {
        LEDState[pos]._button_press = 1;
        LEDState[pos]._button = button;
        LEDState[pos]._active = 1;
    }
    else if (FRONT_hold == 0x41)
    {
        LEDState[pos]._button_press = 1;
        LEDState[pos]._button = button;
        LEDState[pos]._active = 1;
    }
    else if (FRONT_hold == 0x42)
    {
        LEDState[pos]._button_press = 1;
        LEDState[pos]._button = button;
        LEDState[pos]._active = 1;
    }
        //    else if (BACK_hold == 0x21)
        //    {
        //	LEDState[pos]._button_press = 1;
        //	LEDState[pos]._button = button;
        //	LEDState[pos]._active = 1;
        //    }

    else if ((LEDState[pos]._button == button) && LEDState[pos]._active)
    {
        i = LEDState[pos]._button_press;
        i++;
        if (i >= j)
        {
            LEDState[pos]._button_press = 0;
            LEDState[pos]._active = 0;
            LEDState[pos]._tris_a_contrib = 0;
            LEDState[pos]._tris_d_contrib = 0;
        }
        else
        {
            LEDState[pos]._button_press = i;
        }
    }
    else if ((LEDState[pos]._button != button) && LEDState[pos]._active)
    {
        LEDState[pos]._button = button;
        LEDState[pos]._button_press = 0;
    }
    else
    {
        LEDState[pos]._button = button;
        LEDState[pos]._button_press = 0;
        LEDState[pos]._active = 1;
    }

    if (LEDState[pos]._active)
    {
        LEDState[pos]._LEDScan = 0;
        k = k * 3;
        k = k + LEDState[pos]._button_press; // three entries per button offset by a button press
        if (BACK_hold & 0x20)
        {
            k = k + (BACK_hold & 0x03);
        }
        j = Led_size[k];
        LEDState[pos]._sequence_len = (uint8_t) j;
        l_pnt = (uint16_t*) Led_Point[k];
        LEDState[pos]._program = l_pnt;
        LEDState[pos]._ticreload = l_pnt[0];
        LEDState[pos]._ticcnt = 0;
        LEDState[pos]._offset = 0;
        LEDState[pos]._Delay = 0;

        if (pos == 0)
        {
            l_pnt = (uint16_t*) LedRear_FullPWR;
            ind = 3 - DimB._DimCnt + LEDState[0]._Delay;
            LEDState[0]._timer_timeout = l_pnt[ind];

            if (LEDState[1]._active && (LEDState[1]._button == 2) && (LEDState[1]._button_press >= 1))
            {
                LEDState[pos]._holdoff = 1;
            }
            else
            {
                LEDState[pos]._holdoff = 0;
            }
        }
        else if (pos == 1)
        {
            l_pnt = (uint16_t*) LedFront_FullPWR;
            ind = 3 - DimB._DimCnt + LEDState[1]._Delay;
#ifdef AMAZON_VER
            if (DimCycle & 0x80)
            {
                ind++;
            }
#endif
            LEDState[1]._timer_timeout = l_pnt[ind];

            if (LEDState[0]._active && (LEDState[0]._button == 1) && (LEDState[0]._button_press >= 1))
            {
                LEDState[pos]._holdoff = 1;
            }
            else
            {
                LEDState[pos]._holdoff = 0;
            }
        }
    }
}

void ServiceLEDs(uint8_t pos)
{
    uint16_t* l_pnt;
    uint8_t point, maxo, multiple, test, trisa_work, trisd_work, ind;
    UintByte work;

    if (LEDState[pos]._active && !LEDState[pos]._holdoff)
    {
        if (LEDState[pos]._LEDScan == 0)
        {
            l_pnt = LEDState[pos]._program;
            LEDState[pos]._LEDframe = 0;
            point = LEDState[pos]._offset + 1;
            LEDState[pos]._LedMask._int[0] = l_pnt[point];
            LEDState[pos]._LedMask._int[1] = 0;
            point++;
            work._int = l_pnt[point];
            LEDState[pos]._LEDactive = work._byte[0] & 0x0f;
            LEDState[pos]._Delay = work._byte[1] & 0x03;
#ifdef LE_VER
            if ((LEDState[1]._button == 4) && (LEDState[1]._button_press == 0) && (DimB._DimCnt == 2))
            {
                LEDState[pos]._Delay--;
                LEDState[pos]._Delay--;
            }
#endif
#ifdef LE_HYBRID_VER
#ifdef	LE_HYBRID_SPEC_VER_ONE
            if ((LEDState[1]._button == 4) && (LEDState[1]._button_press == 1) && (DimB._DimCnt == 2))
            {
                LEDState[pos]._Delay--;
                LEDState[pos]._Delay--;
            }
#else
            if ((LEDState[1]._button == 4) && (LEDState[1]._button_press == 0) && (DimB._DimCnt == 2))
            {
                LEDState[pos]._Delay--;
                LEDState[pos]._Delay--;
            }
#endif
#endif
            LEDState[pos]._Backoff = 0x01;
            LEDState[pos]._LedShifter = 0x00000001;
            LEDState[pos]._ShifterCnt = 0;
        } // all LEDs off for this program

        if (LEDState[pos]._LEDactive != 0)
        {
            multiple = LEDState[pos]._LEDactive;
        }

        trisa_work = 0;
        trisd_work = 0;
        while (multiple > 0)
        {
            while (!(LEDState[pos]._LedShifter & LEDState[pos]._LedMask._long) && (LEDState[pos]._ShifterCnt < 11))
            {
                LEDState[pos]._LedShifter = LEDState[pos]._LedShifter << 1;
                LEDState[pos]._ShifterCnt++;
            }

            switch (LEDState[pos]._ShifterCnt)
            {
            case 0x0:
            {
                trisd_work = trisd_work | 0x20;
                break;
            }
            case 0x1:
            {
                trisd_work = trisd_work | 0x10;
                break;
            }
            case 0x2:
            {
                trisd_work = trisd_work | 0x02;
                break;
            }
            case 0x3:
            {
                trisd_work = trisd_work | 0x01;
                break;
            }
            case 0x4:
            {
                //                trisd_work = trisd_work | 0x01;
                break;
            }
            case 0x5:
            {
#ifdef AMAZON_VER
                if (!(DimCycle == 0x81))
                {
                    trisd_work = trisd_work | 0x08;
                }
#else
                trisd_work = trisd_work | 0x08;
#endif
                break;
            }
            case 0x6:
            {
                trisd_work = trisd_work | 0x04;
                break;
            }
            case 0x7:
            {
                trisa_work = trisa_work | 0x20;
                break;
            }
            case 0x8:
            {
                trisa_work = trisa_work | 0x10;
                break;
            }
            case 0x9:
            {
#ifdef AMAZON_VER
                if (!(DimCycle == 0x81))
                {
                    trisa_work = trisa_work + 0x10;
                }
#else
                trisd_work = trisd_work + 0x40;
#endif
                break;
            }
            case 0xA:
            {
                if ((pos == 0) && (LEDState[pos]._button == 3))
                {
                    if (!LEDState[2]._active && !TopHold)
                    {
                        trisd_work = trisd_work + 0x80;
                    }
                }
                else
                {
                    trisd_work = trisd_work + 0x80;
                }
                break;
            }
            }
            LEDState[pos]._LEDScan++;
            LEDState[pos]._LedShifter = LEDState[pos]._LedShifter << 1;
            LEDState[pos]._ShifterCnt++;
            multiple--;
        }
        LEDState[pos]._tris_a_contrib = trisa_work;
        LEDState[pos]._tris_d_contrib = trisd_work;

        if (LEDState[pos]._LEDScan >= LEDState[pos]._LEDactive)
        {
            LEDState[pos]._LEDScan = 0;
            if (LEDState[pos]._sequence_len > 1)
            {
                LEDState[pos]._ticcnt++;
                if (LEDState[pos]._ticcnt >= LEDState[pos]._ticreload)
                {
                    LEDState[pos]._ticcnt = 0;
                    LEDState[pos]._offset = LEDState[pos]._offset + 3;
                    maxo = LEDState[pos]._sequence_len * 3;
                    if (LEDState[pos]._offset >= maxo)
                    {
                        if (DimB._Bat_Status)
                        {
                            BACK_hold_trip = 0;
                            LEDState[0]._active = 0;
                            LEDState[1]._active = 0;
                            RestoreNavDim();
                            DimB = DimB_Hold;
                            DimB._Nav_Dim = 0;
                            DimB._Bat_Status = 0;
                        }
                        else
                        {
                            LEDState[pos]._offset = 0;
                            if (pos == 0)
                            {
                                LEDState[1]._holdoff = 0;
                                LEDState[1]._ticcnt = 0;
                                LEDState[1]._offset = 0;
                            }
                            else if (pos == 1)
                            {
                                LEDState[0]._holdoff = 0;
                                LEDState[0]._ticcnt = 0;
                                LEDState[0]._offset = 0;
                            }
                        }
                    }
                    l_pnt = LEDState[pos]._program;
                    point = LEDState[pos]._offset;
                    LEDState[pos]._ticreload = l_pnt[point];
                    if (!(LEDState[pos]._ticreload & 0x01))
                    {
                        LEDState[pos]._ticreload = LEDState[pos]._ticreload * 2;
                    }
                }
            }
        }
    }
    if (NotifyState)
    {
        TRISDbits.TRISD7 = 1;
        if (PgmStat == 0x81)
        {
            PgmStatDwell++;
            if (PgmStatDwell >= 5)
            {
                NotifyState = 0;
                IO_RE2_SetLow();
            }
            else
            {
                IO_RE2_SetHigh();
            }
        }
    }
}

void ServiceTopLED(uint8_t pos)
{
    uint16_t* l_pnt;
    uint8_t point, maxo, multiple, test;

    if (LEDState[pos]._active)
    {
        LEDState[pos]._tris_d_contrib = 0;

        if (LEDState[pos]._LEDScan == 0)
        {
            l_pnt = LEDState[pos]._program;
            LEDState[pos]._LEDframe = 0;
            point = LEDState[pos]._offset + 1;
            LEDState[pos]._LedMask._int[1] = l_pnt[point];
            point++;
            LEDState[pos]._LedMask._int[0] = l_pnt[point];
            LEDState[pos]._LEDactive = (LEDState[pos]._LedMask._t[0] & 0x3f);
            LEDState[pos]._LedShifter = 0x80000000;
            LEDState[pos]._ShifterCnt = 0;
            LEDState[pos]._Delay = (LEDState[pos]._LedMask._t[0] >> 6) & 0x03;

            if (!LEDState[0]._active)
            {
                l_pnt = (uint16_t*) LedRear_FullPWR;
                if (DimB._Half_PWR)
                {
                    l_pnt = (uint16_t*) LedRear_HalfPWR;
                }
                if (DimB._SuperMax)
                {
                    timer1ReloadVal = l_pnt[0];
                }
                else if (LEDState[pos]._Delay)
                {
                    timer1ReloadVal = l_pnt[LEDState[pos]._Delay];
                }
            }
        }

        if (LEDState[pos]._LedMask._t[3] == 0x04)
        {
            LEDState[pos]._tris_d_contrib = LEDState[pos]._tris_d_contrib | 0x80;
            LEDState[pos]._LEDScan++;
            LEDState[pos]._ShifterCnt++;
        }

        if (LEDState[pos]._LEDScan >= LEDState[pos]._LEDactive)
        {
            LEDState[pos]._LEDScan = 0;
            if (LEDState[pos]._sequence_len > 1)
            {
                LEDState[pos]._ticcnt++;
                if (LEDState[pos]._ticcnt >= LEDState[pos]._ticreload)
                {
                    LEDState[pos]._ticcnt = 0;
                    LEDState[pos]._offset = LEDState[pos]._offset + 3;
                    maxo = LEDState[pos]._sequence_len * 3;
                    if (LEDState[pos]._offset >= maxo)
                    {
                        LEDState[pos]._offset = 0;
                    }
                    l_pnt = LEDState[pos]._program;
                    point = LEDState[pos]._offset;
                    LEDState[pos]._ticreload = l_pnt[point];
                }
            }
        }
    }
    else
    {
        LEDState[2]._tris_a_contrib = 0;
        LEDState[2]._tris_d_contrib = 0;
        LEDState[2]._tris_d_contrib = 0;
        LEDState[2]._tris_d_contrib = 0;
    }
}

void SendLEDs(void)
{
    uint8_t trisa_work, trisd_work;
    uint16_t* b_pnt;

    Nop();
    Nop();
    Nop();
#ifdef ALLIR_VER
    if (!TurnOffBlink)
    {
        trisd_work = 0;
    }
    else
    {
        trisd_work = 0x80;
    }
    trisa_work = 0;
#else
    if (!TurnOffBlink)
    {
        trisd_work = 0;
    }
    else
    {
        trisd_work = 0x40;
    }
    trisa_work = 0;
#endif

    if ((!(LEDState[0]._active) && !(LEDState[1]._active) && !(LEDState[2]._active) && !(timer0Cycle & 0x01) && (BLINKState._onIdle) || BLINKState._onForce))
    {
        switch (BLINKState._port)
        {
        case NO_PORT:
            break;
        case PORT_AM:
            TRISA = BLINKState._bitmap | 0b00000011; // RA0, RA1 analog inputs
            break;
        case PORT_DM:
            TRISD = BLINKState._bitmap;
            break;
        }
        if (BLINKState._onForce)
        {
            b_pnt = (uint16_t*) LedRear_FullPWR;
            timer1ReloadVal = (0xffff - b_pnt[3]) / 16;
            timer1ReloadVal = 0xffff - timer1ReloadVal;
        }
        else
        {
            timer1ReloadVal = 0xfe80;
        }
        IO_RE2_SetHigh();
        TMR1_StartTimer();
    }
    else
    {
        if (LEDState[0]._active)
        {
            trisa_work = trisa_work | LEDState[0]._tris_a_contrib;
            trisd_work = trisd_work | LEDState[0]._tris_d_contrib;
        }
        if (LEDState[1]._active)
        {
            trisa_work = trisa_work | LEDState[1]._tris_a_contrib;
            trisd_work = trisd_work | LEDState[1]._tris_d_contrib;
        }
        if (LEDState[2]._active)
        {
            trisd_work = trisd_work | LEDState[2]._tris_d_contrib;
        }

        if (DimB._MaxOutF || DimB._MaxOutWL)
        {
            TRISA = trisa_work | 0b00000011; // RA0, RA1 analog inputs
            TRISD = trisd_work;
        }
            //    else if (!(timer0Cycle & 0x01))
        else if ((timer0Cycle & 0x01))
        {
            if (T3CONbits.TMR3ON)
            {
                // Allow All
                TRISA = trisa_work | 0b00000011; // RA0, RA1 analog inputs
                TRISD = trisd_work;
            }
            else
            {
                // Allow D8, D9, D10
                TRISA = (trisa_work & 0b01110000) | 0b00000011;
                // Allow D3, D6, D7, D11
                TRISD = trisd_work & 0b11001100;
            }
            if (DimB._Bat_Status)
            {
                b_pnt = (uint16_t*) LedRear_FullPWR;
                timer1ReloadVal = (0xffff - b_pnt[3]) / 2;
                timer1ReloadVal = 0xffff - timer1ReloadVal;
            }
            else
            {
                timer1ReloadVal = LEDState[0]._timer_timeout;
#ifdef AMAZON_VER
                if (DimCycle & 0x80)
                {
                    if (DimCycle & 0x01)
                    {
                        DimCycle = 0x80;
                    }
                    else
                    {
                        DimCycle = 0x81;
                    }
                }
#else
                timer1ReloadVal = LEDState[0]._timer_timeout;
#endif
            }
            //            if (LEDState[0]._active || LEDState[1]._active || LEDState[2]._active)
            //            {
            TMR1_StartTimer();
            //            }
        }
        else
        {
            if (T1CONbits.TMR1ON)
            {
                // Allow All
                TRISA = trisa_work | 0b00000011;
                if (!TurnOff)
                {
                    TRISD = trisd_work;
                }
            }
            else
            {
                // Allow
                TRISA = (trisa_work & 0b00000000) | 0b00000011;
                // Allow D1, D2, D3, D4, D5
                TRISD = trisd_work & 0b01110011;
            }
            if (DimB._Bat_Status)
            {
                b_pnt = (uint16_t*) LedFront_FullPWR;
                timer3ReloadVal = (0xffff - b_pnt[3]) / 2;
                timer3ReloadVal = 0xffff - timer3ReloadVal;
            }
            else
            {
                timer3ReloadVal = LEDState[1]._timer_timeout;
            }
            //            if (LEDState[0]._active || LEDState[1]._active || LEDState[2]._active)
            //            {
            TMR3_StartTimer();
            //            }
        }
    }
}

void ReadBattery(void)
{
    uint16_t ad_val;
    uint8_t loop, hold;

    hold = OSCCONbits.IRCF;
    OSCCONbits.IRCF = 0x03;

    bat_hold = VREFCON0;
    VREFCON0 = 0b10100000;

    while (!(VREFCON0 & 0b01000000))
    {
        Nop();
    }
    ADC_Initialize();
    ADCON1 = 0b00001000; // TRIGSEL CCP2; NVCFG VSS; PVCFG Vref;
    ADC_SelectChannel(channel_AN1); // AN1:PA1
    bat_loop = 8;
    bat_ad_val = 0;
    while (bat_loop)
    {
        Nop();
        Nop();
        ADC_StartConversion();
        while (!ADC_IsConversionDone())
        {
            Nop();
        }
        bat_ad_val = bat_ad_val + ADC_GetConversionResult();
        bat_loop--;
    }
    bat_ad_val = bat_ad_val / 8;
    TRISCbits.RC1 = 0; // disable current test function
    VREFCON0 = bat_hold;
    ADCON1 = 0b00000000; // TRIGSEL CCP2; NVCFG VSS; PVCFG VDD;
    Battery_res = bat_ad_val;
    OSCCONbits.IRCF = hold;
}

uint8_t EEPROM_Read(uint8_t address)
{
    uint8_t data;

    EEADR = address;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.RD = 1;
    data = EEDATA;
    return data;
}

void EEPROM_Write(uint8_t address, uint8_t data)
{
    EEADR = address;
    EEDATA = data;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;
    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 0;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    while (PIR2bits.EEIF == 0);
    PIR2bits.EEIF = 0;
    EECON1bits.WREN = 0;
}

uint8_t ReadFlash(uint32_t addr)
{
    TBLPTR = addr;
    asm("TBLRD");
    return TABLAT;
}

#define APP_SIGNATURE_ADDRESS                   0x1006  //0x1006 and 0x1007 contains the "signature" WORD, indicating successful erase/program/verify operation
#define APP_SIGNATURE_VALUE                     0x600D  //leet "GOOD", implying that the erase/program was a success and the bootloader intentionally programmed the APP_SIGNATURE_ADDRESS with this value
#define APP_VERSION_ADDRESS                     0x1100  //0x1100 and 0x1101 should contain the application image firmware version number
#define APP_VERSION_VALUE                       0x0201

const unsigned int
__attribute__((address(APP_SIGNATURE_ADDRESS)))
FlashSignatureWord = APP_SIGNATURE_VALUE;

const unsigned int
__attribute__((address(APP_VERSION_ADDRESS)))
FlashVersionWord = APP_VERSION_VALUE;

/**
 End of File
 */