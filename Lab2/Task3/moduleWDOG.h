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


	#define 	MODULE_NAME 			"moduleWDOG"		//Module name for /var/log/syslog
	#define 	GPIO_USRLED 			199				//Port number of USRLED on iMX53 starter board
	#define 	GPIO_USRBTN1 			46				//User button 1 on iMX53 starter board
	#define 	GPIO_USRBTN2 			47				//User button 2 on iMX53 starter board

	
	

	static int __init init_routine(void);
	static void __exit exit_routine(void);
	

#endif /* MODULEWDOG_H_ */
