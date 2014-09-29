#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#include "modulePWM.h"


/*
 *
*/
MODULE_LICENSE ("GPL");
MODULE_DESCRIPTION ("Simple linux kernel module example.");
MODULE_AUTHOR ("Student");
/*-----------------------------------------------------------------------*/







module_init(init_routine);
module_exit(exit_routine);




int usrbtn1_irq_num;
volatile int usrled_val = 0;

static irqreturn_t usrbtn1_irq_handler( int irq, void *dev_id ){
    usrled_val = 1 & ~usrled_val;
    gpio_set_value(GPIO_USRLED, usrled_val);
    return( IRQ_HANDLED );
}


//#define CLKSRC 			24/*Clock source*/
#define LOW_FREQUENCY_CLOCK (11b<<24)

//#define OM 			22/*EPIT output mode*/
#define EPIT_DISCONNECTED 	(00b<<22)


#define  SET_BITS(WHOM, WHAT)   \
	WHOM|=(WHAT); 

#define CLEAR_BITS(WHOM,WHAT) \
	WHOM&=~(WHAT);



void initEPIT(){
	unsigned long int EPIT1_EPITCR=0;
	SET_BITS(EPIT1_EPITCR, LOW_FREQUENCY_CLOCK);

	//EPIT1_EPITCR|=(CLKSRC);
	

	//EPIT1_EPITCR
		
	
}




static int __init init_routine(void){
    int result = 0;
    
    printk( KERN_INFO "%s: initialization.\n", MODULE_NAME);

    if(gpio_is_valid(GPIO_USRLED)==-1){
    	printk(KERN_INFO "[ERROR]: %s: gpio_is_invalid\n");
    	return -1;
    }

    printk("gpio_is_valid: %i", result);
    __ERROR_RETURN(result, "USRLED gpio_is_valid");
    gpio_free( GPIO_USRLED );
    result = gpio_request( GPIO_USRLED, MODULE_NAME );
    __ERROR_RETURN(result, "USRLED gpio_request");
    result = gpio_direction_output( GPIO_USRLED, usrled_val);
    __ERROR_RETURN(result, "USRLED gpio_direction_output");
    
    result = -1 + gpio_is_valid( GPIO_USRBTN1 );
    __ERROR_RETURN(result, "GPIO_USRBTN1 gpio_is_valid");
    gpio_free( GPIO_USRBTN1 );
    result = gpio_request( GPIO_USRBTN1, MODULE_NAME );
    __ERROR_RETURN(result, "GPIO_USRBTN1 gpio_request" );
    result = gpio_direction_input( GPIO_USRBTN1 );
    __ERROR_RETURN(result, "GPIO_USRBTN1 gpio_direction_input");
    usrbtn1_irq_num = gpio_to_irq( GPIO_USRBTN1 );
    result = request_irq( usrbtn1_irq_num, (*usrbtn1_irq_handler), IRQF_TRIGGER_FALLING,MODULE_NAME, NULL );
    __ERROR_RETURN(result, "GPIO_USRBTN1 request_irq");
    gpio_free( GPIO_USRBTN1 );
    
    printk( KERN_INFO "%s: started.\n", MODULE_NAME);
    return 0;
}
static void __exit exit_routine(void)
{
    free_irq( usrbtn1_irq_num, NULL );
    gpio_free( GPIO_USRLED );
    gpio_set_value(GPIO_USRLED, 1);
    printk( KERN_INFO "%s: stopped.\n", MODULE_NAME);
}
