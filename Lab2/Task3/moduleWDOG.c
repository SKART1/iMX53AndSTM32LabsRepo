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
	unsigned long int WATCHDOG_EPITCR=0;
	SET_BITS(WATCHDOG_EPITCR, (0x3C<<WT_TIME_OUT_FIELD_OFFSET));
	SET_BITS(WATCHDOG_EPITCR,WDW_AT_WAIT_CONTINUE);

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