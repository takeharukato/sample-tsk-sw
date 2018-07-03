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

#include <kern/freestanding.h>

/*
 *  Port I/O without wait
 */
static inline void 
out_port_byte(uint16_t port, uint8_t data) {

	__asm__ __volatile__ ("outb %b1, %w0\n\t"
	    :/* No output */
	    : "Nd" (port), "a" (data));

	return;
}

static inline void
out_port_word (uint16_t port, uint16_t data) {

	__asm__ __volatile__ ("outw %w1, %w0\n\t"
	    :/* No output */
	    : "Nb" (port), "a" (data));

	return;
}

static inline void 
out_port_dword (uint16_t port, unsigned long data) {

	__asm__  __volatile__ ("outl %1, %w0\n\t"
	    : /* No output */
	    : "Nb" (port), "a" (data));

	return;
}

static inline uint8_t
in_port_byte(uint16_t port) {
	uint8_t ret;

	__asm__ __volatile__ ("inb %w1, %b0\n\t"
	    : "=a" (ret)
	    : "Nd" (port));

	return ret;
}

static inline uint16_t
in_port_word(uint16_t port) {
	uint16_t ret;

	__asm__ __volatile__ ("inw %w1, %b0\n\t"
	    : "=a" (ret)
	    : "Nd" (port));

	return ret;
}

static inline unsigned long
in_port_dword (uint16_t port) {
	unsigned long ret;

	__asm__ __volatile__ ("inl %w1, %w0\n\t"
	    : "=a" (ret)
	    : "Nd" (port));

	return ret;
}
#endif  /*  _HAL_PORT_H   */
