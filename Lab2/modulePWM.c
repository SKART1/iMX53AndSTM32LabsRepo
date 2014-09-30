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




int usrbtn1_irq_num;
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
int initLED(unsigned int portNumber){
	//Checking if GPIO is valid (>0, <MAX) and so on - rather primitive function
	//See more in http://lwn.net/Articles/532717/
	//and http://lwn.net/Articles/532714/
	if(gpio_is_valid(GPIO_USRLED)!=1){
		printk(KERN_INFO "[ERROR]: %s: initLED gpio_is_valid\n", MODULE_NAME);
		return -1;
	}

	//
	gpio_free(GPIO_USRLED);

	//Allocate the GPIO
	/*GPIOs must be allocated before use, though the current implementation does not enforce this requirement. The basic allocation function is:
	    int gpio_request(unsigned int gpio, const char *label);
   	  The gpio parameter indicates which GPIO is required, while label associates a string with it that can later appear in sysfs. 
	  The usual convention applies: a zero return code indicates success; otherwise the return value will be a negative error number. 
	  A GPIO can be returned to the system with:
    	void gpio_free(unsigned int gpio);
	*/
	if(gpio_request(GPIO_USRLED, MODULE_NAME)!=0){
		printk(KERN_INFO "[ERROR]: %s: initLED gpio_request\n", MODULE_NAME);
		return -1;
	}
	
	//Set the direction of GPIO and also initial value
	if(gpio_direction_output(GPIO_USRLED,usrled_val)!=0){
		printk(KERN_INFO "[ERROR]: %s: initLED gpio_direction_output\n", MODULE_NAME);
		return -1;
	}
};

 /*
  * Initialize USERBUTTONS - first checks if the ports are correct, after that setting pins (ports) as inputs.
  *	and add interrupt handlers for event PRESSING button
  */
int initButtons(){
	//
	if(gpio_is_valid( GPIO_USRBTN1 )==0){
		printk(KERN_INFO "[ERROR]: %s: initButtons gpio_is_valid\n", MODULE_NAME);
	};	
	gpio_free(GPIO_USRBTN1);

	//
	if(gpio_request( GPIO_USRBTN1, MODULE_NAME )!=0){

	};

	if(gpio_direction_input(GPIO_USRBTN1)!=0){

	};


	//
	/*Some GPIO controllers can generate interrupts when an input GPIO changes value.
	  In such cases, code wishing to handle such interrupts should start by determining which IRQ number is associated with a given GPIO line:
	  	int gpio_to_irq(unsigned int gpio);
	  The given gpio must have been obtained with gpio_request() and put into the input mode first. 
	  If there is an associated interrupt number, it will be passed back as the return value from gpio_to_irq(); 
	  otherwise a negative error number will be returned.
	*/

	//usrbtn1_irq_num = gpio_to_irq(GPIO_USRBTN1);
	//result = request_irq( usrbtn1_irq_num, (*usrbtn1_irq_handler), IRQF_TRIGGER_FALLING,MODULE_NAME, NULL );
	//__ERROR_RETURN(result, "GPIO_USRBTN1 request_irq");
	//gpio_free( GPIO_USRBTN1 );
};
/*===============================================================================*/



static int __init init_routine(void){

	printk( KERN_INFO "%s: initialization.\n", MODULE_NAME);

	

	

	printk( KERN_INFO "[INFO]: %s: started.\n", MODULE_NAME);
	return 0;
}

static void __exit exit_routine(void){
    free_irq(usrbtn1_irq_num, NULL);
    gpio_free(GPIO_USRLED);
    gpio_set_value(GPIO_USRLED, 1);
    printk(KERN_INFO "[INFO]: %s: stopped.\n", MODULE_NAME);
}

module_init(init_routine);
module_exit(exit_routine);
