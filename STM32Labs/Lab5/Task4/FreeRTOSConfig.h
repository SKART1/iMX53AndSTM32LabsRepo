/*
    FreeRTOS V7.4.0 - Copyright (C) 2013 Real Time Engineers Ltd.

    FEATURES AND PORTS ARE ADDED TO FREERTOS ALL THE TIME.  PLEASE VISIT
    http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.

    >>>>>>NOTE<<<<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details. You should have received a copy of the GNU General Public License
    and the FreeRTOS license exception along with FreeRTOS; if not itcan be
    viewed here: http://www.freertos.org/a00114.html and also obtained by
    writing to Real Time Engineers Ltd., contact details for whom are available
    on the FreeRTOS WEB site.

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************


    http://www.FreeRTOS.org - Documentation, books, training, latest versions, 
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, and our new
    fully thread aware and reentrant UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High 
    Integrity Systems, who sell the code with commercial support, 
    indemnification and middleware, under the OpenRTOS brand.
    
    http://www.SafeRTOS.com - High Integrity Systems also provide a safety 
    engineered and independently SIL3 certified version for use in safety and 
    mission critical applications that require provable dependability.
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define RVDS_ARMCM4F
#define configUSE_PREEMPTION		1  	                                // если равно 1, использовать вытесняющее ядро, 0 - кооперативное ядро.
										//если 0 - то вызов планировщика долэжен быть явным из кода	


//Create an idle priority task to implement the functionality.
//This is a more flexible solution but has a higher RAM usage overhead.
#define configUSE_IDLE_HOOK		1  	                                //An idle task hook is a function that is called during each cycle of the idle task. If you want application functionality to run at the idle priority then there are two options:
										//Implement the functionality in an idle task hook.
										//There must always be at least one task that is ready to run. It is therefore imperative that the hook function does not call any API functions that might cause the idle task to block (vTaskDelay(), or a queue or semaphore function with a block time, for example). It is ok for co-routines to block within the hook function.

										

#define configUSE_TICK_HOOK		0	                                //The tick interrupt can optionally call an application defined hook (or callback) function - the tick hook. The tick hook provides a convenient place to implement timer functionality.
										//The tick hook will only get called if configUSE_TICK_HOOK is set to 1 within FreeRTOSConfig.h. When this is set the application must provide the hook function with the following prototype:
										//vApplicationTickHook() executes from within an ISR so must be very short, not use much stack, and not call any API functions that don't end in "FromISR" or "FROM_ISR".
										//See the demo application file crhook.c for an example of how to use a tick hook.



#define configCPU_CLOCK_HZ			( ( unsigned long ) 48000000 )	//Enter the frequency in Hz at which the internal clock that driver the peripheral used to generate the tick interrupt will be executing - 
										//this is normally the same clock that drives the internal CPU clock. This value is required in order to correctly configure timer peripherals.
										


#define configTICK_RATE_HZ			( ( portTickType ) 48000 )	//The frequency of the RTOS tick interrupt.
										//The tick interrupt is used to measure time. Therefore a higher tick frequency means time can be measured to a higher resolution.
										




#define configMAX_PRIORITIES		( ( unsigned portBASE_TYPE ) 5 )	//The number of priorities available to the application tasks. Any number of tasks can share the same priority. 
										//Co-routines are prioritised separately - see configMAX_CO_ROUTINE_PRIORITIES.




#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 128 )		//The size of the stack used by the idle task. Generally this should not be reduced from the value set in the FreeRTOSConfig.h file provided with the demo application for the port you are using.
										//Like the stack size parameter to the xTaskCreate() function, the stack size is specified in words, not bytes.			



#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 17 * 1024 ) )		//The total amount of RAM available to the RTOS kernel.



#define configMAX_TASK_NAME_LEN		( 16 )					//The maximum permissible length of the descriptive name given to a task when the task is created. 


#define configUSE_TRACE_FACILITY	0					//Включить выключить трассировку (что это?)


#define configUSE_16_BIT_TICKS		0					//Causes TickType_t to be defined (typedef'ed) as an unsigned 16bit type. 
										//Если 0 - используется 32-битный unsigned. 16 бит подходит для 8 и 16 битной архитектуры процессора, при этом максимальное значение тиков равно 65535 тиков. 
										//Максимальное время задержки или блокировки при 16-битном счетчике составляет 262 сек, при 32 бит - 17179869 сек.


#define configIDLE_SHOULD_YIELD		1					//Tasks that share the same priority will time slice. Assuming none of the tasks get preempted, it might be assumed that each task of at a given priority will 
										//be allocated an equal amount of processing time - and if the shared priority is above the idle priority then this is indeed the case.


#define configQUEUE_REGISTRY_SIZE       10                                      //The queue registry has two purposes, both of which are associated with RTOS kernel aware debugging:
                                                                                //It allows a textual name to be associated with a queue for easy queue identification within a debugging GUI.
                                                                                //It contains the information required by a debugger to locate each registered queue and semaphore.
                                                                                //configQUEUE_REGISTRY_SIZE defines the maximum number of queues and semaphores that can be registered. Only those queues and semaphores that you want to view using a RTOS kernel aware debugger need be registered.


#define configUSE_MUTEXES               1                                       //Разрешить мьютексы

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0                                       //Coroutine in FreeRTOS is a very simple and lightweight task that has very limited use of stack
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )                                   //The number of priorities available to the application co-routines. Any number of co-routines can share the same priority. Tasks are prioritised separately - see configMAX_PRIORITIES.





/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete			1
#define INCLUDE_vTaskCleanUpResources	        0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay			1
#define INCLUDE_xTaskGetSchedulerState          1





//Суть в чем. Есть у нас прерывание диспетчера. И в этом прерывании он что-то делает со своими структурами (очередями, задачами и прочей системной фигней).
//Но диспетчер это не самая важная вещь на свете, поэтому его часто вешают на самое зачуханное прерывание с самым низким приоритетом. И его все могут перебивать. 
//Но есть нюанс. Допустим диспетчер что-то делает с очередью, смотрит кого там можно разблокировать по ее состоянию, пишет туда что-то свое, а тут опа прерывание и в нем некая [****]FromISR записала в ту самую очередь. Что у нас будет? У нас будет лажа. Т.к. на выходе из прерывания диспетчер похерит все что предыдущее прерывание туда писало — он то запись до конца не провел. Т.е. налицо классическое нарушение атомарности.
//Чтобы этого не было, диспетчер на время критических записей запрещает прерывания. Чтобы ему никто не мог помешать делать свое дело. Но запрещает он не все прерывания, а только определенную группу. Скажем все прерывания с приоритетом (меньше число, старше приоритет) 15 по 11, а 1 по 10 нет. В результате на 1 по 10 мы можем вешать что то ну очень сильно важное и никакой диспетчер это не перебьет. 
/* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255
(lowest) to 0 (1?) (highest). */
#define configKERNEL_INTERRUPT_PRIORITY 		255
//у STM32 в байте с приоритетом играет роль только старшая тетрада, т.е. у нас есть 16 уровней приоритетов от старшего к младшему: 0х00, 0х10, 0х20, 0х30…0xF0
//Т.е. 255 это уровень 0xF0 — самый младший, а 191 уровень 0xB0 и, таким образом, все прерывания в которых мы можем использовать API фукнции должны быть сконфигурированы с приоритетом от 0xF0 до 0xB0, не старше. 
//Иначе будет трудноуловимый глюк. 
//Прерывания же не использующие API могут быть с каким угодно приоритетом от самого низкого до самого старшего. 
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	        191 /* equivalent to 0xb0, or priority 11. */
/* This is the value being used as per the ST library which permits 16
priority values, 0 to 15.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY	15





//Тем самым, мы привяжем вектора прерываний на диспетчер ОС. Как вы можете видеть
//SysTick используется как таймер службы диспетчера. У другого МК тут можно повесить
//Другой таймер.
#define vPortSVCHandler         SVC_Handler
#define xPortPendSVHandler      PendSV_Handler
#define xPortSysTickHandler     SysTick_Handler

#endif /* FREERTOS_CONFIG_H */

