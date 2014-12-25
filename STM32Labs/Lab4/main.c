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
  
  
  /* ���������� ������������ ��� ����� F  G  */
  
  /*!< RCC AHB1 peripheral clock register,                          Address offset: 0x30 */
  /*   � ���� ������� ���������� */
  /* #define RCC_AHB1Periph_GPIOG             ((uint32_t)0x00000040) */
  /* � ������� ��� ���������� � ������� ( ENABLE ) */ 
  
  //�������� ������������ �����  BUTTON_RCC (RCC_AHB1Periph_GPIOG) - ����� G
  RCC_AHB1PeriphClockCmd(BUTTON_RCC, ENABLE);


  /*#define RCC_AHB1Periph_GPIOF             ((uint32_t)0x00000020)*/
  //�������� ������������ �����  PIN1_RCC (RCC_AHB1Periph_GPIOF) - ����� F
  RCC_AHB1PeriphClockCmd(PIN1_RCC, ENABLE);
  
  /**#define RCC_AHB1Periph_GPIOF             ((uint32_t)0x00000020)*/
  //�������� ������������ �����  PIN2_RCC (RCC_AHB1Periph_GPIOF) - ����� F
  RCC_AHB1PeriphClockCmd(PIN2_RCC, ENABLE);
  
  /* ��������� ������ ������ � ������ */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  
  
  
  
  // ----------------------------------------------------
  //  INIT GPIO
  // ----------------------------------------------------
  //�������������� ���� ����� ������ � ���� ����������� ��������� GPIO_InitStruct (����������)
  //��� ������ - ������ �� ������ ������������������ �� ����
  GPIO_InitStruct.GPIO_Pin = BUTTON_PIN;          // 6� ��� 
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;       // ��� ����� � ������ �� ����
  
  
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;     //�� �� ������� ��� �������� ���� �� ���� � ��� ������ ��� ��������� �� ����
                                                  // �������� ��������� - ������� ���� (�.�.) �������� ����������. ���� ����������� ������������� (�������� ����������). ������ ��� �����!!!!
                                                  //GPIO_OType_PP - ����������� ������������� ��������. ������ ���� ���� ���������� � �������, ���� ������ � �����. 
                                                  // 0: Output push-pull (reset state)
                                                  // 1: Output open-drain
  
  
  
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz; //����� ������������ ����� 
  
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;   //�������� ���� �� ����� - ������ ��������� � 1. �� �����������
  GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);       //������ �������� - ����. ���������� ��� ������ � ��������� GPIO_InitStruct. 
  
  
    
  //�������������� ���� ��� ����������
  //��������� - �����
  GPIO_InitStruct.GPIO_Pin = PIN1_PIN;          //��� N ����� 
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;    //��� ����� �� �����  
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   //����������� ������������� �����    
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz; //����� ������������ ����� 
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;   //�������� ���� �� ����� - ������ ��������� � 1. �� �����������
    
  GPIO_Init(PIN1_PORT, &GPIO_InitStruct);       //������ �������� - ����. ���������� ��� ������ � ��������� GPIO_InitStruct. 
  
  //�������������� ������ ��������� - ���� �����
  GPIO_InitStruct.GPIO_Pin = PIN2_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;    //��� ����� �� �����  
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   //����������� ������������� ����� 
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz; //����� ������������ ����� 
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;   //�������� ���� �� ����� - ������ ��������� � 1. �� �����������

  GPIO_Init(PIN2_PORT, &GPIO_InitStruct);      //������ �������� - ����. ���������� ��� ������ � ��������� GPIO_InitStruct. 
  
  
  //��������� ��������� - ��� N ����� F
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN); 
  
  // ----------------------------------------------------
  //  INIT USART
  // ----------------------------------------------------
  //   ��� ������ �������� �������������� - �   board_config.h   
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);         // �������� ������������ ����� D 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);        // �������� ������������ ���������� ����� 
  
  /**
  * @brief  Changes the mapping of the specified pin.
  * @param  GPIOx: where x can be (A..K) to select the GPIO peripheral for STM32F405xx/407xx and STM32F415xx/417xx devices
  *                      x can be (A..I) to select the GPIO peripheral for STM32F42xxx/43xxx devices.
  *                      x can be (A, B, C, D and H) to select the GPIO peripheral for STM32F401xx devices. 
  * @param  GPIO_PinSource: specifies the pin for the Alternate function.
  *         This parameter can be GPIO_PinSourcex where x can be (0..15).
  * @param  GPIO_AFSelection: selects the pin to used as Alternate function.
  */  
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);     // ������������� ����  D  ��� 8 �� ����� �������������� ������� UART 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);     // ������������� ����  D  ��� 9 �� ����� �������������� ������� UART 
  
  //������������� ���� 8 ����� D ��� ������� ����� (�������������� �������, ��� ������ - ����������� ������������� ��������, ��������, ��������)
  GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AF;                 // !!!!!GPIO Alternate function Mode 
  GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_100MHz;            
  GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;                
  GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;

  GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_8;
  GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  //������������� ���� 9 ����� D ��� ������� ����� (�������������� �������, ��� ������ - ����������� ������������� ��������, ��������, ��������)
  GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_9;
  GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  
  //������������� USART - ��� �����. ��������. ���������� ���. ���������� �������� ���, ׸������ � �.�. - ��. USART
  USART_InitStruct.USART_BaudRate       = 115200 ;
  USART_InitStruct.USART_WordLength     = USART_WordLength_9b;
  USART_InitStruct.USART_StopBits       = USART_StopBits_1;
  USART_InitStruct.USART_Parity         = USART_Parity_Even;
  USART_InitStruct.USART_HardwareFlowControl    = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode           = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART3, &USART_InitStruct);
  
  //�������� USART3
  USART_Cmd(USART3, ENABLE);
  
  //������ ��������
  //USART_SendData(USART3, 0x55);

  //������� ����
  while(1){
    //������� ��������� ������
    if(GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == BIT_COND){
      //����� 0 - �������� ����� � ����� ��������� ����� ���������� � �����.
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
      //��������� �� ����� ����������
      else if(mode==3){
        Mode3();
        mode++;      
      }
      //��������� �� ����������
      else if(mode==4){
        Mode4();
        mode++;      
      }
      //��������� �� DMA
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
  //�������� �������� STA1 �� �����
  GPIO_SetBits(PIN2_PORT, PIN2_PIN);
  
  for(i = 0; i <(uint8_t) strlen(SendBuf); i++){
    //���� ����� TXE. ������� ��� ��������. ���� ���� ���������� - ���������� �����. ���������� ����� ���� ��������� � ����������� ���� ���������� ����� ���� ����������� - ������� ���������� ���� ����
    while( USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    //��������� ��������� ����
    USART_SendData(USART3, SendBuf[i]);
  }
  
  //��������� �������� STA1 �� �����
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);
}

void Mode1(void){
 //��������� ��� ������������� � ��������� ����������
  NVIC_InitTypeDef	NVIC_InitStruct;
  
  //�������� �������� STA1 �� �����
  GPIO_SetBits(PIN2_PORT, PIN2_PIN);
    

  //�� ���������� �������� 4 ����. �� ����� ������������ ����� �������� ������������ � ��������������� ����������� 4 ���������� ����������
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //NVIC_PriorityGroup_2:       2 bits for pre-emption priority          2 bits for subpriority
  
  NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;                //�������� ���������� �� USART � 3  
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;        //��� ������ �������� - ��� ���� ��������� 2 ���� = 4 �������� �� 4 �� 0
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;               //��� ������ �������� - ��� ���� ��������� 2 ���� = 4 �������� �� 4 �� 0
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;                  //�������� ��� ��������� ���������� ��������� � ���� NVIC_IRQChannel ����
  NVIC_Init(&NVIC_InitStruct);                                  //���������� ��� ���������
  
  //��������� ��������� ������� ���� ��������� �������� (Transmit Data Register empty interrupt)
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  
  //��������� �������� STA1 �� �����
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);  
}



void Mode2(void){  
  // ��������� ��� ��������� ����������
  NVIC_InitTypeDef NVIC_InitStructure;  
  //��������� ��� ���������  DMA. DMA - ��������� ���������� ������ ��� ������� ��  
  DMA_InitTypeDef  DMA_InitStructure;
  
  
  
  
  //�������� �������� STA1 �� �����
  GPIO_SetBits(PIN2_PORT, PIN2_PIN);
    
  
  //�� ���������� �������� 4 ����. �� ����� ������������ ����� �������� ������������ � ��������������� ����������� 4 ���������� ����������
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//NVIC_PriorityGroup_2:       2 bits for pre-emption priority          2 bits for subpriority
   
  /* Enable the USART2 RX DMA Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;       //� ����������� ������� ������� � ������ DMA1 ���� ��������� �������. � ������� �� ������� ����� ���������� ���� �� 
                                                                //�� ���������� ������ (��������! � ������� ������ - ���� �����, ���� � ��������������)  . ����������� ������� - ��� ������ ����� - ��� ���� ���������.
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     //��� ������ �������� - ��� ���� ��������� 2 ���� = 4 �������� �� 4 �� 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            //��� ������ �������� - ��� ���� ��������� 2 ���� = 4 �������� �� 4 �� 0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //�������� ��� ��������� ���������� ��������� � ���� NVIC_IRQChannel ����
  NVIC_Init(&NVIC_InitStructure);                               //���������� ��� ���������
  
  
  
  //�������������� DMA
  //http://www.mcustep.ru/init/stm32f4/46-dma-v-stm32f4-opisanie.html
  //Deinitialize the DMAy Streamx registers to their default reset values
  DMA_DeInit(DMA1_Stream3);                                                    
  
  //������������� ����� ���������
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;                                //Specifies the channel used for the specified stream
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SendBuf;
   
  
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                       //Specifies if the data will be transferred from memory to peripheral or from memory to memory or from peripheral to memory.

  
  
  
  DMA_InitStructure.DMA_BufferSize = 5;        /*!< Specifies the buffer size, in data unit, of the specified Stream. 
                                          The data unit is equal to the configuration set in DMA_PeripheralDataSize
                                          or DMA_MemoryDataSize members depending in the transfer direction. */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;               // Specifies the Memory data width. For memory to device
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;       //                                 For device to memory
    
  
  
  
  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              // ������� ������ ������ �� ��������� -��� ���� ���������� ���������� - ������ ���� ��� �� "�����"
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       // ���������� ��������� � ������ �������� �������, ���������������� ��������� ����� ������ ������


  
  
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                               //DMA_Mode_Normal
                                                                                //����������� (��������, ���������) ����� ������������ ��� �������� ����������� ������� � ����������� ������� ������ (�������� ��� ������������ ADC). 
                                                                                //��� ������� ���������� ���������� ���� CIRC � �������� DMA_SxCR. 
                                                                                //����� ������������ ��������� �����, �� ����� ������ DMA ���������� ���������� ������������ ��������� ������ ������������� ��������������� ��������� ��������� NDTR, ������������������� �� ����� ������������ ������. 
                                                                                //����� ������ ���������� �������� ������ �� DMA �� �����������, � ���������� ������� � �������������� ����� �����������.
                                                                                //���������. ���� ������������ �������� �������� "������<->FIFO �����" (Mburst), �� ����������� ������ ����������� ��������� �������:
                                                                                //DMA_SxNDTR ������ ���� ������ ((Mburst_beat) * Msize / Psize)
                                                                                //������� �������: DMA_SxNDTR ������ �������� ��� ������� �� ((Mburst_beat) * Msize / Psize ),
                                                                                //��� 
                                                                                //Mburst_beat = 4, ��� MBURST = 01: INCR4 (incremental burst of 4 beats)
                                                                                //Mburst_beat = 8, ��� MBURST = 10: INCR8 (incremental burst of 8 beats)
                                                                                //Mburst_beat = 16, ��� MBURST = 11: INCR16 (incremental burst of 16 beats)
                                                                                //��������� Msize / Psize ����� ���� ������ 1/4, 1/2, 1, 2, 4 � ����������� �� MSIZE � PSIZE
                                                                                //���� ��� ������� �� �����������, �� ������ ����� ��������� ����������� ������.
                                                                                //����� ����� NTDR ������ ���� ������� (������_������(Pburst) * ������_��������_������(Psize)) ��� ������� �������� "FIFO ����� <-> ���������". ����� ��������� DMA ����� ���������������. ��� ������� ����� ��������� ���� ������������ ����������� "�� ������ � ������" + �������� �������� (Pburst).
                                                                                  
  
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                           //Specifies the software priority for the DMAy Streamx (DMA_Priority_Low || DMA_Priority_Medium || DMA_Priority_High  || DMA_Priority_VeryHigh)
  
  
  
  
  
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;                         //(FIFO enable) ����� ������������ ��� ����������� � ��� EN � �������� DMA_SxCR ���������� � 1, �� ��� ������ ������� �� ���������, ������ ���������� � FIFO �����. ����� ���������� ������������� ������, FIFO ����� ������������ � ������ ��������� � ����� ����������. 
                                                                                //(Direct mode) � ������ Direct mode (��� DMDIS � �������� DMA_SxFCR ���������� � "0"), ������ � FIFO ������ �� ������������� �� �������������� ������, � ���������� ���������� � ����� ����������.
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;                 //Specifies the FIFO threshold level.
  
  
  
  
  
  
  
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                   //It specifies the amount of data to be transferred in a single non interruptable transaction
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;           // ��������� � �������� �������� ������ (Single and burst transfers)
                                                                                //���������� DMA ����� ���������� ������ ���������� ���������� (single) ��� ���������� �������� (��������), ���������� �� 4, 8 ��� 16 - �� ��������� ������. ������ ������ ��������������� � PBURST � MBURST �������� DMA_SxCR, ���������� ��� ���� AHB ������. ������ ��������� �� ���������� ����, � ���������� ��������� ������ � ������. � ����� ����������� ��������������� ������ ������ ��������� ������, ������� ��������� �����, ��������. �� ����� �������� ������ ���������, ������ ��������� ������ ������ � AHB ������� ���, ��� �������������� ���������� ����������� ���������. 
                                                                                //� ����������� �� ������������ xBURST, ������ ������ DMA ����� ������������ ��������� ���������� ���������� �� ���� AHB:
                                                                                //  -����� ���� AHB �������� �� ��������� ��������, �� ������ ������ DMA ���������� �������� ������ ������ �������� ������ (byte, half-word ��� word � ����������� �� xSIZE ����� � �������� DMA_SxCR).
                                                                                //
                                                                                //  -����� ���� AHB �������� �� �������� ��������, �� ������ ������ DMA ���������� �������� 4-�, 8-�� ��� 16-�� ��������� ������ � ����������� �� xBURST.
                                                                                //
                                                                                //  � ������ ������ (direct mode) ����� ����� ������������ ������ ��������� (single) �������� � MBURST � PBURST �� ����� ��������.
                                                                                //
                                                                                //  ��������� ������ (�������� DMA_SxPAR or DMA_SxM0AR) ������ ���� ������� ����� �������, ����� ��� ���������� ��� �������� ������ ���� ��������� �� �������� �������, ������ ������� ��������.
                                                                                //
                                                                                //  ������������ �������� �������� ������ ���� � ������������ � ���������� AHB, ��� ������ (burst) �� ������ ���������� �������� ������� � 1�� (1024 ����). ��� ��� ����������� �������� ������������, ������� ����� ���� �������� ��� ������ ������������ ����� 1��. ��� ������, ��� ����� �� ������ "��������" ��� �������, ����� ����� �������� ���������� ������ AHB.
                                                                                //
                                                                                //  ��� ��� �������, ��� �������� (burst) �������� ��������� ������ ��� ���������� ���������� ������.
                                                                                //  - ����� ��� PINC = 0, �� PBURST ���� ������ ���� ����� 0b00
                                                                                //  - ����� ��� MINC = 0, �� MBURST ���� ������ ���� ����� 0b00 
                                                                           
  
  
  
  //������ ��������� � ����������
  DMA_Init(DMA1_Stream3, &DMA_InitStructure);
  
  
  
  //��������� ������ ������ � ������ �� �������� ����� USART3
  USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
  
  DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);
  
  DMA_Cmd(DMA1_Stream3, ENABLE);
  
  
  //��������� �������� STA1 �� �����
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);
}


//��������� �� ����� 
void Mode3(void){  
  uint8_t i =0;
  //�������� �������� STA1 �� �����
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
  
  //��������� �������� STA1 �� �����
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);
}


//��������� �� ���������� 
void Mode4(void){
  //��������� ��� ������������� � ��������� ����������
  NVIC_InitTypeDef	NVIC_InitStruct;
  
  //�������� �������� STA1 �� �����
  GPIO_SetBits(PIN2_PORT, PIN2_PIN);
    

  //�� ���������� �������� 4 ����. �� ����� ������������ ����� �������� ������������ � ��������������� ����������� 4 ���������� ����������
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //NVIC_PriorityGroup_2:       2 bits for pre-emption priority          2 bits for subpriority
  
  NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;                //�������� ���������� �� USART � 3  
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;        //��� ������ �������� - ��� ���� ��������� 2 ���� = 4 �������� �� 4 �� 0
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;               //��� ������ �������� - ��� ���� ��������� 2 ���� = 4 �������� �� 4 �� 0
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;                  //�������� ��� ��������� ���������� ��������� � ���� NVIC_IRQChannel ����
  NVIC_Init(&NVIC_InitStruct);                                  //���������� ��� ���������
  
  //��������� ��������� ������� ���� ��������� �������� (Transmit Data Register empty interrupt)
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  
  
  
  //��������� �������� STA1 �� �����
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);  

}


void Mode5(void){

   // ��������� ��� ��������� ����������
  NVIC_InitTypeDef NVIC_InitStructure;  
  //��������� ��� ���������  DMA. DMA - ��������� ���������� ������ ��� ������� ��  
  DMA_InitTypeDef  DMA_InitStructure;
  
  
  
  
  //�������� �������� STA1 �� �����
  GPIO_SetBits(PIN2_PORT, PIN2_PIN);
    
  
  //�� ���������� �������� 4 ����. �� ����� ������������ ����� �������� ������������ � ��������������� ����������� 4 ���������� ����������
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//NVIC_PriorityGroup_2:       2 bits for pre-emption priority          2 bits for subpriority
   
  // Enable the USART2 RX DMA Interrupt 
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream1_IRQn;       //� ����������� ������� ������� � ������ DMA1 ���� ��������� �������. � ������� �� ������� ����� ���������� ���� �� 
                                                                //�� ���������� ������ (��������! � ������� ������ - ���� �����, ���� � ��������������)  . ����������� ������� - ��� ������ ����� - ��� ���� ���������.
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     //��� ������ �������� - ��� ���� ��������� 2 ���� = 4 �������� �� 4 �� 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            //��� ������ �������� - ��� ���� ��������� 2 ���� = 4 �������� �� 4 �� 0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //�������� ��� ��������� ���������� ��������� � ���� NVIC_IRQChannel ����
  NVIC_Init(&NVIC_InitStructure);                               //���������� ��� ���������
  
  
  
  //�������������� DMA
  //http://www.mcustep.ru/init/stm32f4/46-dma-v-stm32f4-opisanie.html
  //Deinitialize the DMAy Streamx registers to their default reset values
  DMA_DeInit(DMA1_Stream1);                                                    
  
  //������������� ����� ���������
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;                                //Specifies the channel used for the specified stream
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)recvBuff;
   
  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                       //Specifies if the data will be transferred from memory to peripheral or from memory to memory or from peripheral to memory.

  
  
  
  DMA_InitStructure.DMA_BufferSize = 5;        //!< Specifies the buffer size, in data unit, of the specified Stream. 
                                                //  The data unit is equal to the configuration set in DMA_PeripheralDataSize
                                                // or DMA_MemoryDataSize members depending in the transfer direction. 
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;               // Specifies the Memory data width. For memory to device
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;       //                                 For device to memory
    
  
  
  
  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              // ������� ������ ������ �� ��������� -��� ���� ���������� ���������� - ������ ���� ��� �� "�����"
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       // ���������� ��������� � ������ �������� �������, ���������������� ��������� ����� ������ ������


  
  
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                               //DMA_Mode_Normal
                                                                                //����������� (��������, ���������) ����� ������������ ��� �������� ����������� ������� � ����������� ������� ������ (�������� ��� ������������ ADC). 
                                                                                //��� ������� ���������� ���������� ���� CIRC � �������� DMA_SxCR. 
                                                                                //����� ������������ ��������� �����, �� ����� ������ DMA ���������� ���������� ������������ ��������� ������ ������������� ��������������� ��������� ��������� NDTR, ������������������� �� ����� ������������ ������. 
                                                                                //����� ������ ���������� �������� ������ �� DMA �� �����������, � ���������� ������� � �������������� ����� �����������.
                                                                                //���������. ���� ������������ �������� �������� "������<->FIFO �����" (Mburst), �� ����������� ������ ����������� ��������� �������:
                                                                                //DMA_SxNDTR ������ ���� ������ ((Mburst_beat) * Msize / Psize)
                                                                                //������� �������: DMA_SxNDTR ������ �������� ��� ������� �� ((Mburst_beat) * Msize / Psize ),
                                                                                //��� 
                                                                                //Mburst_beat = 4, ��� MBURST = 01: INCR4 (incremental burst of 4 beats)
                                                                                //Mburst_beat = 8, ��� MBURST = 10: INCR8 (incremental burst of 8 beats)
                                                                                //Mburst_beat = 16, ��� MBURST = 11: INCR16 (incremental burst of 16 beats)
                                                                                //��������� Msize / Psize ����� ���� ������ 1/4, 1/2, 1, 2, 4 � ����������� �� MSIZE � PSIZE
                                                                                //���� ��� ������� �� �����������, �� ������ ����� ��������� ����������� ������.
                                                                                //����� ����� NTDR ������ ���� ������� (������_������(Pburst) * ������_��������_������(Psize)) ��� ������� �������� "FIFO ����� <-> ���������". ����� ��������� DMA ����� ���������������. ��� ������� ����� ��������� ���� ������������ ����������� "�� ������ � ������" + �������� �������� (Pburst).
                                                                                  
  
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                           //Specifies the software priority for the DMAy Streamx (DMA_Priority_Low || DMA_Priority_Medium || DMA_Priority_High  || DMA_Priority_VeryHigh)
  
  
  
  
  
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                         //(FIFO enable) ����� ������������ ��� ����������� � ��� EN � �������� DMA_SxCR ���������� � 1, �� ��� ������ ������� �� ���������, ������ ���������� � FIFO �����. ����� ���������� ������������� ������, FIFO ����� ������������ � ������ ��������� � ����� ����������. 
                                                                                //(Direct mode) � ������ Direct mode (��� DMDIS � �������� DMA_SxFCR ���������� � "0"), ������ � FIFO ������ �� ������������� �� �������������� ������, � ���������� ���������� � ����� ����������.
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;                 //Specifies the FIFO threshold level.
  
  
  
  
  
  
  
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                   //It specifies the amount of data to be transferred in a single non interruptable transaction
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;           // ��������� � �������� �������� ������ (Single and burst transfers)
                                                                                //���������� DMA ����� ���������� ������ ���������� ���������� (single) ��� ���������� �������� (��������), ���������� �� 4, 8 ��� 16 - �� ��������� ������. ������ ������ ��������������� � PBURST � MBURST �������� DMA_SxCR, ���������� ��� ���� AHB ������. ������ ��������� �� ���������� ����, � ���������� ��������� ������ � ������. � ����� ����������� ��������������� ������ ������ ��������� ������, ������� ��������� �����, ��������. �� ����� �������� ������ ���������, ������ ��������� ������ ������ � AHB ������� ���, ��� �������������� ���������� ����������� ���������. 
                                                                                //� ����������� �� ������������ xBURST, ������ ������ DMA ����� ������������ ��������� ���������� ���������� �� ���� AHB:
                                                                                //  -����� ���� AHB �������� �� ��������� ��������, �� ������ ������ DMA ���������� �������� ������ ������ �������� ������ (byte, half-word ��� word � ����������� �� xSIZE ����� � �������� DMA_SxCR).
                                                                                //
                                                                                //  -����� ���� AHB �������� �� �������� ��������, �� ������ ������ DMA ���������� �������� 4-�, 8-�� ��� 16-�� ��������� ������ � ����������� �� xBURST.
                                                                                //
                                                                                //  � ������ ������ (direct mode) ����� ����� ������������ ������ ��������� (single) �������� � MBURST � PBURST �� ����� ��������.
                                                                                //
                                                                                //  ��������� ������ (�������� DMA_SxPAR or DMA_SxM0AR) ������ ���� ������� ����� �������, ����� ��� ���������� ��� �������� ������ ���� ��������� �� �������� �������, ������ ������� ��������.
                                                                                //
                                                                                //  ������������ �������� �������� ������ ���� � ������������ � ���������� AHB, ��� ������ (burst) �� ������ ���������� �������� ������� � 1�� (1024 ����). ��� ��� ����������� �������� ������������, ������� ����� ���� �������� ��� ������ ������������ ����� 1��. ��� ������, ��� ����� �� ������ "��������" ��� �������, ����� ����� �������� ���������� ������ AHB.
                                                                                //
                                                                                //  ��� ��� �������, ��� �������� (burst) �������� ��������� ������ ��� ���������� ���������� ������.
                                                                                //  - ����� ��� PINC = 0, �� PBURST ���� ������ ���� ����� 0b00
                                                                                //  - ����� ��� MINC = 0, �� MBURST ���� ������ ���� ����� 0b00 
                                                                           
  
  
  
  //������ ��������� � ����������
  DMA_Init(DMA1_Stream1, &DMA_InitStructure);
  
  
  
  //��������� ������ ������ � ������ �� ����� ����� USART3
  USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
  
  DMA_ITConfig(DMA1_Stream1, DMA_IT_TC, ENABLE);
  
  DMA_Cmd(DMA1_Stream1, ENABLE);
  
  
  
  //��������� �������� STA1 �� �����
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);
  
  
  

}





// ----------------------------------------------------
//  INITERRUPT HANDLER. ������ �������� ��� ������� - ����������  
// ----------------------------------------------------
//������ ����� �� ����� ������� � ���� ���������� ����� �� ���� ������� ���� ���� ��������� �������� �����
void USART3_IRQHandler(void){
  //�������� �������� STA1 �� �����
  GPIO_SetBits(PIN2_PORT, PIN2_PIN);
  //���� ��� TXE (������������ �� ��� ������� ��������� �������� ���������, ��... . � ������ ���������� ���������� ����������� ������������ ���� ����� �������  
  if( USART_GetITStatus(USART3,USART_IT_TXE) == SET){
    //�������� ���� ���� ��� ��������� ����������.
    USART_ClearFlag(USART3, USART_IT_TXE);
    if(index < strlen(SendBuf)){
      //��������� ������
      USART_SendData(USART3, SendBuf[index++]);
    }
    else{
        //��������� ��������� ������� ���� ��������� �������� (Transmit Data Register empty interrupt)
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
    
    //������ ������� - ����� ������� ����
    USART_ClearFlag(USART3, USART_IT_RXNE);
  }
  
  //��������� �������� STA1 �� �����
  GPIO_ResetBits(PIN2_PORT, PIN2_PIN);
}

void DMA1_Stream3_IRQHandler(void){
    //�������������� DMA
  //http://www.mcustep.ru/init/stm32f4/46-dma-v-stm32f4-opisanie.html
  //Deinitialize the DMAy Streamx registers to their default reset values
  DMA_DeInit(DMA1_Stream3); 
}


void DMA1_Stream1_IRQHandler(void){
    //�������������� DMA
  //http://www.mcustep.ru/init/stm32f4/46-dma-v-stm32f4-opisanie.html
  //Deinitialize the DMAy Streamx registers to their default reset values
  //int i =10;
  DMA_DeInit(DMA1_Stream1); 
}
