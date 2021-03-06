/*********************************************************************************/
/*									Headers										 */	
/*********************************************************************************/
//System headers
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/ioport.h>
#include <asm/io.h>


//User headers
#include "modulePWM.h"	//Header with prototypes and constants
#include "EPIT.h"  		//Header with EPIT constants
/*===============================================================================*/




/*********************************************************************************/
/*									Macro										 */	
/*********************************************************************************/

/*===============================================================================*/




/*********************************************************************************/
/*							Internal module constants							 */	
/*********************************************************************************/
#define LOW_FREQUENCY_CLOCK_SOURCE_HZ	32000
#define PRESCALER						0b001	
/*===============================================================================*/




/*********************************************************************************/
/*							Global variables									 */	
/*********************************************************************************/
ProgrammContext programmContext={0,0,0,NULL,0,0};
/*===============================================================================*/


/*********************************************************************************/
/*									Interrupts									 */	
/*********************************************************************************/
 /*
  *Increment current duty cycle (increment length of HIGH value of LED, decrement length of LOW value of LED state
  */
static irqreturn_t usrBtn1IRQHandler(int irq, void *dev_id){
	if(programmContext.modulusRegisterValueLEDHigh<((LOW_FREQUENCY_CLOCK_SOURCE_HZ/PRESCALER)/REQUIRED_FREQUENCY_HZ) && programmContext.modulusRegisterValueLEDLow>programmContext.modulusRegisterValueLEDStep){
		programmContext.modulusRegisterValueLEDHigh+=programmContext.modulusRegisterValueLEDStep;
		programmContext.modulusRegisterValueLEDLow-=programmContext.modulusRegisterValueLEDStep;
	}	
    return( IRQ_HANDLED );
}


 /*
  *Decrement current duty cycle (decrement length of HIGH value of LED,increment length of LOW value of LED state
  */
static irqreturn_t usrBtn2IRQHandler(int irq,void *dev_id){
	if(programmContext.modulusRegisterValueLEDLow<((LOW_FREQUENCY_CLOCK_SOURCE_HZ/PRESCALER)/REQUIRED_FREQUENCY_HZ) && programmContext.modulusRegisterValueLEDHigh>programmContext.modulusRegisterValueLEDStep){
		programmContext.modulusRegisterValueLEDHigh-=programmContext.modulusRegisterValueLEDStep;
		programmContext.modulusRegisterValueLEDLow+=programmContext.modulusRegisterValueLEDStep;
	}
    return( IRQ_HANDLED );
}


 /*
  *Invert LED value. Load new value to LOAD register of EPIT depending on value of LED
  *Clean interrupt bit in EPIT status register
  */
static irqreturn_t irqEPITHandler(int irq, void *dev_id){
	programmContext.usrled_val = 1 & ~ programmContext.usrled_val;
    gpio_set_value(GPIO_USRLED, programmContext.usrled_val);
	//New start value of timer
	if(programmContext.usrled_val==0){
		iowrite32(programmContext.modulusRegisterValueLEDLow, ((char *)programmContext.EPITRegistersMapBegin)+LOAD_REGISTER_OFFSET_BYTES);
	}
	else{
		iowrite32(programmContext.modulusRegisterValueLEDHigh, ((char *)programmContext.EPITRegistersMapBegin)+LOAD_REGISTER_OFFSET_BYTES);
	}
	//Cleaning interupt bit in EPIT status register
	iowrite32(ioread32(((char *)programmContext.EPITRegistersMapBegin)+STATUS_REGISTER_OFFSET_BYTES)|0x00000001,((char *)programmContext.EPITRegistersMapBegin)+STATUS_REGISTER_OFFSET_BYTES);
	return( IRQ_HANDLED );
}
/*===============================================================================*/





/*********************************************************************************/
/*									Functions									 */	
/*********************************************************************************/
 /*
  * Function to init EPIT timer for half programm half hardware PWM.
  * The idea of algoryth is to separate period of the timer for two parts
  *	depending on filling factor. In the first one the output LED will be high
  *	and in the second will be low. So this values will be loaded in LOAD register in
  * rotation. The c 
  *  	
  */
int initEPITForPWMAndStart(){
	/*Declaring variables*/
	//Control register for EPIT N1
	unsigned long int EPIT1_EPITCR=0;
	void *clockControllerMemoryMapping; 	
	/*-------------------------------------------------*/


	/*Initializing part*/
	//Values for modulus register - during which LED will be switch on or off
	if((LOW_FREQUENCY_CLOCK_SOURCE_HZ/PRESCALER)/REQUIRED_FREQUENCY_HZ>100){
		programmContext.modulusRegisterValueLEDHigh=((LOW_FREQUENCY_CLOCK_SOURCE_HZ/PRESCALER)/REQUIRED_FREQUENCY_HZ)*START_DUTY_CYCLE;
		programmContext.modulusRegisterValueLEDLow=((LOW_FREQUENCY_CLOCK_SOURCE_HZ/PRESCALER)/REQUIRED_FREQUENCY_HZ)*(1-START_DUTY_CYCLE);
		programmContext.modulusRegisterValueLEDStep=((LOW_FREQUENCY_CLOCK_SOURCE_HZ/PRESCALER)/REQUIRED_FREQUENCY_HZ)/20;
	}
	else{	
		printk("[ERROR]: LOW_FREQUENCY_CLOCK_SOURCE_HZ, PRESCALER, REQUIRED_FREQUENCY_HZ values gives result too LOW for correct PWM work \n");
		goto ERROR_STEP0;
	};

	//EPIT control register
	//Clock source is 32 kHZ oscillator	
	SET_CLKSRC_LOW_FREQUENCY_CLOCK(EPIT1_EPITCR);
	//No actions are neccessary on output pins(?!)
	SET_OM_EPIT_DISCONNECTED(EPIT1_EPITCR);
	//Timer will work even in STOP state of system
	SET_STOPEN_ENABLED(EPIT1_EPITCR);
	//Timer will work even in WAIT state of system
	SET_WAITEN_ENABLED(EPIT1_EPITCR);
	//Timer will work even in DEBUG state of system
	SET_DBGEN_ENABLED(EPIT1_EPITCR);
	//Timer counter register will be immideatelly set to value in MODULUS register if it is changed
	SET_IOVW_COUNTER_OVERRITEN(EPIT1_EPITCR);
	//Prescaller equals to 1 - means now prescale
	SET_PRESCALER(EPIT1_EPITCR,PRESCALER);
	//Timer will reload it`s value from modulus register - really do not used. We will overite MODULUS value manually 
	//when timer will reach zero and because of "SET_BITS(EPIT1_EPITCR,IOVW_COUNTER_OVERRITEN);" above 
	SET_RLD_LOAD_FROM_MODULUS(EPIT1_EPITCR);
	//Switch on interrupts caused by equality of timer value and compare registers
	SET_OCIEN_COMP_INTER_ENAB(EPIT1_EPITCR);
	//Counter will continue from previous values if it will be paused
	SET_ENMOD_COUNTER_WILL_CONTINUE(EPIT1_EPITCR);
	//Timer is immediatelly switch on
	SET_EN_TIMER_ENABLED(EPIT1_EPITCR);
	/*-------------------------------------------------*/


	/*Write settings to device*/	
	//EPIT settings
	if(request_mem_region(EPIT_CONFIG_REG_BASE_ADRESS,EPIT_CONFIG_REG_TOTAL_LENGTH, "EPIT")==NULL){
		printk("[ERROR]: request_mem_region error\n");
		goto ERROR_STEP1;
	}	
	//map physical memory to virtual memory of module
	programmContext.EPITRegistersMapBegin=ioremap(EPIT_CONFIG_REG_BASE_ADRESS,  EPIT_CONFIG_REG_TOTAL_LENGTH);	

	//Write EPIT config register
	iowrite32(EPIT1_EPITCR, ((char *)programmContext.EPITRegistersMapBegin)+CONTROL_REGISTER_OFFSET_BYTES);

	//Compare register is set to zero: because of algorythm
	iowrite32(0L, ((char *)programmContext.EPITRegistersMapBegin)+COMPARE_REGISTER_OFFSET_BYTES);

	//Start value - let it be LED high length (because LED is also started at that moment)
	iowrite32(programmContext.modulusRegisterValueLEDHigh, ((char *)programmContext.EPITRegistersMapBegin)+LOAD_REGISTER_OFFSET_BYTES);
	/*-------------------------------------------------*/


	/* IRQ */	
	//request irq
	if(request_irq(EPIT_IRQ_NUMBER,(*irqEPITHandler),IRQF_TRIGGER_FALLING, MODULE_NAME,NULL)!=0){
		printk(KERN_INFO "[ERROR]: %s: EPIT 2 request_irq\n", MODULE_NAME);
		goto ERROR_STEP2;
	};	
	//Enable IRQ
	//TODO check if it is needed to disable IRQ...
	//enable_irq(EPIT_IRQ_NUMBER);



	/*Switch on CLOCKING*/
	//map memory
	if(request_mem_region(0x53FD4070,4, "CLOCK")==NULL){
		printk("[ERROR]: request_mem_region clock error\n");
		goto ERROR_STEP3;
	}	
	clockControllerMemoryMapping=ioremap(0x53FD4070, 4);
	//Clock settings
	iowrite32(ioread32(clockControllerMemoryMapping)|0x0000000C,clockControllerMemoryMapping);
	//unmap memory
	iounmap(clockControllerMemoryMapping);
	release_mem_region(0x53FD4070, 4);
	return 0;


	
ERROR_STEP3:
	//disable_irq(EPIT_IRQ_NUMBER);	
	free_irq(EPIT_IRQ_NUMBER,MODULE_NAME); 
ERROR_STEP2:
	iounmap(programmContext.EPITRegistersMapBegin);
	release_mem_region(EPIT_CONFIG_REG_BASE_ADRESS,  EPIT_CONFIG_REG_TOTAL_LENGTH);
ERROR_STEP1:
ERROR_STEP0:
	return -1;
}
 /*-------------------------------------------------------------------------------*/

 /*
  *Uninitialize EPIT: 
  *1). Stop timer 
  *2). Disable IRQ if necessary 
  *3). Release(free) IRQ  
  *4). Unmap memory region
  *5). Release memory region
  */
void deInitEPIT(){
	//TODO: Stop timer
	//TODO check if it is needed to disable IRQ...

	//Timer is immediatelly switch on
	unsigned long int EPIT1_EPITCR=0;

	SET_EN_TIMER_DISABLED(EPIT1_EPITCR);
	//Write EPIT config register
	iowrite32(EPIT1_EPITCR, ((char *)programmContext.EPITRegistersMapBegin)+CONTROL_REGISTER_OFFSET_BYTES);
	/*-------------------------------------------------*/



	//disable_irq(EPIT_IRQ_NUMBER);
	free_irq(EPIT_IRQ_NUMBER,NULL); 
	iounmap(programmContext.EPITRegistersMapBegin);
	release_mem_region(EPIT_CONFIG_REG_BASE_ADRESS,  EPIT_CONFIG_REG_TOTAL_LENGTH);
}
 /*-------------------------------------------------------------------------------*/


 /*
  * Initialize LEDs - first checks if the ports are correct, after that setting pin (port) as output
  */
int initLED(unsigned int portLEDNumber){
	//Checking if GPIO is valid on this type of architecture (>0, <MAX) and so on - rather primitive function
	//See more in http://lwn.net/Articles/532717/
	//and http://lwn.net/Articles/532714/
	if(gpio_is_valid(portLEDNumber)==0){
		printk(KERN_INFO "[ERROR]: %s: initLED gpio_is_valid\n", MODULE_NAME);
		goto ERROR_STEP0;
	}


	//Allocate the GPIO
	/*GPIOs must be allocated before use, though the current implementation does not enforce this requirement. The basic allocation function is:
	    int gpio_request(unsigned int gpio, const char *label);
   	  The gpio parameter indicates which GPIO is required, while label associates a string with it that can later appear in sysfs. 
	  The usual convention applies: a zero return code indicates success; otherwise the return value will be a negative error number. 
	  A GPIO can be returned to the system with:
    	void gpio_free(unsigned int gpio);
	*/
	if(gpio_request(portLEDNumber, MODULE_NAME)!=0){
		printk(KERN_INFO "[ERROR]: %s: initLED gpio_request\n", MODULE_NAME);
		goto ERROR_STEP1;
	}

	
	//Set the direction of GPIO and also initial value
	/*For compatibility with legacy interfaces to GPIOs, setting the direction
	  of a GPIO implicitly requests that GPIO (see below) if it has not been
	  requested already.  That compatibility is being removed from the optional
	  gpiolib framework. (Documentation/gpio.txt)
	*/
	if(gpio_direction_output(portLEDNumber,programmContext.usrled_val)!=0){
		printk(KERN_INFO "[ERROR]: %s: initLED gpio_direction_output\n", MODULE_NAME);
		goto ERROR_STEP2;
	}
	return 0;


ERROR_STEP2:
ERROR_STEP1:
	//We have requested gpio - so we should free it even if it were error
	gpio_free(portLEDNumber);
ERROR_STEP0:
	return -1;
};
 /*-------------------------------------------------------------------------------*/


 /*
  *	We need just to say system we no longer need this GPIO port
  */
void deInitLed(unsigned long int portLEDNumber){
	gpio_free(portLEDNumber);
}
/*-------------------------------------------------------------------------------*/



 /*
  * Initialize USERBUTTONS - first checks if the ports are correct, after that setting pins (ports) as inputs.
  *	and add interrupt handlers for event PRESSING button
  */
int initButtons(unsigned long int  portButton1Number, unsigned long int  portButton2Number){
	//Checking if GPIO is valid on this type of architecture (>0, <MAX) and so on - rather primitive function
	//See more in http://lwn.net/Articles/532717/
	//and http://lwn.net/Articles/532714/
	if(gpio_is_valid(portButton1Number)==0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 1 gpio_is_valid\n", MODULE_NAME);
		goto ERROR_STEP0_1;;
	};	
	if(gpio_is_valid(portButton2Number)==0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 gpio_is_valid\n", MODULE_NAME);
		goto ERROR_STEP0_2;
	};	

	//Allocate gpio (see description above)
	if(gpio_request(portButton1Number, MODULE_NAME )!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 1 gpio_request\n", MODULE_NAME);
		goto ERROR_STEP1_1;
	};
	if(gpio_request(portButton2Number, MODULE_NAME )!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 gpio_request\n", MODULE_NAME);
		goto ERROR_STEP1_2;
	};


	//Set direction as input
	if(gpio_direction_input(portButton1Number)!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 1 gpio_direction_input\n", MODULE_NAME);
		goto ERROR_STEP2_1;
	};
	if(gpio_direction_input(portButton2Number)!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 gpio_direction_input\n", MODULE_NAME);
		goto ERROR_STEP2_2;
	};


	//Get number of irq associated with this gpio (if exist)
	/*Some GPIO controllers can generate interrupts when an input GPIO changes value.
	  In such cases, code wishing to handle such interrupts should start by determining which IRQ number is associated with a given GPIO line:
	  	int gpio_to_irq(unsigned int gpio);
	  The given gpio must have been obtained with gpio_request() and put into the input mode first. 
	  If there is an associated interrupt number, it will be passed back as the return value from gpio_to_irq(); 
	  otherwise a negative error number will be returned.
	*/
	if((programmContext.usrBtn1IRQNumber = gpio_to_irq(portButton1Number))<0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 1 gpio_to_irq\n", MODULE_NAME);
		goto ERROR_STEP3_1;
	};
	if((programmContext.usrBtn2IRQNumber = gpio_to_irq(portButton2Number))<0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 gpio_to_irq\n", MODULE_NAME);
		goto ERROR_STEP3_2;
	};

	//Set irq handler to function  usrBtn1IRQHandler
	//enable_irq(programmContext.usrBtn1IRQNumber);
	if(request_irq(programmContext.usrBtn1IRQNumber,(*usrBtn1IRQHandler),IRQF_TRIGGER_FALLING,MODULE_NAME,NULL)!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 1 request_irq\n", MODULE_NAME);
		goto ERROR_STEP4_1;
	};

	//enable_irq(programmContext.usrBtn2IRQNumber);
	if(request_irq(programmContext.usrBtn2IRQNumber,(*usrBtn2IRQHandler),IRQF_TRIGGER_FALLING,MODULE_NAME,NULL)!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 request_irq\n", MODULE_NAME);
		goto ERROR_STEP4_2;
	};		
	return 0;

ERROR_STEP4_2:
	free_irq(programmContext.usrBtn1IRQNumber, NULL);
ERROR_STEP4_1:
ERROR_STEP3_1:  	
ERROR_STEP3_2:  
ERROR_STEP2_2:   
ERROR_STEP2_1:   
	gpio_free(portButton2Number);
ERROR_STEP1_2:	
	gpio_free(portButton1Number);	
ERROR_STEP1_1:	
ERROR_STEP0_2:
ERROR_STEP0_1:
	return -1;	
};
 /*-------------------------------------------------------------------------------*/



 /*
  *Say to system - no longer our IRQ handlers and no longer need this GPIO
  */
void deInitButtons(unsigned long int  portButton1Number, unsigned long int portButton2Number){
	//delete IRQ`s	
	//disable_irq(programmContext.usrBtn1IRQNumber);
    free_irq(programmContext.usrBtn1IRQNumber, NULL);

	//disable_irq(programmContext.usrBtn2IRQNumber);
    free_irq(programmContext.usrBtn2IRQNumber, NULL);
	//Say to kernel "We no longer use gpio`s"
	gpio_free(portButton1Number);	
	gpio_free(portButton2Number);		
};
 /*-------------------------------------------------------------------------------*/




 /*
  * 
  */
static int __init init_routine(void){
	printk( KERN_INFO "%s: initialization.\n", MODULE_NAME);
	programmContext.usrled_val=0;

	if(initLED(GPIO_USRLED)!=0){
		goto ERROR_STEP0;
	}

	if(initButtons(GPIO_USRBTN1, GPIO_USRBTN2)!=0){
		goto ERROR_STEP1;
	}
	if(initEPITForPWMAndStart()!=0){
		goto ERROR_STEP2;
	}
	printk( KERN_INFO "[INFO] %s: initialization finished. Module has been started\n", MODULE_NAME);

	return 0;

ERROR_STEP2:	
ERROR_STEP1:	
ERROR_STEP0:	
	return -1;
}
 /*-------------------------------------------------------------------------------*/



 /*
  * 
  */
static void __exit exit_routine(void){
	
	deInitButtons(GPIO_USRBTN1, GPIO_USRBTN2);
	printk( KERN_INFO "[INFO]: deInitButtons\n");

	deInitLed(GPIO_USRLED);
	printk( KERN_INFO "[INFO]: deInitLED\n");



	deInitEPIT();
	

	printk(KERN_INFO "[INFO]: %s: stopped.\n", MODULE_NAME);
}
 /*-------------------------------------------------------------------------------*/
/*===============================================================================*/


/*********************************************************************************/
/*									Init and Deinit module						 */	
/*********************************************************************************/
module_init(init_routine);
module_exit(exit_routine);
/*===============================================================================*/
