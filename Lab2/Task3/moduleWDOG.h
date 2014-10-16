/*
 * 	Description of interface and constants
 * 	necessary for correct work of module PWM
 *
 *  	Created on: Sep 28, 2014
 *      Author: Arthur Enikoff
 */

#ifndef MODULEWDOG_H_
#define MODULEWDOG_H_



	/*********************************************************************************/
	/*				Obligatory constants for module building 						 */
	/*				          licence and author									 */	
	/*********************************************************************************/
	MODULE_LICENSE ("GPL");
	MODULE_DESCRIPTION ("Simple linux kernel module example.");
	MODULE_AUTHOR ("Student");
	/*-------------------------------------------------------------------------------*/


	#define 	MODULE_NAME 						"moduleWDOG"		//Module name for /var/log/syslog
	#define 	GPIO_USRLED 						199					//Port number of USRLED on iMX53 starter board

	#define 	WATCHDOG_TIMEOUT					120					//X*0.5	seconds timeout
	#define 	WATCHDOG_INTERRUPT_BEFORE_FIRE		60					//X*0.5 seconds before timeout



	typedef struct ProgrammContext{
		volatile int usrled_val;	
		void *WATCHDOGRegistersMapBegin;
		int WATCHDOGTimesToFire;
	} ProgrammContext;

	

	static int __init init_routine(void);
	static void __exit exit_routine(void);
	

#endif /* MODULEWDOG_H_ */
