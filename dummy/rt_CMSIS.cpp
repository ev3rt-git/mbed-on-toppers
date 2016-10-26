#include "cmsis_os.h"
#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>

#include <type_traits>

#define DEBUG_RT_CMSIS

#if defined(DEBUG_RT_CMSIS)
#define SVC_PERROR(expr) svc_perror(__FILE__, __LINE__, #expr, (expr))
#else
#define SVC_PERROR(expr) assert(expr)
#endif

// Cast a CMSIS type to TOPPERS type
#define C2T_ID(id) ((ID)(id)) 
//#define C2T_PRI(pri) (TMIN_TPRI-(pri)+3) // TODO: how to decide this mapping?
#define C2T_PRI(pri) (TMIN_TPRI-(pri)+6) // TODO: how to decide this mapping?

#if TKERNEL_PRVER == UINT_C(0x2021)
#define dly_tsk_ms  dly_tsk
#define tsnd_dtq_ms tsnd_dtq
#define trcv_dtq_ms trcv_dtq
#else
#error "Only HRP2 kernel is supported."
#endif

extern "C" {

// ==== Thread Management ====

/// Create a thread and add it to Active Threads and set it to state READY
osThreadId osThreadCreate (const osThreadDef_t *thread_def, void *argument) {
    assert(thread_def->instances == 1);
    assert(thread_def->tpriority >= osPriorityIdle && thread_def->tpriority <= osPriorityRealtime);

    T_CTSK ctsk;
    static_assert(sizeof(ctsk.exinf) == sizeof(argument), "thread argument must have same size with exinf in TSK");
    static_assert(std::is_same<decltype(thread_def->pthread(NULL)), decltype(ctsk.task(0))>::value, "thread return value must be void");
    ctsk.tskatr  = TA_ACT; 
	ctsk.tskatr |= TA_DOM(TDOM_KERNEL); // FIXME: only system tasks are supported
    ctsk.exinf   = (intptr_t)argument;
    ctsk.task    = (TASK)thread_def->pthread;
    ctsk.itskpri = C2T_PRI(thread_def->tpriority);
    ctsk.stksz   = thread_def->stacksize ? thread_def->stacksize : DEFAULT_STACK_SIZE;
    ctsk.stk     = (STK_T*)thread_def->stack_pointer;
    ctsk.sstksz  = 0; // 4096/*DEFAULT_SSTKSZ*/; // FIXME: we are system tasks, we do not need sstk (hard coded, should we use DEFAULT_SSTKSZ?, or use a extern symbol to get this value
    ctsk.sstk    = NULL; // TODO: should we pass NULL to kernel?

    ER_ID ercd = acre_tsk(&ctsk);
    if (ercd <= 0) {
        SVC_PERROR(ercd);
        return NULL;
    }
    return (osThreadId)ercd;
}

/// Terminate execution of a thread and remove it from ActiveThreads
osStatus osThreadTerminate (osThreadId thread_id) {
    ER ercd;

    ID cur_tid;
    ercd = get_tid(&cur_tid);
    if (ercd == E_CTX) return osErrorISR;
    SVC_PERROR(ercd);
    if (cur_tid == C2T_ID(thread_id)) ext_tsk();

    ercd = ter_tsk(C2T_ID(thread_id));
    SVC_PERROR(ercd);
    return osOK;
}

// ==== Generic Wait Functions ====

/// Wait for Timeout (Time Delay)
osStatus osDelay (uint32_t millisec) {
    static_assert(sizeof(millisec) <= sizeof(RELTIM), "millisec must be in range of RELTIM");
    assert(millisec >= 1);

    ER ercd = dly_tsk_ms(millisec);
    if (ercd == E_CTX) return osErrorISR; // Not allowed in ISR
    SVC_PERROR(ercd);
    return osOK;
}

// ==== Message Queue Management Functions ====

osMessageQId osMessageCreate (const osMessageQDef_t *queue_def, osThreadId thread_id) {
    assert(thread_id == NULL);   

    T_CDTQ cdtq;
    cdtq.dtqatr = TA_NULL;
    cdtq.dtqcnt = queue_def->queue_sz;
    cdtq.dtqmb  = queue_def->pool;
    assert(cdtq.dtqmb != NULL);

    ER_ID ercd = acre_dtq(&cdtq);
    if (ercd <= 0) {
        SVC_PERROR(ercd);
        return NULL;
    }
    return (osMessageQId)ercd;
}

osStatus osMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec) {
    static_assert(sizeof(info) == sizeof(intptr_t), "message must have same size with data in DTQ");
    assert(millisec < (1UL << (sizeof(TMO) * 8 - 1)) || millisec == osWaitForever); // FIXME: TMO is not unsigned!, TMO in Gen3 kernel is microseconds

    TMO tmout = (millisec == osWaitForever) ? TMO_FEVR : millisec;
    ER  ercd  = tsnd_dtq_ms(C2T_ID(queue_id), info, tmout); // FIXME: interrupt context is not supported yet

    if (ercd == E_TMOUT) return osErrorTimeoutResource;
    SVC_PERROR(ercd);
    return osOK;
}

os_InRegs osEvent osMessageGet (osMessageQId queue_id, uint32_t millisec) {
    osEvent ret;
    static_assert(sizeof(ret.value.v) == sizeof(intptr_t), "message must have same size with data in DTQ");
    assert(millisec < (1UL << (sizeof(TMO) * 8 - 1)) || millisec == osWaitForever); // FIXME: TMO is not unsigned!, TMO in Gen3 kernel is microseconds

    TMO tmout = (millisec == osWaitForever) ? TMO_FEVR : millisec;
    ER  ercd  = trcv_dtq_ms(C2T_ID(queue_id), (intptr_t*)(&(ret.value.v)), tmout); // FIXME: interrupt context is not supported yet

    if (ercd == E_TMOUT) {
        ret.status = millisec ? osEventTimeout : osOK;
    } else {
        SVC_PERROR(ercd);
        ret.status = osEventMessage;
    }

    return ret;
}

}
