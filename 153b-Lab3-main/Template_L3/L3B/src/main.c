/*
 * ECE 153B - Winter 2023
 *
 * Lab: 6B
 */

#include "stm32l476xx.h"

void LED_Pin_Init(void);
void TIM2_CH1_Init(void);
void SERVO_Pin_Init(void);
void TIM5_CH1_Init(void);

static uint32_t pscValLED = 99;
static uint32_t arrValLED = 300;

// Prescaler = (clock frequency / desired timer tick frequency) - 1
// ARR = (desired period / timer tick period) - 1

// 4MHz MSI clock. Set the prescaler TIM5->PSC to count at 100kHz.
// period of 0.02s (50Hz)

static uint32_t pscValSERVO = 39;
static uint32_t arrValSERVO = 1999;
static int delay = 1000;

void LED_Pin_Init(void){
  // Enable GPIO Clocks
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	

	// Initialize Green LED
	// GPIO Mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE5);
	GPIOA->MODER |= GPIO_MODER_MODE5_1;
	
	// Alternative Function
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL5); 
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL5_0;
	
	//Set I/O output speed value as very high speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5_1 | GPIO_OSPEEDER_OSPEEDR5_0;
	
	// GPIO Push-Pull: No pull-up, no pull-down
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5;
}
	

void TIM2_CH1_Init(void){
	// Enable the timer clock
	//TIM2->CR1 |= TIM_CR1_CEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	
	// Counting direction: 0 = up-counting, 1 = down-counting
	//Bit 4 DIR: Direction
	//0: Counter used as upcounter
	//1: Counter used as downcounter
	TIM2->CR1 &= ~(TIM_CR1_DIR); 

	// Prescaler
	TIM2->PSC = pscValLED;

  // Auto-reload
	TIM2->ARR = arrValLED;  

	// Disable output compare mode
	TIM2->CCMR1 &= ~(TIM_CCMR1_OC1M);

	//Set the output compare mode bits to PWM mode 1. (0110)
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1;
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_2;
	
	// Output 1 preload enable
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE;
	
	// Select output polarity: active high
	TIM2->CCER &= ~(TIM_CCER_CC1P);

 	// Enable output for ch1
	TIM2->CCER |= TIM_CCER_CC1E;

	// Output Compare Register for channel 1 
	TIM2->CCER |= TIM_CCER_CC1E;

	//Set the capture/compare value to 50% for now.
	TIM2->CCR1 = 1;

	// Enable counter
	TIM2->CR1 |= TIM_CR1_CEN;
}

void SERVO_Pin_Init(void){
	// Set mode as Alternative Function 1
	GPIOA->MODER &= ~(GPIO_MODER_MODE0);
	GPIOA->MODER |= GPIO_MODER_MODE0_1;
	
	// Alternative Function
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL0);
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL0_0;

	//Set I/O output speed value as very high speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_1 | GPIO_OSPEEDER_OSPEEDR0_0;

	//Set I/O as no pull-up pull-down 
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR0;
}

void TIM5_CH1_Init(void){
	// Enable the timer clock
	//TIM5->CR1 |= TIM_CR1_CEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
	
	// Counting direction: 0 = up-counting, 1 = down-counting
	//Bit 4 DIR: Direction
	//0: Counter used as upcounter
	//1: Counter used as downcounter
	TIM5->CR1 &= ~(TIM_CR1_DIR); 

	// Prescaler
	TIM5->PSC = pscValSERVO; // Prescaler = (clock frequency / desired timer tick frequency) - 1

  // Auto-reload
	TIM5->ARR = arrValSERVO; // ARR = (desired period / timer tick period) - 1

	// Disable output compare mode
	TIM5->CCMR1 &= ~(TIM_CCMR1_OC1M);

	//Set the output compare mode bits to PWM mode 1. (0110)
	TIM5->CCMR1 |= TIM_CCMR1_OC1M_1;
	TIM5->CCMR1 |= TIM_CCMR1_OC1M_2;
	
	// Output 1 preload enable
	TIM5->CCMR1 |= TIM_CCMR1_OC1PE;
	
	// Select output polarity: active high
	TIM5->CCER &= ~(TIM_CCER_CC1P);

 	// Enable output for ch1
	TIM5->CCER |= TIM_CCER_CC1E;

	// Output Compare Register for channel 1 
	TIM5->CCER |= TIM_CCER_CC1E;
 
	// Set the output compare register for channel 1 TIM5->CCR1 to have a duty cycle of 50%:
	TIM5->CCR1 = 100;

	// Enable counter
	TIM5->CR1 |= TIM_CR1_CEN;
}

int main(void) {
	int i;
	int led = 0;
	int dire = 1;
	
	LED_Pin_Init();
	SERVO_Pin_Init();
	TIM2_CH1_Init();
	TIM5_CH1_Init();
		
    while(1) {

		if (TIM2->CCR1 == arrValLED)
			led = 1;
		if (TIM2->CCR1 == 1)
			led = 0;
		
		if (led == 1)
			TIM2->CCR1 -= 1;
		else
			TIM2->CCR1 += 1;
		
		// -90
		// Pulse Width should be 1ms = 0.001s. Period is 0.02s, so set Duty Cycle to 5% to get 0.001s
		
		// Center (0)
		// Pulse Width should be 1.5ms = 0.0015s. Period is 0.02s, so set Duty Cycle to 7.5% to get 0.0015s
		
		// 90
		// Pulse Width should be 2ms = 0.002s. Period is 0.02s, so set Duty Cycle to 10% to get 0.002s
		
		if(TIM5->CCR1 == arrValSERVO)
		{
			dire = dire + 1;
		
			if (dire == 1) 
				TIM5->CCR1 = 225; 

			if (dire == 2)
				TIM5->CCR1 = 125;
			
			if (dire == 3)
				TIM5->CCR1 = 45;
			
			if (dire == 4)
			{
				TIM5->CCR1 = 125;
				dire = 1;
			}
	}
	
		for(i=0;i<delay;i++);  		// delay
    }
}
