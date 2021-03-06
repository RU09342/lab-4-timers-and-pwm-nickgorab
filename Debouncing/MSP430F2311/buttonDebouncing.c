/*
 * buttonDebouncing.c
 *
 *   Created on:  October 4, 2017
 *  Last Edited:  October 11, 2017
 *       Author:  Nick Gorab
 *        Board:  2311
 */


 #include "msp430.h"
 #include "config.h"

void buttonInitialize(void) {
  B1_DIR |= ~BUTTON1;   // Sets Pin 1.1 as an input
  B1_REN |=  BUTTON1;   // Enables the resistor on Pin 1.1
  B1_OUT |=  BUTTON1;   // Enables the pull-up resistor for Pin 1.1
  B1_IE  |=  BUTTON1;   // Enables the interrupt on Pin 1.1
  B1_IES |= ~BUTTON1;   // Falling edge select
  B1_IFG &= ~BUTTON1;   // Enables the interrupt on Pin 1.1
}

void ledInitialize(void) {
  L1_DIR  |= LED1;
}

void main(void) {


    WDTCTL = WDTPW + WDTHOLD;  // Disables the Watchdog Timer
    HIGHZ;

    buttonInitialize();
    ledInitialize();

    TB0CCR0 = 10000;                  // Sets CCR0 value to avoid bouncing
    TB0CTL = TBSSEL_1 + MC_0 + TBCLR; // SMCLK, continuous, Stopped
    TB0CCTL0 = CCIE;                  // Enables interrupts for clock

  _BIS_SR(LPM0_bits + GIE);   // Enables Low Power Mode 0 and Global Interrupt
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    L1_OUT ^=  LED1;      // Toggles the LED on Pin 1.0 via XOR
    B1_IE  &= ~BUTTON1;   // Disables interrupt
    B1_IFG &= ~BUTTON1;   // Removes interrupt flag
    TB0CTL |= BIT4;       // Sets timer to up-mode
}

#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B (void) {


    B1_IE  |= BUTTON1;  // Enables the interrupt
    TB0CTL &= ~BIT4;    // Stops
    TB0CTL |= BIT2;     // Resets the clock
    B1_IFG &= ~BUTTON1; // Clears interrupt flag
}

