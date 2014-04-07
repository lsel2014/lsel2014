/*
* File: PICsun_main.c
* Author: Javier Gonzalez Recio
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <xc.h> // include standard header file
#include <stdbool.h> /* For true/false definition */
#include <pic12f1822.h>
#include <stdint.h>
#include "PICsun_constants.h"

#define I2C_ADDR 0x20
#define _XTAL_FREQ 8000000 // 8MHz

// set Config bits
#pragma config FOSC=INTOSC, PLLEN=OFF, WDTE=OFF, MCLRE=OFF
#pragma config CLKOUTEN=OFF, IESO=OFF, FCMEN=OFF, CP=OFF, CPD=OFF, BOREN=OFF
#pragma config WRT=OFF, STVREN=ON, BORV=LO, LVP=OFF

#define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))

#define STATE1 0b00001001 // 0x09 master write last was address
#define STATE2 0b00101001 // 0x29 master write last was data
#define STATE3 0b00001101 // 0x0d master read last was address
#define STATE4 0b00101100 // 0x2c master write last was data
#define STATE5 0b00101000 // 0x28

uint16_t simulated_time;
uint16_t sunrise_seconds;
uint16_t sunset_seconds;
uint16_t peak_light_time;
int16_t light_peak;
float delta;
float pwmdc;

uint8_t send_buffer[I2C_SEND_BUF_SIZE];
uint8_t send_index=0;
volatile uint8_t daynight;
volatile uint8_t rcvd_byte;
volatile uint8_t rcv_buffer[I2C_BUF_SIZE];
volatile bool buffer_valid, newcmd,stream;

void setPWMdc(uint16_t dc) {
    if (dc > 1023)dc = 1023;
    CCPR1L = dc >> 2;
    CCP1CON = ((CCP1CON & 0b11001111) | ((dc << 4)&0b00110000));
}

void I2CWrite(uint8_t data) {
    while(BF); //wait while buffer is full
    do {
        WCOL = 0; //clear write collision flag
        SSP1BUF = data;
    } while (WCOL); //do until write collision flag is clear
    if (SEN) CKP = 1; //release the SCL line
}

void interrupt isr(void) {
    if (SSP1IF) //I2C interrupt handler
    {
        SSP1IF = 0; //Clear interrupt flag
        uint8_t i2cStatus, value;


        if (i2cStatus != 0x30) {
            i2cStatus = (SSPSTAT & 0b00101101); //Mask out unimportant bits
            // D/A, S, R/W, BF

            switch (i2cStatus) {
                    //State 1 RX address, this state is activated when a new I2c Call is received
                case STATE1: // STATE 1 master write last byte was ADDRESS
                    //SSPSTAT bits: D/A=0, S=1, R/W=0, BF=1
                    value = SSPBUF; //read buffer, clear BF
                    if (SSPOV)SSPOV = 0; //clear receive overflow indicator
                    if (SEN) CKP = 1; //release the SCL line
                    break;

                case STATE2: // STATE2 last byte was data
                    //SSPSTAT bits D/A=1, S=1, R/W=0, BF=1
                    value = SSPBUF; //read buffer, clear BF
                    if (value == 0xFF) {
                        newcmd = true;
                        rcvd_byte = 0;
                    } else if (value== 0xFE){
                        *((uint16_t*)send_buffer)=simulated_time;
                        send_buffer[2]=daynight;
                        send_buffer[3]=rcv_buffer[0];
                        send_buffer[4]=rcv_buffer[1];
                        send_buffer[5]=rcv_buffer[2];
                        send_buffer[6]=rcv_buffer[3];
                        send_buffer[7]=rcv_buffer[4];
                        send_buffer[8]=rcv_buffer[5];
                        send_buffer[9]=0xFF;
                        send_index=0;
                    }else
                    {
                        if (newcmd && !buffer_valid) {
                            rcv_buffer[rcvd_byte++] = value;
                        }
                        if (rcvd_byte == I2C_BUF_SIZE) {
                            newcmd = false;
                            buffer_valid = true;
                        }
                    }
                    if (SEN) CKP = 1; //release the SCL line
                    break;
                case STATE3: // STATE 3 master read last byte was address
                    //SSPSTAT bits: D/A = 0, S=1, R/W=1, BF=0

                    value = SSPBUF; //dummy read
                    if (send_index >= I2C_SEND_BUF_SIZE)
                        send_index= 0;
                    I2CWrite(send_buffer[send_index]); //write back the index of status requested
                    send_index++;
                    stream=false;
                    break;

                case STATE4: // STATE 4 last byte was data
                    //SSPSTAT bits: D/A=1, S=1, R/W=1, BF=0

                    if (send_index>= I2C_SEND_BUF_SIZE)
                        send_index = 0;
                    I2CWrite(send_buffer[send_index]); //write back the index of status requested
                   if(stream)send_index++;
                   stream=true;
                    break;

                case STATE5: // STATE 5 logic reset by NACK from master
                    //SSPSTAT bits: D/A=1, S=1, R/W=0, BF=0, CKP=1
                    break;
                default:
                    if (SEN) CKP = 1; //release the SCL line
                    break;
            }//end switch (i2cStatus)

        }// end if status !=30
    }//end I2C interrupt handler
}

void calc_times(void) {
    if (rcv_buffer[0] < 24 && rcv_buffer[3] < 24) {
        sunrise_seconds = rcv_buffer[0] * 50 + (rcv_buffer[1]*5)/6;
        sunset_seconds = rcv_buffer[3] * 50+ (rcv_buffer[4]*5)/6;
        peak_light_time = sunrise_seconds + ((sunset_seconds - sunrise_seconds) >> 1);
        /*Empirical formula from http://www.ccfg.org.uk/conferences/downloads/P_Burgess.pdf*/
        /*Linear fitting + LED driver nonlinearity make a good approximation*/
        light_peak = -80 + 60 * (int16_t) (rcv_buffer[3] - rcv_buffer[0]);
        if (light_peak > 1023)light_peak = 1023;
        delta = (float)light_peak /(float)((sunset_seconds - sunrise_seconds) >> 1);
        simulated_time = 0; //reset the simulation
    }
    buffer_valid = false;
}

void simulate(void) {

    if (simulated_time < sunrise_seconds || simulated_time > sunset_seconds) {
        pwmdc = 0;
        daynight=0;
    } else if (simulated_time < peak_light_time) {
        pwmdc += delta;
        daynight=1;
    } else if (simulated_time > peak_light_time) {
        pwmdc -= delta;
        daynight=1;
    }

    setPWMdc((uint16_t)(1023.0 - pwmdc));
    PIR1bits.TMR2IF = 0; //Clear Timer2 interrupt flag
    T2CONbits.TMR2ON = 1; //Enable Timer2
    TRISAbits.TRISA5 = 0; //Enable output

}

void __init(void) {

    // OSCCON: OSCILLATOR CONTROL REGISTER
    OSCCONbits.SPLLEN = 0; //PLL disabled
    OSCCONbits.IRCF = 0b1110; //OSC frequency=8Mhz
    OSCCONbits.SCS = 0x02; //internal oscillator block

    // Set up I/O pins
    // PORT A Assignments
    TRISAbits.TRISA0 = 0; // RA0 = nc
    TRISAbits.TRISA1 = 1; // RA1 = SCL
    TRISAbits.TRISA2 = 1; // RA2 = SDA
    TRISAbits.TRISA3 = 0; // RA3 = nc (MCLR)
    TRISAbits.TRISA4 = 0; // RA4 = nc
    TRISAbits.TRISA5 = 1; // RA5 = CCP1

    // ANSELA: PORTA ANALOG SELECT REGISTER
    ANSELA = 0x00; // all pins are digital I/O

    //APFCON: ALTERNATE PIN FUNCTION CONTROL REGISTER
    APFCONbits.CCP1SEL = 1;

    // OPTION_REG: OPTION REGISTER
    OPTION_REG = 0b00000000;

    // WPUA: WEAK PULL-UP PORTA REGISTER
    WPUA = 0b00000000;

    // LATA: PORTA DATA LATCH REGISTER
    LATA = 0b00000000;

    SSP1CON1 = 0b00110110; // SSP1EN = 1, CKP = 1, SSP1M = Slave mode 7bit
    SSP1CON2bits.SEN = 1; // Clock stretching is enabled for both slave transmit and slave receive (stretch enabled)
    SSP1ADD = I2C_ADDR << 1; // bits 7 to 1

    //Timer 2 for PWM
    PR2 = 0xFF;
    CCP1CONbits.CCP1M = 0b1100; //1100 = PWM mode: P1A, P1C active-high; P1B, P1D active-high
    T2CONbits.T2CKPS = 0b11; //Prescaler = 64

    PIR1bits.SSP1IF = 0; // Clear MSSP interrupt request flag
    PIE1bits.SSP1IE = 1; // Enable MSSP interrupt enable bit

    INTCONbits.GIE = 1; // GIE: Global Interrupt Enable bit
    INTCONbits.PEIE = 1; // PEIE: Peripheral Interrupt Enable bit

    __delay_us(100000);
}

int16_t main() {

    __init();

    //default sunrise and sunset
    rcv_buffer[0] = DEFAULT_SUNRISE_H;
    rcv_buffer[1] = DEFAULT_SUNRISE_M;
    rcv_buffer[2] = DEFAULT_SUNRISE_S;
    rcv_buffer[3] = DEFAULT_SUNSET_H;
    rcv_buffer[4] = DEFAULT_SUNSET_M;
    rcv_buffer[5] = DEFAULT_SUNSET_S;
    buffer_valid = true;

    while (1) {
        if (buffer_valid == true) {
            calc_times();
        }

        simulate();
        __delay_us(100000);
        simulated_time++;
        if (simulated_time >= DAY_SIM) {
            simulated_time = 0;
        }
    }


    return (EXIT_SUCCESS);
}