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






/*********************************************************************************/
/*									Functions									 */	
/*********************************************************************************/