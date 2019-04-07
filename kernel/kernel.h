/*
 * kernel.h
 *
 *  Created on: 02/01/2019
 *      Author: evandro
 */

#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "MK22F51212.h"

#ifndef ptrTask
typedef void(*ptrTask_t)(void);
#endif

#ifndef kernel_tick
typedef uint32_t kernelTick_t;
#endif

#ifndef idTask
typedef uint8_t idTask_t;
#endif

#ifndef KERNEL_NULL
#define KERNEL_NULL ((void *)0)
#endif

typedef enum
{
	kernel_fail = false,
	kernel_ok = true,
}kernelStatus_t;

typedef enum
{
	kernel_task_running = true,
	kernel_task_waiting = false,
}kernelTask_t;

typedef enum
{
	Task_Ready = 0,
	Task_Blocked,
	Task_Paused,
	//Task_Deleted,
}stateTask_t;

typedef enum
{
	Priority_Idle = 0,
	Priority_Low,
	Priority_Medium,
	Priority_High,
}priorityTask_t;

typedef struct
{
	ptrTask_t task;
	stateTask_t state;
	priorityTask_t priority;
	kernelTick_t pausedtime;
	kernelTask_t kernel_task_state;
}strTask_t;

/**
 * Parametros da Lista
 */
typedef struct no
{
	idTask_t index;
	strTask_t param;
	struct no * next;
}noKernel_t;


void kernel_init(void);
void kernel_run(void);
void kernel_add_task_ilde(ptrTask_t task);
idTask_t kernel_add_task(ptrTask_t task, priorityTask_t priority, stateTask_t state);
void kernel_task_delete(idTask_t index);
void kernel_task_delay(idTask_t id, kernelTick_t tick);
void kernel_idle(idTask_t id);

#endif /* KERNEL_KERNEL_H_ */
