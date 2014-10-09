/*********************************************************************************/
/*									Headers										 */	
/*********************************************************************************/
//System headers
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

//User headers
#include "moduleWDOG.h"	//Header with prototypes and constants
#include "WDOG.h"  		//Header with EPIT constants
/*===============================================================================*/




/*********************************************************************************/
/*									Macro										 */	
/*********************************************************************************/
#define  SET_BITS(WHOM, WHAT)   \
	WHOM|=(WHAT); 

#define CLEAR_BITS(WHOM,WHAT) \
	WHOM&=~(WHAT);
/*===============================================================================*/


int initWATCHDOGAndStart(){
	
	int wdogRegisterMapBegin; 
	unsigned long int WATCHDOG_INTERRUPT_CONTROL_REG=0;
	unsigned long int WATCHDOG_EPITCR=0;
	
	
	SET_BITS(WATCHDOG_EPITCR, (0x3C<<WT_TIME_OUT_FIELD_OFFSET));
	SET_BITS(WATCHDOG_EPITCR,WDW_AT_WAIT_CONTINUE);
	
	// assertion signal (Razreshaem)
	CLEAR_BITS(WATCHDOG_EPITCR, WDA_ASSERT_WDOG_SIGNAL);
	// assertion signal (Razreshaem)
	CLEAR_BITS(WATCHDOG_EPITCR,SRS_ASSERT_WDOG_RESET_SIGNAL);
	
	// Generiruet WDOG 
	SET_BITS(WATCHDOG_EPITCR, WDT_TIMEOUT_ASSERTION_ENABLED);
	
	// Enable WDOG !!!
	SET_BITS(WATCHDOG_EPITCR, WDE_WATCHDOG_ENABLED);

	// Timer is working during debug mode
	CLEAR_BITS(WATCHDOG_EPITCR, WDBG_DEBUG_DISABLED);
	// Timer is working when low power 
	CLEAR_BITS(WATCHDOG_EPITCR, WDZST_LOW_POWER_DISABLED);
	
	/*Write settings to device*/	
	//EPIT settings
		
	if(request_mem_region(WATCH_DOG_CONFIG_REGISTERS_BASE_ADDRESS,WDOG_REGISTERS_TOTAL_LENGTH, "EPIT")==NULL){
		printk("[ERROR]: request_mem_region error\n");
		goto ERROR_STEP1;
	}

	wdogRegisterMapBegin = ioremap(WATCH_DOG_CONFIG_REGISTERS_BASE_ADDRESS,	WDOG_REGISTERS_TOTAL_LENGTH);
	printk( KERN_INFO "[INFO] %s: WDOGRegisterMapBegin = %x\n", MODULE_NAME, wdogRegisterMapBegin);
	
	//Write EPIT config register
	//TODO add offsets of registers in EPIT.h
	iowrite16(WATCHDOG_EPITCR, EPITRegistersMapBegin+WDOG_CONTROL_REGISTER_OFFSET_BYTES);
	
	// На этапе конфигурации регистр Watchdog Service Register настраивать не нужно
	// В режиме работы вотчдога,  таймер надо циклически обслуживать, записывая магические числа 5555 АААА в этот  регистр
	
	
	/* Interrupt control register  */
	CLEAR_BITS(WATCHDOG_INTERRUPT_CONTROL_REG, (0x04<<INTERRUPT_BEFORE_TIME_FIELD_OFFSET));
	
	SET_BITS(WATCHDOG_INTERRUPT_CONTROL_REG, WDOG_INTERRUPT_ENABLED);
	
	iowrite16(WATCHDOG_INTERRUPT_CONTROL_REG, EPITRegistersMapBegin+WDOG_INTERRUPT_CONTROL_REGISTER_OFFSET_BYTES);
	
	/* IRQ */	
	//request irq
	if(request_irq())
	
	
	
	
	
	
	
	
	
};


void deInitWATCHDOG(){
	//TODO create
}





/*********************************************************************************/
/*									Functions									 */	
/*********************************************************************************/
 /*
  * 
  */
static int __init init_routine(void){
	printk( KERN_INFO "%s: initialization.\n", MODULE_NAME);

	printk( KERN_INFO "[INFO] %s: initialization finished. Module has been started\n", MODULE_NAME);


	return -1;
}
 /*-------------------------------------------------------------------------------*/

 /*
  * 
  */
static void __exit exit_routine(void){

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