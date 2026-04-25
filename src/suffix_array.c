#include "../include/suffix_array.h"
#include "../include/dbg.h"
#include <limits.h>
#include <string.h>

static int suffix_array_compare(const void* s1, const void* s2)
{
    return strcmp(*((const char**)s1), *((const char**)s2));
}

SuffixArray* suffixArrayCreate(const char* string)
{
    SuffixArray* sa;

    sa = malloc(sizeof(*sa));
    check_mem(sa);

    sa->n = strlen(string) + 1;
    sa->string = string;

    sa->suffix = malloc(sizeof(*sa->suffix) * sa->n);
    check_mem(sa->suffix);

    for (size_t i = 0; i < sa->n; ++i) {
        sa->suffix[i] = string + i;
    }

    qsort(sa->suffix, sa->n, sizeof(*sa->suffix), suffix_array_compare);

    return sa;
error:
    return NULL;
}

void suffixArrayDestroy(SuffixArray* suffix_array)
{
    free(suffix_array->suffix);
    free(suffix_array);
}

size_t suffixArraySearch(SuffixArray* suffix_array, const char* substring, size_t* first)
{
    size_t len = strlen(substring);
    size_t lo = 0;
    size_t hi = suffix_array->n;
    size_t mid;
    int cmp;

    while (lo + 1 < hi) {
        mid = (lo + hi) / 2;
        cmp = strncmp(suffix_array->suffix[mid], substring, len);

        if (cmp == 0) {
            /* we have a winner */
            /* search backwards and forwards for first and last */
            for (lo = mid;
                 lo > 0 && strncmp(suffix_array->suffix[lo - 1], substring, len) == 0;
                 lo--)
                ;
            for (hi = mid; hi < suffix_array->n
                 && strncmp(suffix_array->suffix[hi + 1], substring, len) == 0;
                 hi++)
                ;

            if (first) {
                *first = lo;
            }

            return hi - lo + 1;
        } else if (cmp < 0) {
            lo = mid;
        } else {
            hi = mid;
        }
    }

    return 0;
}

char* suffixArrayBWT(SuffixArray* suffix_array)
{
    char* bwt;
    size_t i;

    bwt = malloc(suffix_array->n);
    check_mem(bwt);

    for (i = 0; i < suffix_array->n; i++) {
        if (suffix_array->suffix[i] == suffix_array->string) {
            /* wraps around to null */
            bwt[i] = '\0';
        } else {
            bwt[i] = suffix_array->suffix[i][-1];
        }
    }

    return bwt;
error:
    return NULL;
}

char* inverseBWT(size_t len, const char* string)
{
    /* basic trick: stable sort of s gives successor indices */
    /* then we just thread through starting from the nul */

    size_t* successor;
    int c;
    size_t count[UCHAR_MAX + 1];
    size_t offset[UCHAR_MAX + 1];
    size_t i;
    char* ret;
    size_t thread;

    successor = malloc(sizeof(*successor) * len);
    check_mem(successor);

    /* counting sort */
    for (c = 0; c <= UCHAR_MAX; c++) {
        count[c] = 0;
    }

    for (i = 0; i < len; i++) {
        count[(unsigned char)string[i]]++;
    }

    offset[0] = 0;

    for (c = 1; c <= UCHAR_MAX; c++) {
        offset[c] = offset[c - 1] + count[c - 1];
    }

    for (i = 0; i < len; i++) {
        successor[offset[(unsigned char)string[i]]++] = i;
    }

    /* find the nul */
    for (thread = 0; string[thread]; thread++)
        ;

    /* thread the result */
    ret = malloc(len);
    check_mem(ret);

    for (i = 0, thread = successor[thread]; i < len; i++, thread = successor[thread]) {
        ret[i] = string[thread];
    }

    free(successor);
    return ret;
error:
    free(successor);
    return NULL;
}
