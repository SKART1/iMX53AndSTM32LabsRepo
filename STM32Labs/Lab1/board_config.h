#ifndef _BOARD_CONFIG_H
#define _BOARD_CONFIG_H

#ifdef _STM_DISCOVERY_BOARD_
  #define LED_RCC       RCC_AHB1Periph_GPIOD
  #define LED_PORT      GPIOD
  #define LED_PIN       GPIO_Pin_12


#endif

#ifdef _STM_IAR_BOARD_
  #define LED_RCC       RCC_AHB1Periph_GPIOF
  #define LED_PORT      GPIOF
  #define LED_PIN       GPIO_Pin_6


  #define BTN_RCC       RCC_AHB1Periph_GPIOG
  #define BTN_PORT      GPIOG
  #define BTN_PIN       GPIO_Pin_6


#endif


#endif