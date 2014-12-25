#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#define _STM_IAR_BOARD_
#include "board_config.h"

void MCO_Enable(void);
int main()
{
  GPIO_InitTypeDef	GPIO_InitStruct;
  
  RCC_AHB1PeriphClockCmd(LED_RCC, ENABLE);

  GPIO_InitStruct.GPIO_Pin = LED_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(LED_PORT, &GPIO_InitStruct);

  MCO_Enable();
  
  while(1){
    GPIO_ToggleBits(LED_PORT, LED_PIN);
  }
}

void MCO_Enable(void){
  // GPIO Init
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  GPIO_InitTypeDef		GPIO_InitStruct;

  GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_8;
  GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_5);
}