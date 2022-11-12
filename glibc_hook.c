#include "glibc_hook.h"

#define CALLER (__builtin_return_address(0))
#define LOG(...) (printf(__VA_ARGS__))

extern void *__libc_malloc(size_t size);
extern void *__libc_free(void *ptr);

static void* (*_malloc_hook)(size_t, void*) = NULL;
static void* (*_free_hook)(void*, void*) = NULL;

static void glibc_hook_set();
static void glibc_hook_unset();

static __uint64_t PAGESIZE;

static void *_malloc(size_t size) {
    return __libc_malloc(size);
}

static void _free(void *ptr) {
    __libc_free(ptr);
}

void *malloc(size_t size){
    if (_malloc_hook == NULL) {
        return _malloc(size);
    }
    void *caller = CALLER;
    return _malloc_hook(size, caller);
}

void free(void* ptr){
    if (_free_hook == NULL) {
        _free(ptr);
        return;
    }
    void *caller = CALLER;
    _free_hook(ptr, caller);
    return;
}

static void *malloc_hook(size_t size, void* caller) {
    glibc_hook_unset();
    LOG("malloc hook\n");
    size_t mem_size = (size + 16 + PAGESIZE - 1) & (~(PAGESIZE - 1));
    void *result = mmap(0, mem_size, PROT_WRITE|PROT_READ,
                        MAP_ANON|MAP_PRIVATE, -1, 0);
    if (result == (void*)-1) {
        LOG("malloc failed, ");
        LOG("errno: %d\n", errno);
    }
    LOG("%p, %d\n", result, errno);
    *((__uint64_t*)result + 1) = mem_size;
    glibc_hook_set();
    return (char*)result + 16;
}

static void *free_hook(void *ptr, void *caller) {
    glibc_hook_unset();
    LOG("free hook\n");
    if (munmap((char*)ptr-16, *((__uint64_t*)ptr - 1))) {
        LOG("munmap failed, p: %p, len: %lu\n",
              (char*)ptr-16, *((__uint64_t*)ptr - 1));
        LOG("errno: %d\n", errno);
    }
    glibc_hook_set();
}

static void glibc_hook_set() {
    _malloc_hook = malloc_hook;
    _free_hook = free_hook;
}

static void glibc_hook_unset() {
    _malloc_hook = NULL;
    _free_hook = NULL;
}

void glibc_hook_init() {
    PAGESIZE = sysconf(_SC_PAGESIZE);
    glibc_hook_set();
}
