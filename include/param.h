#if !defined(_PARAM_H)
#define _PARAM_H
#include "autoconf.h"
#define HEAP_SIZE    ((CONFIG_HEAP_SIZE)*1024UL*1024UL)
#define STACK_SIZE      ((CONFIG_KSTACK_PAGE_NR)*4UL*1024UL)
#endif  /*   _PARAM_H  */
