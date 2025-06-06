#ifndef _DOLPHIN_DSP_H_
#define _DOLPHIN_DSP_H_

#include <dolphin/os.h>

typedef void (*DSPCallback)(void* task);

typedef struct STRUCT_DSP_TASK {
	/*0x00*/ volatile u32 state;
	/*0x04*/ volatile u32 priority;
	/*0x08*/ volatile u32 flags;
	/*0x0C*/ u16* iram_mmem_addr;
	/*0x10*/ u32 iram_length;
	/*0x14*/ u32 iram_addr;
	/*0x18*/ u16* dram_mmem_addr;
	/*0x1C*/ u32 dram_length;
	/*0x20*/ u32 dram_addr;
	/*0x24*/ u16 dsp_init_vector;
	/*0x26*/ u16 dsp_resume_vector;
	/*0x28*/ DSPCallback init_cb;
	/*0x2C*/ DSPCallback res_cb;
	/*0x30*/ DSPCallback done_cb;
	/*0x34*/ DSPCallback req_cb;
	/*0x38*/ struct STRUCT_DSP_TASK* next;
	/*0x3C*/ struct STRUCT_DSP_TASK* prev;
	OSTime t_context;
	OSTime t_task;
} DSPTaskInfo;

#ifdef __cplusplus
extern "C" {
#endif

u32 DSPCheckMailToDSP(void);
u32 DSPCheckMailFromDSP(void);
u32 DSPReadCPUToDSPMbox(void);
u32 DSPReadMailFromDSP(void);
void DSPSendMailToDSP(u32 mail);
void DSPAssertInt(void);
void DSPInit(void);
BOOL DSPCheckInit(void);
void DSPReset(void);
void DSPHalt(void);
void DSPUnhalt(void);
u32 DSPGetDMAStatus(void);
DSPTaskInfo* DSPAddTask(DSPTaskInfo* task);
DSPTaskInfo* DSPCancelTask(DSPTaskInfo* task);
DSPTaskInfo* DSPAssertTask(DSPTaskInfo* task);

DSPTaskInfo* __DSPGetCurrentTask(void);
void __DSP_boot_task(DSPTaskInfo* task);
void __DSP_insert_task(DSPTaskInfo* task);
void __DSP_exec_task(DSPTaskInfo* curr, DSPTaskInfo* next);
void __DSP_remove_task(DSPTaskInfo* task);

#ifdef __cplusplus
}
#endif

#endif
