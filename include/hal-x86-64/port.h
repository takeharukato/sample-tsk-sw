/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Port I/O                                                          */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_PORT_H)
#define  _HAL_PORT_H 

/*
 *  Port I/O without wait
 */
static inline void 
out_port_byte(unsigned short port, unsigned char data) {

	__asm__ __volatile__ ("outb %b1, %w0"
	    :/* No output */
	    : "Nd" (port), "a" (data));
}
#endif  /*  _HAL_PORT_H   */
