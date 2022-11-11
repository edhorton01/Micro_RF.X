/*
 * File:   main.h
 * Author: ed
 *
 * Created on February 18, 2017, 4:33 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

    void ServiceKeyPress(void);
    void init_button(uint8_t button, uint8_t pos);
    void ServiceCmd(void);
    void RestoreNavDim(void);
    void ServiceLEDs(uint8_t pos);
    void ServiceTopLED(uint8_t pos);
    void SendLEDs(void);
    void CalibrateCurrent(void);
    void clean_up(void);
    void ReadBattery(void);
    void EEPROM_Write(uint8_t address, uint8_t data);
    uint8_t EEPROM_Read(uint8_t address);

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

