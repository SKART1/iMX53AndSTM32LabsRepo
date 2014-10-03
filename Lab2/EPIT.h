/*
 * 	Description of interface and constants
 * 	necessary for correct work of module PWM
 *
 *  	Created on: Sep 28, 2014
 *      Author: Arthur Enikoff
 */

#ifndef EPIT_H_
#define EPIT_H_
	//EPIT register description
	#define BASE_ADRESS 						0x53FAC000			//Adress of the first register for 
	#define REGISTERS_TOTAL_LENGTH 				20					//Number of registers in row
	#define	CONTROL_REGISTER_OFFSET_BYTES		0
	#define	STATUS_REGISTER_OFFSET_BYTES		4
	#define LOAD_REGISTER_OFFSET_BYTES			8
	#define	COMPARE_REGISTER_OFFSET_BYTES		12

	//IRQ
	#define EPIT_IRQ_NUMBER 			40 					//Number of IRQ for EPIT

	//EPITx_EPITCR field possible values (see page 1142 of manual)
	#define CLKSRC_LOW_FREQUENCY_CLOCK 	(0b11<<24) 	/*Clock source 32 kHZ*/

	#define OM_EPIT_DISCONNECTED 		(0b00<<22)	/*EPIT is not connected at any source*/
	

	#define STOPEN_DISABLED 			(0b0<<21)	/*EPIT does not work in stop mode*/	
	#define STOPEN_ENABLED 				(0b1<<21)	/*EPIT works even in stop mode*/

	#define WAITEN_DISABLED				(0b0<<19)	/*EPIT is disabled in wait mode*/
	#define WAITEN_ENABLED				(0b1<<19)	/*EPIT is enabled in wait mode*/

	#define DBGEN_DISABLED				(0b0<<18)	/*EPIT is disabled in debug mode*/
	#define DBGEN_ENABLED				(0b1<<18)	/*EPIT is enabled in debug mode*/

	#define IOVW_NO_COUNTER_OVERRITEN	(0b0<<17)	/*If we change LOAD register - timer counter value is changed*/
	#define IOVW_COUNTER_OVERRITEN		(0b1<<17)	/*If we change LOAD register - timer counter value is not changed*/

	#define SWR_RESET_TIMER				(0b1<<16)	/*Bit for software reset of timer*/

	#define PRESCALER_BASE_ADDRESS		4			/*Start of PRESCALER part in EPITx_EPITCR field (PRESCALER length is 3 bytes*/		

	#define RLD_FREE_RUNNING_MODE		(0b0<<3)	/*When timer reaches zero it rolls over to 0xFFF_FFFF*/
	#define RLD_LOAD_FROM_MODULUS		(0b1<<3)	/*When timer reach zero it reloads from modulus register*/
	
	#define OCIEN_COMP_INTER_DISABL		(0b0<<2)	/*switch off interrupt when value in timer equal to value in compare register*/
	#define OCIEN_COMP_INTER_ENAB		(0b1<<2)	/*switch on interrupt when value in timer equal to value in compare register*/
	
	#define ENMOD_COUNTER_WILL_CONTINUE	(0b0<<1)	/*Counter will continue to count from value it had when it was stopped*/
	#define ENMOD_COUNTER_WILL_RESTART	(0b1<<1)	/*Counter will continue to count from zeroes (or MODULUS) when it will start again*/

	#define EN_TIMER_DISABLED			(0b0<<0)	/*Counter is disabled*/
	#define EN_TIMER_ENABLED			(0b1<<0)	/*Counter is enabled*/

	//Functions
	#define SWITCH_ON_TIMER
	#define SWITCH_OFF_TIMER
#endif /* EPIT_H_ */
