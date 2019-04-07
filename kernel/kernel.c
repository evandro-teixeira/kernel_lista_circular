/*
 * kernel.c
 *
 *  Created on: 02/01/2019
 *      Author: evandro
 */
#include "kernel.h"
/**
 *
 */
static idTask_t id = 0;
static ptrTask_t idleTask;
static kernelTick_t kernelTick = 0;
noKernel_t* kernel_list;
noKernel_t* kernel_list_init;
idTask_t id_idle;

/**
 *
 */
noKernel_t* kernel_init_list(void);
noKernel_t* kernel_list_insert(noKernel_t* list, idTask_t index, strTask_t param);
noKernel_t* kernel_list_get(noKernel_t* list, idTask_t index);
noKernel_t* kernel_list_remove(noKernel_t* list, idTask_t index, idTask_t max);
void kernel_list_delete(noKernel_t* list);
void kernel_task_idle(void);
void kernel_tick_init(void);
uint32_t kernel_tick_get(void);
/**
 *
 */
void kernel_init(void)
{
	kernel_tick_init();
	kernel_list = kernel_init_list();
	id_idle = kernel_add_task(kernel_task_idle,Priority_Idle,Task_Ready);
}

/**
 *
 */
void kernel_task_delay(idTask_t index, kernelTick_t tick)
{
	uint8_t i = 0;

	for(i=0;i<id;i++)
	{
		if(kernel_list->index == index)
		{
			kernel_list->param.pausedtime = tick + kernel_tick_get();
			kernel_list->param.state = Task_Paused;
			break;
		}
		kernel_list = kernel_list->next;
	}
}

/**
 *
 */
void kernel_run(void)
{
	ptrTask_t taskRun = {KERNEL_NULL};
	idTask_t index = 0;

	while(1)
	{
		// Checa se existe alguma tarefa pausada
		for(index=0;index < id;index++)
		{
			if(kernel_list->param.state == Task_Paused)
			{
				if(kernel_list->param.pausedtime <= kernel_tick_get())
				{
					kernel_list->param.state = Task_Ready;
				}
			}

			kernel_list = kernel_list->next;
		}

		// Busca tarefa de alta prioridade
		for(index=1;index < (id+1);index++)
		{
			if( (kernel_list->param.state == Task_Ready) &&
				(kernel_list->param.priority == Priority_High) &&
				(kernel_list->param.kernel_task_state == kernel_task_waiting))
			{
				taskRun = kernel_list->param.task;
				kernel_list->param.kernel_task_state = kernel_task_running;
				break;
			}

			kernel_list = kernel_list->next;
		}
		if(index > id)
		{
			// Busca tarefa de media prioridade
			for(index=1;index < (id+1);index++)
			{
				if( (kernel_list->param.state == Task_Ready) &&
					(kernel_list->param.priority == Priority_Medium) &&
					(kernel_list->param.kernel_task_state == kernel_task_waiting))
				{
					taskRun = kernel_list->param.task;
					kernel_list->param.kernel_task_state = kernel_task_running;
					break;
				}

				kernel_list = kernel_list->next;
			}
			if(index > id)
			{
				// Busca tarefa de baixa prioridade
				for(index=1;index < (id+1);index++)
				{
					if( (kernel_list->param.state == Task_Ready) &&
						(kernel_list->param.priority == Priority_Low) &&
						(kernel_list->param.kernel_task_state == kernel_task_waiting))
					{
						taskRun = kernel_list->param.task;
						kernel_list->param.kernel_task_state = kernel_task_running;
						break;
					}

					kernel_list = kernel_list->next;
				}
				if(index > id)
				{
					// Busca tarefa de Idle
					for(index=1;index < (id+1);index++)
					{
						if( (kernel_list->param.state == Task_Ready) &&
							(kernel_list->param.priority == Priority_Idle) &&
							(kernel_list->param.kernel_task_state == kernel_task_waiting))
						{
							taskRun = kernel_list->param.task;
							kernel_list->param.kernel_task_state = kernel_task_running;
							break;
						}

						kernel_list = kernel_list->next;
					}
					if(!(index > id))
					{
						kernel_list->param.kernel_task_state = kernel_task_waiting;
					}
				}
				else
				{
					kernel_list->param.kernel_task_state = kernel_task_waiting;
				}
			}
			else
			{
				kernel_list->param.kernel_task_state = kernel_task_waiting;
			}
		}
		else
		{
			kernel_list->param.kernel_task_state = kernel_task_waiting;
		}

		if(taskRun != KERNEL_NULL)
			taskRun();
	}
}

/**
 *
 */
idTask_t kernel_add_task(ptrTask_t task, priorityTask_t priority, stateTask_t state)
{
	strTask_t param;
	param.pausedtime = 0;
	param.priority = priority;
	param.state = state;
	param.task = task;
	param.kernel_task_state = kernel_task_waiting;
	kernel_list = kernel_list_insert(kernel_list,++id,param);
	return id;
}

/**
 *
 */
void kernel_add_task_ilde(ptrTask_t task)
{
	idleTask = task;
}

/**
 *
 */
void kernel_task_idle(void)
{
	if(idleTask != KERNEL_NULL)
		idleTask();
}

/**
 *
 */
void kernel_task_delete(idTask_t index)
{
	kernel_list = kernel_list_remove(kernel_list,index,id);
}

/**
 *
 */
void kernel_idle(idTask_t id)
{
	kernel_task_delay(id,0);
}

/*************** List ********************************************/
/**
 * @brief
 */
noKernel_t* kernel_init_list(void)
{
	return KERNEL_NULL;
}

/**
 *
 */
noKernel_t* kernel_list_insert(noKernel_t* list, idTask_t index, strTask_t param)
{
	static noKernel_t* init_no;
	noKernel_t* new_no = (noKernel_t*)malloc(sizeof(noKernel_t));

	// checa se lista já possui algum elemento
	if(list == KERNEL_NULL)
	{
		init_no = new_no;
	}
	else
	{
		list->next = new_no;
	}

	new_no->index = index;
	new_no->param = param;
	new_no->next = init_no;
	return new_no;
}

/**
 *
 */
noKernel_t* kernel_list_get(noKernel_t* list, idTask_t index)
{
	noKernel_t* item;
	for(item = list; item != KERNEL_NULL; item = item->next)
	{
		if(item->index == index)
			return item;
	}
	return KERNEL_NULL;
}

/**
 *
 */
noKernel_t* kernel_list_remove(noKernel_t* list, idTask_t index, idTask_t max)
{
	idTask_t i = 0;
	noKernel_t* previous;// = list;
	noKernel_t* item = list;

	for(i=1;i<(max+1);i++)
	{
		previous = item;
		if(item->index == index)
		{
			item = item->next;
			break; // achou
		}
		//previous = item;
		item = item->next;
	}
	if(i > max)
	{
		return list;
	}
	else
	{
		// Busca o no que aponta para index que  deseja remover
		noKernel_t* no = item;

		for(i=1;i<(max+1);i++)
		{
			if(no->next == previous)
			{
				no->next = previous->next;
				break; // achou
			}
			no = no->next;
		}
		if(i > max)
		{
			return list;
		}

		free(previous);
		previous->next = item->next;
		return no;
	}
}

/**
 *
 */
void kernel_list_delete(noKernel_t* list)
{
	noKernel_t* aux = list;
	while(aux != KERNEL_NULL)
	{
		noKernel_t* no_next = aux->next;
		free(aux);
		aux = no_next;
	}
}

/**
 *
 */
void kernel_tick_init(void)
{
	uint32_t ticks = SystemCoreClock/1000;
	SysTick->LOAD  = ticks - 1;
	SysTick->VAL   = 0;
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	NVIC_EnableIRQ(SysTick_IRQn);
}

/**
 *
 */
uint32_t kernel_tick_get(void)
{
	return kernelTick;
}

/**
 *
 */
void SysTick_Handler(void)
{
	kernelTick++;
}
