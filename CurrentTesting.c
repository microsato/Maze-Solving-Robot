/*------------------------------------------------EEE3099S LINE FOLLOWER ROBOT----------------------
 * Title: 		EEE3099S LINE FOLLOWER ROBOT
 * Authors: 		Mic Rosato, Kai Brown, Jack Forrest, Tapiwa Courtz
 * Date Created:	19/09/2019
 * Last Modified:	23/09/2019
*/
//------------------------------------------------Included Libraries--------------------------------
#include "stm32f0xx.h"
#include <stdio.h>
#include <string.h>
/*------------------------------------------------I/O Explanation-----------------------------------
Sensors: (inputs)
Sensor 1 (outer right) = PB12
Sensor 2 (inner right) = PB13
Sensor 3 (middle) = PB14
Sensor 4 (inner left right) = PB15
Sensor 5 (outer left) = PA8
Sensor 6 (back) = PA9

Motor Channels: (outputs) -DO NOT ENABLE FORWARD AND BACK TOGETHER
Right Motor: PA0 = forward, PA1 = back
Left Motor: PA5 = forward, PA6 = back

SW0: Reset (pin 7)
SW1: PB10 (input) (needs Pull up - switch connects to ground)
LED0: PB1
*/
//------------------------------------------------Variables ----------------------------------------
typedef int bool;
#define true 1
#define false 0
bool buttonPressed= false;
bool doneTurning = true;
//state variable: 0- waiting, 1- solving, 2- paused(solving), 3- waiting(solved), 4- racing, 5- paused(racing).

//motor commands
int AllOutputsLow = ~(GPIO_ODR_0|GPIO_ODR_1|GPIO_ODR_5|GPIO_ODR_6); //and with GPIOA_ODR before new command
int RightMotorForward = GPIO_ODR_0; //Shrivel left
int RightMotorBack = GPIO_ODR_1;
int LeftMotorForward = GPIO_ODR_5; //shrivel right
int LeftMotorBack = GPIO_ODR_6;
int TurnMotorsRight = GPIO_ODR_5|GPIO_ODR_1;
int TurnMotorsLeft = GPIO_ODR_6|GPIO_ODR_0;
int MotorsReverse = GPIO_ODR_1|GPIO_ODR_6;
int MotorsForward = GPIO_ODR_0|GPIO_ODR_5;
//sensor commands
int S1 = GPIO_IDR_12;
int S2 = GPIO_IDR_13;
int S3 = GPIO_IDR_14;
int S4 = GPIO_IDR_15;
int S5 = GPIO_IDR_8;
int S6 = GPIO_IDR_9;
int AllInputsLowA = GPIO_IDR_8|GPIO_IDR_9;
int AllInputsLowB = GPIO_IDR_12|GPIO_IDR_8|GPIO_IDR_8|GPIO_IDR_8;
//Switch and LED
int SW1 = GPIO_IDR_10;
int LED = GPIO_ODR_1;

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
while(1){
	if (!(GPIOB->IDR & GPIO_IDR_10)){ //if button on PB10 pressed (goes low)
		buttonPressed = true;
		GPIOB -> ODR |=0b10;

	}

	while(buttonPressed){

	if ((GPIOB->IDR & S3 )){ //if only s3 high
	  straight();
	  //go straight motor
  	}
	else if (GPIOB->IDR S2){
		turnLeft();
		doneTurning = false;
		//delay
		while (!doneTurning){
			//might need delay here
			if ((GPIOB->IDR & S3)&&(GPIOB->IDR & S3)){ //when s3 is high again it means we have reached turning destination
				doneTurning = true; //might need to add more sensor checks
			}
		}
	}
  	else if ((GPIOB->IDR & S1)&&(GPIOB->IDR & S2)&&(GPIOB->IDR & S3)){ // |-
		straight();
	  // go straight
  	}
  	else if ((GPIOB->IDR & S1)&&(GPIOB->IDR & S2)){ // -
	  stop();
	  turnRight(); //lock rotation - need to make
	  doneTurning = false;
	  //delay
	  while (!doneTurning){
		  //might need delay here
		  if ((GPIOB->IDR & S3)){ //when s3 is high again it means we have reached turning destination
			  doneTurning = true; //might need to add more sensor checks
		  }
	  }

  	}
  	else if ((GPIOA->IDR & S5)&&(GPIOB->IDR & S4)&&(GPIOB->IDR & S3)){ // -|
	  stop();
	  turnLeft(); //need to make
	  doneTurning = false;
	  //delay
	  while (!doneTurning){
		  if (GPIOB->IDR & S3){ //when s3 is high again it means we have reached turning destination
			  doneTurning = true; //might need to add more sensor checks
		  }
	  }
  	}
  	else if ((GPIOA->IDR & S5)&&(GPIOB->IDR & S4)){ // -
	  stop();
	  turnLeft(); //need to make
	  doneTurning = false;
	  //delay
	  while (!doneTurning){
		  if ((GPIOB->IDR & S3)&&(GPIOB->IDR & S1)&&(GPIOB->IDR & S2)&&(GPIOB->IDR & S4)&&(GPIOA->IDR & S5)){ //when s3 is high again it means we have reached turning destination
			  doneTurning = true; //might need to add more sensor checks
		  }
	  }
  	}
  	else if ((GPIOA->IDR & S5)&&(GPIOB->IDR & S4)&&(GPIOB->IDR & S2)&&(GPIOB->IDR & S1)){ //T
	  stop();
	  turnLeft(); //need to make
	  doneTurning = false;
	  //delay
	  while (!doneTurning){
		  if ((GPIOB->IDR & S3)&&(GPIOB->IDR & S1)&&(GPIOB->IDR & S2)&&(GPIOB->IDR & S4)&&(GPIOA->IDR & S5)){ //when s3=high,s1,s2,s4,s5=low again it means we have reached turning destination
			  doneTurning = true; //might need to add more sensor checks
		  }
	  }
  	}
  	else if ((GPIOA->IDR & S5)&&(GPIOB->IDR & S4)&&(GPIOB->IDR & S3)&&(GPIOB->IDR & S2)&&(GPIOB->IDR & S1)){ //4 way
	  stop();
	  turnLeft(); //need to make
	  doneTurning = false;
	  //delay
	  while (!doneTurning){
		  if ((GPIOB->IDR & S3)&&!(GPIOB->IDR & S1)&&(GPIOB->IDR & S2)&&(GPIOB->IDR & S4)&&(GPIOA->IDR & S5)){ //when s3 is high again it means we have reached turning destination
			  doneTurning = true; //might need to add more sensor checks
		  }
	  }
  	}
  	else if ((GPIOA->IDR & S4)&&(GPIOB->IDR & S5)&&(GPIOB->IDR & S3)&&(GPIOB->IDR & S2)&&(GPIOB->IDR & S1)&&(GPIOA->IDR & S6)){
	  stop();
	  endMaze();//need to make
  	}
	else{ //no sensors are high = deadend
		stop();
		turnAround();
		doneTurning = false;
		//delay
	  while (!doneTurning){
		  if ((GPIOB->IDR & S3)&&!(GPIOB->IDR & S1)&&!(GPIOB->IDR & S2)){ //when s3 is high again it means we have reached turning destination
			  doneTurning = true; //might need to add more sensor checks
			  stop();
		  }

	  }
	}


	}
}
}
//--------------------------------------------TURN FUNCTIONS---------------------------------------
void stop(void){
	GPIOA ->ODR |= AllOutputsLow;
}
void straight(void){
	GPIOA ->ODR &= AllOutputsLow;
	GPIOA ->ODR |= MotorsForward ;
}
void turnLeft(void){
	GPIOA ->ODR &= AllOutputsLow;
 	GPIOA ->ODR |= TurnMotorsLeft;
}

void turnRight(void){
	GPIOA ->ODR &= AllOutputsLow;
	GPIOA ->ODR |= TurnMotorsRight;
}

void turnAround(void){
	GPIOA ->ODR &= AllOutputsLow;
	GPIOA->ODR |= TurnMotorsRight;
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

	/* SENSORS */
	/* setting PA8, PA9 and PB 12-15 to inputs for sensor */
	GPIOA->MODER &=~(GPIO_MODER_MODER8|GPIO_MODER_MODER9);
    GPIOB->MODER &=~(GPIO_MODER_MODER12|GPIO_MODER_MODER13|GPIO_MODER_MODER14|GPIO_MODER_MODER15);
	/* pull down resistor for pin 8 and 9 of port A and pin 12-15 of Port B */
	GPIOA->PUPDR&=~(GPIO_PUPDR_PUPDR8|GPIO_PUPDR_PUPDR9);
	GPIOA->PUPDR|=(GPIO_PUPDR_PUPDR8_1|GPIO_PUPDR_PUPDR9_1);
    GPIOB->PUPDR&=~(GPIO_PUPDR_PUPDR12|GPIO_PUPDR_PUPDR13|GPIO_PUPDR_PUPDR14|GPIO_PUPDR_PUPDR15);
    GPIOB->PUPDR|=(GPIO_PUPDR_PUPDR12_1|GPIO_PUPDR_PUPDR13_1|GPIO_PUPDR_PUPDR14_1|GPIO_PUPDR_PUPDR15_1);

	/* Switch and LED */
	/* set switch PB10 to input*/
    GPIOB->MODER &=~(GPIO_MODER_MODER10);
    /* set LED0 PB1 to output */
    GPIOB->MODER &=~(GPIO_MODER_MODER1);
    GPIOB->MODER |=(GPIO_MODER_MODER1_0);
	/* Pull-up resistor for start switch PB10 */
    GPIOB->PUPDR&=~(GPIO_PUPDR_PUPDR10);
    GPIOB->PUPDR|=(GPIO_PUPDR_PUPDR10_0);

    /* Motor IC */
    /*set PA3, PA2 AF mode*/
	GPIOA ->MODER |= (GPIO_MODER_MODER2_1|GPIO_MODER_MODER3_1);
    /*set PA0,A1,A5,A6 to output mode*/
	GPIOA ->MODER |= (GPIO_MODER_MODER0_0|GPIO_MODER_MODER1_0|GPIO_MODER_MODER5_0|GPIO_MODER_MODER6_0);
	GPIOA ->ODR &= AllOutputsLow; //set all outputs low initially
}

// ---------------------------------------------INIT PWM--------------------------------------------
void init_pwm(void){
  	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;


  	GPIOA->AFR[1] |= (2 << (4*(3 - 8))); // PA3_AF = AF2 (ie: map to TIM2_CH3) EN1
  	GPIOA->AFR[1] |= (2 << (4*(2 - 8))); // PA2_AF = AF2 (ie: map to TIM2_CH4) EN2

  	TIM2->ARR = 8000;  // f = 1 KHz
  	// specify PWM mode: OCxM bits in CCMRx. We want mode 1
  	TIM2->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1); // PWM Mode 1
  	TIM2->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1); // PWM Mode 1
  	// set PWM percentages
  	TIM2->CCR3 =  100* 80; // Red = 20%
  	TIM2->CCR4 = 100 * 80; // Green = 90%

  	// enable the OC channels
  	TIM2->CCER |= TIM_CCER_CC3E;
  	TIM2->CCER |= TIM_CCER_CC4E;

  	TIM2->CR1 |= TIM_CR1_CEN; // counter enable

}


