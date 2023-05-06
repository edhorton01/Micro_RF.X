/*
 * File:   types.h
 * Author: ed
 *
 * Created on February 18, 2017, 3:09 PM
 */

#ifndef TYPES_H
#define	TYPES_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef union {

        struct {
            uint32_t _long;
        };

        struct {
            uint16_t _int[2];
        };

        struct {
            uint8_t _t[4];
        };

    } LongInt;

    typedef union {

        struct {
            int16_t _int;
        };

        struct {
            uint8_t _t[2];
        };
    } SintByte;

    typedef union {

        struct {
            uint16_t _int;
        };

        struct {
            uint8_t _byte[2];
        };
    } UintByte;

    typedef union {

        struct {
            uint8_t _int;
        };

        struct {
            unsigned _scan_st : 1;
            unsigned _scan_end : 1;
            unsigned _pressed : 1;
            unsigned _new_cmd : 1;
            unsigned _interrupt : 1;
            unsigned _startCycle : 1;
        };
    } KEYstateBITS;

    typedef struct {
        uint8_t _two_press_cnt;
        uint8_t _DB_cnt;
        uint8_t _bounce;
        uint8_t _cmd;
    } KEYstateControl;

    typedef struct {
        uint8_t _RF_cmd;

        union {

            struct {
                uint8_t _int;
            };

            struct {
                unsigned _new_cmd : 1;
                unsigned _pair_mode : 1;
                unsigned _pair_active : 1;
                unsigned _just_paired : 1;
            };
        };
    } RF_Cmd;

    typedef struct {
        uint8_t _button;
        uint8_t _button_press;
        uint8_t _sequence_len;
        uint8_t _active;
        uint8_t _offset;
        uint8_t _holdoff;
        uint16_t* _program;
        uint16_t _ticcnt;
        uint16_t _ticreload;
        uint8_t _LEDScan;
        uint8_t _LEDframe;
        uint8_t _LEDactive;
        uint8_t _ShifterCnt;
        uint8_t _SeqPos;
        uint8_t _Delay;
        uint8_t _Backoff;
        uint32_t _LedShifter;
        LongInt _LedMask;
        uint8_t _tris_a_contrib;
        uint8_t _tris_d_contrib;
        uint16_t _timer_timeout;
    } LEDControl;

    typedef struct {
        uint8_t _DimCnt;
        uint8_t _DimDir;

        union {

            struct {
                uint8_t _status;
            };

            struct {
                unsigned _MaxOut : 1;
                unsigned _DA_Dim : 1;
                unsigned _Half_PWR : 1;
                unsigned _SuperMax : 1;
                unsigned _SOS_Mode : 1;
                unsigned _Cust_Mode : 1;
                unsigned _IR_Mode : 1;
                unsigned _German_IR : 1;
            };
        };

        union {

            struct {
                uint8_t _cmd;
            };

            struct {
                unsigned _MaxOutR : 1;
                unsigned _MaxOutF : 1;
                unsigned _MaxOutWL : 1;
                unsigned _MaxOutD : 1;
                unsigned _Bat_Status : 1;
                unsigned _Nav_Dim : 1;
                unsigned _GA_Mode : 1;
            };
        };
    } DIMstateBITS;

    typedef struct {
        uint8_t _port;
        uint8_t _bitmap;

        union {

            struct {
                uint8_t _flag;
            };

            struct {
                unsigned _onIdle : 1;
                unsigned _onActive : 1;
                unsigned _onForce : 1;
            };
        };
    } BLINKstateBITS;

    typedef struct {
        uint16_t _rear_counter;
        uint16_t _front_counter;

        union {

            struct {
                uint8_t _flags;
            };

            struct {
                unsigned _rear_dt_inprog : 1;
                unsigned _front_dt_inprog : 1;
                unsigned _rear_dt_fire : 1;
                unsigned _front_dt_fire : 1;
            };
        };
    } DoubleTap;

    typedef struct {
        uint8_t _delaycnt;
        uint8_t _delaytarget;
        uint8_t _statuscnt;
        uint8_t _pulsecnt;

        union {

            struct {
                uint16_t _flags;
            };

            struct {
                unsigned _RF_Active : 1;
                unsigned _active : 1;
                unsigned _finished : 1;
                unsigned _prime_rx : 1;
                unsigned _go_rx : 1;
                unsigned _window : 1;
                unsigned _new_rx : 1;
                unsigned _new_tx : 1;
                unsigned _rx_on_full : 1;
                unsigned _rx_on_pulse : 1;
            };
        };
    } TmrDelay;


#ifdef	__cplusplus
}
#endif

#endif	/* TYPES_H */

