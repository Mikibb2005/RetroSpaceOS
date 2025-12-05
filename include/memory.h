#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

void kheap_init();
void *kmalloc(size_t size);
void kfree(void *ptr);
void *kmemset(void *ptr, int value, size_t num);
void *kmemcpy(void *dest, const void *src, size_t n);

#endif
