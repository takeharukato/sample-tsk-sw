/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  IRQ Management                                                    */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_IRQ_H)
#define  _KERN_IRQ_H 

#include <kern/freestanding.h>

#include <kern/kern_types.h>
#include <kern/list.h>

#include <hal/exception.h>

#define IRQ_ATTR_EXCLUSIVE     (0x1)
#define IRQ_ATTR_SHARED        (0x0)
#define IRQ_ATTR_NESTABLE      (0x0)
#define IRQ_ATTR_NON_NESTABLE  (0x2)
#define IRQ_ATTR_HANDLER_MASK  (0xf)
#define IRQ_ATTR_LEVEL         (0x00)
#define IRQ_ATTR_EDGE          (0x10)

#define IRQ_FOUND          (0)
#define IRQ_NOT_FOUND      (1)

#define IRQ_HANDLED        (0)
#define IRQ_NOT_HANDLED    (1)

#if defined(CONFIG_IRQ_NR)
#define NR_IRQS         (CONFIG_NR_IRQS)
#else
#define NR_IRQS         (256)
#endif  /*  CONFIG_IRQ_MAX_NR  */

typedef struct _irq_handler_ent{
	list_t link;
	int (*handler)(irq_no _irq, struct _exception_frame *_exc, void *private);
	void *private;
}irq_handler_ent;

typedef struct _irq_ctrlr{
	int (*config_irq)(struct _irq_ctrlr *_ctrlr, irq_no _irq, irq_attr _attr, irq_prio _prio);
	void (*enable_irq)(struct _irq_ctrlr *_ctrlr, irq_no _irq);
	void (*disable_irq)(struct _irq_ctrlr *_ctrlr, irq_no _irq);
	void (*eoi)(struct _irq_ctrlr *_ctrlr, irq_no _irq);
	int (*initialize)(struct _irq_ctrlr *_ctrlr);
	void (*finalize)(struct _irq_ctrlr *_ctrlr);
	void *private;
}irq_ctrlr;

typedef struct _irq_line{
	irq_no            nr;
	irq_attr        attr;
	irq_prio        prio;
	list_head_t handlers;
	irq_ctrlr    *ctrlrp;
}irq_line;

typedef struct _irq_finder{
	struct _list                                                 link;
	int (*find_pending)(struct _exception_frame *_exc, irq_no *_irqp);
}irq_finder;

typedef struct _irq_manage{
	list_head_t       find_irqs;
	irq_line      irqs[NR_IRQS];
}irq_manage;

int irq_initialize_manager(void);
int irq_register_ctrlr(irq_no _irq, irq_ctrlr *_ctrlrp);
int irq_unregister_ctrlr(irq_no _irq);
int irq_register_handler(irq_no _irq, irq_attr _attr, irq_prio _prio, void *_private, int (*_handler)(irq_no _irq, struct _exception_frame *_exc, void *_private));
int irq_unregister_handler(irq_no irq, int (*handler)(irq_no _irq, struct _exception_frame *_exc, void *_private));
void irq_register_pending_irq_finder(struct _irq_finder *_ent);
void irq_unregister_pending_irq_finder(struct _irq_finder *_ent);
int irq_handle_irq(struct _exception_frame *_exc);
#endif  /*  _KERN_IRQ_H   */
