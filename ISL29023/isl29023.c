// isl29023.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Modified from TivaWare program
//
// Requirements:
// 	Requires Texas Instruments' TivaWare.
//
// Description:
//	Interfaces with Intersil ISL29023 ambient light sensor
//
// Notes:
//	
//
//****************************************************************************************************


// Includes ------------------------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"

#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "utils/uartstdio.h"



// Defines -------------------------------------------------------------------------------------------
#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

#define ISL29023_I2C_ADDRESS 0x44
#define ISL29023_REG_COMMANDI 0x00
#define ISL29023_REG_COMMANDII 0x01
#define ISL29023_REG_DATALSB 0x02
#define ISL29023_REG_DATAMSB 0x03
#define ISL29023_REG_LOWINTLSB 0x04
#define ISL29023_REG_LOWINTMSB 0x05
#define ISL29023_REG_UPINTLSB 0x06
#define ISL29023_REG_UPINTMSB 0x07


// Variables -----------------------------------------------------------------------------------------


// Functions -----------------------------------------------------------------------------------------
void ConfigureUART(void){

	// Enable the peripherals used by UART
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	// Set GPIO A0 and A1 as UART pins.
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

        // Configure UART clock using UART utils
        UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
        UARTStdioConfig(0, 115200, 16000000);
}

void ConfigureI2C3(void){
	
	// Enable peripherals used by I2C
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C3);

	// Setup GPIO
	ROM_GPIOPinTypeI2CSCL(GPIO_PORTD_BASE, GPIO_PIN_0);
	ROM_GPIOPinTypeI2C(GPIO_PORTD_BASE, GPIO_PIN_1);

	// Set GPIO D0 and D1 as SCL and SDA
	ROM_GPIOPinConfigure(GPIO_PD0_I2C3SCL);
	ROM_GPIOPinConfigure(GPIO_PD1_I2C3SDA);

	// Initialize as master - 'true' for fastmode, 'false' for regular
	ROM_I2CMasterInitExpClk(I2C3_BASE, ROM_SysCtlClockGet(), true);

}

void FloatToPrint(float floatValue, uint32_t splitValue[2]){
	int32_t i32IntegerPart;
	int32_t i32FractionPart;

        i32IntegerPart = (int32_t) floatValue;
        i32FractionPart = (int32_t) (floatValue * 1000.0f);
        i32FractionPart = i32FractionPart - (i32IntegerPart * 1000);
        if(i32FractionPart < 0)
        {
            i32FractionPart *= -1;
        }

	splitValue[0] = i32IntegerPart;
	splitValue[1] = i32FractionPart;
}



// Main ----------------------------------------------------------------------------------------------
int main(void){

	// Enable lazy stacking
	ROM_FPULazyStackingEnable();

	// Set the system clock to run at 40Mhz off PLL with external crystal as reference.
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	// Initialize the UART and write status.
	ConfigureUART();
	UARTprintf("I2C3 Example\n");

	// Enable LEDs
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

	// Enable I2C3
	ConfigureI2C3();

	// Create print variables
	uint32_t printValue[2];

	while(1){

		// Blink LED
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_GREEN);
		ROM_SysCtlDelay(ROM_SysCtlClockGet()/3/10);	// Delay for 100ms (1/10s) :: ClockGet()/3 = 1second
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);

		// Delay for second
		ROM_SysCtlDelay(ROM_SysCtlClockGet()/3);
	}

}