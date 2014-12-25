#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#define _STM_IAR_BOARD_
#include "board_config.h"

int main()
{
  
  GPIO_InitTypeDef	GPIO_InitStruct;
  GPIO_InitTypeDef	GPIOButton;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6; //((uint16_t)0x0040)
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(LED_PORT, &GPIO_InitStruct);
  
  /*Включение по кнопке*/
  RCC_AHB1PeriphClockCmd(BTN_RCC, ENABLE);
  GPIOButton.GPIO_Pin = BTN_PIN;
  GPIOButton.GPIO_Mode = GPIO_Mode_IN;  
  GPIOButton.GPIO_OType = GPIO_OType_PP;  
  GPIOButton.GPIO_Speed = GPIO_Speed_100MHz;
  GPIOButton.GPIO_PuPd = GPIO_PuPd_UP;
  
  
  
  GPIO_Init(BTN_PORT, &GPIOButton);


  while(1){
    if(GPIOG->IDR & GPIO_Pin_6){
       GPIOF->ODR|=GPIO_Pin_6;
    }
    else{
      GPIOF->ODR&=~(GPIO_Pin_6);
    }
  }
  
}
