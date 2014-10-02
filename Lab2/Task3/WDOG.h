/*
 * 	Description of interface and constants
 * 	necessary for correct work of module PWM
 *
 *  	Created on: Sep 28, 2014
 *      Author: Arthur Enikoff
 */

#ifndef WDOG_H_
#define WDOG_H_


	#define BASE_ADDRESS	0x53F98000					
	
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

	
	#define WAIT_DISABLE					(0b0<<7)	/*Continue WDOG-1 timer operation (Default).*/
	#define WAIT_ENABLE						(0b1<<7)	/*Suspend WDOG-1 timer operation.*/
														/*Watchdog Disable for Wait. This bit determines the operation of WDOG-1 during Low Power WAIT mode.*/
														/*This is a write once only bit.*/
	
	/* RESERVED 6 bit */
	
	#define WDW_ASSERT_WDOG_SIGNAL					(0b0<<5)	/*Assert WDOG output.*/
	#define NO_ASSERT_WDOG_SIGNAL				(0b1<<5)	/*No effect on system (Default)*/
															/*WDOG assertion. Controls the software assertion of the WDOG signal.*/
	
	#define ASSERT_WDOG_RESET_SIGNAL			(0b0<<4)	/*Assert system reset signa*/
	#define NO_ASSERT_WDOG_RESET_SIGNAL			(0b1<<4)	/*No effect on the system (Default)*/
															/*Software Reset Signal. Controls the software assertion of the WDOG-generated reset signal wdog_rst .*/ 
															/*This bit automatically resets to "1" after it has been asserted to "0".*/
															/*NOTE: This bit does not generate the software reset to the block.*/
															
	#define TIMEOUT_ASSERTION_DISABLED			(0b0<<3)	/*No effect on WDOG (Default)*/
	#define TIMEOUT_ASSERTION_ENABLED			(0b1<<3)	/*Assert WDOG upon a Watchdog Time-out event*/
															/*WDOG Time-out assertion. Determines if the WDOG gets asserted upon a Watchdog Time-out Event.*/
															/*This is a write-one once only bit.*/
															/*NOTE: There is no effect on wdog_rst (WDOG Reset) upon writing on this bit. WDOG gets asserted*/
															/*along with wdog_rst if this bit is set.*/
	
	
													
	#define WATCHDOG_DISABLED			(0b0<<2)	/*Disable the Watchdog (Default)*/
	#define WATCHDOG_ENABLED			(0b1<<2)	/*Enable the Watchdog.*/
													/*Watchdog Enable. Enables or disables the WDOG-1 block. This is a write one once only bit. It is not*/
													/*possible to clear this bit by a software write, once the bit is set*/
													/*NOTE: This bit can be set/reset in debug mode (exception).*/
	
	
												
	#define DEBUG_DISABLED			(0b0<<1)	/*Continue WDOG-1 timer operation (Default)*/
	#define DEBUG_ENABLED			(0b1<<1)	/*Suspend the watchdog timer*/
												/*Determines the operation of the WDOG-1 during DEBUG mode. This bit is write once-only*/
												
												
													
	#define LOW_POWER_DISABLED			(0b0<<0)	/*Continue timer operation (Default)*/
	#define LOW_POWER_ENABLED			(0b1<<0)	/*Suspend the watchdog timer*/
													/*Watchdog Low Power. Determines the operation of the WDOG-1 during low-power modes.*/
													/*This bit is write once-only*/

	//Functions
	
#endif /* WDOG_H_ */
