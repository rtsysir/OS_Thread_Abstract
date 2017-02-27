


#ifndef RTOS_ABSTRACT_
#define RTOS_ABSTRACT_

#include <stdlib.h>



#ifdef _UCOS_
#undef _FREERTOS_
#undef _DARWIN_
#include "includes.h"
#endif


#ifdef _FREERTOS_
#undef _UCOS_
#undef _DARWIN_
#include "includes.h"
#endif


#ifdef _DARWIN_
#undef _UCOS_
#undef _FREERTOS_
#include <time.h>
#include <errno.h>
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif
#include <pthread.h>
#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif


#include <errno.h>
#endif



#ifndef OS_DEBUG
#define OS_DEBUG 1
#endif

#if (OS_DEBUG==1)
#define OS_DMSG(args...)       printf("OS DEBUG:");printf(args)
#define OS_DMSG_STR(str)       printf("OS DEBUG:");printf((const char*)str); printf("\n")
#define OS_DMSG_HEX(num)       printf(" "); printf("%02X",num)
#define OS_DMSG_INT(num)       printf(" "); printf("%u",num)
#define OS_ERR(str)			   perror(str);
#else
#define OS_DMSG(args...)
#define OS_DMSG_STR(str)
#define OS_DMSG_HEX(num)
#define OS_DMSG_INT(num)
#define OS_ERR(str)
#endif


#ifdef __cplusplus
extern "C" {
#endif

#define NUM_OF_BLOCK_16 1
#define BLOCK_SIZE_16 16
#define NUM_OF_MUTEX 4
#define NUM_OF_SEMAPHORE 16

typedef  void *(*fpointer)(void *);

typedef struct
{
	uint8_t *name;
	fpointer func;
	void *fun_arg;
	uint16_t stackSize;
	int priority;
	uint8_t flag;
	void *thread;
	void *thread_attrib;
}BSP_Task_t;


int Init_Memory(void);

 int BSP_free(void *block);
 void *BSP_calloc(int len);
static int BSP_freeSem(void *sem);
void 	*BSP_newSem(void);
void  	*BSP_SemCreate (uint8_t    *p_name,uint32_t   cnt,uint16_t      *p_err);
int  	BSP_SemDel (void  *p_sem,uint16_t  *p_err);
int 	BSP_SemPend(void *p_sem,uint32_t timeout,uint16_t  *p_err);
void 	BSP_SemPost(void *p_sem,uint16_t  *p_err);

static void *BSP_newMutex(void) ;
static int BSP_freeMutex(void *mutex) ;
void  		*BSP_MutexCreate 		(uint16_t *p_err);
int  		BSP_MutexDel 			(void  *p_mutex,uint16_t  *p_err);
void 		BSP_MutexLock			(void *p_mutex,uint16_t  *p_err);
void 		BSP_MutexUnlock			(void *p_mutex,uint16_t  *p_err);

int BSP_InitTask(BSP_Task_t *task,fpointer func,void *arg);
int BSP_CreateTask(BSP_Task_t *task);
int BSP_DelTask(BSP_Task_t *task);

#ifdef __cplusplus
}
#endif

#endif//DMA_INIT_

