#include <stdlib.h>
void free(void *ptr) {
    __builtin_assigner_free(ptr);
}
void *malloc(size_t size) {
    return __builtin_assigner_malloc(size);
}
