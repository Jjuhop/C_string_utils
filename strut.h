#ifndef _STRUT_H
#define _STRUT_H

/* --- HEADER PART --- */

// bcap and ccap

#ifndef _STRUT_SIZE_T
#include <stddef.h>
#define _STRUT_SIZE_T size_t
#endif

struct strut_heap_string {
    _STRUT_SIZE_T  len;
    _STRUT_SIZE_T bcap;
    char*         data;
};

struct strut_string {
    union {
        struct strut_heap_string hs;
        char ss[sizeof(struct strut_heap_string)];
    };
};

typedef struct strut_string StrutStr;
#ifdef STRUT_ENABLE_SHORTHAND
typedef StrutStr Str;
#endif

//////////////////////////////////////////////////////////////////
/*
  The struct strut_string may hold EITHER a heap-allocated string,
  or alternatively for very short strings store them directly in
  the structure on stack.

  Rules to determine the type:
  0. Any 0-initialized structure is a valid empty stack string.
     This follows from the other points.
  1. If the heap-side data pointer is NULL, the struct is a short
     STACK string.
  2. If the heap-side data pointer is non-NULL ...
     a) ... AND the highest byte of the heap-side length field is 0
        (data[7] of the stack-side), the struct is a HEAP string.
     b) ... AND the highest byte of the heap-side length field is not
        0 (data[7] of the stack-side), the struct is a STACK string


*/
/////////////////////////////////////////////////////////////////

// Determine endianness if not supplied
#ifndef _STRUT_IS_LITTLE_ENDIAN

// Some general endianness checks
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ || \
    defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)

#define _STRUT_IS_LITTLE_ENDIAN 0

#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ || \
    defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)

#define _STRUT_IS_LITTLE_ENDIAN 1

#else
#error "Unknown endianness! Specify flag _STRUT_IS_LITTLE_ENDIAN value 0 or 1 manually!"
#endif

#endif  // not defined _STRUT_IS_LITTLE_ENDIAN

// Now define the index of the most significant byte of heap string len
// This define should have value 0 or 1
#if _STRUT_IS_LITTLE_ENDIAN
#define _STRUT_CHECKBYTE_IND (sizeof(_STRUT_SIZE_T) - 1)
#else
#define _STRUT_CHECKBYTE_IND 0
#endif

// We cannot use the last byte of len due to logic reasons
// Note that the cap maximum is 1 more to account for the NULL termination char
// Therefore cap may need its final byte in a rare case
#define STRUT_MAXLEN (1L << ((sizeof(_STRUT_SIZE_T) - 1) * 8) - 1)
#define STRUT_MAXCAP (STRUT_MAXLEN + 1)
#define STRUT_MAX_STACKLEN (sizeof(struct strut_heap_string) - 1)

#define STRUT_IS_HEAP(str) ((str).hs.data != NULL && (str).ss[_STRUT_CHECKBYTE_IND] == 0)

/////////// Some basic operations that need to be supplied if you wish to detach from standard library

#ifndef STRUT_PRINT
#include <stdio.h>
#define STRUT_PRINT(...) printf(__VA_ARGS__)
#endif

#ifndef STRUT_EPRINT
#include <stdio.h>
#define STRUT_EPRINT(...) fprintf(stderr, __VA_ARGS__)
#endif

#ifndef STRUT_TERMINATE
#include <stdlib.h>
#define STRUT_TERMINATE(code) exit(code)
#endif

#ifndef STRUT_MALLOC
#include <stdlib.h>
#define STRUT_MALLOC(size) malloc(size)
#endif

#ifndef STRUT_FREE
#include <stdlib.h>
#define STRUT_FREE(ptr) free(ptr)
#endif

#define STRUT_ASSERT(cond, ...) if (!(cond)) { STRUT_EPRINT("ASSERTION FAILED at %s:%d\n", __FILE__, __LINE__); STRUT_EPRINT(__VA_ARGS__); STRUT_TERMINATE(1); }

/// @brief Initializes the string. Note that 0-initializing the structure is also a valid way to initialize it.
/// @param s String to be initialized
/// @param ccap Capacity (in terms of chars before NULL) to be reserved exactly.
/// @return 0 on success, non-0 on failure
///
/// @note Calling this on a string that has been processed with other STRUT functions before is undefined
/// behaviour and may result in memory leaks. However, calling this RIGHT AFTER strut_free is allowed.
int strut_init(StrutStr* s, _STRUT_SIZE_T ccap);

/// @brief Frees the memory that was associated with the string (if any)
/// @param s The string that is to be freed
/// @note After this the struct is zeroed out and usable again, either with `strut_init` or any other
/// strut function that expects a string
void strut_free(StrutStr* s);

/// @brief Clear the contents setting size to 0, but doesn't free memory
/// @param s The string that is to be cleared
void strut_clear(StrutStr* s);

/// @brief Makes sure that the string can accomodate `bonus_ccap` more chars before needing to reallocate
/// @param s The string that is to be operated on
/// @param bonus_ccap The additional chars NOT including NULL char that we want to have room for.
/// @return 0 on success, non-0 on failure
int strut_ensure_additional_ccap(StrutStr* s, _STRUT_SIZE_T bonus_ccap);

/// @brief Appends the contents of a C string to this string
/// @param dst To which we append
/// @param src From which we append
/// @return 0 on success, non-0 on failure
int strut_appc(StrutStr* dst, const char* src);

/// @brief Appends the contents of one string to another
/// @param dst To which we append
/// @param src From which we append
/// @return 0 on success, non-0 on failure
int strut_app(StrutStr* dst, StrutStr* src);

/// @brief Tells how many chars NOT including NULL termination char can be added without reallocation
/// @param s The string to observe
/// @return Number of capacity for chars left
_STRUT_SIZE_T strut_get_remaining_ccap(StrutStr* s);

/// @brief Gets the current length of the string
/// @param s The string to observe
/// @return The length of the string
_STRUT_SIZE_T strut_get_len(StrutStr* s);

/// @brief Gets the associated NULL-terminated C string
/// @param s The string to observe
/// @return C string
const char* strut_get_cstr(StrutStr* s);




/// @brief Prints a debug message too see if the memory layout on the machine is as it should be
void _strut_validate_mem_layout();

/// @brief Prints debug info on the string instance
void _strut_debug_str(StrutStr* s);

/////// Internal helper implementations, incl. some c string functions ///////
int _strut_app_impl(StrutStr* dst, const char* src_data, _STRUT_SIZE_T src_len);

_STRUT_SIZE_T _strut_strlen(const char* s);
void _strut_strcpy(char* dst, const char* src);
int _strut_strcmp(const char* a, const char* b);

#endif  // _STRUT_H


/* --- IMPLEMENTATION PART --- */
#ifdef STRUT_IMPLEMENTATION

int strut_init(StrutStr* s, _STRUT_SIZE_T ccap) {
    // Allocate only if this cap doesn't fit on stack
    if (ccap > STRUT_MAX_STACKLEN) {
        // Leave room for NULL char
        s->hs.data = (char*)STRUT_MALLOC((ccap + 1) * sizeof(char));
        if (!s->hs.data) return -1;
        s->hs.len = 0;
        s->hs.bcap = (ccap + 1);
    }
    return 0;
}

void strut_free(StrutStr* s) {
    if (STRUT_IS_HEAP(*s)) STRUT_FREE(s->hs.data);
    s->hs.data = NULL;
    s->hs.len = (_STRUT_SIZE_T)0;
    s->hs.bcap = (_STRUT_SIZE_T)0;
}

void strut_clear(StrutStr *s) {
    if (STRUT_IS_HEAP(*s)) {
        s->hs.len = (_STRUT_SIZE_T)0;
        STRUT_ASSERT(s->hs.data, "Heap str mush have non-NULL ptr");
        if (s->hs.bcap > 0) s->hs.data[0] = 0;
    } else {
        s->hs.data = NULL;
        s->hs.len = (_STRUT_SIZE_T)0;
        s->hs.bcap = (_STRUT_SIZE_T)0;
    }
}

int strut_ensure_additional_ccap(StrutStr* s, _STRUT_SIZE_T bonus_ccap) {
    if (strut_get_remaining_ccap(s) >= bonus_ccap) {
        // No need to do anything we already have enough cap
        return 0;
    }
    // Must do an allocation
    if (STRUT_IS_HEAP(*s)) {
        _STRUT_SIZE_T new_bcap = s->hs.len + bonus_ccap + 1;   // Account for NULL  
        if (new_bcap > STRUT_MAXCAP) return -1; // TODO: Some fail message?
        char* new_data = (char*)STRUT_MALLOC(new_bcap * sizeof(char));
        if (!new_data) return -1;
        _strut_strcpy(new_data, s->hs.data);
        // Something to free as well
        STRUT_FREE(s->hs.data);
        s->hs.data = new_data;
        s->hs.bcap = new_bcap;
        // len unchanged
        return 0;
    } else {
        _STRUT_SIZE_T len = _strut_strlen(s->ss);
        _STRUT_SIZE_T new_bcap = len + bonus_ccap + 1;
        if (new_bcap > STRUT_MAXCAP) return -1;
        char* new_data = (char*)STRUT_MALLOC(new_bcap * sizeof(char));
        if (!new_data) return -1;
        _strut_strcpy(new_data, s->ss);
        // Transition from stack string to heap string
        // Must zero out the _STRUT_CHECKBYTE_IND, should happen autom. as len is quite small
        s->hs.len = len;
        s->hs.bcap = new_bcap;
        s->hs.data = new_data;
        STRUT_ASSERT(s->ss[_STRUT_CHECKBYTE_IND] == 0, "Something went wrong");
        return 0;
    }
}

int strut_appc(StrutStr* dst, const char* src) {
    return _strut_app_impl(dst, src, _strut_strlen(src));
}

int strut_app(StrutStr* dst, StrutStr* src) {
    return _strut_app_impl(dst, strut_get_cstr(src), strut_get_len(src));
}

_STRUT_SIZE_T strut_get_remaining_ccap(StrutStr* s) {
    if (STRUT_IS_HEAP(*s)) {
        return s->hs.bcap - s->hs.len - 1;
    } else {
        return STRUT_MAX_STACKLEN - _strut_strlen(s->ss);
    }
}

_STRUT_SIZE_T strut_get_len(StrutStr* s) {
    if (STRUT_IS_HEAP(*s)) {
        return s->hs.len;
    } else {
        return _strut_strlen(s->ss);
    }
}

const char* strut_get_cstr(StrutStr* s) {
    if (STRUT_IS_HEAP(*s)) {
        return s->hs.data;
    } else {
        return s->ss;
    }
}

void _strut_validate_mem_layout() {
    StrutStr s = {0};
    for (_STRUT_SIZE_T i = 0; i < STRUT_MAX_STACKLEN; i++) {
        s.ss[i] = (char)i;
    }
    s.ss[_STRUT_CHECKBYTE_IND] = (char)0xff;
    STRUT_PRINT("******* VALIDATING STRUT MEMORY LAYOUT *******\n");
    STRUT_PRINT(" - Size of heap string struct: %zu\n", sizeof(struct strut_heap_string));
    STRUT_PRINT(" - Size of combined string:    %zu\n", sizeof(StrutStr));
    STRUT_PRINT("Stack char array byte values (indices from 0 to max):\n");
    for (_STRUT_SIZE_T i = 0; i < STRUT_MAX_STACKLEN; i++) {
        STRUT_PRINT("%02hhX ", s.ss[i]);
    }
    char highlight_offset[3 * _STRUT_CHECKBYTE_IND + 2] = {0};
    highlight_offset[0] = '\n';
    for (_STRUT_SIZE_T i = 1; i < 3 * _STRUT_CHECKBYTE_IND + 1; i++) {
        highlight_offset[i] = ' ';
    }
    STRUT_PRINT("%s", highlight_offset);
    STRUT_PRINT("^^\nHeap string components (len, bcap, ptr)\n");
    const char* fmtstr = sizeof(_STRUT_SIZE_T) == 8 ? "%016zX, %016zX, %p\n" : "%08zX, %08zX, %p\n";
    STRUT_PRINT(fmtstr, s.hs.len, s.hs.bcap, s.hs.data);
    STRUT_PRINT("^^\n");
    STRUT_PRINT("The highest byte of heap len should match with %zuth byte in stack array side\n", _STRUT_CHECKBYTE_IND);
    if ((char)(s.hs.len >> ((sizeof(_STRUT_SIZE_T) - 1) * 8)) == s.ss[_STRUT_CHECKBYTE_IND]) {
        STRUT_PRINT("...OK\n");
    } else {
        STRUT_EPRINT("...ERROR, DOES NOT MATCH!\n");
    }
    STRUT_PRINT("**********************************************\n");
}

void _strut_debug_str(StrutStr* s) {
    STRUT_PRINT("******* STRUT STRING INFO *******\n");
    STRUT_PRINT("Raw bytes:  ");
    for (_STRUT_SIZE_T i = 0; i < STRUT_MAX_STACKLEN; i++) {
        STRUT_PRINT("%02hhX ", s->ss[i]);
    }
    STRUT_PRINT("\nAs chars:   ");
    for (_STRUT_SIZE_T i = 0; i < STRUT_MAX_STACKLEN; i++) {
        STRUT_PRINT(" %c ", s->ss[i]);
    }
    STRUT_PRINT("\n---\nHeap side values:\n");
    STRUT_PRINT("\tLength:   %zu\n", s->hs.len);
    STRUT_PRINT("\tCapacity: %zu\n", s->hs.bcap);
    STRUT_PRINT("\tData ptr: %p\n", s->hs.data);
    STRUT_PRINT("The string should be located in %s\n", STRUT_IS_HEAP(*s) ? "HEAP" : "STACK");
    STRUT_PRINT("*********************************\n");
}

/////// Internal implementations on some c string functions ///////

int _strut_app_impl(StrutStr* dst, const char* src_data, _STRUT_SIZE_T src_len) {
    int err = strut_ensure_additional_ccap(dst, src_len);
    if (err) return -1;
    // Ok to append
    if (STRUT_IS_HEAP(*dst)) {
        _strut_strcpy(dst->hs.data + dst->hs.len, src_data);
        dst->hs.len += src_len;
    } else {
        char* dst_ptr = dst->ss;
        while (*dst_ptr) { dst_ptr++; }
        _strut_strcpy(dst_ptr, src_data);
    }
    return 0;
}

_STRUT_SIZE_T _strut_strlen(const char* s) {
    _STRUT_SIZE_T len = 0;
    while (*s++) { len++; }
    return len;
}

void _strut_strcpy(char* dst, const char* src) {
    while (*src) { *dst++ = *src++; }
    *dst = '\0';
}

int _strut_strcmp(const char* a, const char* b) {
    int diff = 0;
    do {
        diff = *a - *b;
    } while (*a++ && *b++ && diff == 0);
    return diff;
}

#endif // STRUT_IMPLEMENTATION