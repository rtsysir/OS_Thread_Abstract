/**
 * @file ucos3_abstract.c
 *
 * @date Ordibehesht 8, 1395 AP
 * @author jupiter
 * @description
 */

#include "rtos_abstract.h"

#include <stdlib.h>

#ifdef _UCOS_
OS_MEM RTOS_Mem16;
CPU_INT08U CommBuf16[NUM_OF_BLOCK_16][BLOCK_SIZE_16]; /* 16 buffers of 32 words of 32 bits */

OS_MEM BSP_Mem32;
CPU_INT08U *CommBuf32[1][32]; /* 16 buffers of 32 words of 32 bits */

OS_MEM BSP_Mem64;
CPU_INT08U *CommBuf64[1][64]; /* 16 buffers of 32 words of 32 bits */

//OS_MEM       BSP_Mem128;
//CPU_INT08U  *CommBuf128[NUM_OF_BLOCK_16][128];          /* 16 buffers of 32 words of 32 bits */
OS_MEM BSP_MemSem;
CPU_INT08U CommBufSem[NUM_OF_SEMAPHORE][sizeof(OS_SEM)]; /* 16 buffers of 32 words of 32 bits */

OS_MEM BSP_MemMutex;
CPU_INT08U CommBufMutex[NUM_OF_MUTEX][sizeof(OS_MUTEX)]; /* 16 buffers of 32 words of 32 bits */

/**
 *
 * @return
 */
int Init_Memory(void) {

	OS_ERR err = OS_ERR_FATAL_RETURN;

	OSMemCreate(&BSP_Mem16,
			"buffer 16 byte",
			&CommBuf16[0][0],
			NUM_OF_BLOCK_16,
			BLOCK_SIZE_16 * sizeof(CPU_INT08U),
			&err);

	OSMemCreate(&BSP_Mem32,
			"buffer 32 byte",
			&CommBuf32[0][0],
			4,
			32 * sizeof(CPU_INT08U),
			&err);
	OSMemCreate(&BSP_Mem64,
			"buffer 64 byte",
			&CommBuf64[0][0],
			4,
			64 * sizeof(CPU_INT08U),
			&err);
//	OSMemCreate(&BSP_Mem128,
//                "buffer 128 byte",
//                &CommBuf128[0][0],
//                NUM_OF_BLOCK_16,
//                128 * sizeof(CPU_INT08U),
//                &err);
	OSMemCreate(&BSP_MemSem,
			"buffer 16 byte",
			&CommBufSem[0][0],
			NUM_OF_SEMAPHORE,
			sizeof(OS_SEM),
			&err);

	OSMemCreate(&BSP_MemMutex,
			"buffer 16 byte",
			&CommBufMutex[0][0],
			NUM_OF_MUTEX,
			sizeof(OS_MUTEX),
			&err);

	return 0;
}
/**
 *
 * @param buff
 * @param len
 * @return
 */
void *BSP_calloc(int len) {

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

}
/**
 *
 * @param block
 * @return
 */
int BSP_free(void *block) {

	OS_ERR err = OS_ERR_FATAL_RETURN;
	if((uint32_t)CommBuf16<(uint32_t)block &&
			(uint32_t)block <(uint32_t)(CommBuf16+(BLOCK_SIZE_16*NUM_OF_BLOCK_16)))
	{
		OSMemPut(&BSP_Mem16,block,&err);
	}
	return 0;

}
/**
 *
 */
void *BSP_newSem(void) {

	void *sem;
	OS_ERR err = OS_ERR_FATAL_RETURN;
	sem = OSMemGet(&BSP_MemSem,&err);
	return sem;

}
/**
 *
 * @param sem
 * @return
 */
static int BSP_freeSem(void *sem) {
	if (sem != NULL) {

		OS_ERR err = OS_ERR_MEM_INVALID_P_MEM;

		OSMemPut(&BSP_MemSem,sem,&err);
		return err;

	}
	/**
	 *
	 * @param p_name
	 * @param cnt
	 * @param p_err
	 */
	void *BSP_SemCreate(uint8_t *p_name, uint32_t cnt, uint16_t *p_err) {

		OS_SEM *sem = NULL;
		*p_err = OS_ERR_MEM_INVALID_P_MEM;
		sem = BSP_newSem();
		if(sem != NULL )
		{
			OSSemCreate(sem,p_name,cnt,p_err);
			return sem;
		}
		return NULL;

	}
	/**
	 *
	 * @param p_sem
	 * @param p_err
	 * @return
	 */
	int BSP_SemDel(void *p_sem, uint16_t *p_err) {

		if (p_sem != NULL) {

			OS_OBJ_QTY ret;
			int err = OS_ERR_FATAL_RETURN;

			ret = OSSemDel((OS_SEM *)p_sem,OS_OPT_DEL_NO_PEND,p_err);
			err = BSP_freeSem(p_sem);
			if(err != OS_ERR_NONE)
			{
				return OS_ERR_MEM_INVALID_P_MEM;
			}

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

			OSSemPost((OS_SEM *)p_sem,OS_OPT_POST_1,p_err);
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

			uint32_t RetVal=0;
			CPU_TS ts;
			OS_SEM_CTR ret=0;

			ret = OSSemPend((OS_SEM *)p_sem,timeout,OS_OPT_PEND_BLOCKING,&ts,p_err);

			return ret;
		}
		return -1;
	}
	/**
	 *
	 */
	static void *BSP_newMutex(void) {

		void *mutex;
		OS_ERR err = OS_ERR_FATAL_RETURN;
		mutex = OSMemGet(&BSP_MemMutex,&err);
		return mutex;
	}
	/**
	 *
	 * @param mutex
	 * @return
	 */
	static int BSP_freeMutex(void *mutex) {

		OS_ERR err = OS_ERR_MEM_INVALID_P_MEM;
		if(mutex != NULL)
		{
			OSMemPut(&BSP_MemMutex,mutex,&err);
		}
		else
		{
			while(1);
		}
		return err;

		return 0;
	}

	/**
	 *
	 * @param p_err
	 */
	void *BSP_MutexCreate(uint16_t *p_err) {

		OS_MUTEX *mutex;
		*p_err = OS_ERR_MEM_INVALID_P_MEM;
		mutex = (OS_MUTEX *)BSP_newMutex();
		if(mutex != NULL )
		{
			OSMutexCreate(mutex,"mutx",p_err);
			return mutex;
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

			OS_OBJ_QTY ret;
			int err = OS_ERR_FATAL_RETURN;

			ret = OSMutexDel((OS_MUTEX *)p_mutex,OS_OPT_DEL_NO_PEND,p_err);
			err = BSP_freeMutex(p_mutex);
			if(err != OS_ERR_NONE)
			{
				return OS_ERR_MEM_INVALID_P_MEM;
			}
			return ret;

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

			OS_ERR err = OS_ERR_FATAL_RETURN;
			CPU_TS ts;

			OSMutexPend((OS_MUTEX *)p_mutex,
					(OS_TICK )0,
					(OS_OPT )OS_OPT_PEND_BLOCKING,
					(CPU_TS *)&ts,
					(OS_ERR *)&p_err);

		}
	}

	/**
	 *
	 * @param p_mutex
	 * @param p_err
	 */
	void BSP_MutexUnlock(void *p_mutex, uint16_t *p_err) {
		if (p_mutex != NULL) {

			OS_ERR err = OS_ERR_FATAL_RETURN;
			uint32_t RetVal=0;
			CPU_TS ts;
			OS_SEM_CTR ret=0;

			OSMutexPost((OS_MUTEX *)&p_mutex,
					(OS_OPT )OS_OPT_POST_NONE,
					(OS_ERR *)&p_err);

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
	}

	int BSP_DelTask(BSP_Task_t *task) {
		// free attribute and wait for the other threads

	}
#endif
