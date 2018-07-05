/* -*- mode: C; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Segment descriptor in Intel chips                                 */
/*                                                                    */
/**********************************************************************/
#if !defined(__HAL_X64_H)
#define __HAL_X64_H

#define X64_RFLAGS_CF   (0) /*< キャリーフラグ */
#define X64_RFLAGS_PF   (2) /*< パリティフラグ (演算結果の下位8ビット) */
#define X64_RFLAGS_AF   (4) /*< 補助キャリーフラグ(BCD演算用) */
#define X64_RFLAGS_ZF   (6) /*< ゼロフラグ */
#define X64_RFLAGS_SF   (7) /*< サインフラグ */
#define X64_RFLAGS_TF   (8) /*< トラップフラグ、トレースフラグ */
#define X64_RFLAGS_IF   (9) /*< 割り込みフラグ */
#define X64_RFLAGS_DF  (10) /*< ディレクションフラグ */
#define X64_RFLAGS_OF  (11) /*< オーバーフローフラグ */
#define X64_RFLAGS_IO1 (12) /*< IOPL1 */
#define X64_RFLAGS_IO2 (13) /*< IOPL2 */
#define X64_RFLAGS_NT  (14) /*< ネストタスクフラグ */
#define X64_RFLAGS_RF  (16) /*< デバッグレジスターの命令ブレイクポイントを(1回のみ)無効にする */
#define X64_RFLAGS_VM  (17) /*< 仮想86モード */
#define X64_RFLAGS_AC  (18) /*< 変更可能であれば、i486、Pentium以降のCPUである */
#define X64_RFLAGS_VIF (19) /*< 仮想割り込みフラグ (Pentium以降) */
#define X64_RFLAGS_VIP (20) /*< 仮想割り込みペンディングフラグ (Pentium以降) */
#define X64_RFLAGS_ID  (21) /*< 変更可能であれば、CPUID命令に対応している */
#define X64_RFLAGS_NR  (22) /*< 最終位置  */
#define X64_RFLAGS_RESV ( ( 1 << 1) | ( 1 << 3) | ( 1 << 5 ) )

#define MISC_ENABLE             (0x000001A0)
#define EFER                    (0xC0000080)

#define CR0_PAGING              (1 << 31)
#define CR0_CACHE_DISABLE       (1 << 30)
#define CR0_NOT_WRITE_THROUGH   (1 << 29)
#define CR0_ALIGNMENT_MASK      (1 << 18)
#define CR0_WRITE_PROTECT       (1 << 16)
#define CR0_NUMERIC_ERROR       (1 << 5)
#define CR0_EXTENSION_TYPE      (1 << 4)
#define CR0_TASK_SWITCHED       (1 << 3)
#define CR0_FPU_EMULATION       (1 << 2)
#define CR0_MONITOR_FPU         (1 << 1)
#define CR0_PROTECTION          (1 << 0)

#define CR4_VME                 (1 << 0)
#define CR4_PMODE_VIRTUAL_INT   (1 << 1)
#define CR4_TIMESTAMP_RESTRICT  (1 << 2)
#define CR4_DEBUGGING_EXT       (1 << 3)
#define CR4_PSE                 (1 << 4)
#define CR4_PAE                 (1 << 5)
#define CR4_MCE                 (1 << 6)
#define CR4_GLOBAL_PAGES        (1 << 7)
#define CR4_PERF_COUNTER        (1 << 8)
#define CR4_OS_FXSR             (1 << 9)
#define CR4_OS_XMMEXCEPT        (1 << 10)
#define CR4_VMX_ENABLE          (1 << 13)
#define CR4_SMX_ENABLE          (1 << 14)
#define CR4_PCID_ENABLE         (1 << 17)
#define CR4_OS_XSAVE            (1 << 18)

#define PAGE_SHIFT               (CONFIG_HAL_PAGE_SHIFT)               /*  4KiB  */
#define KERN_STRAIGHT_PAGE_SIZE  (CONFIG_HAL_STRAIGHT_PAGE_SIZE)         /*  2MiB  */

#if !defined(ASM_FILE)
#include <kern/freestanding.h>

static inline void
wrmsr(uint32_t msr_id, uint64_t msr_value){

	asm volatile ( "wrmsr" : : "c" (msr_id), "A" (msr_value) );
}

static inline uint64_t 
rdmsr(uint32_t msr_id) {
	uint64_t msr_value;

	asm volatile ( "rdmsr" : "=A" (msr_value) : "c" (msr_id) );

	return msr_value;
}

#endif  /*  !ASM_FILE  */

#endif  /*  __HAL_X64_H  */
