#include "stm32f0xx.h"
#include <stdio.h>
#include <string.h>
//------------------------------------------------Variables ----------------------------------------
int RightMotorForward = 0b1; //Shrivel left
int RightMotorBack = 0b010;
int LeftMotorForward = 0b100000; //shrivel right
int TurnLeftMotors = 0b1000001;
int TurnRightMotors = 0b0100010;
int ReverseMotors = 0b1000010;
int BothForward = 0b0100001;
//------------------------------------------------FUNCTION DEFINITIONS------------------------------
void init_GPIO(void);
void init_pwm(void);
void init_EXTI(void);
void init_NVIC(void);


// --------------------------------------------------MAIN-------------------------------------------
void main (void){
  init_GPIO();
	init_pwm();
	init_EXTI();
	init_NVIC();
	for(;;){}
}
// ---------------------------------------------INIT GPIO-------------------------------------------
void init_GPIO(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;//Enable clock on GPIOB
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;//Enable clock on GPIOA

	GPIOA -> MODER |= 0b0 ; //sets all sensors to input mode - not really necessary

	/* SENSORS */
	/* setting PA8 and PB 12-15 to inputs for sensor */
	//GPIOA->MODER &=~(GPIO_MODER_MODER8);
    //GPIOB->MODER &=~(GPIO_MODER_MODER12|GPIO_MODER_MODER13|GPIO_MODER_MODER14|GPIO_MODER_MODER15);
	/* pull down resistor for pin 8 and 9 of port A and pin 12-15 of Port B */
	GPIOA->PUPDR&=~(GPIO_PUPDR_PUPDR8|GPIO_PUPDR_PUPDR9);
	GPIOA->PUPDR|=(GPIO_PUPDR_PUPDR8_1|GPIO_PUPDR_PUPDR9_1);
    GPIOB->PUPDR&=~(GPIO_PUPDR_PUPDR12|GPIO_PUPDR_PUPDR13|GPIO_PUPDR_PUPDR14|GPIO_PUPDR_PUPDR15);
    GPIOB->PUPDR|=(GPIO_PUPDR_PUPDR12_1|GPIO_PUPDR_PUPDR13_1|GPIO_PUPDR_PUPDR14_1|GPIO_PUPDR_PUPDR15_1);

	/* Start up procedure pins */
	/* inputs: start switch PB10 and LED0 PB1 */
    	GPIOB->MODER &=~(GPIO_MODER_MODER10|GPIO_MODER_MODER1);//set mode on PB10 to
	/* Pull-up resistor for start switch PB10 */
    	GPIOB->PUPDR&=~(GPIO_PUPDR_PUPDR10);
    	GPIOB->PUPDR|=(GPIO_PUPDR_PUPDR10_0);

/* Motor IC */
	GPIOA ->MODER |=0b01010010100101;//Set PA3, PA2 AF mode and set PA0,A1,A5,A6 to output mode
	GPIOA ->ODR |= 0b1000010; //Turns motor
}

void init_pwm(void)
{
  	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;


  	GPIOA->AFR[1] |= (2 << (4*(3 - 8))); // PA3_AF = AF2 (ie: map to TIM2_CH3) EN1
  	GPIOA->AFR[1] |= (2 << (4*(2 - 8))); // PA2_AF = AF2 (ie: map to TIM2_CH4) EN2

  	TIM2->ARR = 8000;  // f = 1 KHz
  	// specify PWM mode: OCxM bits in CCMRx. We want mode 1
  	TIM2->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1); // PWM Mode 1
  	TIM2->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1); // PWM Mode 1
  	// set PWM percantages
  	TIM2->CCR3 =  100* 80; // Red = 20%
  	TIM2->CCR4 = 100 * 80; // Green = 90%

  	// enable the OC channels
  	TIM2->CCER |= TIM_CCER_CC3E;
  	TIM2->CCER |= TIM_CCER_CC4E;

  	TIM2->CR1 |= TIM_CR1_CEN; // counter enable

}

void init_EXTI(void) {
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN; // clock for the system configuration controller
  SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA; // set interrupt 12 to be sourced from port A for EXTICR4
  //EXTI->IMR |= EXTI_IMR_MR0; // un-mask the interrupt ?? is this necessary ??
  EXTI->RTSR |= EXTI_RTSR_TR0; // enable the rising edge trigger for interrupt 12

}

void init_NVIC(void) {
  NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void EXTI4_15_IRQHandler(void) {
  // clear the interrupt pending bit by writing to it
  EXTI->PR |= EXTI_PR_PR0;
  if (!GPIOB->IDR & GPIO_IDR_10){
	  GPIOA ->ODR |= TurnLeftMotors;
  }
  if (GPIOB->IDR & GPIO_IDR_14){ // |
	  //go straight motor
  }
  if ((GPIOB->IDR & GPIO_IDR_12)&&(GPIOB->IDR & GPIO_IDR_13)&&(GPIOB->IDR & GPIO_IDR_14)){ // |-
	  // go straight
  }
  if ((GPIOB->IDR & GPIO_IDR_12)&&(GPIOB->IDR & GPIO_IDR_13)){ // -
	  //go right
  }
  if ((GPIOA->IDR & GPIO_IDR_8)&&(GPIOB->IDR & GPIO_IDR_15)&&(GPIOB->IDR & GPIO_IDR_14)){ // -|
	  //go left
  }
  if ((GPIOA->IDR & GPIO_IDR_8)&&(GPIOB->IDR & GPIO_IDR_15)){ // -
	  //go left
  }
  if ((GPIOA->IDR & GPIO_IDR_8)&&(GPIOB->IDR & GPIO_IDR_15)&&(GPIOB->IDR & GPIO_IDR_13)&&(GPIOB->IDR & GPIO_IDR_12)){ //T
	  //go left
  }
  if ((GPIOA->IDR & GPIO_IDR_8)&&(GPIOB->IDR & GPIO_IDR_15)&&(GPIOB->IDR & GPIO_IDR_14)&&(GPIOB->IDR & GPIO_IDR_13)&&(GPIOB->IDR & GPIO_IDR_12)){ //4 way
	  //go left
  }
  if ((GPIOA->IDR & GPIO_IDR_8)&&(GPIOB->IDR & GPIO_IDR_15)&&(GPIOB->IDR & GPIO_IDR_14)&&(GPIOB->IDR & GPIO_IDR_13)&&(GPIOB->IDR & GPIO_IDR_12)&&(GPIOA->IDR & GPIO_IDR_9)){
	  //Stop
	  //Flash LED
  }

}

