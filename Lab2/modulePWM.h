/*
 * 	Description of interface and constants
 * 	necessary for correct work of module PWM
 *
 *  	Created on: Sep 28, 2014
 *      Author: Arthur Enikoff
 */

#ifndef MODULEPWM_H_
#define MODULEPWM_H_

	/*********************************************************************************/
	/*				Obligatory constants for module building 						 */
	/*				          licence and author									 */	
	/*********************************************************************************/
	MODULE_LICENSE ("GPL");
	MODULE_DESCRIPTION ("Simple linux kernel module example.");
	MODULE_AUTHOR ("Arthur Enikov and Yura Filatov");
	/*-------------------------------------------------------------------------------*/


	#define 	MODULE_NAME 			"modulePWM"		//Module name for /var/log/syslog
	#define 	GPIO_USRLED 			199				//Port number of USRLED on iMX53 starter board
	#define 	GPIO_USRBTN1 			46				//User button 1 on iMX53 starter board
	#define 	GPIO_USRBTN2 			47				//User button 2 on iMX53 starter board

	#define 	REQUIRED_FREQUENCY_HZ	2				//Required frequency by task
	#define 	START_DUTY_CYCLE		0.5				//Duty cycle at start

	
	typedef struct ProgrammContext{
		int usrBtn1IRQNumber;
		int usrBtn2IRQNumber;
		volatile int usrled_val;

		void * EPITRegistersMapBegin;
		volatile unsigned long int modulusRegisterValueLEDHigh;
		volatile unsigned long int modulusRegisterValueLEDLow;
		unsigned long int modulusRegisterValueLEDStep;
	} ProgrammContext;


	static int __init init_routine(void);
	static void __exit exit_routine(void);
	int initEPITForPWM();



#endif /* MODULEPWM_H_ */
