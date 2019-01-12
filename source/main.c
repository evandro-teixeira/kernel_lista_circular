/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 **/

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_port.h"
#include "../kernel/kernel.h"

void task_led_red(void);
void task_led_blue(void);
void task_led_green(void);

idTask_t red, blue;

/*!
 * @brief Application entry point.
 */
int main(void)
{
	/* Init board hardware. */
	BOARD_InitPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();

	/* Add your code here */
    CLOCK_EnableClock(kCLOCK_PortA);
    CLOCK_EnableClock(kCLOCK_PortD);
    PORT_SetPinMux(PORTA, 1U, kPORT_MuxAsGpio);
    PORT_SetPinMux(PORTA, 2U, kPORT_MuxAsGpio);
    PORT_SetPinMux(PORTD, 5U, kPORT_MuxAsGpio);

	LED_RED_INIT(LOGIC_LED_OFF);
	LED_BLUE_INIT(LOGIC_LED_ON);
	LED_GREEN_INIT(LOGIC_LED_OFF);

	kernel_init();

	red = kernel_add_task(task_led_red,Priority_High,Task_Ready);
	blue = kernel_add_task(task_led_blue,Priority_High,Task_Ready);
	kernel_add_task_ilde(task_led_green);

	kernel_run();

	for(;;)
	{
		/* Infinite loop to avoid leaving the main function */
		__asm("NOP"); /* something to use as a breakpoint stop while looping */
	}
}

/**
 *
 */
void task_led_red(void)
{
	uint32_t i = 0;
	static uint8_t counter = 0;

	LED_RED_TOGGLE();
/*	for(i=0;i<100000;i++);
	LED_RED_TOGGLE();
	for(i=0;i<100000;i++);
	*/

	counter++;
	if(counter > 100)
	{
		//counter = 0;
		kernel_task_delete(red);
	}

	kernel_idle(red);
	//kernel_task_delay(red,0);
}

/**
 *
 */
void task_led_blue(void)
{
	uint32_t i = 0;
	static uint8_t counter = 0;

	LED_BLUE_TOGGLE();
/*	for(i=0;i<100000;i++);
	LED_BLUE_TOGGLE();
	for(i=0;i<100000;i++);
	*/
/*
	counter++;
	if(counter > 200)
	{
		counter = 0;
		red = kernel_add_task(task_led_red,Priority_High,Task_Ready);
	}
*/
	kernel_idle(blue);
	//kernel_task_delay(blue,0);
}

/**
 *
 */
void task_led_green(void)
{
	uint32_t i = 0;
/*
	LED_GREEN_TOGGLE();
	for(i=0;i<100000;i++);
	LED_GREEN_TOGGLE();
	for(i=0;i<100000;i++);
	*/
}
