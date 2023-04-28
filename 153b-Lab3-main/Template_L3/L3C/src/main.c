#include <stdio.h> 
 
#include "stm32l476xx.h"

uint32_t volatile currentValue = 0;
uint32_t volatile lastValue = 0;
uint32_t volatile overflowCount = 0;
uint32_t volatile timeInterval = 0;
uint32_t volatile dist = 0;

void Input_Capture_Setup() {
	//  1. Set up PB6
	// Enable the clock for GPIO Port B.
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

	GPIOB->MODER &= ~(GPIO_MODER_MODE6);       		// Clear PB6 MODER
  	GPIOB->MODER |= GPIO_MODER_MODE6_1;         	// Set PB6 MODER to Alternative Mode
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6);			// Set PB6 PUPDR to set it to No Pull Up, No Pull Down (Cleared)
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6);			// Clear AFR[0] for PB6
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_1;			// Set AFR[0] func to AF2 (TIM4_CH1) for PB6

	// 2. Enable Timer 4 in RCC APB1ENR
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;			

	// 3. Set the prescaler to 15.
	TIM4->PSC = 15;
	
	// 4. Enable auto reload preload in the control register and set the auto reload value to its maximum value.
	TIM4->CR1 |= TIM_CR1_ARPE;
	TIM4->ARR = 0xFFFF;
	
	// 5. In the capture/compare mode register, set the input capture mode bits such that the input capture is mapped to timer input 1.
	TIM4->CCMR1 &= ~(TIM_CCMR1_CC1S);
	TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;
	
	// 6. In the capture/compare enable register, set bits to capture both rising/falling edges and enable capturing.
	TIM4->CCER |= TIM_CCER_CC1P;
	TIM4->CCER |= TIM_CCER_CC1NP;
	TIM4->CCER |= TIM_CCER_CC1E;
	
	// 7. In the DMA/Interrupt enable register, enable both interrupt and DMA requests. In addition, enable the update interrupt.
	TIM4->DIER |= TIM_DIER_CC1IE;
	TIM4->DIER |= TIM_DIER_CC1DE;
	TIM4->DIER |= TIM_DIER_UIE;	
	
	// 8. Enable update generation in the event generation register.
	TIM4->EGR |= TIM_EGR_UG;
	
	// 9. Clear the update interrupt flag.
	TIM4->SR &= ~(TIM_SR_UIF);
	
	// 10. Set the direction of the counter and enable the counter in the control register.						
	TIM4->CR1 &= ~(TIM_CR1_DIR);
	TIM4->CR1 |= TIM_CR1_CEN;

	// 11. Enable the interrupt (TIM4_IRQn) in the NVIC and set its priority to 2.
	NVIC_EnableIRQ(TIM4_IRQn);
	NVIC_SetPriority(TIM4_IRQn, 2);
}

void TIM4_IRQHandler(void) {
	// Check for overflow interupt at overflow interupt flag
	if (TIM4->SR & TIM_SR_UIF) {
		// if overflow interupt occours, reset flag and increase overflowCount
		TIM4->SR &= ~(TIM_SR_UIF);
		overflowCount++;
	}

	//Check timer 4 status register (compare chanel 1 inturupt flag) 
	if (TIM4->SR & TIM_SR_CC1IF) {

		//Reset interrupt flag
		TIM4->SR &= ~(TIM_SR_CC1IF);

		// Check if rising edge interrupt is triggered
		if (GPIOB->IDR & GPIO_IDR_ID6) {
			// record the compare channel register value value in rising edge CCR
			lastValue = TIM4->CCR1;
			overflowCount = 0;
		}
		else { // Else: falling edge interrupt is triggered:

			//r ecord the compare channel register value in falling edge CCR
			currentValue = TIM4->CCR1;
			timeInterval = (currentValue + overflowCount * 0xFFFF)  - lastValue;
		}
	}
}

void Trigger_Setup() {		
	// 1. Set up PA9.
	// Enable the clock for GPIO Port A.
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
		
	GPIOA->MODER &= ~(GPIO_MODER_MODE9);       		// Clear Port A, Pin 9 MODER
  	GPIOA->MODER |= GPIO_MODER_MODE9_1;         	// Set Port A, Pin 9 MODER to Alternative Mode
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED9);		// Set Port A, Pin 9 OSPEEDR to Very High
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9);			// Clear Port A, Pin 9 PUPDR to set it to No Pull Up, No Pull Down
	GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL9);			// Clear AFR[0] for Port A, Pin 9
	GPIOA->AFR[1] |= GPIO_AFRH_AFSEL9_0;			// Choose AF1 (TIM1_CH2) for Port A, Pin 9
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT9);			// Set the output type of PA9 to push-pull.		

	// 2. Enable Timer 1 in RCC APB2ENR.
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;		

	// 3. Set the prescaler to 15.
	TIM1->PSC = 15;

	// 4. Enable auto reload preload in the control register and set the auto reload value to its maximum value (0xFFFF)
	TIM1->CR1 |= TIM_CR1_ARPE;
	TIM1->ARR = 0xFFFF;

	// 5. Set the CCR value that will trigger the sensor
	TIM1->CCR2 = 10;

	// 6. In the capture/compare mode register, set the output compare mode such that the timer operates in PWM Mode 1 and enable output compare preload.
	TIM1->CCMR1 &= ~(TIM_CCMR1_OC2M);
	TIM1->CCMR1 |= TIM_CCMR1_OC2M_1;
	TIM1->CCMR1 |= TIM_CCMR1_OC2M_2;
	TIM1->CCMR1 |= TIM_CCMR1_OC2PE;
	TIM1->CCER &= ~(TIM_CCER_CC2P);					// **May not be Necessary -> Setting the output polarity for compare 1 to active high.
	
	// 7. Enable the output in the capture/compare enable register.
	TIM1->CCER |= TIM_CCER_CC2E;


	// 8. In the break and dead-time register, set the bits (at the same time) for main output enable, off-state selection for run mode, and off-state selection for idle mode.
	TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_OSSR | TIM_BDTR_OSSI;

	// 9. Enable update generation in the event generation register.
	TIM1->EGR |= TIM_EGR_UG;	
	// 10. Enable update interrupt in the DMA/Interrupt enable register and clear the update interrupt flag in the status register.
	TIM1->DIER |= TIM_DIER_UIE;
	TIM1->SR &= ~(TIM_SR_UIF);

	//11. Set the direction of the counter and enable the counter in the control register.
	TIM1->CR1 &= ~(TIM_CR1_DIR);
	TIM1->CR1 |= TIM_CR1_CEN;
}

int main(void) {	
	// Enable High Speed Internal Clock (HSI = 16 MHz)
	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0); // Wait until HSI is ready
	
	// Select HSI as system clock source 
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while ((RCC->CFGR & RCC_CFGR_SWS) == 0); // Wait until HSI is system clock source
  
	// Input Capture Setup
	Input_Capture_Setup();
	
	// Trigger Setup
	Trigger_Setup();
	
	while(1) {
		// distance (cm) = pulse width (timeInterval in microsec) / 58
		dist = timeInterval / 58;
	}
}