#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#define _STM_IAR_BOARD_
#include "board_config.h"

xQueueHandle MyQueue;

void Task1(void *pvParameters);
void Task2(void *pvParameters);

int main()
{
  GPIO_InitTypeDef	GPIO_InitStruct;

  // ----------------------------------------------------
  //  ENABLE RCC
  // ----------------------------------------------------
  RCC_AHB1PeriphClockCmd(PIN1_RCC, ENABLE);
  RCC_AHB1PeriphClockCmd(BUTTON_RCC, ENABLE);
  
  // ----------------------------------------------------
  //  INIT GPIO
  // ----------------------------------------------------
  GPIO_InitStruct.GPIO_Pin = PIN1_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(PIN1_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = BUTTON_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);

  
  MyQueue = xQueueCreate( 10, sizeof(uint8_t));
  
  //Указатель на функцию, имя задачи, размер стека, параметры, приоритет и непонятно что
  xTaskCreate( (*Task1), (signed char *) "Action", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1 ), NULL);
  xTaskCreate( (*Task2), (signed char *) "Action", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1 ), NULL);
  vTaskStartScheduler();
}

//Прочитать состояние порта. Если состояние изменилось, то отправить новое значение в очередь
void Task1(void *pvParameters){
  uint8_t bt_state = 0;
  uint8_t bt_old_state = 0;
  while(1){
    bt_state = GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN);
    if (bt_state != bt_old_state){
      bt_old_state = bt_state;
      xQueueSend(MyQueue, &bt_state, 0);
    }
  };
}

//Попытаться прочитать из очереди MyQueue. Если ошибка- значит пусто. Иначе вывести на светодиод результат
void Task2(void *pvParameters){
  uint8_t data = 0;
  while(1){
    if (xQueueReceive(MyQueue, &data, 0) == pdFAIL) continue;
    if (data == 0)
      GPIO_ResetBits(PIN1_PORT, PIN1_PIN);
    else
      GPIO_SetBits(PIN1_PORT, PIN1_PIN);
  }
}



