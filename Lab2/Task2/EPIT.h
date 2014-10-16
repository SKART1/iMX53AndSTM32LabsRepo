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
	#define EPIT_CONFIG_REG_BASE_ADRESS			0x53FAC000			//Adress of the first register for 
	#define EPIT_CONFIG_REG_TOTAL_LENGTH 		20					//Number of registers in row
	#define	CONTROL_REGISTER_OFFSET_BYTES		0
	#define	STATUS_REGISTER_OFFSET_BYTES		4
	#define LOAD_REGISTER_OFFSET_BYTES			8
	#define	COMPARE_REGISTER_OFFSET_BYTES		12

	//IRQ
	#define EPIT_IRQ_NUMBER 			40 					//Number of IRQ for EPIT

	//EPITx_EPITCR field possible values (see page 1142 of manual)
	#define SET_CLKSRC_LOW_FREQUENCY_CLOCK(WHOM)		/*Clock source 32 kHZ*/	\
		WHOM|=(0b11<<24);
	
	#define SET_OM_EPIT_DISCONNECTED(WHOM) 				/*EPIT is not connected at any source*/\
		WHOM&=~((~0b00)<<22);		

	#define SET_STOPEN_DISABLED(WHOM) 					/*EPIT does not work in stop mode*/\
		WHOM&=~((~0b0)<<21);
	#define SET_STOPEN_ENABLED(WHOM) 					/*EPIT works even in stop mode*/\
		WHOM|=(0b1<<21);	
	
	#define SET_WAITEN_DISABLED(WHOM)					/*EPIT is disabled in wait mode*/\
		WHOM&=~((~0b0)<<19);
	#define SET_WAITEN_ENABLED(WHOM)					/*EPIT is enabled in wait mode*/\
		WHOM|=(0b1<<19);

	#define SET_DBGEN_DISABLED(WHOM)					/*EPIT is disabled in debug mode*/\
		WHOM&=~((~0b0)<<18);
	#define SET_DBGEN_ENABLED(WHOM)						/*EPIT is enabled in debug mode*/\
		WHOM|=(0b1<<18);

	#define SET_IOVW_NO_COUNTER_OVERRITEN(WHOM)			/*If we change LOAD register - timer counter value is changed*/\
		WHOM&=~((~0b0)<<17);		
	#define SET_IOVW_COUNTER_OVERRITEN(WHOM)			/*If we change LOAD register - timer counter value is not changed*/\
		WHOM|=(0b1<<17);

	#define SET_SWR_RESET_TIMER(WHOM)					/*Bit for software reset of timer*/\
		WHOM|=(0b1<<16);
	
	#define SET_PRESCALER_BASE_ADDRESS(WHOM, WHAT)		/*Start of PRESCALER part in EPITx_EPITCR field (PRESCALER length is 3 bytes*/\
		WHOM&=~((~0b0000)<<4);\															
		WHOM|=(WHAT<<4);
					

	#define SET_RLD_FREE_RUNNING_MODE(WHOM)			/*When timer reaches zero it rolls over to 0xFFF_FFFF*/\
		WHOM&=~((~0b0)<<3);
	#define SET_RLD_LOAD_FROM_MODULUS(WHOM)			/*When timer reach zero it reloads from modulus register*/\
		WHOM|=(0b1<<3);
	
	#define SET_OCIEN_COMP_INTER_DISABL(WHOM)			/*switch off interrupt when value in timer equal to value in compare register*/\
		WHOM&=~((~0b0)<<2);
	#define SET_OCIEN_COMP_INTER_ENAB(WHOM)			/*switch on interrupt when value in timer equal to value in compare register*/\
		WHOM|=(0b1<<2);
	
	#define SET_ENMOD_COUNTER_WILL_CONTINUE(WHOM)		/*Counter will continue to count from value it had when it was stopped*/\
		WHOM&=~((~0b0)<<1);
	#define SET_ENMOD_COUNTER_WILL_RESTART(WHOM)		/*Counter will continue to count from zeroes (or MODULUS) when it will start again*/\
		WHOM|=(0b1<<1);

	#define SET_EN_TIMER_DISABLED(WHOM)				/*Counter is disabled*/\
		WHOM&=~((~0b0)<<0);
	#define SET_EN_TIMER_ENABLED(WHOM)				/*Counter is enabled*/\
		WHOM|=(0b1<<0);


#endif /* EPIT_H_ */
