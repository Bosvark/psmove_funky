#include <math.h>
#include "stm32f10x.h"
#include "timer.h"

#define POWER_PORT			GPIOD
#define POWER_PIN			GPIO_Pin_11

#define LED_PORT			GPIOC
#define LED_PORT_AF			RCC_APB2Periph_GPIOC
#define LED_RED				GPIO_Pin_6
#define LED_GREEN			GPIO_Pin_7
#define LED_BLUE			GPIO_Pin_8

void power_up(){
	// The voltage regulator that powers the LED's and other sensors, needs to be switched on
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = POWER_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(POWER_PORT, &GPIO_InitStructure);

	GPIO_SetBits(POWER_PORT, POWER_PIN);	// Switch it on
}

void do_the_funky(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(LED_PORT_AF | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LED_RED | LED_GREEN | LED_BLUE;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(LED_PORT, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	// Base timer configuration
	TIM_TimeBaseStructure.TIM_Period = 665;
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 24000000) - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	// PWM channel 1 config
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	// PWM channel 2 config
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	// PWM channel 3 config
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	// Enable timer
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);

	while(1){
		float val = (exp(sin(TimerUpTime()/2000.0*3.14159265359)) - 0.36787944)*108.0;
		TIM_OCInitStructure.TIM_Pulse = val;
		TIM_OC1Init(TIM3, &TIM_OCInitStructure);

		val = (exp(sin(TimerUpTime()/1500.0*3.14159265359)) - 0.36787944)*108.0;
		TIM_OCInitStructure.TIM_Pulse = val;
		TIM_OC2Init(TIM3, &TIM_OCInitStructure);

		val = (exp(sin(TimerUpTime()/1000.0*3.14159265359)) - 0.36787944)*108.0;
		TIM_OCInitStructure.TIM_Pulse = val;
		TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	}
}

#define SYSTICK_FREQUENCY_HZ       1000

int main(void)
{
	SystemInit();

	// Use SysTick as reference for the timer
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / SYSTICK_FREQUENCY_HZ);

	power_up();

	do_the_funky();
}
