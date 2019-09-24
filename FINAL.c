/*------------------------------------------------EEE3099S LINE FOLLOWER ROBOT----------------------
 * Title:			EEE3099S LINE FOLLOWER ROBOT
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
//state variable: 0- waiting, 1- solving, 2- paused(solving), 3- waiting(solved), 4- racing, 5- paused(racing).
int State = 0;
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
int AllInputsLow1 = GPIO_IDR_12|GPIO_IDR_13|GPIO_IDR_14|GPIO_IDR_15;
int AllInputsLow2 = GPIO_IDR_8|GPIO_IDR_9;
//Switch and LED
int SW1 = GPIO_IDR_10;
int LED = GPIO_ODR_1;

//------------------------------------------------FUNCTION DEFINITIONS------------------------------
void init_GPIO(void);
void init_pwm(void);
void init_EXTI4_15(void);
void EXTI4_15_IRQHandler(void);

void straight(void);
void turnAround (void);
void stop(void);
void turnLeft(void);
void turnRight(void);
void delay();

// --------------------------------------------------MAIN-------------------------------------------
int main (void){					//JUST FOR TESTING PURPOSES FOR NOW
  	init_GPIO();
	init_pwm();
	init_EXTI4_15();
	GPIOA->ODR &= AllOutputsLow;	//prevent motors moving from previous debug session
	GPIOB->ODR &= ~LED; //set LED low
	while(1){						//infinite loop, with alternating states
		while(State == 0){			//State 0: Waiting
		}
		while(State == 1){			//State 1: Solving
			//Polling sensors and calls to decide() function here. State set from 1 to 3 in decide().
			//**********************
		}
		while(State == 2){			//State 2: Paused (Solving)
			GPIOA->ODR &= AllOutputsLow;
		}
		while(State == 3){			//State 3: Waiting (Solved)
			GPIOA->ODR &= AllOutputsLow;
		}
		while(State == 4){			//State 4: Racing
			//Polling sensors and calls to decide() function, but with different execution
		}
		while(State == 5){			//State 5: Paused (Racing)
			GPIOA->ODR &= AllOutputsLow;
		}
	}
}


//--------------------------------------------TURN FUNCTIONS---------------------------------------
void stop(void){
	GPIOA ->ODR |= AllOutputsLow;
	Delay(8000);
}
void straight(void){
	GPIOA ->ODR &= AllOutputsLow;
	GPIOA ->ODR |= MotorsForward ;
}
void turnLeft(void){
	GPIOA ->ODR = 0;
 	GPIOA ->ODR |= TurnMotorsLeft;
 	Delay(8000);
}

void turnRight(void){
	GPIOA ->ODR &= AllOutputsLow;
	GPIOA ->ODR |= TurnMotorsRight;
	Delay(8000);
}

void turnAround(void){
	stop();
	//GPIOA ->ODR &= AllOutputsLow;
	//GPIOA->ODR |= TurnMotorsRight;
	//Delay(8000);
}

void LeftAdjust(void){
	GPIOA ->ODR &= AllOutputsLow;
	GPIOA ->ODR |= TurnMotorsLeft;
}

void RightAdjust(void){
	GPIOA ->ODR &= AllOutputsLow;
	GPIOA ->ODR |= TurnMotorsRight;
}

void Delay(int n){
	for (int i = 0; i<= n; i++){
		for (int x = 0; x <= 1000; x++){
			
		}
	}
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

// ---------------------------------------------INIT EXTI4_15---------------------------------------
void init_EXTI4_15(void){
	RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN; // enable clock for	the sys conf controller
	SYSCFG -> EXTICR[2] |= 0b00000000000000000000000100000000; // map PB10 to EXTI3
	EXTI -> IMR |= EXTI_IMR_MR10; // unmask external interrupt 10
	EXTI -> FTSR |= EXTI_FTSR_TR10; // trigger on falling edge
	NVIC_EnableIRQ(EXTI4_15_IRQn); // enable EXTI4_15 interrupt in the NVIC
}

//--------------------------------------------EXTI4_15_IRQHandler-----------------------------------
void EXTI4_15_IRQHandler(void){
	EXTI -> PR |= EXTI_PR_PR10; // clear the interrupt pending bit
	if (GPIOB->ODR & LED){
		GPIOB->ODR |= LED; //set LED high
	}
	else{
		GPIOB->ODR &= ~LED; //set LED low
	}
	//state variable: 0- waiting, 1- solving, 2- paused(solving), 3- waiting(solved), 4- racing, 5- paused(racing).
	if(State == 0){			//button pressed to start the program
		State++;
	}
	else if(State == 1){	//pressed while solving, pauses solving
		State++;
	}
	else if(State == 2){	//pressed while paused(solving), resumes solving
		State--;
	}
	else if(State == 3){	//pressed while solved, starts racing
		State++;
	}
	else if(State == 4){	//pressed while racing, pauses racing
		State++;
	}
	else{					//pressed while paused(racing), resumes racing
		State--;
	}
}

