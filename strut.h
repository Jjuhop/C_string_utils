#ifndef STRUT_H
#define STRUT_H

/* --- HEADER PART --- */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct heap_string {
    char* data;
    size_t len;
    size_t cap;
};

typedef struct heap_string StringH;

int strut_init(StringH* s, size_t cap);
void strut_free(StringH* s);

int strut_app(StringH* dst, StringH* src);
int strut_appc(StringH* dst, const char* src);

size_t strut_len(StringH* s);
const char* strut_data(StringH* s);

#endif  // STRUT_H


/* --- IMPLEMENTATION PART --- */
#define STRUT_IMPLEMENTATION
#ifdef STRUT_IMPLEMENTATION

int strut_init(StringH* s, size_t cap) {
    assert(!s->data && "String already initialized!\n");
    s->data = (char*)calloc((cap + 1), sizeof(char));    // Leave room for \0
    if (!s->data) return -1;
    s->len = 0;
    s->cap = cap;
    return 0;
}

void strut_free(StringH* s) {
    if (s->data) free(s->data);
    s->data = NULL;
    s->len = 0;
    s->cap = 0;
}


int strut_app(StringH* dst, StringH* src) {
    if (dst->cap - dst->len < src->len) {
        size_t new_cap = dst->len + src->len + 1;
        char* new_alloc = (char*)realloc(dst->data, new_cap);
        if (!new_alloc) return -1;
        dst->data = new_alloc;
        dst->cap = new_cap;
    }
    strcpy(dst->data + dst->len, src->data);
    dst->len += src->len;
    return 0;
}

int strut_appc(StringH* dst, const char* src) {
    size_t src_len = strlen(src);
    if (dst->cap - dst->len < src_len) {
        size_t new_cap = dst->len + src_len + 1;
        char* new_alloc = (char*)realloc(dst->data, new_cap);
        if (!new_alloc) return -1;
        dst->data = new_alloc;
        dst->cap = new_cap;
    }
    strcpy(dst->data + dst->len, src);
    dst->len += src_len;
    return 0;
}


size_t strut_len(StringH *s) {
    return s->len;
}

const char* strut_data(StringH* s) {
    return s->data;
}

#endif // STRUT_IMPLEMENTATION