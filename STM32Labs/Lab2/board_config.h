#ifndef _BOARD_CONFIG_H
#define _BOARD_CONFIG_H

#ifdef _STM_DISCOVERY_BOARD_
  #define LED_RCC       RCC_AHB1Periph_GPIOD
  #define LED_PORT      GPIOD
  #define LED_PIN       GPIO_Pin_12

  #define PIN1_RCC      RCC_AHB1Periph_GPIOD
  #define PIN1_PORT     GPIOD
  #define PIN1_PIN      GPIO_Pin_2

  #define PIN2_RCC      RCC_AHB1Periph_GPIOD
  #define PIN2_PORT     GPIOD
  #define PIN2_PIN      GPIO_Pin_6

  #define BUTTON_RCC      RCC_AHB1Periph_GPIOA
  #define BUTTON_PORT     GPIOA
  #define BUTTON_PIN      GPIO_Pin_0

#endif

#ifdef _STM_IAR_BOARD_
  #define LED_RCC       RCC_AHB1Periph_GPIOF
  #define LED_PORT      GPIOF
  #define LED_PIN       GPIO_Pin_6

  #define PIN1_RCC      RCC_AHB1Periph_GPIOF
  #define PIN1_PORT     GPIOF
  #define PIN1_PIN      GPIO_Pin_8

  #define PIN2_RCC      RCC_AHB1Periph_GPIOF
  #define PIN2_PORT     GPIOF
  #define PIN2_PIN      GPIO_Pin_6

  #define BUTTON_RCC      RCC_AHB1Periph_GPIOG
  #define BUTTON_PORT     GPIOG
  #define BUTTON_PIN      GPIO_Pin_6

#endif


#endif