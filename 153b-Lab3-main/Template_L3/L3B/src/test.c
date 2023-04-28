	// Enable the timer clock

	//TIM2_CR1
	//Bit 0 CEN: Counter enable
	//0: Counter disabled
	//1: Counter enabled
	TIM2->CR1 |= TIM_CR1_CEN;

	// Counting direction: 0 = up-counting, 1 = down-counting
	//Bit 4 DIR: Direction
	//0: Counter used as upcounter
	//1: Counter used as downcounter
	TIM2->CR1 &= ~(TIM_CR1_DIR); 

  	// Prescaler
	//TIM2_PSC
	TIM2->PSC = 15;

  // Auto-reload
	//TIM2_ARR
	TIM2->ARR = 0xFFFF;

	// Disable output compare mode
	// Clear the output compare mode bits.
	// TIM2_CCMR1 Bits 6:4 OC1M: Output Compare 1 mode -> 0000
	TIM2->CCMR1 &= ~(TIM_CCMR1_OC1M)

	//Set the output compare mode bits to PWM mode 1.
	// PWM mode 1
	//TIM2_CCMR1 Bits 6:4 = 1100
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1;
	
	// Output 1 preload enable
	//TIM2_CCMR1 Bit 3 OC1PE = 1
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE;
	
	// Select output polarity: active high
	//TIM2_CCER Bit 1 CC1P: Capture/Compare 1 output polarity = 0: OC1 active high
	TIM2->CCER &= ~(TIM_CCER_CC1P);

 	// Enable output for ch1
  	//TIM2_CCER Bit 0 CC1E: Capture/Compare 1 output enable = 1 1: On
	TIM2->CCER |= TIM_CCER_CC1E;

	// Output Compare Register for channel 1 
	// TIM2_CCR1 CCR1[15:0]
	// (26.4.14) If channel CC1 is configured as output:: CCR1 is the value to be loaded in the actual capture/compare 1 register (preload value).
	TIM2->CCR1 |= TIM2_CCR1 ?

	// Enable counter
	//TIM2_CR1
	//Bit 0 CEN: Counter enable
	// 0: Counter disabled
	// 1: Counter enabled
	TIM2->CR1 |= TIM_CR1_CEN;