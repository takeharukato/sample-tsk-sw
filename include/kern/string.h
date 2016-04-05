/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  id bitmap operations                                              */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_STRING_H)
#define _KERN_STRING_H
#include <errno.h>
#include "kern/kern_types.h"

void *memset(void *, int , size_t );
void *memcpy(void *, const void *, size_t );
void *memmove(void *, const void *, size_t );
size_t strlen(const char *);
#endif  /*  _KERN_STRING_H  */
