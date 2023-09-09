/*
 * File:   main.h
 * Author: ed
 *
 * Created on February 18, 2017, 4:33 PM
 */

//#define	    FCC_MODE 1
#define     NO_FRONTHOLD    1

// STANDARD so use NORMAL_VER for th1s group
#define     STANDARD 1
//#define	    NON_RF      1
//#define       NORMAL_VER 1
//#define	    AMAZON_VER 1
//#define       WHITEGREEN 1
//#define	    WHITEORANGE	1
//#define	    WHITERED 1
#define       WHITEWHITE	1
//#define	    WHITEYELLOW	1

// NORMAL BEACON so use NORMAL_VER for th1s group
//#define     NORMAL_VER 1
//#define	    NON_RF      1
//#define	    GREENGREEN 1
//#define	    ORANGEORANGE 1
//#define	    REDGREEN	1
//#define	    REDRED 1
//#define	    YELLOWYELLOW 1

//Nav light so use NAVLIGHT_VER for th1s group
#define	    NAVLIGHT_VER 1
//#define	    NON_RF      1
//#define	    FORCE_NAVLIGHT 1
//#define	    REDGREEN	1
//#define     STANDARD 1
//#define     WHITEWHITE	1

//IR Model
//#define	    NON_RF      1
//#define       NORMAL_VER 1
//#define	    ALLIR_VER
//#define	    IR_LED 1

//IR German Hybrid
//#define     GERMAN_VER 1
//#define	    IRGREEN	1


#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

    void ServiceKeyPress(void);
    void init_button(uint8_t button, uint8_t pos);
    void ServiceCmd(void);
    void ServiceRFCmd(void);
    void RestoreNavDim(void);
    void ServiceLEDs(uint8_t pos);
    void ServiceTopLED(uint8_t pos);
    void SendLEDs(void);
    void CalibrateCurrent(void);
    void clean_up(void);
    void ReadBattery(void);
    void EEPROM_Write(uint8_t address, uint8_t data);
    uint8_t EEPROM_Read(uint8_t address);
    uint8_t ReadFlash(uint32_t addr);
    void writeUserID(uint8_t *flashWrBufPtr);

    enum port_sel {
        NO_PORT, PORT_AM, PORT_DM
    };

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

