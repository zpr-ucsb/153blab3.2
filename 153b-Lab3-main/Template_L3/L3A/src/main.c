/*
 * ECE 153B - Winter 2023
 *
 * Name(s):
 * Section:
 * Lab: 2B
 */

#include "stm32l476xx.h"
#include "LED.h"
#include "SysTimer.h"

void System_Clock_Init(void) {
	// Select MSI as system clock source
	RCC->CFGR |= RCC_CFGR_SW_MSI; // [TODO] - Replace with value that will make MSI the system clock source
	
	// Set MSI clock range
	RCC->CR &= ~RCC_CR_MSIRANGE;
	RCC->CR |= RCC_CR_MSIRANGE_7; // [TODO] - Replace with value that will make range 8 MHz
	// Bits 7:4 MSIRANGE[3:0]: 0111: range 7 around 8 MHz

	// Use the MSI clock range that is defined in RCC_CR
	RCC->CR |= RCC_CR_MSIRGSEL; // [TODO] - Replace with value that will select range in RCC->CR
	//Bit 3 MSIRGSEL: 1: MSI Range is provided by MSIRANGE[3:0] in the RCC_CR register
	
	// Enable MSI oscillator
	RCC->CR |= RCC_CR_MSION; // [TODO] - Replace with value that will enable MSI
	//Bit 0 MSION: 1: MSI oscillator ON
	
	// Wait until MSI is ready
	//Bit 1 MSIRDY: MSI clock ready flag 1: MSI oscillator ready
	/* [TODO] - Replace with value that checks whether MSI is ready */
	while((RCC->CR & RCC_CR_MSIRDY) == 0);
}

int main() {
	// Initialization 
	System_Clock_Init();
	SysTick_Init();
	LED_Init();
	
	while(1) {
		delay(100);
		Green_LED_Toggle();
	}
}
