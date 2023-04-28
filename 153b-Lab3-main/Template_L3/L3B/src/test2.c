	// [TODO]
	GPIOA->MODER &= ~(GPIO_MODER_MODE9);        // Clear Port A, Pin 9 MODER

	//Configure PA9 to be used as alternative function TIM1 CH2
  	GPIOA->MODER |= GPIO_MODER_MODE9_1; 			// Set mode to alternative funtion
	GPIOA->AFR[0] &= ~(GPIO_AFRH_AFSEL9);			// Clear AF1 for Port A, Pin 9
	GPIOA->AFR[0] |= GPIO_AFRH_AFSEL9;				// Choose AF1 (TIM1_CH2) for Port A, Pin 9

	//Set PA9 to no pull-up, no pull-down.
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9);
	// Set the output type of PA9 to push-pull.
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT9);
	
	//Set PA9 to very high output speed.
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED9);


	//Enable Timer 1 in RCC APB2ENR.
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	//Set the prescaler to 15.
	TIM1->PSC = 15;	

	//Enable auto reload preload in the control register and set the auto reload value to its maximum value.
	TIM1->CR1 |= TIM_CR1_ARPE;
	TIM1->ARR = 0xFFFF;

	//Set the CCR value that will trigger the sensor. 
	TIM1->CCMR1 &= ~(TIM_CCMR1_CC1S);
	TIM1->CCMR1 |= TIM_CCMR1_CC1S_0;

	// In the capture/compare mode register, set the output compare mode such that the timer operates in PWM Mode 1 and enable output compare preload.
	TIM1->CCER |= TIM_CCER_CC1NP;
	TIM1->CCER |= TIM_CCER_CC1E;
	TIM1->CCER |= TIM_CCER_CC1P;

	// Enable the output in the capture/compare enable register.
	TIM1->DIER |= TIM_DIER_CC1IE;
	TIM1->DIER |= TIM_DIER_CC1DE
	TIM1->DIER |= TIM_DIER_UIE;
	TIM1->EGR |= TIM_EGR_UG;
	//In the break and dead-time register, set the bits (at the same time) for main output enable, off-state selection for run mode, and off-state selection for idle mode.
