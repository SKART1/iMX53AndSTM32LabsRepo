#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#define _STM_IAR_BOARD_
#include "board_config.h"


void Mode0(void);
void Mode1(void);

void MCO_Enable(void);
int main()
{
	uint8_t	mode = 0;
	GPIO_InitTypeDef	GPIO_InitStruct;
	
	// ----------------------------------------------------
	//  ENABLE RCC
	// ----------------------------------------------------
	RCC_AHB1PeriphClockCmd(PIN1_RCC, ENABLE);
	RCC_AHB1PeriphClockCmd(PIN2_RCC, ENABLE);
        RCC_AHB1PeriphClockCmd(BUTTON_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	DBGMCU_APB1PeriphConfig(DBGMCU_TIM2_STOP, ENABLE);
	DBGMCU_APB1PeriphConfig(DBGMCU_TIM3_STOP, ENABLE);
	
	// ----------------------------------------------------
	//  INIT GPIO
	// ----------------------------------------------------
	GPIO_InitStruct.GPIO_Pin = PIN1_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(PIN1_PORT, &GPIO_InitStruct);
        
        GPIO_InitStruct.GPIO_Pin = PIN2_PIN;
        GPIO_Init(PIN2_PORT, &GPIO_InitStruct);
        
	GPIO_InitStruct.GPIO_Pin = BUTTON_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);
	
	Mode0();
	
	while(1){
		if(GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_RESET){
			if (mode == 0){
				Mode1();
				mode = 1;
			}
			else{
				Mode0();
				mode = 0;
			}
			while(GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_RESET);
		}
	}
}
void TIM2_IRQHandler(void){
	uint32_t	i = 0;
	
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	
	GPIO_SetBits(PIN1_PORT, PIN1_PIN);
	while(i++ < 1000){}
	GPIO_ResetBits(PIN1_PORT, PIN1_PIN);
	
}
void TIM3_IRQHandler(void){

	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	
	GPIO_ToggleBits(PIN2_PORT, PIN2_PIN);
}
void Mode0(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	NVIC_InitTypeDef		NVIC_InitStruct;
	
	// ----------------------------------------------------
	//  INIT TIMER 2
	// ----------------------------------------------------
	TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStruct.TIM_Prescaler 	= 0;
	TIM_TimeBaseStruct.TIM_Period		= 40000;
	TIM_TimeBaseStruct.TIM_ClockDivision = 0x00;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	// ----------------------------------------------------
	//  INIT TIMER 3
	// ----------------------------------------------------
	TIM_TimeBaseStruct.TIM_Period		= 100;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	// ----------------------------------------------------
	//  INIT NVIC
	// ----------------------------------------------------
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			= 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 0;
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}

void Mode1(void){
	NVIC_InitTypeDef		NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			= 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 0;
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_Init(&NVIC_InitStruct);
}
