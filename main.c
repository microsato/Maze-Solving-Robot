#include "stm32f0xx.h"
#include <stdio.h>
#include <string.h>
//------------------------------------------------Variables ----------------------------------------
typedef int bool;
#define true 1
#define false 0
bool buttonPressed=false;
int RightMotorForward = 0b1; //Shrivel left
int RightMotorBack = 0b010;
int LeftMotorForward = 0b100000; //shrivel right
int TurnLeftMotors = 0b1000001;
int TurnRightMotors = 0b0100010;
int ReverseMotors = 0b1000010;
int BothForward = 0b0100001;
bool doneTurning = true;
//------------------------------------------------FUNCTION DEFINITIONS------------------------------
void init_GPIO(void);
void init_pwm(void);
void init_EXTI(void);
void init_NVIC(void);
void turnLeft(void);
void turnRight(void);
void straight(void);
void turnAround (void);
void endMaze(void);
void stop(void);

// --------------------------------------------------MAIN-------------------------------------------
void main (void){
  	init_GPIO();
	init_pwm();
	
	if (!(GPIOB->IDR & GPIO_IDR_10)){ //if button on PB10 pressed (goes low)
		buttonPressed = true;
	} 

	while(buttonPressed){
	if (GPIOB->IDR & GPIO_IDR_14){ //if only s3 high 
	  straight();
	  //go straight motor
  	}
  	else if ((GPIOB->IDR & GPIO_IDR_12)&&(GPIOB->IDR & GPIO_IDR_13)&&(GPIOB->IDR & GPIO_IDR_14)){ // |-
		straight();
	  // go straight
  	}
  	else if ((GPIOB->IDR & GPIO_IDR_12)&&(GPIOB->IDR & GPIO_IDR_13)){ // -
	  stop();
	  turnRight(); //lock rotation - need to make 
	  doneTurning = false;
	  //delay
	  while (!doneTurning){
		  //might need delay here 
		  if (GPIOB->IDR & GPIO_IDR_14){ //when s3 is high again it means we have reached turning destination
			  doneTurning = true; //might need to add more sensor checks 
		  }
	  }
	  
  	}
  	else if ((GPIOA->IDR & GPIO_IDR_8)&&(GPIOB->IDR & GPIO_IDR_15)&&(GPIOB->IDR & GPIO_IDR_14)){ // -|
	  stop();
	  turnLeft(); //need to make
	  doneTurning = false;
	  //delay
	  while (!doneTurning){
		  if (GPIOB->IDR & GPIO_IDR_14){ //when s3 is high again it means we have reached turning destination
			  doneTurning = true; //might need to add more sensor checks 
		  }
	  }
  	}
  	else if ((GPIOA->IDR & GPIO_IDR_8)&&(GPIOB->IDR & GPIO_IDR_15)){ // -
	  stop();
	  turnLeft(); //need to make
	  doneTurning = false;
	  //delay
	  while (!doneTurning){
		  if (GPIOB->IDR & GPIO_IDR_14){ //when s3 is high again it means we have reached turning destination
			  doneTurning = true; //might need to add more sensor checks 
		  }
	  }
  	}
  	else if ((GPIOA->IDR & GPIO_IDR_8)&&(GPIOB->IDR & GPIO_IDR_15)&&(GPIOB->IDR & GPIO_IDR_13)&&(GPIOB->IDR & GPIO_IDR_12)){ //T
	  stop();
	  turnLeft(); //need to make
	  doneTurning = false;
	  //delay
	  while (!doneTurning){
		  if (GPIOB->IDR & GPIO_IDR_14){ //when s3 is high again it means we have reached turning destination
			  doneTurning = true; //might need to add more sensor checks 
		  }
	  }
  	}
  	else if ((GPIOA->IDR & GPIO_IDR_8)&&(GPIOB->IDR & GPIO_IDR_15)&&(GPIOB->IDR & GPIO_IDR_14)&&(GPIOB->IDR & GPIO_IDR_13)&&(GPIOB->IDR & GPIO_IDR_12)){ //4 way
	  stop();
	  turnLeft(); //need to make
	  doneTurning = false;
	  //delay
	  while (!doneTurning){
		  if (GPIOB->IDR & GPIO_IDR_14){ //when s3 is high again it means we have reached turning destination
			  doneTurning = true; //might need to add more sensor checks 
		  }
	  }
  	}
  	else if ((GPIOA->IDR & GPIO_IDR_8)&&(GPIOB->IDR & GPIO_IDR_15)&&(GPIOB->IDR & GPIO_IDR_14)&&(GPIOB->IDR & GPIO_IDR_13)&&(GPIOB->IDR & GPIO_IDR_12)&&(GPIOA->IDR & GPIO_IDR_9)){
	  stop();
	  endMaze();//need to make
	
  	}
	else{ //no sensors are high = deadend 
		stop();
		turnAround();
		doneTurning = false;
		//delay 
	  while (!doneTurning){
		  if (GPIOB->IDR & GPIO_IDR_14){ //when s3 is high again it means we have reached turning destination
			  doneTurning = true; //might need to add more sensor checks 
		  }
	  }
	}


	}
}
//--------------------------------------------TURN FUNCTIONS---------------------------------------
void turnLeft(void){
 	GPIOA ->ODR |= LeftMotorForward;
}

void turnRight(void){
	GPIOA ->ODR |= RightMotorForward;
}

void turnAround(void){
	GPIOA->ODR |= TurnRightMotors;
}

void endMaze(void){
	buttonPressed= false;
	GPIOB->ODR|=0b10; //set LED on PB1 high
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
	GPIOA ->ODR |=0b0000000000000000; //set all outputs low initially
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
