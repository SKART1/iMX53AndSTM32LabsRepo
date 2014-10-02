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


ProgrammContext programmContext;



/*********************************************************************************/
/*									Interrupts									 */	
/*********************************************************************************/
static irqreturn_t usrBtn1IRQHandler(int irq,void *dev_id){
	//TODO: Add duty cycle decremnet by 10%
    //usrled_val = 1 & ~usrled_val;
    //gpio_set_value(GPIO_USRLED, usrled_val);

	printk("[INFO]: User Button 1\n");
    return( IRQ_HANDLED );
}

static irqreturn_t usrBtn2IRQHandler(int irq, void *dev_id){
   // usrled_val = 1 & ~usrled_val;
   // gpio_set_value(GPIO_USRLED, usrled_val);
	//TODO: Add duty cycle increment 10%
	printk("[INFO]: User Button 2\n");
    return( IRQ_HANDLED );
}

static irqreturn_t irqEPITHandler(int irq, void *dev_id){
	
	//TODO add cleaning interupt bit in EPIT status register
	printk("[INFO]: Timer\n");
	programmContext.usrled_val = 1 & ~ programmContext.usrled_val;
    gpio_set_value(GPIO_USRLED, programmContext.usrled_val);

	iowrite32(ioread32(((char *)programmContext.EPITRegistersMapBegin)+4)|0x00000001,((char *)programmContext.EPITRegistersMapBegin)+4);
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
	//Load register value
	//unsigned long int MODULUS_REGISTER_VALUE_LED_HIGH=0;
	//unsigned long int MODULUS_REGISTER_VALUE_LED_LOW=0;

	//Control register for EPIT N1
	unsigned long int EPIT1_EPITCR=0;
	void *test; 	
	/*-------------------------------------------------*/


	/*Initializing part*/
	//Values for modulus register - during which LED will be switch on or off
	if((LOW_FREQUENCY_CLOCK_SOURCE_HZ/PRESCALER)/REQUIRED_FREQUENCY_HZ>100){
		programmContext.modulusRegisterValueLEDHigh=((LOW_FREQUENCY_CLOCK_SOURCE_HZ/PRESCALER)/REQUIRED_FREQUENCY_HZ)*START_DUTY_CYCLE;
		programmContext.modulusRegisterValueLEDLow=((LOW_FREQUENCY_CLOCK_SOURCE_HZ/PRESCALER)/REQUIRED_FREQUENCY_HZ)*(1-START_DUTY_CYCLE);
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
	//Timer is immediatelly switch on
	SET_BITS(EPIT1_EPITCR,EN_TIMER_ENABLED);
	/*-------------------------------------------------*/


	/*Write values to device*/	
	//EPIT settings
	if(request_mem_region(BASE_ADRESS,REGISTERS_TOTAL_LENGTH, "EPIT")==NULL){
		printk("[ERROR]: request_mem_region error\n");
		return -1;
	}	
	programmContext.EPITRegistersMapBegin=ioremap(BASE_ADRESS,  REGISTERS_TOTAL_LENGTH);	
printk("%X\n", ioread32(((char *)programmContext.EPITRegistersMapBegin)));
printk("%X\n", ioread32(((char *)programmContext.EPITRegistersMapBegin)+4));
printk("%X\n", ioread32(((char *)programmContext.EPITRegistersMapBegin)+8));
printk("%X\n", ioread32(((char *)programmContext.EPITRegistersMapBegin)+12));

	iowrite32(EPIT1_EPITCR, programmContext.EPITRegistersMapBegin);

	//Compare register is zero -because of algorythm
	iowrite32(0L, ((char *)programmContext.EPITRegistersMapBegin)+12);

	//Compare register is zero -because of algorythm
	iowrite32(programmContext.modulusRegisterValueLEDHigh, ((char *)programmContext.EPITRegistersMapBegin)+8);
	/*-------------------------------------------------*/


	/* IRQ */
	//Enable IRQ
	enable_irq(EPIT_IRQ_NUMBER);
	//request irq
	if(request_irq(EPIT_IRQ_NUMBER,(*irqEPITHandler),IRQF_TRIGGER_FALLING, MODULE_NAME,NULL)!=0){
		printk(KERN_INFO "[ERROR]: %s: EPIT 2 request_irq\n", MODULE_NAME);
		return -1;
	};	

	/*Switch on CLOCKING*/
	//map memory
	if(request_mem_region(0x53FD4070,4, "CLOCK")==NULL){
		printk("[ERROR]: request_mem_region clock error\n");
		return -1;
	}	
	test=ioremap(0x53FD4070, 4);
	//Clock settings
	iowrite32(ioread32(test)|0x0000000C,test);

	//unmap memory
	iounmap(test);
	release_mem_region(0x53FD4070, 4);


printk("%X\n", ioread32(((char *)programmContext.EPITRegistersMapBegin)+16));
printk("%X\n", ioread32(((char *)programmContext.EPITRegistersMapBegin)+16));
printk("%X\n", ioread32(((char *)programmContext.EPITRegistersMapBegin)+16));
printk("%X\n", ioread32(((char *)programmContext.EPITRegistersMapBegin)+16));
printk("%X\n", ioread32(((char *)programmContext.EPITRegistersMapBegin)+16));
	return 0;
}
 /*-------------------------------------------------------------------------------*/

 /*
  *
  */
void deInitEPIT(){
	free_irq(EPIT_IRQ_NUMBER,MODULE_NAME); 
	disable_irq(EPIT_IRQ_NUMBER);
	iounmap(programmContext.EPITRegistersMapBegin);
	release_mem_region(BASE_ADRESS,  REGISTERS_TOTAL_LENGTH);
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
	if(gpio_direction_output(portLEDNumber,programmContext.usrled_val)!=0){
		printk(KERN_INFO "[ERROR]: %s: initLED gpio_direction_output\n", MODULE_NAME);
		goto ERROR;
	}
	return 0;


ERROR:
	//We have requested gpio - so we should free it even if it were error
	gpio_free(portLEDNumber);	
	return -1;
};

 /*
  */
void deInitLed(unsigned long int portLEDNumber){
	gpio_free(portLEDNumber);
}
/*===============================================================================*/

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
		return -1;
	};	
	if(gpio_is_valid(portButton2Number)==0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 gpio_is_valid\n", MODULE_NAME);
		return -1;
	};	

	//Allocate gpio (see description above)
	if(gpio_request(portButton1Number, MODULE_NAME )!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 1 gpio_request\n", MODULE_NAME);
		goto ERROR;
	};
	if(gpio_request(portButton2Number, "TESTMY" )!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 gpio_request\n", MODULE_NAME);
		goto ERROR;
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
	if((programmContext.usrBtn1IRQNumber = gpio_to_irq(portButton1Number))<0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 1 gpio_to_irq\n", MODULE_NAME);
		goto ERROR;
	};
	if((programmContext.usrBtn2IRQNumber = gpio_to_irq(portButton2Number))<0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 gpio_to_irq\n", MODULE_NAME);
		goto ERROR;
	};

	//Set irq handler to function  usrBtn1IRQHandler
	if(request_irq(programmContext.usrBtn1IRQNumber,(*usrBtn1IRQHandler),IRQF_TRIGGER_FALLING,MODULE_NAME,NULL)!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 1 request_irq\n", MODULE_NAME);
		goto ERROR;
	};
	if(request_irq(programmContext.usrBtn2IRQNumber,(*usrBtn2IRQHandler),IRQF_TRIGGER_FALLING,MODULE_NAME,NULL)!=0){
		printk(KERN_INFO "[ERROR]: %s: initButtons 2 request_irq\n", MODULE_NAME);
		goto ERROR;
	};		
	return 0;

ERROR:
	//We have requested gpio - so we should free it even if it were error
    free_irq(programmContext.usrBtn1IRQNumber, NULL);
    free_irq(programmContext.usrBtn2IRQNumber, NULL);
	gpio_free(portButton1Number);	
	gpio_free(portButton2Number);
	return -1;	
};
/*===============================================================================*/

/*
*/
void deInitButtons(unsigned long int  portButton1Number, unsigned long int portButton2Number){
	//delete IRQ`s	
    free_irq(programmContext.usrBtn1IRQNumber, NULL);
    free_irq(programmContext.usrBtn2IRQNumber, NULL);

	gpio_free(portButton1Number);	
	gpio_free(portButton2Number);		
};
/*===============================================================================*/



static int __init init_routine(void){
	printk( KERN_INFO "%s: initialization.\n", MODULE_NAME);
	programmContext.usrled_val=0;
	if(initLED(GPIO_USRLED)!=0){
		goto ERROR;
	}

	if(initButtons(GPIO_USRBTN1, GPIO_USRBTN2)!=0){
		goto ERROR;
	}
	if(initEPITForPWMAndStart()!=0){
		goto ERROR;
	}
	printk( KERN_INFO "[INFO] %s: initialization finished. Module has been started\n", MODULE_NAME);

	return 0;

ERROR:
	deInitButtons(GPIO_USRBTN1, GPIO_USRBTN2);
	deInitLed(GPIO_USRLED);
	deInitEPIT();
	return -1;
}

static void __exit exit_routine(void){
	deInitButtons(GPIO_USRBTN1, GPIO_USRBTN2);
	deInitLed(GPIO_USRLED);
	deInitEPIT();

	printk(KERN_INFO "[INFO]: %s: stopped.\n", MODULE_NAME);
}

module_init(init_routine);
module_exit(exit_routine);
