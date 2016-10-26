#include "cmsis_os.h"
#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>

#define DEBUG_RT_CMSIS

#if defined(DEBUG_RT_CMSIS)
#define SVC_PERROR(expr) svc_perror(__FILE__, __LINE__, #expr, (expr))
#else
#define SVC_PERROR(expr) assert(expr)
#endif

#define Id2ID(id) ((ID)(id)) // Cast an RTX Id to TOPPERS ID

// ==== Message Queue Management Functions ====

extern "C" {

osMessageQId osMessageCreate (const osMessageQDef_t *queue_def, osThreadId thread_id) {
    assert(thread_id == NULL);   

    T_CDTQ cdtq;
    cdtq.dtqatr = TA_NULL;
    cdtq.dtqcnt = queue_def->queue_sz;
    cdtq.dtqmb  = queue_def->pool;
    assert(cdtq.dtqmb != NULL);

    ER_ID ercd = acre_dtq(&cdtq);
    if (ercd != E_OK) {
        SVC_PERROR(ercd);
        return NULL;
    }
    return (osMessageQId)ercd;
}

osStatus osMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec) {
    static_assert(sizeof(info) == sizeof(intptr_t), "message must have same size with data in DTQ");
    assert(millisec < (1UL << (sizeof(TMO) * 8 - 1)) || millisec == osWaitForever); // FIXME: TMO is not unsigned!

    TMO tmout = (millisec == osWaitForever) ? TMO_FEVR : millisec;
    ER  ercd  = tsnd_dtq(Id2ID(queue_id), info, tmout); // FIXME: interrupt context is not supported yet

    if (ercd == E_TMOUT) return osErrorTimeoutResource;
    SVC_PERROR(ercd);
    return osOK;
}

os_InRegs osEvent osMessageGet (osMessageQId queue_id, uint32_t millisec) {
    osEvent ret;
    static_assert(sizeof(ret.value.v) == sizeof(intptr_t), "message must have same size with data in DTQ");
    assert(millisec < (1UL << (sizeof(TMO) * 8 - 1)) || millisec == osWaitForever); // FIXME: TMO is not unsigned!

    TMO tmout = (millisec == osWaitForever) ? TMO_FEVR : millisec;
    ER  ercd  = trcv_dtq(Id2ID(queue_id), reinterpret_cast<intptr_t*>(&(ret.value.v)), tmout); // FIXME: interrupt context is not supported yet

    if (ercd == E_TMOUT) {
        ret.status = millisec ? osEventTimeout : osOK;
    } else {
        SVC_PERROR(ercd);
        ret.status = osEventMessage;
    }

    return ret;
}

}
