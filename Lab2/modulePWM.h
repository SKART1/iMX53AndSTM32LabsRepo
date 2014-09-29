/*
 * 	Description of interface and constants
 * 	necessary for correct work of module PWM
 *
 *  Created on: Sep 28, 2014
 *      Author: art
 */

#ifndef MODULEPWM_H_
#define MODULEPWM_H_


#define GPIO_USRLED 199
#define GPIO_USRBTN1 46
#define MODULE_NAME "modulePWM"



static int __init init_routine(void);
static void __exit exit_routine(void);
void initEPIT();



#endif /* MODULEPWM_H_ */
