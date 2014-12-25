#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"


#include "FreeRTOS.h"
//Semaphore
#include "semphr.h"



#include "task.h"
#include "queue.h"
#define _STM_IAR_BOARD_
#include "board_config.h"


//---------------------------------
volatile static uint8_t CPU_IDLE = 0;

volatile int test=0;


volatile uint8_t test2=0;


//Объект очередь для IPC
xQueueHandle MyQueue=NULL;
xSemaphoreHandle ourTestSemaphore=NULL;



void MainThread(void *pvParameters);
void IdlePriorityTask(void *pvParameters);

//Сконфигурировать порт на выход
void ConfigPins(void){
  GPIO_InitTypeDef	 GPIO_InitStruct;
  /* Enable clock for GPIOB */
  RCC_AHB1PeriphClockCmd(PIN1_RCC, ENABLE);
  
  
  // ----------------------------------------------------
  //  INIT GPIO
  // ----------------------------------------------------
  GPIO_InitStruct.GPIO_Pin = PIN1_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(PIN1_PORT, &GPIO_InitStruct);  
  
  
   RCC_AHB1PeriphClockCmd(PIN2_RCC, ENABLE);  
  // ----------------------------------------------------
  //  INIT GPIO
  // ----------------------------------------------------
  GPIO_InitStruct.GPIO_Pin = PIN2_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(PIN2_PORT, &GPIO_InitStruct);
}



//Сконфигурировать порт светодиода на выход
void ConfigureLed(void){
  GPIO_InitTypeDef	 GPIO_InitStruct;
  /* Enable clock for GPIOB */
  RCC_AHB1PeriphClockCmd(LED_RCC, ENABLE);
  
  
  // ----------------------------------------------------
  //  INIT GPIO
  // ----------------------------------------------------
  GPIO_InitStruct.GPIO_Pin = LED_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(LED_PORT, &GPIO_InitStruct);  
}

//Сконфигурировать обработчик прерывания по кнопке (EXTI9_5_IRQHandler)
void Configure_Button_Interrupt(void) {
    /* Set variables used */
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    /* Enable clock for GPIOB */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    /* Set pin as input */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOG, &GPIO_InitStruct);
    
    /* Tell system that you will use PB12 for EXTI_Line12 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource6);
    
    /* PB12 is connected to EXTI_Line12 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line6;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);
 
    /* Add IRQ vector to NVIC */
    /* PB12 is connected to EXTI_Line12, which has EXTI15_10_IRQn vector */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    /* Set priority */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);
}


volatile uint8_t flagSoftwareCounter;

//----------------------------------
void vApplicationIdleHook( void ) {             //это и есть поток с минимальным приоритетом
        static portTickType LastTick; 
        static long long int count;                     //наш трудяга счетчик
        static long long int max_count ;                //максимальное значение счетчика, вычисляется при калибровке и соответствует 100% CPU idle

        

        while(1){
          count++;                                                  //приращение счетчика
        
        
        
          if((xTaskGetTickCount() - LastTick ) > 5000){     //если прошло 1000 тиков (1 сек для моей платфрмы)
                  LastTick = xTaskGetTickCount();
                  if(count > max_count){                             //это калибровка
                    max_count = count;  
                  }    
                  if(test2==1){
                    CPU_IDLE = 100 * count / max_count;               //вычисляем текущую загрузку
                                                      
                  }
                  count = 0;                                              //обнуляем счетчик
          }
        }
}


//---------------------------------
uint8_t GetCPU_IDLE(void) {
        return CPU_IDLE;
}




int main()
{
  ConfigureLed();
  ConfigPins();  
  Configure_Button_Interrupt();
  
  /* Create the semaphore that is used to demonstrate a task being
  synchronised with an interrupt. */
  vSemaphoreCreateBinary(ourTestSemaphore);
  //Take semaphore
  xSemaphoreTake(ourTestSemaphore, portMAX_DELAY );

  
  
  //Create queue
  MyQueue = xQueueCreate( 10, sizeof(uint8_t));
  

    
  //Создание задачей
  //Указатель на функцию, имя задачи, размер стека, параметры, приоритет и непонятно что
  xTaskCreate( (*MainThread), (signed char *) "Action", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 2 ), NULL);
  xTaskCreate( (*IdlePriorityTask), (signed char *) "Action2", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY), NULL);
 //  xTaskCreate( (*IdlePriorityTask), (signed char *) "Action3", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY), NULL);
  xTaskCreate( (*IdlePriorityTask), (signed char *) "Action4", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY), NULL);
  
  
  
  
  //Вызвать диспетчирезацию
  vTaskStartScheduler();
}



void veryLongAndDifficultFunction(){
  volatile int k=0;
  
      
  for(int i=0;i<1000; i++){
    for(int j=0;j<1000; j++){
        k=k+i*j;    
    }      
  }  
}



//Прочитать состояние порта. Если состояние изменилось, то отправить новое значение в очередь
void IdlePriorityTask(void *pvParameters){
  if(test2==0){
    vTaskDelay(10000);
    test2=1;
  }
  while(1);
}

//Прочитать состояние порта. Если состояние изменилось, то отправить новое значение в очередь
void MainThread(void *pvParameters){
  uint8_t data = 0;

  //Занять семафор 1 раз
  while(1){
     
    //Ожидать семафор
    if(xSemaphoreTake(ourTestSemaphore, portMAX_DELAY) == pdTRUE){
      //1 на вывод светодиод
      GPIO_SetBits(PIN2_PORT, PIN2_PIN);
      
      
      test=GetCPU_IDLE();
      
      veryLongAndDifficultFunction();
      
      //Считать данные из очереди
      if (xQueueReceive(MyQueue, &data, 0) == pdFAIL){
        //вернуться к while(1)
        continue; 
      }
      
      
      if (data == 0){
        //GPIO_ResetBits(PIN_LED, PIN_LED);
      }
      else{
        //GPIO_SetBits(PIN_LED, PIN_LED);
      }    
    } 
    
    //0 на вывод светодиод
    GPIO_ResetBits(PIN2_PORT, PIN2_PIN);
  }
}


void EXTI9_5_IRQHandler(void){
  uint8_t bt_state=1;
  long lHigherPriorityTaskWoken = pdFALSE;
  
  //Отправить данные
  xQueueSend(MyQueue, &bt_state, 0);   
  
  
  /* First the handler releases a semaphore.
  lHigherPriorityTaskWoken has been initialised to zero. */
  xSemaphoreGiveFromISR(ourTestSemaphore, &lHigherPriorityTaskWoken );

  /* If there was a task that was blocked on the semaphore, and giving the
  semaphore caused the task to unblock, and the unblocked task has a priority
  higher than the currently executing task (the task that this interrupt
  interrupted), then lHigherPriorityTaskWoken will have been set to pdTRUE.
  Passing pdTRUE into the following macro call will cause this interrupt to
  return directly to the unblocked, higher priority, task. */
  portEND_SWITCHING_ISR( lHigherPriorityTaskWoken );
  
  
  //Сбросить флаг прерывания
  EXTI_ClearITPendingBit(EXTI_Line6);
}






