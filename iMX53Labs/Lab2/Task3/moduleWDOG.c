/*********************************************************************************/
/*									Headers										 */	
/*********************************************************************************/
//System headers


//User headers
#include "moduleWDOG.h"	//Header with prototypes and constants
#include "WDOG.h"  		//Header with EPIT constants
/*===============================================================================*/




/*********************************************************************************/
/*									Macro										 */	
/*********************************************************************************/
		
/*===============================================================================*/



/*********************************************************************************/
/*							Global variables									 */	
/*********************************************************************************/
ProgrammContext programmContext={0,NULL,0};
/*===============================================================================*/



/*********************************************************************************/
/*									Interrupts									 */	
/*********************************************************************************/
/*
  *Invert LED value. 
  */
static irqreturn_t irqWATCHDOGBeforefFire(int irq, void *dev_id){
	printk(KERN_INFO "[INFO]: Interrupt has occured. Reset will be soon\n");
	programmContext.usrled_val = 1 & ~ programmContext.usrled_val;
    gpio_set_value(GPIO_USRLED, programmContext.usrled_val);

	//reset watchdog
	programmContext.WATCHDOGTimesToFire--;
//	if(programmContext.WATCHDOGTimesToFire!=0){
		iowrite16(0x5555,((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_SERVICE_REGISTER_OFFSET_BYTES));
		iowrite16(0xAAAA,((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_SERVICE_REGISTER_OFFSET_BYTES));
//	}

	//clear interrupt
	iowrite16(ioread16(((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_INTERRUPT_CONTROL_REGISTER_OFFSET_BYTES)) | 0x4000, ((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_INTERRUPT_CONTROL_REGISTER_OFFSET_BYTES));    ;

	return( IRQ_HANDLED );
}
/*===============================================================================*/





/*********************************************************************************/
/*									Functions									 */	
/*********************************************************************************/
 /*
  * TODO add description
  */
int initWATCHDOGAndStart(){
	/*Declaring variables*/	
	//void * WATCHDOGRegistersMapBegin;	
	unsigned long int WATCHDOG_INTERRUPT_CONTROL_REG=0;
	unsigned long int WATCHDOG_EPITCR=0;
	unsigned short int temp=0;
	/*-------------------------------------------------*/	

	/*Initializing part*/
	//WATCHDOG_WCR Main control register 
	//set watchdog timeout. After which signals will be sended
	SET_WT_TIME_OUT(WATCHDOG_EPITCR, WATCHDOG_TIMEOUT);	
	//Let watchdog work in wait mode
	SET_WDW_AT_WAIT_CONTINUE(WATCHDOG_EPITCR);
	//Software control of ~(WDOG-1) signal. If this bit is set we will programmatically generate low level of this signal
	SET_WDA_NO_ASSERT_WDOG_SIGNAL(WATCHDOG_EPITCR);
	//Software control of ~(wdog_rst) signal. If this bit is set we will programmatically generate low level of this signal
	SET_SRS_NO_ASSERT_WDOG_RESET_SIGNAL(WATCHDOG_EPITCR);
	//We do not need to set low level to signal ~(WDOG-1). See picture 79-4 on page 5008
	SET_WDT_TIMEOUT_ASSERTION_DISABLED(WATCHDOG_EPITCR);
	//Enable WDOG
	SET_WDE_WATCHDOG_ENABLED(WATCHDOG_EPITCR);
	//Timer is working during debug mode
	SET_WDBG_DEBUG_ENABLED(WATCHDOG_EPITCR);
	//Timer is working when low power 
	SET_WDZST_LOW_POWER_ENABLED(WATCHDOG_EPITCR);


	//WDOG_WICR WDOG-1 Interrupt generation control register
	//Set how long before fire interrupt will occure
	SET_INTERRUPT_TIMEOUT_BEFORE_FIRE(WATCHDOG_INTERRUPT_CONTROL_REG,WATCHDOG_INTERRUPT_BEFORE_FIRE);
	//Clear the field that interrupt has occured (just in case)
	SET_WDOG_INTERRUPT_HAS_NOT_OCCURED(WATCHDOG_INTERRUPT_CONTROL_REG);
	//Enable interrupt from watchdog (it can be cleared till reset)
	SET_WDOG_INTERRUPT_ENABLED(WATCHDOG_INTERRUPT_CONTROL_REG);
	/*-------------------------------------------------*/	



	/* IRQ */	
	//request irq
	if(request_irq(WATCHDOG_BEFORE_FIRE_IRQ_NUMBER,(*irqWATCHDOGBeforefFire),IRQF_TRIGGER_FALLING, MODULE_NAME,NULL)!=0){
		printk(KERN_INFO "[ERROR]: %s: initWATCHDOGAndStart request_irq\n", MODULE_NAME);
		goto ERROR_STEP1;
	};	
	//Enable IRQ
	//TODO check if it is needed to disable IRQ...
	//enable_irq(WATCHDOG_BEFORE_FIRE_IRQ_NUMBER);
	/*-------------------------------------------------*/



	/*Write settings to device*/	
	//Request memory and map memory (memory already used by somebody!!!)
	if(request_mem_region(WDOG_CONFIG_REGISTERS_BASE_ADDRESS,WDOG_REGISTERS_TOTAL_LENGTH, "WATCHDOGMyTest")&&0/*NULL*/){
		printk("[ERROR]: initWATCHDOGAndStart request_mem_region error\n");
		goto ERROR_STEP2;
	}
	programmContext.WATCHDOGRegistersMapBegin = ioremap(WDOG_CONFIG_REGISTERS_BASE_ADDRESS,WDOG_REGISTERS_TOTAL_LENGTH);



	//We do not need hardware 16seconds timer to be activated after reset. (It is used to prevent cores from hanging after reset even if watchdog is not started). See 79.4.3 page 5005
	temp=ioread16((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_MISCELLANEOUS_CONTROL_REGISTER_OFFSET_BYTES);
	SET_PDE_DISABLE_POWER_DOWN_COUNTER(temp);
	iowrite16(temp,((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_MISCELLANEOUS_CONTROL_REGISTER_OFFSET_BYTES));


	//Write WATCHDOG_WICR (interrupt before watchdog)
	iowrite16(WATCHDOG_INTERRUPT_CONTROL_REG, ((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_INTERRUPT_CONTROL_REGISTER_OFFSET_BYTES));

	//Write WATCHDOG config register
	iowrite16(WATCHDOG_EPITCR, ((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_CONTROL_REGISTER_OFFSET_BYTES));
	// На этапе конфигурации регистр Watchdog Service Register настраивать не нужно
	// В режиме работы WATCHDOG`а,  таймер надо циклически обслуживать, записывая магические числа 0x5555 0xАААА в этот  регистр


	printk("CONTROL_REGISTER: %X\n", ioread16((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_CONTROL_REGISTER_OFFSET_BYTES));
	printk("WDOG_SERVICE_REGISTER: %X\n", ioread16((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_SERVICE_REGISTER_OFFSET_BYTES));
	printk("WDOG_RESET_STATUS_REGISTER: %X\n", ioread16((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_RESET_STATUS_REGISTER_OFFSET_BYTES));
	printk("WDOG_INTERRUPT_CONTROL_REGISTER: %X\n", ioread16((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_INTERRUPT_CONTROL_REGISTER_OFFSET_BYTES));
	printk("WDOG_RESET_STATUS_REGISTER: %X\n", ioread16((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_RESET_STATUS_REGISTER_OFFSET_BYTES));

	printk("WDOG_MISCELLANEOUS_CONTROL_REGISTER: %X\n", ioread16((char *)programmContext.WATCHDOGRegistersMapBegin+WDOG_MISCELLANEOUS_CONTROL_REGISTER_OFFSET_BYTES));
	printk("WDOG_MISCELLANEOUS_CONTROL_REGISTER: %X\n", temp);

	
	return 0;
	/*-------------------------------------------------*/	
	
	
	/*Error block. Labels are in reverse order*/
ERROR_STEP2:	
	free_irq(WATCHDOG_BEFORE_FIRE_IRQ_NUMBER,NULL);
ERROR_STEP1:	
	return -1;
	/*-------------------------------------------------*/	
};
 /*-------------------------------------------------------------------------------*/



 /*
  *We cannot do anything. If watchdog is enabled only reset can stop him =(
  */
void deInitWATCHDOG(){
	//TODO create
	//Unmap memory and free memory
	iounmap(programmContext.WATCHDOGRegistersMapBegin);
	release_mem_region(WDOG_CONFIG_REGISTERS_BASE_ADDRESS,WDOG_REGISTERS_TOTAL_LENGTH);
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






/*********************************************************************************/
/*									Functions									 */	
/*********************************************************************************/
 /*
  * 
  */
static int __init init_routine(void){
	programmContext.WATCHDOGTimesToFire=2;
	printk( KERN_INFO "%s: initialization.\n", MODULE_NAME);
	if(initLED(GPIO_USRLED)==-1){
		goto ERROR_STEP1;
	}
	if(initWATCHDOGAndStart()!=0){
		goto ERROR_STEP2;
	}
	printk( KERN_INFO "[INFO] %s: initialization finished. Module has been started\n", MODULE_NAME);
	return 0;	



ERROR_STEP2:
	deInitLed(GPIO_USRLED);	
ERROR_STEP1:
	return -1;
}
 /*-------------------------------------------------------------------------------*/

 /*
  * 
  */
static void __exit exit_routine(void){
	deInitLed(GPIO_USRLED);
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
