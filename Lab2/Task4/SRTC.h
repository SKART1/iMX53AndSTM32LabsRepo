/*
 * 	Description of interface and constants
 * 	necessary for correct work of module SRTC
 *
 *  	Created on: Sep 28, 2014
 *      Author: Arthur Enikoff
 */

#ifndef SRTC_H_
#define SRTC_H_


	#define SRTC_REGISTERS_BASE_ADDRESS	0x53FA_4000					
	
	#define SRTC_REGISTERS_TOTAL_LENGTH 64 	// in bytes  (see 4423)
	#define SRTC_LP_CONTROL_REGISTER_OFFSET_BYTES 16//  
	
	
	
	/* 23 - 31 bit reserved */
	
	/*18-23 bits */
	/*SRTC Calibration Value
	These five bits indicates signed calibration value for SRTC LP timer.
	In this, SCAL_LP[21:18] represent 4 bit value and SCAL_LP[22] represents the sign.
	Hence, SCAL_LP[22:18] allow calibration +15 to -16 in SRTC LP timer*/
	#define SRTC_CALIBRATION_VALUE_FIELD_OFFSET				5	
	
	
	/*17-16 bits */
	/* SRTCCALMODEL_LP*/
	/*Decides the SRTC LP calibration mode..*/
	#define SCALM_TIMER_CALIBRATION_DISABLED					(0b00<<16)	/*Timer calibration disabled.*/
	#define SCALM_TIMER_CALIBRATION_ENABLED						(0b01<<16)	/*Timer calibration enabled in all modes except coin cell mode*/
	#define SCALM_TIMER_CALIBRATION_ALL_MODES_ENABLED			(0b11<<16)	/*Timer calibration enabled in all modes*/	
	
	
	/* Initialization state exit bit*/
	/*When set, this bit ends Initialization state of the SRTC state machine.*/
	/*NOTE: IE bit is a control bit to end the Initialization state. For the current state of SRTC LP see the
	state_lp field in the SRTC_LPSR.*/
	#define IE_SRTC_IN_INITIALIZATION_STATE						(0b0<<15)	/*SRTC is in Initialization state*/
	#define IE_SRTC_END_INITIALIZATION_STATE					(0b1<<15)	/*End of Initialization state*/
		
	/*Nonvalid state exit bit */
	/*When set this bit ends the Nonvalid state of the SRTC state machine*/
	/*NOTE: NVE bit is a control bit to end the Non Valid state. For the current state of SRTC LP see the
		state_lp field in the SRTC_LPSR*/
	#define NVE_SRTC_IN_NONVALID_STATE							(0b0<<14)	/*SRTC is in Nonvalid state.*/
	#define NVE_SRTC_END_NONVALID_STATE							(0b1<<14)	/*End of Nonvalid state*/
	
	/*Initialization state exit interrupt enable bit */
	/*This bit enables the interrupt on exit from the Initialization State of the SRTC state machine.*/
	#define IEIE_INITIALIZATION_STATE_EXIT_INTERRUPT_DISABLED	(0b0<<13)	/*An interrupt is disabled.*/
	#define IEIE_INITIALIZATION_STATE_EXIT_INTERRUPT_ENABLED	(0b1<<13)	/*An interrupt is enabled.*/
	
	
		
	/*Nonvalid state exit interrupt enable bit */
	/*This bit enables the interrupt on exit from the Nonvalid state of the SRTC state machine.*/
	#define NVEIE_NONVALID_STATE_EXIT_INTERRUPT_DISABLED		(0b0<<12)	/*An interrupt is disabled.*/
	#define NVEIE_NONVALID_STATE_EXIT_INTERRUPT_ENABLED			(0b1<<12)	/*An interrupt is enabled.*/
		

	/*Nonsecure access*/
	/*When set this bit allows nonsecure application to access secure register. This bit is reset by the system por.*/
	#define NSA_NONSECURE_ACCESS_NOT_ALLOWED						(0b0<<11)	/*Nonsecure access of secure register is not allowed*/
	#define NSA_NONSECURE_ACCESS_ALLOWED						(0b1<<11)	/*Nonsecure access is allowed*/
	
	
	
	/*Security Violation */
	/*When set this bit move the SRTC LP to failure state upon termination of Initialization state. Also if the
	current state is Nonvalid/Valid the SRTC LP is moved to failure state. This bit is reset by the system por.*/
	/*NOTE: Setting the SV invalidates the SRTC_LPSCR and SRTC_LPSMCR contents.*/
	#define SV_SECURITY_VIOLATION_DISABLED			(0b0<<10)	/*No security violation*/
	#define SV_SECURITY_VIOLATION_ENABLED			(0b1<<10)	/*Security Violation*/
	
	
	
	
	 /*Lock monotonic counter*/
	/*When set this bit prevents write access to 32-bit secure LP monotonic counter register (SRTC_LPSMCR)
	by all application.Once set this bit cannot be reset except by system por.*/
	#define LMC_WRITE_ACCESS_ALLOWED				(0b0<<9)	/*Write access is allowed*/
	#define LMC_WRITE_ACCESS_NOT_ALLOWED				(0b1<<9)	/*Write access is not allowed*/
	
	
	
	
	 /*Lock time counter*/
	 /*When set this bit prevents write access to secure 47-bit LP counter (SRTC_LPSCMR and
	SRTC_LPSCLR) by all application.Once set this bit cannot be reset except by system por.*/
	#define LTC_WRITE_ACCESS_ALLOWED				(0b0<<8)	/*Write access is allowed*/
	#define LTC_WRITE_ACCESS_NOT_ALLOWED			(0b1<<8)	/*Write access is not allowed*/
														
	
	
	/*Alarm Flag of LP Section
	Enables the alarm interrupt to the core. See SRTC LP Secured Counter Alarm, for more details.*/
	#define ALP_ALARM_INTERRUPT_DISABLED			(0b0<<7)	/*Alarm interrupt disabled*/
	#define ALP_ALARM_INTERRUPT_ENABLED				(0b1<<7)	/*Alarm interrupt enabled*/
															
	 /*Security Interrupt Enable bit
	This bit enables the security interrupt to be raised to core on SRTC security Interrupt (Active low).
	Security interrupt is raised upon setting of the of time read invalidate status bit*/
	#define SI_SECURITY_INTERRUPT_DISABLED			(0b0<<6)	/*Security interrupt disabled*/
	#define SI_SECURITY_INTERRUPT_ENABLED			(0b1<<6)	/*Security interrupt enabled*/
															
		
	 /*Security Alarm Enable bit
	This bit enables the security alarm to be raised to security block on SRTC security alarm (Active high) pin.
	Security alarm is raised upon setting of the of time read invalidate status bit*/
	#define SAE_SECURITY_ALARM_DISABLED				(0b0<<5)	/*Security alarm disabled.*/
	#define SAE_SECURITY_ALARM_ENABLED				(0b1<<5)	/*Security alarm enabled*/
														
	
	 /*Wakeup Alarm Enable bit
	This bit enables the time alarm to be raised on DO_ALARM_OUT pin. See SRTC LP Secured Counter
	Alarm, for more details.DO_ALARM_OUT is asserted only if the main system is powered off as indicated
	by power fail circuity or external signal (dsm_on) input to block. DO_ALARM_OUT's polarity is active low*/											
	#define WAE_ALARM_DISABLED						(0b0<<4)	/*Time alarm disabled*/
	#define WAE_ALARM_ENABLED						(0b1<<4)	/*Time alarm enabled*/
												
												
												
	/*Enables/Disables the LP secure time and secure monotonic counter
	When set the secure LP counter and monotonic counter becomes operational. This bit cannot be reset
	once either of LMC or LTC is set. In high and Medium this bit can only be set in Valid state*/												
	#define EN_LP_RTC_DISABLED						(0b0<<3)	/*Disable the real-time clock and monotonic counter*/
	#define EN_LP_RTC_ENABLED						(0b1<<3)	/*Enable the real-time clock and monotonic counter*/
													

					
	//Functions
	
#endif /* SRTC_H_ */
