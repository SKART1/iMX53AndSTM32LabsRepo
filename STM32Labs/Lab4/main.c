#include <string.h>

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#define _STM_IAR_BOARD_
#include "board_config.h"




void Mode0(void);
void Mode1(void);
void Mode2(void);
void Mode3(void);
void Mode4(void);
void Mode5(void);

char SendBuf[10] = "123456789";
char recvBuff[100]={0};
int recvBuffCounter=0;

uint8_t index = 0;

int main()
{
  uint8_t	mode = 5;
  GPIO_InitTypeDef	GPIO_InitStruct;
  USART_InitTypeDef	USART_InitStruct;

  // ----------------------------------------------------
  //  ENABLE RCC
  // ----------------------------------------------------
  
  
  /* подключаем тактирование для порта F  G  */
  
  /*!< RCC AHB1 peripheral clock register,                          Address offset: 0x30 */
  /*   в этот регистр записываем */
  /* #define RCC_AHB1Periph_GPIOG             ((uint32_t)0x00000040) */
  /* в седьмой бит выставляем в еденицу ( ENABLE ) */ 
  
  //Включаем тактирование ПОРТА  BUTTON_RCC (RCC_AHB1Periph_GPIOG) - порта G
  RCC_AHB1PeriphClockCmd(BUTTON_RCC, ENABLE);


  /*#define RCC_AHB1Periph_GPIOF             ((uint32_t)0x00000020)*/
  //Включаем тактирование ПОРТА  PIN1_RCC (RCC_AHB1Periph_GPIOF) - порта F
  RCC_AHB1PeriphClockCmd(PIN1_RCC, ENABLE);
  
  /**#define RCC_AHB1Periph_GPIOF             ((uint32_t)0x00000020)*/
  //Включаем тактирование ПОРТА  PIN2_RCC (RCC_AHB1Periph_GPIOF) - порта F
  RCC_AHB1PeriphClockCmd(PIN2_RCC, ENABLE);
  
  /* разрешаем прямой доступ к памяти */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  
  
  
  
  // ----------------------------------------------------
  //  INIT GPIO
  // ----------------------------------------------------
  //Инициализируем порт путем записи в поля специальной структуры GPIO_InitStruct (встроенная)
  //Это кнопка - значит мы должны сконфигурироваться на вход
  GPIO_InitStruct.GPIO_Pin = BUTTON_PIN;          // 6й пин 
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;       // Пин порта в режиме на вход
  
  
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;     //Мы не уверены как работает порт на вход и как влияют эти параметры на вход
                                                  // Открытый коллектор - жесткий ноль (т.е.) открытый транзистор. Либо бесконечное сопротивление (закрытый транзистор). Причем это ВЫХОД!!!!
                                                  //GPIO_OType_PP - полноценный транзисторный полумост. Открыт либо один транзистор к питанию, либо другой к земле. 
                                                  // 0: Output push-pull (reset state)
                                                  // 1: Output open-drain
  
  
  
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz; //Некое тактирование порта 
  
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;   //Подтяжка есть на плате - кнопка подтянута к 1. Не подтягивать
  GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);       //Первый параметр - порт. Конкретный пин указан в структуре GPIO_InitStruct. 
  
  
    
  //Инициализируем порт для светодиода
  //Светодиод - выход
  GPIO_InitStruct.GPIO_Pin = PIN1_PIN;          //Пин N порта 
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;    //Пин порта на выход  
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   //Полноценный транзисторный выход    
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz; //Некое тактирование порта 
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;   //Подтяжка есть на плате - кнопка подтянута к 1. Не подтягивать
    
  GPIO_Init(PIN1_PORT, &GPIO_InitStruct);       //Первый параметр - порт. Конкретный пин указан в структуре GPIO_InitStruct. 
  
  //Инициализируем второй светодиод - тоже выход
  GPIO_InitStruct.GPIO_Pin = PIN2_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;    //Пин порта на выход  
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   //Полноценный транзисторный выход 
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz; //Некое тактирование порта 
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;   //Подтяжка есть на плате - кнопка подтянута к 1. Не подтягивать

  GPIO_Init(PIN2_PORT, &GPIO_InitStruct);      //Первый параметр - порт. Конкретный пин указан в структуре GPIO_InitStruct. 
  
  
  //Выключить светодиод - пин N порта F
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN); 
  
  // ----------------------------------------------------
  //  INIT USART
  // ----------------------------------------------------
  //   Тут прогер перестал переоименовыть - в   board_config.h   
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);         // включаем тавтирование порта D 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);        // включаем тактирование устройства Юсарт 
  
  /**
  * @brief  Changes the mapping of the specified pin.
  * @param  GPIOx: where x can be (A..K) to select the GPIO peripheral for STM32F405xx/407xx and STM32F415xx/417xx devices
  *                      x can be (A..I) to select the GPIO peripheral for STM32F42xxx/43xxx devices.
  *                      x can be (A, B, C, D and H) to select the GPIO peripheral for STM32F401xx devices. 
  * @param  GPIO_PinSource: specifies the pin for the Alternate function.
  *         This parameter can be GPIO_PinSourcex where x can be (0..15).
  * @param  GPIO_AFSelection: selects the pin to used as Alternate function.
  */  
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);     // конфигурируем порт  D  пин 8 на набор альтернативных функций UART 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);     // конфигурируем порт  D  пин 9 на набор альтернативных функций UART 
  
  //Конфигурируем пины 8 порта D как обычный вывод (альтернативная функция, тип выхода - полноценный транзисторный полумост, подтяжка, скорость)
  GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AF;                 // !!!!!GPIO Alternate function Mode 
  GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_100MHz;            
  GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;                
  GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;

  GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_8;
  GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  //Конфигурируем пины 9 порта D как обычный вывод (альтернативная функция, тип выхода - полноценный транзисторный полумост, подтяжка, скорость)
  GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_9;
  GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  
  //Конфигурируем USART - как везде. Скорость. Количество бит. Количество стоповых бит, Чётность и т.д. - см. USART
  USART_InitStruct.USART_BaudRate       = 115200 ;
  USART_InitStruct.USART_WordLength     = USART_WordLength_9b;
  USART_InitStruct.USART_StopBits       = USART_StopBits_1;
  USART_InitStruct.USART_Parity         = USART_Parity_Even;
  USART_InitStruct.USART_HardwareFlowControl    = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode           = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART3, &USART_InitStruct);
  
  //Включаем USART3
  USART_Cmd(USART3, ENABLE);
  
  //Пример отправки
  //USART_SendData(USART3, 0x55);

  //Главный цикл
  while(1){
    //Считать состояние кнопки
    if(GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == BIT_COND){
      //Режим 0 - передача фразы с тупым ожиданием флага готовности в цикле.
      if (mode == 0){
        Mode0();
        mode++;
      }
      else if (mode == 1){
        Mode1();
        mode++;
      }
      else if(mode == 2){
        Mode2();
        mode++;
      }
      //Получение по флагу готовности
      else if(mode==3){
        Mode3();
        mode++;      
      }
      //Получение по превыванию
      else if(mode==4){
        Mode4();
        mode++;      
      }
      //Получение по DMA
      else if(mode==5){
        Mode5();
        mode=0;      
      }     
      while(GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == BIT_COND){
        GPIO_ToggleBits(PIN1_PORT, PIN1_PIN);
      };
    }
    GPIO_ToggleBits(PIN1_PORT, PIN1_PIN);
    
  }
}
void Mode0(void){
  uint8_t i =0;
  //Включить лампочку STA1 на плате
  GPIO_SetBits(PIN2_PORT, PIN2_PIN);
  
  for(i = 0; i <(uint8_t) strlen(SendBuf); i++){
    //Ждем флага TXE. Функция для поллинга. Пока флаг установлен - передаччик занят. Прерывания могут быть отключены и отслеживать флаг прерывания может быть невозмождно - поэтмоу используем этот флаг
    while( USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    //Отправить очередной байт
    USART_SendData(USART3, SendBuf[i]);
  }
  
  //Выключить лампочку STA1 на плате
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);
}

void Mode1(void){
 //Структура для инициализации и настройки прерываний
  NVIC_InitTypeDef	NVIC_InitStruct;
  
  //Включить лампочку STA1 на плате
  GPIO_SetBits(PIN2_PORT, PIN2_PIN);
    

  //На приоритеты выделено 4 бита. Их можно распределить между основным приоритетеом и вспомогательным приоритетом 4 возможными вариантами
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //NVIC_PriorityGroup_2:       2 bits for pre-emption priority          2 bits for subpriority
  
  NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;                //Включаем прерывание от USART № 3  
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;        //Чем меньше значение - тем выше приоритет 2 бита = 4 значения от 4 до 0
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;               //Чем меньше значение - тем выше приоритет 2 бита = 4 значения от 4 до 0
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;                  //Включить или выключить прерывание описанное в поле NVIC_IRQChannel выше
  NVIC_Init(&NVIC_InitStruct);                                  //Записываем эту структуру
  
  //Разрешить обработку событий типа окончаний передачи (Transmit Data Register empty interrupt)
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  
  //Выключить лампочку STA1 на плате
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);  
}



void Mode2(void){  
  // Структура для настройки прерывания
  NVIC_InitTypeDef NVIC_InitStructure;  
  //Структура для настройки  DMA. DMA - позволяет перемещать данные без участия ЦП  
  DMA_InitTypeDef  DMA_InitStructure;
  
  
  
  
  //Включить лампочку STA1 на плате
  GPIO_SetBits(PIN2_PORT, PIN2_PIN);
    
  
  //На приоритеты выделено 4 бита. Их можно распределить между основным приоритетеом и вспомогательным приоритетом 4 возможными вариантами
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//NVIC_PriorityGroup_2:       2 bits for pre-emption priority          2 bits for subpriority
   
  /* Enable the USART2 RX DMA Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;       //В контроллере прямого досутпа к памяти DMA1 есть несколько каналов. К каждому из каналов можно подключить один из 
                                                                //из источников данных (ВНИМАНИЕ! у каждого канала - свой набор, хоть и пересекающийся)  . Прироритеты каналов - чем меньше номер - тем выше приоритет.
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     //Чем меньше значение - тем выше приоритет 2 бита = 4 значения от 4 до 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            //Чем меньше значение - тем выше приоритет 2 бита = 4 значения от 4 до 0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //Включить или выключить прерывание описанное в поле NVIC_IRQChannel выше
  NVIC_Init(&NVIC_InitStructure);                               //Записываем эту структуру
  
  
  
  //Инициализируем DMA
  //http://www.mcustep.ru/init/stm32f4/46-dma-v-stm32f4-opisanie.html
  //Deinitialize the DMAy Streamx registers to their default reset values
  DMA_DeInit(DMA1_Stream3);                                                    
  
  //Инициализация полей структуры
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;                                //Specifies the channel used for the specified stream
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SendBuf;
   
  
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                       //Specifies if the data will be transferred from memory to peripheral or from memory to memory or from peripheral to memory.

  
  
  
  DMA_InitStructure.DMA_BufferSize = 5;        /*!< Specifies the buffer size, in data unit, of the specified Stream. 
                                          The data unit is equal to the configuration set in DMA_PeripheralDataSize
                                          or DMA_MemoryDataSize members depending in the transfer direction. */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;               // Specifies the Memory data width. For memory to device
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;       //                                 For device to memory
    
  
  
  
  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              // Регистр данных никуда не смещается -это ведь символьное устройство - данные идут как по "трубе"
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       // Записывать результат в разные элементы массива, инкрементировать указатель после каждой записи


  
  
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                               //DMA_Mode_Normal
                                                                                //Циклический (круговой, замкнутый) режим предназначен для передачи циклических буферов и непрерывных потоков данных (например при сканировании ADC). 
                                                                                //Эта функция включается установкой бита CIRC в регистре DMA_SxCR. 
                                                                                //Когда используется замкнутый режим, то после каждой DMA транзакции количество передаваемых элементов данных автоматически перезагружается начальным значением NDTR, запрограммированным на этапе конфигурации потока. 
                                                                                //После каждой транзакции передача данных по DMA не прерывается, а начинается сначала с установленными ранее параметрами.
                                                                                //Замечание. Если используется пакетная передача "память<->FIFO буфер" (Mburst), то обязательно должно соблюдаться следующее условие:
                                                                                //DMA_SxNDTR должно быть кратно ((Mburst_beat) * Msize / Psize)
                                                                                //другими словами: DMA_SxNDTR должно делиться без остатка на ((Mburst_beat) * Msize / Psize ),
                                                                                //где 
                                                                                //Mburst_beat = 4, при MBURST = 01: INCR4 (incremental burst of 4 beats)
                                                                                //Mburst_beat = 8, при MBURST = 10: INCR8 (incremental burst of 8 beats)
                                                                                //Mburst_beat = 16, при MBURST = 11: INCR16 (incremental burst of 16 beats)
                                                                                //Отношение Msize / Psize может быть равным 1/4, 1/2, 1, 2, 4 в зависимости от MSIZE и PSIZE
                                                                                //Если это условие не соблюдается, то скорее всего нарушится целостность данных.
                                                                                //Кроме этого NTDR должно быть кратным (размер_пакета(Pburst) * размер_элемента_данных(Psize)) для участка передачи "FIFO буфер <-> периферия". Иначе поведение DMA будет непредсказуемым. Это условие нужно проверять если используется направление "из памяти в память" + пакетная передача (Pburst).
                                                                                  
  
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                           //Specifies the software priority for the DMAy Streamx (DMA_Priority_Low || DMA_Priority_Medium || DMA_Priority_High  || DMA_Priority_VeryHigh)
  
  
  
  
  
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;                         //(FIFO enable) Когда используется это направление и бит EN в регистре DMA_SxCR установлен в 1, то при каждом запросе от периферии, данные помещаются в FIFO буфер. После достижения определенного порога, FIFO буфер опустошается и данные прибывают в место назначения. 
                                                                                //(Direct mode) В режиме Direct mode (бит DMDIS в регистре DMA_SxFCR установлен в "0"), данные в FIFO буфере не накапливаются до установленного порога, а немедленно передаются в место назначения.
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;                 //Specifies the FIFO threshold level.
  
  
  
  
  
  
  
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                   //It specifies the amount of data to be transferred in a single non interruptable transaction
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;           // Одиночная и пакетная передача данных (Single and burst transfers)
                                                                                //Контроллер DMA может передавать данные отдельными элементами (single) или неделимыми пакетами (порциями), состоящими из 4, 8 или 16 - ти элементов данных. Размер пакета устанавливается в PBURST и MBURST регистра DMA_SxCR, независимо для двух AHB портов. Размер указывает не количество байт, а количество элементов данных в пакете. В целях обеспечения согласованности каждая группа элементов данных, которая формирует пакет, неделима. Во время передачи пакета элементов, арбитр блокирует всякий доступ к AHB матрице шин, чем обеспечивается абсолютная атомарность трансфера. 
                                                                                //В зависимости от конфигурации xBURST, каждый запрос DMA может инициировать различное количество трансферов на порт AHB:
                                                                                //  -Когда порт AHB настроен на одиночные передачи, то каждый запрос DMA генерирует передачу только одного элемента данных (byte, half-word или word в зависимости от xSIZE битов в регистре DMA_SxCR).
                                                                                //
                                                                                //  -Когда порт AHB настроен на пакетную передачу, то каждый запрос DMA генерирует передачу 4-х, 8-ми или 16-ти элементов данных в зависимости от xBURST.
                                                                                //
                                                                                //  В прямом режиме (direct mode) поток может генерировать только одиночные (single) переводы и MBURST и PBURST не имеют значения.
                                                                                //
                                                                                //  Указатели адреса (регистры DMA_SxPAR or DMA_SxM0AR) должны быть выбраны таким образом, чтобы все переданные или принятые пакеты были выровнены по адресной границе, равной размеру передачи.
                                                                                //
                                                                                //  Конфигурация пакетной передачи должна быть в соответствии с протоколом AHB, где пакеты (burst) не должны пересекать адресную границу в 1КБ (1024 байт). Так как минимальное адресное пространство, которое может быть выделено для одного подчиненного равна 1КБ. Это значит, что пакет не должен "пересечь" эту границу, иначе будет получена неуловимая ошибка AHB.
                                                                                //
                                                                                //  Еще раз повторю, что пакетная (burst) передача разрешена только при включенном инкременте адреса.
                                                                                //  - Когда бит PINC = 0, то PBURST тоже должно быть равно 0b00
                                                                                //  - Когда бит MINC = 0, то MBURST тоже должно быть равно 0b00 
                                                                           
  
  
  
  //Запись структуры в контроллер
  DMA_Init(DMA1_Stream3, &DMA_InitStructure);
  
  
  
  //Разрешаем прямой доступ к памяти по передачи через USART3
  USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
  
  DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);
  
  DMA_Cmd(DMA1_Stream3, ENABLE);
  
  
  //Выключить лампочку STA1 на плате
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);
}


//Получение по флагу 
void Mode3(void){  
  uint8_t i =0;
  //Включить лампочку STA1 на плате
  GPIO_SetBits(PIN2_PORT, PIN2_PIN);
  
    
  do{
    //Wait until data in RX register
    while( USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);
    
    //Get from RX register to array
    recvBuff[i]=(char) USART_ReceiveData(USART3);
    i++;
    if(i>100){
      break;
    }
  }  
  while(recvBuff[i-1]!='a');
  
  //Выключить лампочку STA1 на плате
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);
}


//Получение по прерыванию 
void Mode4(void){
  //Структура для инициализации и настройки прерываний
  NVIC_InitTypeDef	NVIC_InitStruct;
  
  //Включить лампочку STA1 на плате
  GPIO_SetBits(PIN2_PORT, PIN2_PIN);
    

  //На приоритеты выделено 4 бита. Их можно распределить между основным приоритетеом и вспомогательным приоритетом 4 возможными вариантами
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //NVIC_PriorityGroup_2:       2 bits for pre-emption priority          2 bits for subpriority
  
  NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;                //Включаем прерывание от USART № 3  
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;        //Чем меньше значение - тем выше приоритет 2 бита = 4 значения от 4 до 0
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;               //Чем меньше значение - тем выше приоритет 2 бита = 4 значения от 4 до 0
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;                  //Включить или выключить прерывание описанное в поле NVIC_IRQChannel выше
  NVIC_Init(&NVIC_InitStruct);                                  //Записываем эту структуру
  
  //Разрешить обработку событий типа окончаний передачи (Transmit Data Register empty interrupt)
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  
  
  
  //Выключить лампочку STA1 на плате
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);  

}


void Mode5(void){

   // Структура для настройки прерывания
  NVIC_InitTypeDef NVIC_InitStructure;  
  //Структура для настройки  DMA. DMA - позволяет перемещать данные без участия ЦП  
  DMA_InitTypeDef  DMA_InitStructure;
  
  
  
  
  //Включить лампочку STA1 на плате
  GPIO_SetBits(PIN2_PORT, PIN2_PIN);
    
  
  //На приоритеты выделено 4 бита. Их можно распределить между основным приоритетеом и вспомогательным приоритетом 4 возможными вариантами
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//NVIC_PriorityGroup_2:       2 bits for pre-emption priority          2 bits for subpriority
   
  // Enable the USART2 RX DMA Interrupt 
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream1_IRQn;       //В контроллере прямого досутпа к памяти DMA1 есть несколько каналов. К каждому из каналов можно подключить один из 
                                                                //из источников данных (ВНИМАНИЕ! у каждого канала - свой набор, хоть и пересекающийся)  . Прироритеты каналов - чем меньше номер - тем выше приоритет.
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     //Чем меньше значение - тем выше приоритет 2 бита = 4 значения от 4 до 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            //Чем меньше значение - тем выше приоритет 2 бита = 4 значения от 4 до 0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //Включить или выключить прерывание описанное в поле NVIC_IRQChannel выше
  NVIC_Init(&NVIC_InitStructure);                               //Записываем эту структуру
  
  
  
  //Инициализируем DMA
  //http://www.mcustep.ru/init/stm32f4/46-dma-v-stm32f4-opisanie.html
  //Deinitialize the DMAy Streamx registers to their default reset values
  DMA_DeInit(DMA1_Stream1);                                                    
  
  //Инициализация полей структуры
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;                                //Specifies the channel used for the specified stream
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)recvBuff;
   
  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                       //Specifies if the data will be transferred from memory to peripheral or from memory to memory or from peripheral to memory.

  
  
  
  DMA_InitStructure.DMA_BufferSize = 5;        //!< Specifies the buffer size, in data unit, of the specified Stream. 
                                                //  The data unit is equal to the configuration set in DMA_PeripheralDataSize
                                                // or DMA_MemoryDataSize members depending in the transfer direction. 
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;               // Specifies the Memory data width. For memory to device
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;       //                                 For device to memory
    
  
  
  
  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              // Регистр данных никуда не смещается -это ведь символьное устройство - данные идут как по "трубе"
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       // Записывать результат в разные элементы массива, инкрементировать указатель после каждой записи


  
  
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                               //DMA_Mode_Normal
                                                                                //Циклический (круговой, замкнутый) режим предназначен для передачи циклических буферов и непрерывных потоков данных (например при сканировании ADC). 
                                                                                //Эта функция включается установкой бита CIRC в регистре DMA_SxCR. 
                                                                                //Когда используется замкнутый режим, то после каждой DMA транзакции количество передаваемых элементов данных автоматически перезагружается начальным значением NDTR, запрограммированным на этапе конфигурации потока. 
                                                                                //После каждой транзакции передача данных по DMA не прерывается, а начинается сначала с установленными ранее параметрами.
                                                                                //Замечание. Если используется пакетная передача "память<->FIFO буфер" (Mburst), то обязательно должно соблюдаться следующее условие:
                                                                                //DMA_SxNDTR должно быть кратно ((Mburst_beat) * Msize / Psize)
                                                                                //другими словами: DMA_SxNDTR должно делиться без остатка на ((Mburst_beat) * Msize / Psize ),
                                                                                //где 
                                                                                //Mburst_beat = 4, при MBURST = 01: INCR4 (incremental burst of 4 beats)
                                                                                //Mburst_beat = 8, при MBURST = 10: INCR8 (incremental burst of 8 beats)
                                                                                //Mburst_beat = 16, при MBURST = 11: INCR16 (incremental burst of 16 beats)
                                                                                //Отношение Msize / Psize может быть равным 1/4, 1/2, 1, 2, 4 в зависимости от MSIZE и PSIZE
                                                                                //Если это условие не соблюдается, то скорее всего нарушится целостность данных.
                                                                                //Кроме этого NTDR должно быть кратным (размер_пакета(Pburst) * размер_элемента_данных(Psize)) для участка передачи "FIFO буфер <-> периферия". Иначе поведение DMA будет непредсказуемым. Это условие нужно проверять если используется направление "из памяти в память" + пакетная передача (Pburst).
                                                                                  
  
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                           //Specifies the software priority for the DMAy Streamx (DMA_Priority_Low || DMA_Priority_Medium || DMA_Priority_High  || DMA_Priority_VeryHigh)
  
  
  
  
  
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                         //(FIFO enable) Когда используется это направление и бит EN в регистре DMA_SxCR установлен в 1, то при каждом запросе от периферии, данные помещаются в FIFO буфер. После достижения определенного порога, FIFO буфер опустошается и данные прибывают в место назначения. 
                                                                                //(Direct mode) В режиме Direct mode (бит DMDIS в регистре DMA_SxFCR установлен в "0"), данные в FIFO буфере не накапливаются до установленного порога, а немедленно передаются в место назначения.
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;                 //Specifies the FIFO threshold level.
  
  
  
  
  
  
  
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                   //It specifies the amount of data to be transferred in a single non interruptable transaction
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;           // Одиночная и пакетная передача данных (Single and burst transfers)
                                                                                //Контроллер DMA может передавать данные отдельными элементами (single) или неделимыми пакетами (порциями), состоящими из 4, 8 или 16 - ти элементов данных. Размер пакета устанавливается в PBURST и MBURST регистра DMA_SxCR, независимо для двух AHB портов. Размер указывает не количество байт, а количество элементов данных в пакете. В целях обеспечения согласованности каждая группа элементов данных, которая формирует пакет, неделима. Во время передачи пакета элементов, арбитр блокирует всякий доступ к AHB матрице шин, чем обеспечивается абсолютная атомарность трансфера. 
                                                                                //В зависимости от конфигурации xBURST, каждый запрос DMA может инициировать различное количество трансферов на порт AHB:
                                                                                //  -Когда порт AHB настроен на одиночные передачи, то каждый запрос DMA генерирует передачу только одного элемента данных (byte, half-word или word в зависимости от xSIZE битов в регистре DMA_SxCR).
                                                                                //
                                                                                //  -Когда порт AHB настроен на пакетную передачу, то каждый запрос DMA генерирует передачу 4-х, 8-ми или 16-ти элементов данных в зависимости от xBURST.
                                                                                //
                                                                                //  В прямом режиме (direct mode) поток может генерировать только одиночные (single) переводы и MBURST и PBURST не имеют значения.
                                                                                //
                                                                                //  Указатели адреса (регистры DMA_SxPAR or DMA_SxM0AR) должны быть выбраны таким образом, чтобы все переданные или принятые пакеты были выровнены по адресной границе, равной размеру передачи.
                                                                                //
                                                                                //  Конфигурация пакетной передачи должна быть в соответствии с протоколом AHB, где пакеты (burst) не должны пересекать адресную границу в 1КБ (1024 байт). Так как минимальное адресное пространство, которое может быть выделено для одного подчиненного равна 1КБ. Это значит, что пакет не должен "пересечь" эту границу, иначе будет получена неуловимая ошибка AHB.
                                                                                //
                                                                                //  Еще раз повторю, что пакетная (burst) передача разрешена только при включенном инкременте адреса.
                                                                                //  - Когда бит PINC = 0, то PBURST тоже должно быть равно 0b00
                                                                                //  - Когда бит MINC = 0, то MBURST тоже должно быть равно 0b00 
                                                                           
  
  
  
  //Запись структуры в контроллер
  DMA_Init(DMA1_Stream1, &DMA_InitStructure);
  
  
  
  //Разрешаем прямой доступ к памяти по приёму через USART3
  USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
  
  DMA_ITConfig(DMA1_Stream1, DMA_IT_TC, ENABLE);
  
  DMA_Cmd(DMA1_Stream1, ENABLE);
  
  
  
  //Выключить лампочку STA1 на плате
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);
  
  
  

}





// ----------------------------------------------------
//  INITERRUPT HANDLER. Жестко прошитое имя функции - обработчик  
// ----------------------------------------------------
//Скорее всего мы можем попасть в этот обработчик когда мы либо приняли байт либо закончили передачу байта
void USART3_IRQHandler(void){
  //Включить лампочку STA1 на плате
  GPIO_SetBits(PIN2_PORT, PIN2_PIN);
  //Если бит TXE (обозначающий то что событие окончание передачи произошло, то... . В режиме обработчки прерываний рекомендуют использовать этот набор функций  
  if( USART_GetITStatus(USART3,USART_IT_TXE) == SET){
    //Очистить флаг того что произошло прерывание.
    USART_ClearFlag(USART3, USART_IT_TXE);
    if(index < strlen(SendBuf)){
      //Отправить данные
      USART_SendData(USART3, SendBuf[index++]);
    }
    else{
        //Запретить обработку событий типа окончаний передачи (Transmit Data Register empty interrupt)
        USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
        index = 0;
    }
  }
  else if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET){    
    if(recvBuffCounter>100){
      recvBuffCounter=0;
    }
    recvBuff[recvBuffCounter]=(char) USART_ReceiveData(USART3);
    if(recvBuff[recvBuffCounter] == 'a'){
      USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
    } 
    recvBuffCounter++;
    
    //Данные приняты - можно очищать флаг
    USART_ClearFlag(USART3, USART_IT_RXNE);
  }
  
  //Выключить лампочку STA1 на плате
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);
}

void DMA1_Stream3_IRQHandler(void){
    //Инициализируем DMA
  //http://www.mcustep.ru/init/stm32f4/46-dma-v-stm32f4-opisanie.html
  //Deinitialize the DMAy Streamx registers to their default reset values
  DMA_DeInit(DMA1_Stream3); 
}


void DMA1_Stream1_IRQHandler(void){
    //Инициализируем DMA
  //http://www.mcustep.ru/init/stm32f4/46-dma-v-stm32f4-opisanie.html
  //Deinitialize the DMAy Streamx registers to their default reset values
  //int i =10;
  DMA_DeInit(DMA1_Stream1); 
}
