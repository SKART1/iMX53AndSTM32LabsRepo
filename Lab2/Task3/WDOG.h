/*
 * 	Description of interface and constants
 * 	necessary for correct work of module PWM
 *
 *  	Created on: Sep 28, 2014
 *      Author: Arthur Enikoff
 */

#ifndef WDOG_H_
#define WDOG_H_

	//Control registers base adresses see page 5014
	#define WDOG_CONFIG_REGISTERS_BASE_ADDRESS					0x53F98000		
	#define WDOG_REGISTERS_TOTAL_LENGTH 						10 
	#define WDOG_CONTROL_REGISTER_OFFSET_BYTES					0
	#define WDOG_SERVICE_REGISTER_OFFSET_BYTES 					2
	#define WDOG_RESET_STATUS_REGISTER_OFFSET_BYTES 			4
	#define WDOG_INTERRUPT_CONTROL_REGISTER_OFFSET_BYTES		6
	#define WDOG_MISCELLANEOUS_CONTROL_REGISTER_OFFSET_BYTES 	8
	
	//Interrupt before fire	
	#define WATCHDOG_BEFORE_FIRE_IRQ_NUMBER						58
	
	//WATCHDOG configure register bits description
	 /*
	  *Watchdog Time-out Field. This 8-bit field contains the time-out value that is loaded into the Watchdog
	  *counter after the service routine has been performed or after the Watchdog is enabled. After reset,
      *WT[7:0] must have a value written to it before enabling the Watchdog otherwise count value of zero which
	  *is 0.5 seconds is loaded into the counter.
	  *NOTE: The time-out value can be written at any point of time but it is loaded to the counter at the time
	  *when WDOG-1 is enabled or after the service routine has been performed. For more information
	  *see Time-Out Event.
	  *  0x00 - 0.5 Seconds (Default).
	  *  0x01 - 1.0 Seconds.
	  *  0x02 - 1.5 Seconds.
	  *  0x03 - 2.0 Seconds.
	  *  0xff - 128 Seconds.
	  */
	#define WT_TIME_OUT_FIELD_OFFSET				8	

	 /*
	  *Watchdog Disable for Wait. This bit determines the operation of WDOG-1 during Low Power WAIT mode.
	  *This is a write once only bit.
	  */
	#define WDW_AT_WAIT_CONTINUE					(0b0<<7)		/*Continue WDOG-1 timer operation (Default).*/
	#define WDW_AT_WAIT_SUSPEND						(0b1<<7)		/*Suspend WDOG-1 timer operation.*/

	 /*
	  *WDOG assertion. Controls the software assertion of the WDOG signal.
	  */
	#define WDA_ASSERT_WDOG_SIGNAL					(0b0<<5)		/*Assert WDOG output.*/
	#define WDA_NO_ASSERT_WDOG_SIGNAL				(0b1<<5)		/*No effect on system (Default)*/
								
	 /*
	  *Software Reset Signal. Controls the software assertion of the WDOG-generated reset signal wdog_rst. 
	  *This bit automatically resets to "1" after it has been asserted to "0".
	  *NOTE: This bit does not generate the software reset to the block.
	  */
	#define SRS_ASSERT_WDOG_RESET_SIGNAL			(0b0<<4)		/*Assert system reset signal*/
	#define SRS_NO_ASSERT_WDOG_RESET_SIGNAL			(0b1<<4)		/*No effect on the system (Default)*/
															
	 /*
	  *WDOG Time-out assertion. Determines if the WDOG gets asserted upon a Watchdog Time-out Event.
	  *This is a write-one once only bit.
	  *NOTE: There is no effect on wdog_rst (WDOG Reset) upon writing on this bit. WDOG gets asserted
	  *along with wdog_rst if this bit is set.
	  */
	#define WDT_TIMEOUT_ASSERTION_DISABLED			(0b0<<3)		/*No effect on WDOG (Default)*/
	#define WDT_TIMEOUT_ASSERTION_ENABLED			(0b1<<3)		/*Assert WDOG upon a Watchdog Time-out event*/
														
	 /*
	  *Watchdog Enable. Enables or disables the WDOG-1 block. This is a write one once only bit. It is not
	  *possible to clear this bit by a software write, once the bit is set
	  *NOTE: This bit can be set/reset in debug mode (exception).
	  */												
	#define WDE_WATCHDOG_DISABLED					(0b0<<2)		/*Disable the Watchdog (Default)*/
	#define WDE_WATCHDOG_ENABLED					(0b1<<2)		/*Enable the Watchdog.*/
													
	 /*
	  *Determines the operation of the WDOG-1 during DEBUG mode. This bit is write once-only
      */											
	#define WDBG_DEBUG_DISABLED						(0b0<<1)		/*Continue WDOG-1 timer operation (Default)*/
	#define WDBG_DEBUG_ENABLED						(0b1<<1)		/*Suspend the watchdog timer*/
												
	 /*
	  *Watchdog Low Power. Determines the operation of the WDOG-1 during low-power modes.
	  *This bit is write once-only												
	  */
	#define WDZST_LOW_POWER_DISABLED				(0b0<<0)		/*Continue timer operation (Default)*/
	#define WDZST_LOW_POWER_ENABLED					(0b1<<0)		/*Suspend the watchdog timer*/
													



													
	//WDOG_WICR WDOG-1 Interrupt control register 
	#define INTERRUPT_BEFORE_TIME_FIELD_OFFSET 		8				/*Number of 0.5 seconds interval when interrupt will occure before watchdog fire*/

	#define WDOG_INTERRUPT_HAS_NOT_OCCURED 			(0b0<<14)		/* No interrupt has occurred (Default). */
	#define WDOG_INTERRUPT_HAS_OCCURED 				(0b1<<14) 		/* Interrupt has occurred */

	#define WDOG_INTERRUPT_DISABLED 				(0b0<<15)  		/*Disable Interrupt (Default).*/
	#define WDOG_INTERRUPT_ENABLED 					(0b1<<15)  		/*Enable Interrupt */


	//WDOG_WMCR Miscellaneous Control Register
	#define PDE_DISABLE_POWER_DOWN_COUNTER 			(0b0<<15)		/*Stop power down counter (which count for 16 seconds and after that reset the system)*/
	
#endif /* WDOG_H_ */
