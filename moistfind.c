#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <immintrin.h>
#include <sys/types.h>

#ifdef NO_ASM_USAGE
// taken from https://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup
static const char log2lut[256] = 
{
#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
    -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
    LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
    LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
};
#define GET_LOG_TWO(DST, SRC) do { unsigned int v = SRC & 0xfe; unsigned r; register unsigned int t, tt; if (tt = v >> 16) {  r = (t = tt >> 8) ? 24 + log2lut[t] : 16 + log2lut[tt]; } else {  r = (t = v >> 8) ? 8 + log2lut[t] : log2lut[v]; }; DST = r; } while (0)
#else
#define GET_LOG_TWO(DST, SRC) do { asm volatile ("tzcnt %1, %0" : "=r" (DST) : "r" (SRC & 0xfe)); } while (0)
#endif

#define OR_SHR(VAL, SHRN) VAL |= VAL >> SHRN
#define NEAREST_SQUARE(VAL) do { VAL--; OR_SHR(VAL, 1); OR_SHR(VAL, 2); OR_SHR(VAL, 4); OR_SHR(VAL, 8); OR_SHR(VAL, 16); OR_SHR(VAL, 32); VAL++;  } while (0)
#define GET_FIRST_PERIOD(DST, SRC, LEN) do { register char *cpy; for (cpy = &SRC[0]; *cpy && *(cpy - 1) != '.'; ++cpy); DST = (int)(cpy - SRC) == LEN ? -1 : (int)(cpy - SRC); } while (0)

typedef char filename_t[NAME_MAX];
typedef char pathname_t[PATH_MAX];
typedef char *nameptr_t;
typedef int flencls_t;
typedef int namelen_t;
typedef int lenhint_t;

typedef struct {
	namelen_t length;
	flencls_t lenclass;
	filename_t fname;
	lenhint_t extbegin;
} filenm_t;


static inline void init_filenm(filenm_t *fname) {
	fname->length = strlen(&fname->fname[0]);
	GET_LOG_TWO(fname->lenclass, fname->length);
	GET_FIRST_PERIOD(fname->extbegin, fname->fname, fname->length);
}

int main(int argc, char **argv) {
	filenm_t fname = (filenm_t) { .fname = ".abcd1aaa" };
	init_filenm(&fname);
}