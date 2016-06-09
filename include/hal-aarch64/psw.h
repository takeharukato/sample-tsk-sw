/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  Processor Status Word制御                                         */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_PSW_H)
#define  _HAL_PSW_H 
#include "param.h"

typedef uint64_t psw_t;  /*< プロセサステータスワード  */

#if defined(CONFIG_HAL)

/** CPUへの割り込みを無条件に許可する
 */
#define __psw_enable_interrupt()     do{                     \
		__asm__ __volatile__ ("msr daifclr, #2\n\t");	     \
    }while(0)
/** CPUへの割り込みを無条件に禁止する
 */
#define __psw_disable_interrupt()     do{                     \
		__asm__ __volatile__ ("msr daifset, #2\n\t");	      \
    }while(0)


/** PSWを保存する
    @param[in] psw PSW保存用の変数
 */
#define __save_psw(psw)  do{				\
       __asm__ __volatile__ ("mrs %0, daif\n\t"         \
	   :"=r"(psw));					\
    }while(0)

/** PSWを復元する
    @param[in] psw PSW保存用の変数
 */
#define __restore_psw(psw) do{                          \
       __asm__ __volatile__ ("msr daif, %0\n\t"         \
	   ::"r"(psw));					\
   }while(0)
#else  /*  CONFIG_HAL  */
#define __psw_enable_interrupt()  do{}while(0)
#define __psw_disable_interrupt() do{}while(0)
#define barrier() do{}while(0)
#define __save_psw(psw) do{}while(0)
#define __restore_psw(psw) do{}while(0)
#endif  /*  CONFIG_HAL  */

void psw_disable_interrupt(psw_t *);
void psw_restore_interrupt(psw_t *);
#endif  /*  _KERN_PSW_H   */
