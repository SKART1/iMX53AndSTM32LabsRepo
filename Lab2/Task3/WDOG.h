/*
 * 	Description of interface and constants
 * 	necessary for correct work of module PWM
 *
 *  	Created on: Sep 28, 2014
 *      Author: Arthur Enikoff
 */

#ifndef WDOG_H_
#define WDOG_H_


	#define WATCH_DOG_CONFIG_REGISTERS_BASE_ADDRESS	0x53F98000					
	

	
	
	//WATCHDOG configure register bits description
	/*Watchdog Time-out Field. This 8-bit field contains the time-out value that is loaded into the Watchdog
	  counter after the service routine has been performed or after the Watchdog is enabled. After reset,
      WT[7:0] must have a value written to it before enabling the Watchdog otherwise count value of zero which
	  is 0.5 seconds is loaded into the counter.
	  NOTE: The time-out value can be written at any point of time but it is loaded to the counter at the time
	  when WDOG-1 is enabled or after the service routine has been performed. For more information
	  see Time-Out Event.
	   0x00 - 0.5 Seconds (Default).
	   0x01 - 1.0 Seconds.
	   0x02 - 1.5 Seconds.
	   0x03 - 2.0 Seconds.
	   0xff - 128 Seconds.*/

	#define WT_TIME_OUT_FIELD_OFFSET				8	

	 /*Watchdog Disable for Wait. This bit determines the operation of WDOG-1 during Low Power WAIT mode.*/
	 /*This is a write once only bit.*/
	#define WDW_AT_WAIT_CONTINUE					(0b0<<7)	/*Continue WDOG-1 timer operation (Default).*/
	#define WDW_AT_WAIT_SUSPEND						(0b1<<7)	/*Suspend WDOG-1 timer operation.*/
														
	
	
	 /*WDOG assertion. Controls the software assertion of the WDOG signal.*/
	#define WDA_ASSERT_WDOG_SIGNAL					(0b0<<5)	/*Assert WDOG output.*/
	#define WDA_NO_ASSERT_WDOG_SIGNAL				(0b1<<5)	/*No effect on system (Default)*/
															
	 /*Software Reset Signal. Controls the software assertion of the WDOG-generated reset signal wdog_rst .*/ 
	 /*This bit automatically resets to "1" after it has been asserted to "0".*/
	 /*NOTE: This bit does not generate the software reset to the block.*/
	#define SRS_ASSERT_WDOG_RESET_SIGNAL			(0b0<<4)	/*Assert system reset signa*/
	#define SRS_NO_ASSERT_WDOG_RESET_SIGNAL			(0b1<<4)	/*No effect on the system (Default)*/
															
		
	 /*WDOG Time-out assertion. Determines if the WDOG gets asserted upon a Watchdog Time-out Event.*/
	 /*This is a write-one once only bit.*/
	 /*NOTE: There is no effect on wdog_rst (WDOG Reset) upon writing on this bit. WDOG gets asserted*/
	 /*along with wdog_rst if this bit is set.*/
	#define WDT_TIMEOUT_ASSERTION_DISABLED			(0b0<<3)	/*No effect on WDOG (Default)*/
	#define WDT_TIMEOUT_ASSERTION_ENABLED			(0b1<<3)	/*Assert WDOG upon a Watchdog Time-out event*/
														
	
	
	 /*Watchdog Enable. Enables or disables the WDOG-1 block. This is a write one once only bit. It is not*/
	 /*possible to clear this bit by a software write, once the bit is set*/
	 /*NOTE: This bit can be set/reset in debug mode (exception).*/												
	#define WDE_WATCHDOG_DISABLED					(0b0<<2)	/*Disable the Watchdog (Default)*/
	#define WDE_WATCHDOG_ENABLED					(0b1<<2)	/*Enable the Watchdog.*/
													
	
	
	 /*Determines the operation of the WDOG-1 during DEBUG mode. This bit is write once-only*/											
	#define WDBG_DEBUG_DISABLED						(0b0<<1)	/*Continue WDOG-1 timer operation (Default)*/
	#define WDBG_DEBUG_ENABLED						(0b1<<1)	/*Suspend the watchdog timer*/
												
												
												
	 /*Watchdog Low Power. Determines the operation of the WDOG-1 during low-power modes.*/
	 /*This bit is write once-only*/												
	#define WDZST_LOW_POWER_DISABLED				(0b0<<0)	/*Continue timer operation (Default)*/
	#define WDZST_LOW_POWER_ENABLED					(0b1<<0)	/*Suspend the watchdog timer*/
													

	//Functions
	
#endif /* WDOG_H_ */
