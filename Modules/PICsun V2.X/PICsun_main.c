/*
 * File:   PICsun_main.c
 * Author: Javier Gonzalez Recio
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h> // include standard header file
#include <stdbool.h>        /* For true/false definition */
#include <pic12f1822.h>

#include "PICsun_constants.h"

#define I2C_ADDR        0x20
#define _XTAL_FREQ      8000000    // 8MHz

// set Config bits
#pragma config FOSC=INTOSC, PLLEN=OFF, WDTE=OFF, MCLRE=OFF
#pragma config CLKOUTEN=OFF, IESO=OFF, FCMEN=OFF, CP=OFF, CPD=OFF, BOREN=OFF
#pragma config WRT=OFF, STVREN=ON, BORV=LO, LVP=OFF

#define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))

#define STATE1      0b00001001 // 0x09 master write last was address
#define STATE2      0b00101001 // 0x29 master write last was data
#define STATE3      0b00001101 // 0x0d master read last was address
#define STATE4      0b00101100 // 0x2c master write last was data
#define STATE5      0b00101000 // 0x28

volatile unsigned int     real_time;
volatile unsigned long    sunrise_seconds;
volatile unsigned long    sunset_seconds;
volatile unsigned int     present_segment;
volatile unsigned int     segment_duration;

volatile char             rcvd_byte;
volatile char             rcv_buffer[I2C_BUF_SIZE];
volatile bool             buffer_valid, newcmd;


void setPWMdc(unsigned int dc){
    CCPR1L = dc>>2;
    CCP1CON = ((CCP1CON&0b11001111)|((dc<<4)&0b00110000));
}

void I2CWrite(unsigned char data){
    while(BF);      //wait while buffer is full
    do{
        WCOL = 0;           //clear write collision flag
        SSP1BUF = data;
    }while (WCOL);           //do until write collision flag is clear
    if (SEN) CKP = 1;           //release the SCL line
}

void interrupt isr(void) {
 if(SSP1IF)  //I2C interrupt handler
    {
      SSP1IF = 0;      //Clear interrupt flag
      unsigned char i2cStatus, value;


        if(i2cStatus != 0x30)
        {
        i2cStatus = (SSPSTAT & 0b00101101);    //Mask out unimportant bits
                        // D/A, S, R/W, BF

        switch (i2cStatus){
       //State 1 RX address, this state is activated when a new I2c Call is received
            case STATE1: // STATE 1 master write last byte was ADDRESS
                             //SSPSTAT bits: D/A=0, S=1, R/W=0, BF=1
                value = SSPBUF;         //read buffer, clear BF
                if(SSPOV)SSPOV = 0;              //clear receive overflow indicator
                if (SEN) CKP = 1;           //release the SCL line
                break;

            case STATE2: // STATE2  last byte was data
                             //SSPSTAT bits D/A=1, S=1, R/W=0, BF=1
                value=SSPBUF; //read buffer, clear BF
                if(value == 0xFF){
                    newcmd=true;
                    rcvd_byte=0;
                }else{
                if(newcmd && !buffer_valid)
                {
                    rcv_buffer[rcvd_byte++]= value;
                }
                if(rcvd_byte== I2C_BUF_SIZE){
                    newcmd = false;
                    buffer_valid = true;
                }
              }
                if (SEN) CKP = 1;           //release the SCL line
                break;
            case STATE3:    // STATE 3  master read last byte was address
                                //SSPSTAT bits: D/A = 0, S=1, R/W=1, BF=0
                value = SSPBUF; //dummy read
                    if (rcvd_byte >= I2C_BUF_SIZE)
                        rcvd_byte = 0;
                    I2CWrite(rcv_buffer[rcvd_byte]);    //write back the index of status requested
                rcvd_byte++;
                break;

            case STATE4: // STATE 4 last byte was data
                             //SSPSTAT bits: D/A=1, S=1, R/W=1, BF=0
                if(rcvd_byte>=I2C_BUF_SIZE)
                     rcvd_byte = 0;
                I2CWrite(rcv_buffer[rcvd_byte]);    //write back the index of status requested
                rcvd_byte++;
                break;

            case STATE5: // STATE 5 logic reset by NACK from master
                //SSPSTAT bits: D/A=1, S=1, R/W=0, BF=0, CKP=1
                break;
            default:
                if (SEN) CKP = 1;           //release the SCL line
                break;
        }//end switch (i2cStatus)

       }// end if status !=30
    }//end I2C interrupt handler
}

void calc_times(void) {

    sunrise_seconds = rcv_buffer[0] * 3600 + rcv_buffer[1] * 60 + rcv_buffer[2];
    sunset_seconds = rcv_buffer[3] * 3600 + rcv_buffer[4] * 60 + rcv_buffer[5];
    unsigned int light_time = sunset_seconds - sunrise_seconds;
    segment_duration = light_time/LIGHT_SEGMENTS_N;

    real_time = 0; //reset the simulation
    buffer_valid = false;
}

void ready_PWM(unsigned int segment) { //For LIGHT_SEGMENTS_N = 39
    if (segment == 0) {
       setPWMdc(1023);
    } else if (segment == 1) {
       setPWMdc(790);
    } else if (segment == 2) {
       setPWMdc(654);
    } else if (segment == 3) {
        setPWMdc(557);
    } else if (segment == 4) {
       setPWMdc(482);
    } else if (segment == 5) {
       setPWMdc(421);
    } else if (segment == 6) {
       setPWMdc(369);
    } else if (segment == 7) {
       setPWMdc(324);
    } else if (segment == 8) {
       setPWMdc(285);
    } else if (segment == 9) {
       setPWMdc(249);
    } else if (segment == 10) {
      setPWMdc(217);
    } else if (segment == 11) {
      setPWMdc(118);
    } else if (segment == 12) {
      setPWMdc(161);
    } else if (segment == 13) {
      setPWMdc(136);
    } else if (segment == 14) {
      setPWMdc(113);
    } else if (segment == 15) {
      setPWMdc(91);
    } else if (segment == 16) {
      setPWMdc(71);
    } else if (segment == 17) {
      setPWMdc(52);
    } else if (segment == 18) {
      setPWMdc(33);
    } else if (segment == 19) {
      setPWMdc(16);
    } else if (segment == 20) {
      setPWMdc(0);
    }

}

void act_segment(long check_time) {

    if (check_time < sunrise_seconds || check_time > sunset_seconds) {
        present_segment = 0;
    } else {
        long prev_separator = sunrise_seconds + present_segment*segment_duration;
        long next_separator = prev_separator + segment_duration;

        while(!(check_time >= prev_separator && check_time <= next_separator)) {
            present_segment++;
            prev_separator = sunrise_seconds + present_segment*segment_duration;
            next_separator = prev_separator + segment_duration;
        }
    }
}


void simulate(void) {
    unsigned long simulation_time = real_time * SEC_SIM;
    unsigned int PWMtemp;

    act_segment(simulation_time);

    if (present_segment > (LIGHT_SEGMENTS_N/2)+1) {
        PWMtemp = LIGHT_SEGMENTS_N-present_segment;
    } else {
        PWMtemp = present_segment;
    }

    ready_PWM(PWMtemp);

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

int main() {

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
        __delay_us(500000);

        if (++real_time == DAY_SIM) {
            real_time = 0;
        }

    }


    return (EXIT_SUCCESS);
}