#pragma once

#define osMessageCreate(...) 0
#define osWaitForever 0xFFFFFFFF
#define osPriorityNormal 0
#define DEFAULT_STACK_SIZE (4096)
typedef int osStatus;
typedef int osPriority;
//typedef int osEvent;
typedef int osSemaphoreId;
typedef int osSemaphoreDef_t;
typedef int osMutexId;
typedef int osMutexDef_t;
typedef int osThreadId;
typedef int osThreadDef_t;
typedef int osTimerId;
typedef int osTimerDef_t;
typedef int osMailQId;
typedef int osMailQDef_t;
typedef int osPoolId;
typedef int osPoolDef_t;
typedef int osMessageQId;
typedef int osMessageQDef_t;

#define os_InRegs //__value_in_regs

enum os_timer_type {
    osTimerPeriodic,
};

enum {
    osOK,
    osEventMail,
};
typedef struct  {
  osStatus                 status;     ///< status code: event or error information
  union  {
    uint32_t                    v;     ///< message as 32-bit value
    void                       *p;     ///< message or mail as void pointer
    int32_t               signals;     ///< signal flags
  } value;                             ///< event value
  union  {
    osMailQId             mail_id;     ///< mail id obtained by \ref osMailCreate
    osMessageQId       message_id;     ///< message id obtained by \ref osMessageCreate
  } def;                               ///< event definition
} osEvent;


extern "C"
{
osMutexId osMutexCreate (const osMutexDef_t *mutex_def);
osStatus osMutexDelete (osMutexId mutex_id);
osStatus osMutexWait (osMutexId mutex_id, uint32_t millisec);
osStatus osMutexRelease (osMutexId mutex_id);

os_InRegs osEvent osMessageGet (osMessageQId queue_id, uint32_t millisec);
osStatus osMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec);

osStatus osMailPut (osMailQId queue_id, void *mail);
void *osMailAlloc (osMailQId queue_id, uint32_t millisec);
osMailQId osMailCreate (const osMailQDef_t *queue_def, osThreadId thread_id);
osStatus osMailFree (osMailQId queue_id, void *mail);
os_InRegs osEvent osMailGet (osMailQId queue_id, uint32_t millisec);

osSemaphoreId osSemaphoreCreate (const osSemaphoreDef_t *semaphore_def, int32_t count);
osStatus osSemaphoreDelete (osSemaphoreId semaphore_id);
int32_t osSemaphoreWait (osSemaphoreId semaphore_id, uint32_t millisec);
osStatus osSemaphoreRelease (osSemaphoreId semaphore_id);
}
