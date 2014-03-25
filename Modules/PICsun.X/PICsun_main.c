/* 
 * File:   newmain.c
 * Author: Javier Gonzalez Recio
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h> // include standard header file
#include "PICsun_constants.h"

#define I2C_ADDR        0x20
#define _XTAL_FREQ      16000000    // 16MHz

// set Config bits
#pragma config FOSC=INTOSC, PLLEN=OFF, WDTE=OFF, MCLRE=OFF
#pragma config CLKOUTEN=OFF, IESO=OFF, FCMEN=OFF, CP=OFF, CPD=OFF, BOREN=OFF
#pragma config WRT=OFF, STVREN=ON, BORV=LO, LVP=OFF

#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/16000.0)))

volatile unsigned int     sunrise_hms[3] , sunset_hms[3];
volatile unsigned int     real_time;
volatile unsigned int     light_segments[LIGHT_SEGMENTS_N+1];
volatile unsigned int     present_segment;



void i2c_rcv(void) {

    int rcvd_bytes;
    int trash;
    
    trash = SSPBUF;     //Clear the buffer by reading it

    /* Reciving chars
    while (rcvd_bytes < 8) {
        if (SSPSTATbits.BF == 1 && SSPSTATbits.D_nA == 1) {
            if(rcvd_bytes == 0 || rcvd_bytes == 1) {
                sunrise_h[rcvd_bytes] = SSPBUF;
            } else if (rcvd_bytes == 3 || rcvd_bytes == 4 ) {
                sunrise_m[rcvd_bytes-3] = SSPBUF;
            } else if (rcvd_bytes == 6 || rcvd_bytes == 7 ) {
                sunrise_s[rcvd_bytes-6] = SSPBUF;
            } else {
                SSPSTATbits.BF = 0;     //Clear the buffer full flag
            }
            SSPSTATbits.D_nA == 0;
            rcvd_bytes++;
        }
    }

    rcvd_bytes = 0;
    while (rcvd_bytes < 8) {
        if (SSPSTATbits.BF == 1 && SSPSTATbits.D_nA == 1) {
            if(rcvd_bytes == 0 || rcvd_bytes == 1) {
                sunset_h[rcvd_bytes] = SSPBUF;
            } else if (rcvd_bytes == 3 || rcvd_bytes == 4 ) {
                sunset_m[rcvd_bytes-3] = SSPBUF;
            } else if (rcvd_bytes == 6 || rcvd_bytes == 7 ) {
                sunset_s[rcvd_bytes-6] = SSPBUF;
            } else {
                SSPSTATbits.BF = 0;     //Clear the buffer full flag
            }
            SSPSTATbits.D_nA == 0;
            rcvd_bytes++;
        }
    }
    */

    //int
    for (rcvd_bytes = 0; rcvd_bytes <= 2; rcvd_bytes++) {
        if (SSPSTATbits.BF == 1 && SSPSTATbits.D_nA == 1) {
            sunrise_hms[rcvd_bytes] = SSPBUF;
            SSPSTATbits.BF = 0;         //Clear the buffer full flag
            SSPSTATbits.D_nA = 0;       //Clear the data/address flag, so we can recive another byte
            rcvd_bytes++;
        }
    }

    for (rcvd_bytes = 0; rcvd_bytes <= 2; rcvd_bytes++) {
        if (SSPSTATbits.BF == 1 && SSPSTATbits.D_nA == 1) {
            sunset_hms[rcvd_bytes] = SSPBUF;
            SSPSTATbits.BF = 0;         //Clear the buffer full flag
            SSPSTATbits.D_nA = 0;       //Clear the data/address flag, so we can recive another byte
            rcvd_bytes++;
        }
    }

    PIR1bits.SSP1IF = 0; //clear the interruption flag

}

void calc_times(void) {
    /* chars
    int sunrise_seconds = atoi(&sunrise_h[0]) * 3600 + atoi(&sunrise_m[0]) * 60 + atoi(&sunrise_s[0]);
    int sunset_seconds = atoi(&sunset_h[0]) * 3600 + atoi(&sunset_m[0]) * 60 + atoi(&sunset_s[0]);
    */

    //ints
    int sunrise_seconds = sunrise_hms[0] * 3600 + sunrise_hms[1] * 60 + sunrise_hms[2];
    int sunset_seconds = sunset_hms[0] * 3600 + sunset_hms[1] * 60 + sunset_hms[2];
    unsigned int light_time = sunset_seconds - sunrise_seconds;
    unsigned int segment_duration = light_time/LIGHT_SEGMENTS_N;
    int i = 0;

    for (i = 0; i <= LIGHT_SEGMENTS_N; i++) {
        light_segments[i] = sunrise_seconds + i*segment_duration;
    }

    real_time = 0;          //reset the simulation
    present_segment = 0;

}

void ready_pwm(int segment) {
    if (segment == 0) {
        //1023 -> 0b1111111111
        CCPR1L = 0b11111111;
        CCP1CONbits.DC1B = 0b11;
    } else if (segment == 1) {
        //976  -> 0b1111010000
        CCPR1L = 0b11110100;
        CCP1CONbits.DC1B = 0b00;
    } else if (segment == 2) {
        //924  -> 0b1110011100
        CCPR1L = 0b11100111;
        CCP1CONbits.DC1B = 0b00;
    } else if (segment == 3) {
        //864  -> 0b1101100000
        CCPR1L = 0b11011000;
        CCP1CONbits.DC1B = 0b00;
    } else if (segment == 4) {
        //796  -> 0b1100011100
        CCPR1L = 0b11000111;
        CCP1CONbits.DC1B = 0b00;
    } else if (segment == 5) {
        //715  -> 0b1011001011
        CCPR1L = 0b10110010;
        CCP1CONbits.DC1B = 0b11;
    } else if (segment == 6) {
        //616  -> 0b1001101000
        CCPR1L = 0b10011010;
        CCP1CONbits.DC1B = 0b00;
    } else if (segment == 7) {
        //488  -> 0b0111101000
        CCPR1L = 0b01111010;
        CCP1CONbits.DC1B = 0b00;
    } else if (segment == 8) {
        //307  -> 0b0100110011
        CCPR1L = 0b11111111;
        CCP1CONbits.DC1B = 0b11;
    } else if (segment == 9) {
        //42   -> 0b0000101010
        CCPR1L = 0b00001010;
        CCP1CONbits.DC1B = 0b10;
    } else if (segment == 10) {
        //0    -> 0b0000000000
        CCPR1L = 0b00000000;
        CCP1CONbits.DC1B = 0b00;
    }
}

void check_segment(int check_time){
    int i;

    if (check_time < light_segments[0] || check_time > light_segments[LIGHT_SEGMENTS_N]) {
        present_segment = 0;
    } else {
        for (i = 0; i < 10; i++) {
            if (check_time > light_segments[i] && check_time < light_segments[i+1]) {
                present_segment = i+1;
                break;
            }
        }
        for (i = 10; i < LIGHT_SEGMENTS_N; i++) {
            if (check_time > light_segments[i] && check_time < light_segments[i+1]) {
                present_segment = 19-i;
                break;
            }
        }
    }
}

void simulate(void) {
    int simulation_time = real_time * SEC_SIM;

    check_segment(simulation_time);
    ready_pwm(present_segment);

    PIR1bits.TMR2IF = 0;    //Clear Timer2 interrupt flag
    T2CONbits.TMR2ON = 1;   //Enable Timer2
    TRISAbits.TRISA5 = 0;   //Enable output

    __delay_ms(5);

    TRISAbits.TRISA5 = 1;   //Disable output

}

void __init(void) {

    // OSCCON: OSCILLATOR CONTROL REGISTER
    OSCCONbits.SPLLEN=0;    //PLL disabled
    OSCCONbits.IRCF=0x0F;   //OSC frequency=16Mhz
    OSCCONbits.SCS=0x02;    //internal oscillator block

    // Set up I/O pins
    // PORT A Assignments
    TRISAbits.TRISA0 = 0;	// RA0 = nc
    TRISAbits.TRISA1 = 1;	// RA1 = SCL
    TRISAbits.TRISA2 = 1;	// RA2 = SDA
    TRISAbits.TRISA3 = 0;	// RA3 = nc (MCLR)
    TRISAbits.TRISA4 = 0;	// RA4 = nc
    TRISAbits.TRISA5 = 1;	// RA5 = CCP1

    // ANSELA: PORTA ANALOG SELECT REGISTER
    ANSELA=0x00;		// all pins are digital I/O
    
    //APFCON: ALTERNATE PIN FUNCTION CONTROL REGISTER
    APFCONbits.CCP1SEL = 1;

    // OPTION_REG: OPTION REGISTER
    OPTION_REG = 0b00000000;

    // WPUA: WEAK PULL-UP PORTA REGISTER
    WPUA = 0b00000000;

    // LATA: PORTA DATA LATCH REGISTER
    LATA = 0b00000000;

    SSP1CON1 = 0b00110110;      // SSP1EN = 1, CKP = 1, SSP1M = Slave mode 7bit
    SSP1CON2bits.SEN = 1;       // Clock stretching is enabled for both slave transmit and slave receive (stretch enabled)
    SSP1ADD = I2C_ADDR << 1;    // bits 7 to 1

    //Timer 2 for PWM
    PR2 = 0xFF;
    CCP1CONbits.CCP1M = 0b1100; //1100 = PWM mode: P1A, P1C active-high; P1B, P1D active-high
    T2CONbits.T2CKPS = 0b11;    //Prescaler = 64

    PIR1bits.SSP1IF = 0;         // Clear MSSP interrupt request flag
    PIE1bits.SSP1IE = 1;         // Enable MSSP interrupt enable bit

    INTCONbits.GIE  = 1;       // GIE: Global Interrupt Enable bit
    INTCONbits.PEIE = 1;       // PEIE: Peripheral Interrupt Enable bit

    __delay_ms(100);
}


int main() {

    __init();

    //default sunrise and sunset
    sunrise_hms[0] = DEFAULT_SUNRISE_H;
    sunrise_hms[1] = DEFAULT_SUNRISE_M;
    sunrise_hms[2] = DEFAULT_SUNRISE_S;
    sunset_hms[0] = DEFAULT_SUNSET_H;
    sunset_hms[1] = DEFAULT_SUNSET_M;
    sunset_hms[2] = DEFAULT_SUNSET_S;
    calc_times();

    while(1) {
        if((PIR1bits.SSP1IF == 1) && (SSPSTATbits.BF == 1) && (SSPSTATbits.D_nA == 0)) {
            i2c_rcv();      //receive the sunrise and sunset data
            calc_times();
        } else {
            simulate();
            //__delay_ms(995);
            if(++real_time == DAY_SIM){
                real_time = 0;
            }
        }
    }


    return (EXIT_SUCCESS);
}

