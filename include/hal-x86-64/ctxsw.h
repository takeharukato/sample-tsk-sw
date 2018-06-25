/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  context switch                                                    */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_CTXSW_H)
#define _HAL_CTXSW_H

struct _thread;

/** X86-64のスタック切り替え
    @param[in] prev 切り替えられるスレッドのスタックアドレスを格納しているポインタ変数のアドレス
    @param[in] next 切り替えるスレッドのスタックアドレスを格納しているポインタ変数のアドレス
    @note PSW(Processor Status Word - X86-64の場合, RFLAGSとRIPの組)と
          AMD64 ABI Draft 0.99.5(http://www.x86-64.org/documentation/abi.pdf)で規定された
          callee savedレジスタのうち x87 FPU 制御ワード以外のレジスタを退避/復元する
	  x86-64のcallee savedレジスタ(AMD64 ABI Draft 0.99.5 Figure 3.4: Register Usage参照)
 */
static inline void
hal_do_context_switch(void **prev, void **next) {

	__asm__ __volatile__(			
	"pushfq\n\t"	
	"pushq %%rbx\n\t"
	"pushq %%rbp\n\t"
	"pushq %%r12\n\t"
	"pushq %%r13\n\t"
	"pushq %%r14\n\t"
	"pushq %%r15\n\t"
	"movabsq $1f, %%r11\n\t"
	"pushq %%r11\n\t"
	"movq %%rsp, (%%rcx)\n\t"
	"movq (%%rdx), %%rsp\n\t"
	"ret\n\t"
	"1:\n\t"
	"popq %%r15\n\t"
	"popq %%r14\n\t"
	"popq %%r13\n\t"
	"popq %%r12\n\t"
	"popq %%rbp\n\t"
	"popq %%rbx\n\t"
	"popfq\n\t"
	: /* no output */
	: "c"(prev), "d"(next) 
	: "r11" );
}
void __start_thread(void);
void hal_setup_thread_function(struct _thread *, void (*fn)(void *), void *);
#endif  /*  _HAL_CTXSW_H  */
