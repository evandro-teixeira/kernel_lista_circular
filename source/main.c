/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 **/
/*****************************************************************************/
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_gpio.h"
#include "fsl_debug_console.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "../kernel/kernel.h"
/*!
 * @brief Application entry point.
 *
 */
#define DELAY_LED_GREEN		50000

void led_init(void);
void task_led_red(void);
void task_led_blue(void);
void task_led_green(void);
void push_button_sw3_init(void);
void push_button_sw2_init(void);

idTask_t id_red, id_blue/*, id_map*/;
bool flag_sw3 = false;
bool flag_sw2 = false;

int main(void)
{
	/* Init board hardware. */
	BOARD_InitPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();

	/* Add your code here */
	push_button_sw3_init();
	push_button_sw2_init();
	led_init();

	// Inicializa kernel
	kernel_init();

	// Adiciona tarefa no kernel
	id_red = kernel_add_task(task_led_red,Priority_Medium,Task_Ready);
	id_blue = kernel_add_task(task_led_blue,Priority_Medium,Task_Ready);
	kernel_add_task_ilde(task_led_green);

	// Executa Kernel
	kernel_run();

	for(;;)
	{ 	/* Infinite loop to avoid leaving the main function */
		__asm("NOP"); /* something to use as a breakpoint stop while looping */
	}
}
/*****************************************************************************/
void push_button_sw3_init(void)
{

    const port_pin_config_t portb17_pin40_config = {/* Internal pull-up resistor is enabled */
                                                    kPORT_PullUp,
                                                    /* Fast slew rate is configured */
                                                    kPORT_FastSlewRate,
                                                    /* Passive filter is disabled */
                                                    kPORT_PassiveFilterDisable,
                                                    /* Open drain is disabled */
                                                    kPORT_OpenDrainDisable,
                                                    /* Low drive strength is configured */
                                                    kPORT_LowDriveStrength,
                                                    /* Pin is configured as PTB17 */
                                                    kPORT_MuxAsGpio,
                                                    /* Pin Control Register fields [15:0] are not locked */
                                                    kPORT_UnlockRegister};

    /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);

    /* PORTB17 (pin 40) is configured as PTB17 */
    PORT_SetPinConfig(PORTB, 17U, &portb17_pin40_config);

    /* Define the init structure for the input switch pin */
    gpio_pin_config_t sw_config = {
        kGPIO_DigitalInput, 0,
    };

    /* Init input switch GPIO. */
    PORT_SetPinInterruptConfig(PORTB, 17 /* PTB17*/, kPORT_InterruptFallingEdge);
    EnableIRQ(PORTB_IRQn);
    GPIO_PinInit(GPIOB, 17 /* PTB17 */, &sw_config);
}

/**
 *
 */
void push_button_sw2_init(void)
{

    const port_pin_config_t portc1_pinx_config = {/* Internal pull-up resistor is enabled */
                                                    kPORT_PullUp,
                                                    /* Fast slew rate is configured */
                                                    kPORT_FastSlewRate,
                                                    /* Passive filter is disabled */
                                                    kPORT_PassiveFilterDisable,
                                                    /* Open drain is disabled */
                                                    kPORT_OpenDrainDisable,
                                                    /* Low drive strength is configured */
                                                    kPORT_LowDriveStrength,
                                                    /* Pin is configured as PTB17 */
                                                    kPORT_MuxAsGpio,
                                                    /* Pin Control Register fields [15:0] are not locked */
                                                    kPORT_UnlockRegister};

    /* Port C Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);

    /* PORTC1 (pin x) is configured as PTc1 */
    PORT_SetPinConfig(PORTC, 1U, &portc1_pinx_config);

    /* Define the init structure for the input switch pin */
    gpio_pin_config_t sw_config = {
        kGPIO_DigitalInput, 0,
    };

    /* Init input switch GPIO. */
    PORT_SetPinInterruptConfig(PORTC, 1U /* PTC1*/, kPORT_InterruptFallingEdge);
    EnableIRQ(PORTC_IRQn);
    GPIO_PinInit(GPIOC, 1U /* PTB1 */, &sw_config);
}
/*****************************************************************************/
/**
 *
 */
void led_init(void)
{
    CLOCK_EnableClock(kCLOCK_PortA);
    CLOCK_EnableClock(kCLOCK_PortD);
    PORT_SetPinMux(PORTA, 1U, kPORT_MuxAsGpio);
    PORT_SetPinMux(PORTA, 2U, kPORT_MuxAsGpio);
    PORT_SetPinMux(PORTD, 5U, kPORT_MuxAsGpio);

    // Inicializa LED's
	LED_RED_INIT(LOGIC_LED_OFF);
	LED_BLUE_INIT(LOGIC_LED_OFF);
	LED_GREEN_INIT(LOGIC_LED_OFF);
}

/*****************************************************************************/
/**
 *
 */
void PORTC_IRQHandler(void)
{
	GPIO_PortClearInterruptFlags(GPIOC, 1U << 1 /* PTC1 */);
	//flag_sw2 = true;
	kernel_task_unlocked(id_red);
}
/**
 *
 */
void PORTB_IRQHandler(void)
{
	GPIO_PortClearInterruptFlags(GPIOB, 1U << 17 /* PTB17 */);
	//flag_sw3 = true;
	kernel_task_unlocked(id_blue);
}

/*****************************************************************************/
/**
 *
 */
void task_led_red(void)
{
	LED_RED_TOGGLE();
	kernel_task_blocked(id_red);
}

/**
 *
 */
void task_led_blue(void)
{
	LED_BLUE_TOGGLE();

	kernel_task_blocked(id_blue);
}

/**
 *
 */
void task_led_green(void)
{
	static uint32_t i = 0;

	i++;
	if(i > DELAY_LED_GREEN)
	{
		i = 0;
		LED_GREEN_TOGGLE();
	}
}
/*****************************************************************************/
