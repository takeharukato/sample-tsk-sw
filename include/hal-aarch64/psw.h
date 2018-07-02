/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Processor Status Word                                             */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_PSW_H)
#define  _HAL_PSW_H 

#include <kern/freestanding.h>

#include <kern/param.h>
#include <hal/aarch64.h>

typedef uint64_t psw_t;  /*< プロセサステータスワード  */

#if defined(CONFIG_HAL)

/** CPUへの割り込みを無条件に許可する
 */
#define psw_enable_interrupt()     do{				\
      __asm__ __volatile__ ("msr daifclr, %0\n\t" : : "i"(AARCH64_DAIF_IRQ)); \
    }while(0)
/** CPUへの割り込みを無条件に禁止する
 */
#define psw_disable_interrupt()     do{				\
	__asm__ __volatile__ ("msr daifset, %0\n\t" : : "i"(AARCH64_DAIF_IRQ)); \
    }while(0)


/** PSWを保存する
    @param[in] psw PSW保存用の変数
 */
#define __save_psw(psw)  do{				    \
     __asm__ __volatile__ ("mrs %0, daif\n\t":"=r"(psw));   \
    }while(0)

/** PSWを復元する
    @param[in] psw PSW保存用の変数
 */
#define __restore_psw(psw) do{                          \
       __asm__ __volatile__ ("msr daif, %0\n\t"         \
	   ::"r"(psw));					\
   }while(0)
#else  /*  CONFIG_HAL  */
#define psw_enable_interrupt()  do{}while(0)
#define psw_disable_interrupt() do{}while(0)
#define barrier() do{}while(0)
#define __save_psw(psw) do{}while(0)
#define __restore_psw(psw) do{}while(0)
#endif  /*  CONFIG_HAL  */

void psw_disable_and_save_interrupt(psw_t *);
void psw_restore_interrupt(psw_t *);
#endif  /*  _KERN_PSW_H   */
