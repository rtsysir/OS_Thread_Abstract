#include "rtos_abstract.h"

#include <stdlib.h>

#ifdef _FREERTOS_
/**
 *
 * @return
 */
int Init_Memory(void) {

	return 0;
}
/**
 *
 * @param buff
 * @param len
 * @return
 */
void *BSP_calloc(int len) {

#ifdef _UCOS_
	OS_ERR err = OS_ERR_FATAL_RETURN;
	if(len <= BLOCK_SIZE_16)
	{
		buff = OSMemGet(&BSP_Mem16,&err);
		return err;
	}
	if(len <= 32)
	{
		buff = OSMemGet(&BSP_Mem32,&err);
		return err;
	}
	if(len <= 64)
	{
		buff = OSMemGet(&BSP_Mem64,&err);
		return err;
	}
//	if(len <= 128)
//	{
//		buff = OSMemGet(&BSP_Mem128,&err);
//		return err;
//	}
	return -1;
#else //if _FREERTOS_==1
	void *buff;
	buff = (void *) calloc((size_t) len, sizeof(uint8_t));
	if (buff == NULL) {
		OS_DMSG_STR(" BSP_calloc Err\n");
		return NULL;
	}
	return buff;

#endif
}
/**
 *
 * @param block
 * @return
 */
int BSP_free(void *block) {

	if (block != NULL) {
		free(block);
		return 0;
	} else {
		OS_DMSG_STR(" BSP_free Err\n");
		return -1;
	}

}
/**
 *
 */
void *BSP_newSem(void) {

	osSemaphoreId *semid;
	semid = BSP_calloc(sizeof(osSemaphoreId));
	return semid;

}
/**
 *
 * @param sem
 * @return
 */
static int BSP_freeSem(void *sem) {
	if (sem != NULL) {

		BSP_free(sem);
		return 0;
	}
	return -1;

}
/**
 *
 * @param p_name
 * @param cnt
 * @param p_err
 */
void *BSP_SemCreate(uint8_t *p_name, uint32_t cnt, uint16_t *p_err) {

	osSemaphoreDef_t semdef;
	osSemaphoreId *semid;
	//semdef = {0};
	semid = BSP_newSem();
	if (semid != NULL) {
		*semid = osSemaphoreCreate(&semdef, (int32_t) cnt);
		return semid;
	} else {
		return NULL;
	}

}
/**
 *
 * @param p_sem
 * @param p_err
 * @return
 */
int BSP_SemDel(void *p_sem, uint16_t *p_err) {

	if (p_sem != NULL) {

		osStatus status;
		status = osSemaphoreDelete(*(osSemaphoreId*) p_sem);
		*p_err = status;
		BSP_freeSem(p_sem);

		return 0;
	}
	return -1;
}
/**
 *
 * @param p_sem
 * @param p_err
 */
void BSP_SemPost(void *p_sem, uint16_t *p_err) {
	if (p_sem != NULL) {

		osStatus status;
		status = osSemaphoreRelease(*(osSemaphoreId*) p_sem);
		*p_err = status;

	}
}
/**
 *
 * @param p_sem
 * @param timeout
 * @param p_err
 * @return
 */
//ret = OSSemPend(&uart_rx_sem[uid],10,OS_OPT_PEND_BLOCKING,&ts,&err);
int BSP_SemPend(void *p_sem, uint32_t timeout, uint16_t *p_err) {
	if (p_sem != NULL) {

		osStatus status;
		status = osSemaphoreWait(*(osSemaphoreId*) p_sem, timeout);
		*p_err = status;
		return 0;

	}
	return -1;
}
/**
 *
 */
static void *BSP_newMutex(void) {

	osMutexDef_t *mutex;
	mutex = BSP_calloc(sizeof(osMutexDef_t));
	return mutex;

}
/**
 *
 * @param mutex
 * @return
 */
static int BSP_freeMutex(void *mutex) {

	BSP_free(mutex);
	return 0;

	return 0;
}

/**
 *
 * @param p_err
 */
void *BSP_MutexCreate(uint16_t *p_err) {

	osMutexId id;
	osMutexDef_t *mutex;

	mutex = BSP_newMutex();
	if (mutex != NULL) {
		id = osMutexCreate(mutex);
		return id;
	}

	return NULL;
}
/**
 *
 * @param p_mutex
 * @param p_err
 * @return
 */
int BSP_MutexDel(void *p_mutex, uint16_t *p_err) {
	if (p_mutex != NULL) {

		*p_err = (osStatus) osMutexDelete(p_mutex);
		BSP_freeMutex(p_mutex);

	}
	return 0; //no wait
}

/**
 *
 * @param p_mutex
 * @param p_err
 */
void BSP_MutexLock(void *p_mutex, uint16_t *p_err) {
	if (p_mutex != NULL) {

		*p_err = (osStatus) osMutexWait(p_mutex, 0);

	}
}

/**
 *
 * @param p_mutex
 * @param p_err
 */
void BSP_MutexUnlock(void *p_mutex, uint16_t *p_err) {
	if (p_mutex != NULL) {

		*p_err = (osStatus) osMutexRelease(p_mutex);

	}
	return;
}
/**
 *
 * @param task
 * @param func
 * @param arg
 * @return
 */
int BSP_InitTask(BSP_Task_t *task, fpointer func, void *arg) {

	static char name[] = "deafultNmaeTask";
	task->flag = 0;
	task->fun_arg = arg;
	task->func = func;
	task->name = name;
	task->stackSize = 128;
	task->priority = osPriorityNormal;
	return 0;

}
/**
 *
 */
int BSP_CreateTask(BSP_Task_t *task) {
//    OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,              /* Create the start task                                */
//                 (CPU_CHAR     *)"App Task Start",
//                 (OS_TASK_PTR   )AppTaskStart,
//                 (void         *)0u,
//                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,
//                 (CPU_STK      *)&AppTaskStartStk[0u],
//                 (CPU_STK_SIZE  )AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
//                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
//                 (OS_MSG_QTY    )0u,
//                 (OS_TICK       )0u,
//                 (void         *)0u,
//                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
//                 (OS_ERR       *)&err);	

//	pthread_t *rx_thread;
//	pthread_attr_t *attr;

	osThreadDef_t os_thread_def;
	//#name, (thread), (priority), (instances), (stacksz)
	osThreadId threadid;
	os_thread_def.name = task->name;
	os_thread_def.pthread = (os_pthread) task->func;
	os_thread_def.stacksize = task->stackSize;
	os_thread_def.tpriority = task->priority;
	os_thread_def.instances = 0;
	task->thread = osThreadCreate(&os_thread_def, task->fun_arg);
	return 0;

}

int BSP_DelTask(BSP_Task_t *task) {
	// free attribute and wait for the other threads

	osStatus ret;
	ret = osThreadTerminate(task->thread);
	return ret;

}
#endif
