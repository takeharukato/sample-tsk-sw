/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  malloc interface                                                  */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_MALLOC_H)
#define  _MALLOC_H 

#include <stddef.h>
#include <sys/types.h>

#include "kern/heap.h"
#include "kern/chunk.h"

void *kmalloc(size_t );
void kfree(void *);
#endif  /*  _KERN_MALLOC_H  */
