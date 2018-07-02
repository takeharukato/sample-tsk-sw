/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Parameters for kmalloc                                            */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_MALLOC_PARAMS_H)
#define  _KERN_MALLOC_PARAMS_H 

#include <kern/heap.h>

#define MSPACES         (0) 
#define ONLY_MSPACES    (0)
#define USE_LOCKS       (2)
#define USE_SPIN_LOCKS  (0)
#define USE_DL_PREFIX   (1)
#define ABORT          do{}while(1)
#define MALLOC_FAILURE_ACTION do{}while(0)
#define HAVE_MORECORE   (1)
#define MORECORE        kheap_sbrk
#define HAVE_MMAP       (0)
#define HAVE_MREMAP     (0)
#define MMAP_CLEARS     (0)
#define USE_BUILTIN_FFS (0)
#define malloc_getpagesize (4096)
#define USE_DEV_RANDOM  (0)
#define NO_MALLINFO     (0)
#define NO_MALLOC_STATS (1)  /* Avoid using printf family */
#define LACKS_TIME_H    (1)  /* Avoid using time(3) */
#define USE_KMALLOC_PREFIX (1)

#if defined (USE_KMALLOC_PREFIX)
#define dlcalloc               kcalloc
#define dlfree                 kfree
#define dlmalloc               kmalloc
#define dlmemalign             kmemalign
#define dlposix_memalign       kposix_memalign
#define dlrealloc              krealloc
#define dlrealloc_in_place     krealloc_in_place
#define dlvalloc               kvalloc
#define dlpvalloc              kpvalloc
#define dlmallinfo             kmallinfo
#define dlmallopt              kmallopt
#define dlmalloc_trim          kmalloc_trim
#define dlmalloc_stats         kmalloc_stats
#define dlmalloc_usable_size   kmalloc_usable_size
#define dlmalloc_footprint     kmalloc_footprint
#define dlmalloc_max_footprint kmalloc_max_footprint
#define dlmalloc_footprint_limit kmalloc_footprint_limit
#define dlmalloc_set_footprint_limit kmalloc_set_footprint_limit
#define dlmalloc_inspect_all   kmalloc_inspect_all
#define dlindependent_calloc   kindependent_calloc
#define dlindependent_comalloc kindependent_comalloc
#define dlbulk_free            kbulk_free
#endif /* USE_KMALLOC_PREFIX */
#if USE_LOCKS > 1
#define MLOCK_T           mutex
#define INITIAL_LOCK(lk)  kmalloc_initialize_lock(lk)
#define DESTROY_LOCK(lk)  kmalloc_destroy_lock(lk)
#define ACQUIRE_LOCK(lk)  kmalloc_acquire_lock(lk)
#define RELEASE_LOCK(lk)  kmalloc_release_lock(lk)
#define TRY_LOCK(lk)      kmalloc_try_lock(lk)

#endif
#endif  /*  _KERN_KMALLOC_PARAMS_H   */
