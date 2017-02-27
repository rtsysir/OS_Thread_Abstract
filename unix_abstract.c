/**
 * @file os_abstract.c
 *
 * @date Ordibehesht 8, 1395 AP
 * @author jupiter
 * @description
 */

#include "rtos_abstract.h"

#include <stdlib.h>

#ifdef _DARWIN_
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

	void *buff;
	buff = (void *) calloc((size_t) len, sizeof(uint8_t));
	if (buff == NULL) {
		OS_DMSG_STR(" BSP_calloc Err\n");
		return NULL;
	}
	return buff;

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

#ifdef __APPLE__
	dispatch_semaphore_t *semid;
	semid = BSP_calloc(sizeof(dispatch_semaphore_t));
	return semid;
#else
	sem_t *semid;
	semid = BSP_calloc(sizeof(sem_t));
	return semid;
#endif

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

	void *semid;
	semid = BSP_newSem();

#ifdef __APPLE__
	dispatch_semaphore_t sem;

	*(dispatch_semaphore_t*) semid = sem = dispatch_semaphore_create(cnt);
	return semid;
#else
	if (semid != NULL) {
		sem_init(semid, 0, cnt);

		//*semid = osSemaphoreCreate(&semdef, cnt);
		return semid;
	} else {
		return NULL;
	}
#endif

}
/**
 *
 * @param p_sem
 * @param p_err
 * @return
 */
int BSP_SemDel(void *p_sem, uint16_t *p_err) {

	if (p_sem != NULL) {

#ifdef __APPLE__
		BSP_free(p_sem);
#else
		sem_destroy(p_sem);
		BSP_free(p_sem);
#endif
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

#ifdef __APPLE__
		dispatch_semaphore_signal(*(dispatch_semaphore_t*) p_sem);
#else
		sem_post(p_sem);
#endif

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
#ifdef __APPLE__
		dispatch_semaphore_wait(*(dispatch_semaphore_t*) p_sem,
				DISPATCH_TIME_FOREVER);
#else
		int r;

		do {
			r = sem_wait(p_sem);
		}while (r == -1 && errno == EINTR);
#endif

		return 0;

	}
	return -1;
}
/**
 *
 */
static void *BSP_newMutex(void) {

	OS_DMSG_STR("BSP_newMutex \n");
	pthread_mutex_t *mutex;
	mutex = BSP_calloc(sizeof(pthread_mutex_t));
	return mutex;

}
/**
 *
 * @param mutex
 * @return
 */
static int BSP_freeMutex(void *mutex) {

	OS_DMSG_STR("BSP_freeMutex \n");
	BSP_free(mutex);

	return 0;
}

/**
 *
 * @param p_err
 */
void *BSP_MutexCreate(uint16_t *p_err) {

	pthread_mutex_t *mutex;
	OS_DMSG_STR("BSP_MutexCreate \n");
	mutex = (pthread_mutex_t *) BSP_newMutex();
	if (mutex != NULL) {
		if (pthread_mutex_init(mutex, NULL) == -1) {
			OS_ERR("mutex init");
		}
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
		OS_DMSG_STR("BSP_MutexDel \n");
		if (pthread_mutex_destroy(p_mutex) == -1)
			OS_ERR("mutex delet");
		BSP_freeMutex(p_mutex);
		return 1;

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
		OS_DMSG_STR("BSP_MutexLock \n");
		if (pthread_mutex_lock(p_mutex) == -1) {
			OS_ERR("mutex lock");
		}

	}
}

/**
 *
 * @param p_mutex
 * @param p_err
 */
void BSP_MutexUnlock(void *p_mutex, uint16_t *p_err) {
	if (p_mutex != NULL) {

		OS_DMSG_STR("BSP_MutexUnlock \n");
		if (pthread_mutex_unlock(p_mutex) == -1) {
			OS_ERR("mutex unlock");
		}

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

	OS_DMSG_STR("init task");
	task->thread = BSP_calloc(sizeof(pthread_t));
	task->thread_attrib = BSP_calloc(sizeof(pthread_attr_t));
	if (task->thread_attrib == NULL || task->thread == NULL) {
		return -1;
	}
	task->flag = 0;
	task->fun_arg = arg;
	task->func = func;
	task->name = NULL;
	task->stackSize = 0;
	return 0;

}
/**
 *
 */
int BSP_CreateTask(BSP_Task_t *task) {

	if (task->thread_attrib != NULL && task->thread != NULL) {
		OS_DMSG_STR("create task low level");
		pthread_attr_init(task->thread_attrib);
		pthread_attr_setdetachstate(task->thread_attrib,
		PTHREAD_CREATE_JOINABLE);
		/* create a second thread which executes inc_x(&x) */
		if (pthread_create(task->thread, NULL, task->func, task->fun_arg)) {
			OS_DMSG_STR("Error creating thread\n");
		}OS_DMSG_STR("created");
		return 0;
	}OS_DMSG_STR("create task err");
	return -1;

}

int BSP_DelTask(BSP_Task_t *task) {
	// free attribute and wait for the other threads

	OS_DMSG_STR("del task");
	pthread_attr_destroy(task->thread_attrib);
	if (pthread_join((pthread_t) *((pthread_t*) task->thread), NULL)) {
		OS_DMSG_STR("Error joining thread\n");
	}
	pthread_exit(NULL);

}
#endif

