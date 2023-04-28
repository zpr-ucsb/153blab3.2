/*
 * ECE 153B - Winter 2021
 *
 * Name(s):
 * Section:
 * Lab: 2B
 */

#include "LED.h"

void LED_Init(void) {
	// Enable GPIO Clocks
	RCC->AHB2ENR |= (1<<2) + 1;

   // Initialize Green LED
	 GPIOA->MODER |= 1<<10;
	 GPIOA->MODER &= ~(1<<11);
	 GPIOA->OTYPER &= (~(1<<5));
	 GPIOA->PUPDR &= ~((1<<11)+(1<<10));
}

void Green_LED_Off(void) {
	GPIOA->ODR |= 1<<5;
}

void Green_LED_On(void) {
	GPIOA->ODR &= 1<<5;
}

void Green_LED_Toggle(void) {
	GPIOA->ODR ^= 1<<5;
}