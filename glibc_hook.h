#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>

#ifndef GLIBC_HOOK_H_

void *malloc(size_t size);
void free(void* ptr);
void glibc_hook_init() __attribute__((constructor));

#endif  // GLIBC_HOOK_H_
