/*********************************************************************************/
/*									Headers										 */	
/*********************************************************************************/
//System headers
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

//User headers
#include "modulePWM.h"	//Header with prototypes and constants
#include "EPIT.h"  		//Header with EPIT constants
/*===============================================================================*/




/*********************************************************************************/
/*									Macro										 */	
/*********************************************************************************/
#define  SET_BITS(WHOM, WHAT)   \
	WHOM|=(WHAT); 

#define CLEAR_BITS(WHOM,WHAT) \
	WHOM&=~(WHAT);
/*===============================================================================*/




/*********************************************************************************/
/*							Internal module constants							 */	
/*********************************************************************************/
#define LOW_FREQUENCY_CLOCK_SOURCE_HZ	32000
#define PRESCALER						0b001	
/*===============================================================================*/




int usrBtn1IRQNumber;
int usrBtn2IRQNumber;
volatile int usrled_val = 0;


/*********************************************************************************/
/*									Interrupts									 */	
/*********************************************************************************/
static irqreturn_t usrBtn1IRQHandler(int irq,void *dev_id){
	//TODO: Add duty cycle decremnet by 10%
    //usrled_val = 1 & ~usrled_val;
    //gpio_set_value(GPIO_USRLED, usrled_val);
    return( IRQ_HANDLED );
}

static irqreturn_t usrBtn2IRQHandler(int irq, void *dev_id){
   // usrled_val = 1 & ~usrled_val;
   // gpio_set_value(GPIO_USRLED, usrled_val);
	//TODO: Add duty cycle increment 10%
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
int initEPITForPWM(){
	/*Declaring variables*/
	//Load register value
	unsigned long int MODULUS_REGISTER_VALUE_LED_HIGH=0;
	unsigned long int MODULUS_REGISTER_VALUE_LED_LOW=0;

	//Control register for EPIT N1
	unsigned long int EPIT1_EPITCR=0; 	
	/*-------------------------------------------------*/


	/*Initializing part*/
	//Values for modulus register - during which LED will be switch on or off
	if((LOW_FREQUENCY_CLOCK_SOURCE_HZ/PRESCALER)/REQUIRED_FREQUENCY_HZ>100){
		MODULUS_REGISTER_VALUE_LED_HIGH=((LOW_FREQUENCY_CLOCK_SOURCE_HZ/PRESCALER)/REQUIRED_FREQUENCY_HZ)*START_DUTY_CYCLE;
		MODULUS_REGISTER_VALUE_LED_LOW=((LOW_FREQUENCY_CLOCK_SOURCE_HZ/PRESCALER)/REQUIRED_FREQUENCY_HZ)*(1-START_DUTY_CYCLE);
	}
	else{	
		printk("[ERROR]: The of  LOW_FREQUENCY_CLOCK_SOURCE_HZ, PRESCALER, REQUIRED_FREQUENCY_HZ results in value for MODULUS register \n");
		return -1;
	};

	//EPIT control register
	//Clock source is 32 kHZ oscillator	
	SET_BITS(EPIT1_EPITCR, CLKSRC_LOW_FREQUENCY_CLOCK);
	//No actions are neccessary on output pins(?!)
	SET_BITS(EPIT1_EPITCR, OM_EPIT_DISCONNECTED);
	//Timer will work even in STOP state of system
	SET_BITS(EPIT1_EPITCR, STOPEN_ENABLED);
	//Timer will work even in WAIT state of system
	SET_BITS(EPIT1_EPITCR,WAITEN_ENABLED);
	//Timer will work even in DEBUG state of system
	SET_BITS(EPIT1_EPITCR,DBGEN_ENABLED);
	//Timer counter register will be immideatelly set to value in MODULUS register if it is changed
	SET_BITS(EPIT1_EPITCR,IOVW_COUNTER_OVERRITEN);
	//Prescaller equals to 1 - means now prescale
	SET_BITS(EPIT1_EPITCR,(PRESCALER<<PRESCALER_BASE_ADDRESS));
	//Timer will reload it`s value from modulus register - really do not used. We will overite MODULUS value manually 
	//when timer will reach zero and because of "SET_BITS(EPIT1_EPITCR,IOVW_COUNTER_OVERRITEN);" above 
	SET_BITS(EPIT1_EPITCR,RLD_LOAD_FROM_MODULUS);
	//Switch on interrupts caused by equality of timer value and compare registers
	SET_BITS(EPIT1_EPITCR,OCIEN_COMP_INTER_ENAB);
	//Counter will continue from previous values if it will be paused
	SET_BITS(EPIT1_EPITCR,ENMOD_COUNTER_WILL_CONTINUE);
	//
	SET_BITS(EPIT1_EPITCR,EN_TIMER_ENABLED);
	/*-------------------------------------------------*/


	/*Write values to device*/
	//TODO add write to configure registers
	//gpio_set_value(GPIO_USRLED, usrled_val);
	/*-------------------------------------------------*/
	

	return 0;
}
 /*-------------------------------------------------------------------------------*/




 /*
  * Initialize LEDs - first checks if the ports are correct, after that setting pin (port) as output
  */
int initLED(unsigned int portLEDNumber){
	//Checking if GPIO is valid on this type of architecture (>0, <MAX) and so on - rather primitive function
	//See more in http://lwn.net/Articles/532717/
	//and http://lwn.net/Articles/532714/
	if(gpio_is_valid(portLEDNumber)!=1){
		printk(KERN_INFO "[ERROR]: %s: initLED gpio_is_valid\n", MODULE_NAME);
		return -1;
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
		goto ERROR;
	}

	
	//Set the direction of GPIO and also initial value
	/*For compatibility with legacy interfaces to GPIOs, setting the direction
	  of a GPIO implicitly requests that GPIO (see below) if it has not been
	  requested already.  That compatibility is being removed from the optional
	  gpiolib framework. (Documentation/gpio.txt)
	*/
	if(gpio_direction_output(portLEDNumber,usrled_val)!=0){
		printk(KERN_INFO "[ERROR]: %s: initLED gpio_direction_output\n", MODULE_NAME);
		goto ERROR;
	}
	return 0;


ERROR:
	//We have requested gpio - so we should free it even if it were error
	gpio_free(GPIO_USRBTN1);	
	return -1;
};




 /*
  * Initialize USERBUTTONS - first checks if the ports are correct, after that setting pins (ports) as inputs.
  *	and add interrupt handlers for event PRESSING button
  */
int initButtons(unsigned int portButton1Number, unsigned int portButton2Number){
	//Checking if GPIO is valid on this type of architecture (>0, <MAX) and so on - rather primitive function
	//See more in http://lwn.net/Articles/532717/
	//and http://lwn.net/Articles/532714/
	if(gpio_is_valid(portButton1Number)==0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 1 gpio_is_valid\n", MODULE_NAME);
		return -1;
	};	
	if(gpio_is_valid(portButton2Number)==0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 gpio_is_valid\n", MODULE_NAME);
		return -1;
	};	

	//Allocate gpio (see description above)
	if(gpio_request(portButton1Number, MODULE_NAME )!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 1 gpio_request\n", MODULE_NAME);
		return -1;
	};
	if(gpio_request(portButton2Number, MODULE_NAME )!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 gpio_request\n", MODULE_NAME);
		return -1;
	};


	//Set direction as input
	if(gpio_direction_input(portButton1Number)!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 1 gpio_direction_input\n", MODULE_NAME);
		goto ERROR;
	};
	if(gpio_direction_input(portButton2Number)!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 gpio_direction_input\n", MODULE_NAME);
		goto ERROR;
	};


	//Get number of irq associated with this gpio (if exist)
	/*Some GPIO controllers can generate interrupts when an input GPIO changes value.
	  In such cases, code wishing to handle such interrupts should start by determining which IRQ number is associated with a given GPIO line:
	  	int gpio_to_irq(unsigned int gpio);
	  The given gpio must have been obtained with gpio_request() and put into the input mode first. 
	  If there is an associated interrupt number, it will be passed back as the return value from gpio_to_irq(); 
	  otherwise a negative error number will be returned.
	*/
	if((usrBtn1IRQNumber = gpio_to_irq(portButton1Number))<0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 1 gpio_to_irq\n", MODULE_NAME);
		goto ERROR;
	};
	if((usrBtn2IRQNumber = gpio_to_irq(portButton2Number))<0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 gpio_to_irq\n", MODULE_NAME);
		goto ERROR;
	};

	//Set irq handler to function  usrBtn1IRQHandler
	if(request_irq(usrBtn1IRQNumber,(*usrBtn1IRQHandler),IRQF_TRIGGER_FALLING,MODULE_NAME,NULL)!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 1 request_irq\n", MODULE_NAME);
		goto ERROR;
	};
	if(request_irq(usrBtn2IRQNumber,(*usrBtn1IRQHandler),IRQF_TRIGGER_FALLING,MODULE_NAME,NULL)!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 request_irq\n", MODULE_NAME);
		goto ERROR;
	};		
	return 0;

ERROR:
	//We have requested gpio - so we should free it even if it were error
	gpio_free(portButton1Number);	
	return -1;	
};
/*===============================================================================*/



static int __init init_routine(void){
	printk( KERN_INFO "%s: initialization.\n", MODULE_NAME);
	if(initLED(GPIO_USRLED)!=0){
		return -1;
	}

	if(initButtons(GPIO_USRBTN1, GPIO_USRBTN2)!=0){
		return -1
	}

	printk( KERN_INFO "[INFO]%s: initialization finished. Module has been started\n", MODULE_NAME);
	return 0;
}

static void __exit exit_routine(void){
    free_irq(usrBtn1IRQNumber, NULL);
    free_irq(usrBtn2IRQNumber, NULL);

	//We do not use USRBTN1 anymore
	gpio_free(GPIO_USRBTN1);
	//Let LED be switched on
    gpio_set_value(GPIO_USRLED, 1);
	//We do not use led anymore
    gpio_free(GPIO_USRLED);
    printk(KERN_INFO "[INFO]: %s: stopped.\n", MODULE_NAME);
}

module_init(init_routine);
module_exit(exit_routine);
