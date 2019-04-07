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

    // Inicializa LED's
	LED_RED_INIT(LOGIC_LED_OFF);
	LED_BLUE_INIT(LOGIC_LED_ON);
	LED_GREEN_INIT(LOGIC_LED_OFF);

	// Inicializa kernel
	kernel_init();

	// Adiciona tarefa no kernel
	red = kernel_add_task(task_led_red,Priority_High,Task_Ready);
	blue = kernel_add_task(task_led_blue,Priority_High,Task_Ready);
	kernel_add_task_ilde(task_led_green);

	// Executa Kernel
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
	static uint8_t counter = 0;

	LED_RED_TOGGLE();

	counter++;
	if(counter > 100)
	{
		// Deleta Task RED
		counter = 0;
		kernel_task_delete(red);
		return;
	}
	// Pausa Task RED por 1000 tick's
	kernel_task_delay(red,1000);
}

/**
 *
 */
void task_led_blue(void)
{
	LED_BLUE_TOGGLE();
	// Pausa Task BLUE por 2000 tick's
	kernel_task_delay(blue,2000);
}

/**
 *
 */
void task_led_green(void)
{
	uint32_t i = 0;

	LED_GREEN_TOGGLE();
	for(i=0;i<1000;i++);
}
