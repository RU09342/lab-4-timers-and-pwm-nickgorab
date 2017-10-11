/*
 * softwarePWM.c
 *
 *   Created on:  October 8, 2017
 *  Last Edited:  October 11, 2017
 *       Author:  Nick Gorab
 *        Board:  2311
 */


#include <msp430.h>
#include <config.h>

void ledInitialize(void) {  // Initializes the LEDs
    L1_DIR = LED1;          // Sets LEDs as Outputs
    L2_DIR = LED2;
    L1_OUT = ~LED1;
    P1SEL0 |= LED1;         // Multiplexes the Led Signal with the Timer
}

void buttonInitialize(void) {   // Initializes the button
    B1_DIR |= ~BUTTON1;         // Sets direction as input
    B1_REN |=  BUTTON1;         // Enables resistor
    B1_OUT |=  BUTTON1;         // Pull-up Resistor
    B1_IE  |=  BUTTON1;         // Enables interrupt
    B1_IES |= ~BUTTON1;         // Falling edge select
    B1_IFG &= ~BUTTON1;         // Clears any flags
}


volatile unsigned int PWM = 500;    // Starting PWM

void main(void) {

    WDTCTL = WDTPW + WDTHOLD;       // Stops WDT
    HIGHZ;                          // Disables high impedance mode

    ledInitialize();        // Initializes LEDs
    buttonInitialize();     // Initializes buttons
    buttonInterrupt();      // Initializes button interrupt



    TB0CCR0 = 1000-1;           // Timer is set to 1 kHz
    TB0CCTL1 = OUTMOD_7;        // PWM set to reset
    TB0CCR1 = PWM;              // CCR1 value set to the PWM value
    TB0CTL = TBSSEL_2 + MC_1;   // SMCLK, Up-Mpde

    _BIS_SR(LPM0_bits + GIE);   // Low power mode, global interrupt enabled

}

#pragma vector=PORT1_VECTOR         // Button interrupt 
__interrupt void Port_1(void) { 

    __delay_cycles(500);          // Debouncin'
    L2_OUT ^= LED2;               // Toggles LED
    TB0CCR1 += 100;               // Increases PWM

    if(TB0CCR1 == 1100) {        // Resets PWM
        TB0CCR1 = 0;        
    }

    B1_IFG &= ~BUTTON1;           // Clears interrupt on button
}

