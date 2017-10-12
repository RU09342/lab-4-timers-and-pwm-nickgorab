# Config Header
The purpose of this file is to allow the code to operate on all of the boards. 

## Code Architecture
### Dependencies 
There are no dependencies that are required for this file. However, it defines itself to make sure the other files are able to use it. This is done with the following lines. 

```c
#ifndef CONFIG_H_
#define CONFIG_H_
```

### IFDEF Statements

The purpose of this configuration file is to change the Pin addresses which alter between the boards. In order to do this, `ifdef` statements were used to check what processor is being used. Inside of the `ifdef` statement, there are `#define` statements, which are used to create macros to be used in the main code. Below is an example `ifdef` for the MSP430FR5994.

```c
#ifdef __MSP430FR5994__
    #define BUTTON1     BIT5
    #define BUTTON2     BIT6
    #define LED1        BIT0
    #define LED2        BIT1
    #define B1_DIR      P5DIR
    #define B1_IN       P5IN
    #define B1_REN      P5REN
    #define B1_OUT      P5OUT
    #define B1_IE       P5IE
    #define B1_IES      P5IES
    #define B1_IFG      P5IFG
    #define L1_DIR      P1DIR
    #define L1_OUT      P1OUT
    #define B2_DIR      P5DIR
    #define B2_REN      P5REN
    #define B2_OUT      P5OUT
    #define B2_IE       P5IE
    #define B2_IES      P5IES
    #define B2_IFG      P5IFG
    #define L2_DIR      P1DIR
    #define L2_OUT      P1OUT
    #define HIGHZ       (PM5CTL0 &= ~LOCKLPM5)
#endif
```

Not only does the header file take care of the LED and button definintions and control, it wil disable high-impedance mode for the specified processors. For processors that don't have high impedance mode, the function `HIGHZ` is defined as `0`. 

### Additional Board Support
Support can be added for additional board by adding more `ifdef` statements to the config file. All that needs to be done is the altering of the definitions.

### Additional Command Support
In order to add mode macros, all that needs to be done is the addition of another macro using the `#define` function.