/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  IRQ Management                                                    */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

static irq_manage irqMgr;

/** Initialize IRQ manager
 */
int
irq_initialize_manager(void) {
	irq_no                    i;
	irq_manage *mgr = (&irqMgr);
	irq_line             *linep;
	psw_t                   psw;

	psw_disable_interrupt(&psw);

	mgr->find_pending = NULL;
	for(i = 0; NR_IRQS > i; ++i) {
		
		linep = &mgr->irqs[i];
		linep->nr = i;
		linep->attr = (IRQ_ATTR_SHARED | IRQ_ATTR_LEVEL);
		linep->prio = 0;
		init_list_head(&linep->handlers);
		linep->ctrlrp = NULL;
	}
	psw_restore_interrupt(&psw);

	return 0;
}

/** Register a controller
    @param[in] irq IRQ number
    @param[in] ctrlrp a pointer to controler data
 */
int
irq_register_ctrlr(irq_no irq, irq_ctrlr *ctrlrp){
	irq_line             *linep;
	psw_t                   psw;
	irq_manage *mgr = (&irqMgr);

	if ( ( irq < 0 ) || ( irq >= NR_IRQS ) )
		return EINVAL;

	psw_disable_interrupt(&psw);

	(&mgr->irqs[irq])->ctrlrp = ctrlrp;
	
	psw_restore_interrupt(&psw);

	return 0;
}

/** unregister a controller
    @param[in] irq IRQ number
 */
int
irq_unregister_ctrlr(irq_no irq){
	irq_line             *linep;
	psw_t                   psw;
	irq_manage *mgr = (&irqMgr);

	if ( ( irq < 0 ) || ( irq >= NR_IRQS ) )
		return EINVAL;

	psw_disable_interrupt(&psw);

	(&mgr->irqs[irq])->ctrlrp = NULL;
	
	psw_restore_interrupt(&psw);

	return 0;
}

/** Register an IRQ handler
    @param[in] irq     IRQ number
    @param[in] attr    IRQ attribution
    @param[in] prio    IRQ priority
    @param[in] private Hanlder private data
    @param[in] handler IRQ handler
 */
int 
irq_register_handler(irq_no irq, irq_attr attr, irq_prio prio, void *private, int (*handler)(irq_no _irq, struct _exception_frame *_exc, void *_private)){
	int                       rc;
	irq_attr        handler_attr;
	irq_line              *linep;
	irq_handler_ent *handler_ent;
	irq_manage  *mgr = (&irqMgr);
	psw_t                    psw;

	if ( ( irq < 0 ) || ( irq >= NR_IRQS ) )
		return EINVAL;

	handler_attr &= IRQ_ATTR_HANDLER_MASK;

	psw_disable_interrupt(&psw);

	linep = &mgr->irqs[irq];

	if ( !( handler_attr & IRQ_ATTR_NON_NESTABLE ) &&
	     ( linep->attr & IRQ_ATTR_NON_NESTABLE ) ) {

		rc = EINVAL;
		goto restore_irq_out;
	}

	if ( ( handler_attr & IRQ_ATTR_EXCLUSIVE ) &&
	     ( !list_is_empty(&linep->handlers) ) ) {

		rc = EBUSY;
		goto restore_irq_out;
	}

	if ( ( linep->ctrlrp == NULL ) || ( linep->ctrlrp->config_irq == NULL ) ){

		rc = ENOENT;
		goto restore_irq_out;
	}

	handler_ent = kmalloc(sizeof(irq_handler_ent));
	if ( handler_ent == NULL ) {

		rc = ENOMEM;
		goto restore_irq_out;
	}

	init_list_node(&handler_ent->link);
	handler_ent->handler = handler;
	handler_ent->private = private;
	linep->attr = attr;
	linep->prio = prio;
	
	rc = linep->ctrlrp->config_irq(linep->ctrlrp, irq, attr, prio);
	if ( rc != 0 ) 
		goto free_mem_out;

	if ( list_is_empty(&linep->handlers) )
		linep->ctrlrp->enable_irq(linep->ctrlrp, irq);

	list_add(&linep->handlers, &handler_ent->link);

       	psw_restore_interrupt(&psw);

	return 0;

free_mem_out:
	kfree(handler_ent);

restore_irq_out:
	psw_restore_interrupt(&psw);
	return rc;
}

/** Unregister an IRQ handler
    @param[in] irq     IRQ number
    @param[in] handler IRQ handler
 */
int 
irq_unregister_handler(irq_no irq, int (*handler)(irq_no _irq, struct _exception_frame *_exc, void *_private)){
	int                       rc;
	irq_line              *linep;
	list_t                   *lp;
	irq_handler_ent        *entp;
	irq_manage  *mgr = (&irqMgr);
	psw_t                    psw;

	if ( ( irq < 0 ) || ( irq >= NR_IRQS ) )
		return EINVAL;

	psw_disable_interrupt(&psw);

	linep = &mgr->irqs[irq];
	
	list_for_each(lp, linep, handlers){
		
		entp = CONTAINER_OF(lp, irq_handler_ent, link);
		if ( entp->handler == handler ) {

			list_del(lp);
			kfree(entp);
			rc = 0;
			goto restore_irq_out;
		}
	}

	rc = ENOENT;

restore_irq_out:
	psw_restore_interrupt(&psw);

	return rc;
}

/** Register a handler used for finding a pending interrupt
    @param[in] find_pending a handler used for finding a pending interrupt
 */
void
irq_register_pending_irq_finder(int (*find_pending)(struct _exception_frame *_exc, irq_no *_irqp)){
	psw_t                    psw;
	irq_manage  *mgr = (&irqMgr);

	psw_disable_interrupt(&psw);
	mgr->find_pending = find_pending;
	psw_restore_interrupt(&psw);
	
	return ;
}

/** Unregister a handler used for finding a pending interrupt
 */
void 
irq_unregister_pending_irq_finder(void){
	psw_t                    psw;
	irq_manage  *mgr = (&irqMgr);

	psw_disable_interrupt(&psw);
	mgr->find_pending = NULL;
	psw_restore_interrupt(&psw);
	
	return;
}

/** Handle an IRQ 
    @param[in] exc an exception frame
 */
int
irq_handle_irq(struct _exception_frame *exc) {
	int                       rc;
	int               is_handled;
	irq_no                   irq;
	irq_manage  *mgr = (&irqMgr);
	irq_line              *linep;
	list_t                   *lp;
	irq_handler_ent        *entp;
	psw_t                    psw;

	psw_disable_interrupt(&psw);
	
	if ( mgr->find_pending == NULL ) {

		/* FIXME: We should fall into panic in this case. */
		rc = ENOENT;
		goto restore_irq_out;
	}

	rc = mgr->find_pending(exc, &irq);
	if ( rc != IRQ_FOUND )  {

		rc = ESRCH;   /* Spurious interrupt */
		goto restore_irq_out;
	}
	
	linep = &mgr->irqs[irq];

	if ( ( linep->ctrlrp == NULL ) 
	     || ( linep->ctrlrp->disable_irq == NULL ) 
	     || ( linep->ctrlrp->enable_irq == NULL ) 
	     || ( linep->ctrlrp->eoi == NULL ) ) {

		/* FIXME: We should fall into panic in this case. */
		rc = ENOENT;
		goto restore_irq_out;
	}

	linep->ctrlrp->disable_irq(linep->ctrlrp, irq); /* Mask this irq line */
	linep->ctrlrp->eoi(linep->ctrlrp, irq); /* Send EOI for this irq line */

	list_for_each(lp, linep, handlers){
		
		entp = CONTAINER_OF(lp, irq_handler_ent, link);
		if ( entp->handler != NULL ) {

			if ( !( linep->attr & IRQ_ATTR_NON_NESTABLE) ) 
				__psw_enable_interrupt();

			is_handled = entp->handler(irq, exc, entp->private);
			__psw_disable_interrupt();

			if ( is_handled == IRQ_HANDLED ) {

				rc = 0;
				goto handled_out;
			}
		}
	}

	rc = ESRCH;   /* Spurious interrupt */

handled_out:
	linep->ctrlrp->enable_irq(linep->ctrlrp, irq); /* unmask this irq line */

restore_irq_out:
	psw_restore_interrupt(&psw);

	return rc;
}
