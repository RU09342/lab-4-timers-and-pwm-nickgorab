# Button Debouncing
Uses interrupts for debouncing instead of software delays. 

## Code Architecture
### Dependencies 
The `buttonDebouncing.c` code depends on two separate files. The first file is the generic MSP430  header (`msp430.h`), and a config file (`config.h`) which assigns the correct pins for each board. For more information about these, visit their respective `README.h` files.

```c
#include <msp430.h>
#include <config.h>
```
This is the section of the code which is used to include the header files.
### Functions 

#### Button Initialization

In order to make the code easier to digest, a void function was used to initialize the button.
```c
void buttonInitialize(void)
```
Inside of this function the button pin is declared an input, and the resistor is enabled and assigned to the pull up position. Additionally, the interrupt on the button pin is enabled, cleared, and set to falling edge. This function is then called inside of the main function and reduces the amount of possibly confusing code. The lines of code used to initialize the code are found below. 
```c 
B1_DIR |= ~BUTTON1;
B1_REN |=  BUTTON1;
B1_OUT |=  BUTTON1;
B1_IE  |=  BUTTON1;      
B1_IES |= ~BUTTON1;   
B1_IFG &= ~BUTTON1;   
```

#### LED Initialization
Another void function was used to set up the LEDs.
```c
void ledInitialize(void)
```
It just sets the one LED as an output, probably didnt need to make this function but it was just copied and altered from another code.
```c 
L1_DIR = LED1; 
```

#### Main

In the main file there is no use of interrupts, so the Watch Dog timer needed to be disabled using the line 
```c
WDTCTL = WDTPW+WDTHOLD;
```
With the FR6989, FR2311, and FR5994 needing high impedance mode to be disabled, the line 
```c
HIGHZ;
```
is used to disable it. This is a macro defined in the config.h header file, and it will disable the high impedance mode for the specified board. For the other boards, it just opeartaes as a no-op. 

All of the void functions listed above are called, and then a timer is initalized. 
```c
TA0CCR0 = 10000;                 
TA0CTL = TASSEL_1 + MC_0 + TACLR;
TA0CCTL0 = CCIE;                 
```
The timer is set up to run using SMCLK as the source, and operate in continuous mode. The CCR0 value is chosen to be 10000, long enough to avoid any bouncing.

After the clock is initialized, the program is dropped into low power mode while waiting for the interrupt.
```c
_BIS_SR(LPM0_bits + GIE);
```
This disables the CPU until an interrupt happens. The `GIE` bit at the end enables global interrupts. While there are other Low-Power modes that use less power, this one will cause the least amount of changes that need to be made in the code, preventing the timer source from changing.

#### Button ISR
When the button interrupt occurs, the ISR is entered. Inside of the ISR, the state of the LED is toggled. Then the interrupt on the button is disabled so there is no debouncing. After this, the interrupt flag is removed from the button and then the timer is set to operate in Up-Mode. The button ISR can be found below.
```c
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    L1_OUT ^=  LED1;      
    B1_IE  &= ~BUTTON1;   
    B1_IFG &= ~BUTTON1;   
    TA0CTL |= BIT4;       
}
```
#### Timer ISR
When the timer interrupt goes off, it enters this ISR. Inside, the button interrupt is enabled, and the clock is stopped and reset. After this is done, the interrupt is cleared to make sure that the button will operate correctly once pressed again. The ISR can be found below.
```c
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {


    B1_IE  |= BUTTON1;  
    TA0CTL &= ~BIT4;    
    TA0CTL |= BIT2;     
    B1_IFG &= ~BUTTON1; 
}
```


## Differences Between Boards
As you may have noticed, the MSP430F2311 has its own special folder for itself. This is beacuse the board does not have a Timer_A, so Timer_B needs to be used instead. This is a simple fix, just not something that I included in the `config.h` file. For example, instead of `TA0CCR0`, it changes to `TB0CCR0`.