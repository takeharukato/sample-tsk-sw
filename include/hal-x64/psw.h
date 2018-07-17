/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Processor Status Word制御                                         */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_PSW_H)
#define  _HAL_PSW_H 

#include <kern/freestanding.h>

#include <kern/param.h>

typedef uint64_t psw_t;  /*< プロセサステータスワード  */

#if defined(CONFIG_HAL)

/** CPUへの割り込みを無条件に許可する
 */
#define psw_enable_interrupt()     do{                     \
		__asm__ __volatile__ ("sti\n\t");	     \
    }while(0)
/** CPUへの割り込みを無条件に禁止する
 */
#define psw_disable_interrupt()     do{                     \
		__asm__ __volatile__ ("cli\n\t");	      \
    }while(0)

/** アウトオブオーダ実行している命令を待ち合わせる
 */
#define barrier() do{							\
		__asm__ __volatile__ ("" :::"memory");			\
	}while(0)

/** PSWを保存する
    @param[in] psw PSW保存用の変数
 */
#define __save_psw(psw)  do{				\
       __asm__ __volatile__ ("pushfq\n\t"               \
                             "popq %0\n\t"              \
                             "cli\n\t"                  \
	   :"=m"(psw));					\
       barrier();                                       \
    }while(0)

/** PSWを復元する
    @param[in] psw PSW保存用の変数
 */
#define __restore_psw(psw) do{                          \
       barrier();                                       \
       __asm__ __volatile__ ("pushq %0\n\t"             \
			     "popfq\n\t"                \
	   ::"m"(psw));					\
   }while(0)
/*
 * Note: Strictly speaking, there is a race condition between sti and hlt.
 *       But this might not cause serious problem in typical cases.
 */
#define hal_suspend_cpu() do{			\
		__asm__ __volatile__(		\
			"sti\n\t"		\
			"hlt\n\t"		\
			: : :"memory");		\
}while(0)
#else  /*  CONFIG_HAL  */
#define psw_enable_interrupt()  do{}while(0)
#define psw_disable_interrupt() do{}while(0)
#define barrier() do{}while(0)
#define __save_psw(psw) do{}while(0)
#define __restore_psw(psw) do{}while(0)
#define hal_suspend_cpu() do{}while(0)
#endif  /*  CONFIG_HAL  */

void psw_disable_and_save_interrupt(psw_t *);
void psw_restore_interrupt(psw_t *);
#endif  /*  _KERN_PSW_H   */
