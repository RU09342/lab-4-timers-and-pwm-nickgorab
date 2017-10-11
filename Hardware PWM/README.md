# Hardware PWM
Uses PWM and a button to change the brightness of an LED that is connected to an external pin.

## Code Architecture
### Dependencies 
The `hardwarePWM.c` code depends on two separate files. The first file is the generic MSP430  header (`msp430.h`), and a config file (`config.h`) which assigns the correct pins for each board. For more information about these, visit their respective `README.h` files.

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
Inside of this function the LEDs are set to be an output. This is the only difference between the hardware and software PWM. This sets the PWM signal to one of the GPIO pins, so that an external LED can be powered. In this case, I used Pin 1.4.
```c 
P1OUT  |= BIT4;
P1DIR  |= BIT4;
P1SEL0 |= BIT4;
L2_DIR |= LED2;
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
TA0CCR0 = 1000-1;        
TA0CCTL1 = OUTMOD_7;     
TA0CCR1 = PWM;           
TA0CTL = TASSEL_2 + MC_1;
```
The timer is set up to run using SMCLK as the source, and operate in Up-Mode. The CCR0 value is chosen to be 1000, which gives the light a frequenyc of 1 kHz. A second capture-compare register is used to determine the pule width.

After the clock is initialized, the program is dropped into low power mode while waiting for the interrupt.
```c
_BIS_SR(LPM0_bits + GIE);
```
This disables the CPU until an interrupt happens. The `GIE` bit at the end enables global interrupts. While there are other Low-Power modes that use less power, this one will cause the least amount of changes that need to be made in the code, preventing the timer source from changing.

#### Button ISR
When the button interrupt occurs, the ISR is entered. Inside of the ISR, the button that is not being controlled through PWM is toggled to see if debouncing occurs. To prevent debouncing, a `__delay_cycles()` command is used to halt the program for a little. Once the LED is toggled, the the value of the `CCR1` register is increased, changing the brightness of the LED by 10%. There is an `if` statement to check if the light has reached its maximum brightness, and when it has the `CCR1` value is set to zero. The button ISR is found below.
```c
#pragma vector=PORT1_VECTOR    
__interrupt void Port_1(void) {
    __delay_cycles(500);       
    L2_OUT ^= LED2;            
    TA0CCR1 += 100;            
    if(TA0CCR1 == 1100) {      
        TA0CCR1 = 0;        
    }
    B1_IFG &= ~BUTTON1;        
}
```

## Differences Between Boards
As you may have noticed, the MSP430F2311 has its own special folder for itself. This is beacuse the board does not have a Timer_A, so Timer_B needs to be used instead. This is a simple fix, just not something that I included in the `config.h` file. For example, instead of `TA0CCR0`, it changes to `TB0CCR0`.

## Hardware
In order to power the LED, a circuit was used that is similar to the one used for `offboardBlink.c`, using a BJT as a current amplifier to power the LED.