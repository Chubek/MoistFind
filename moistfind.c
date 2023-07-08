#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef NO_ASM_USAGE
// taken from https://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup
static const char log2lut[UCHAR_MAX + 1] = 
{
#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
    -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
    LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
    LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
};
#define GET_LOG_TWO(DST, SRC) do { unsigned int v = SRC & 0xfe; unsigned r; register unsigned int t, tt; if (tt = v >> 16) {  r = (t = tt >> 8) ? 24 + log2lut[t] : 16 + log2lut[tt]; } else {  r = (t = v >> 8) ? 8 + log2lut[t] : log2lut[v]; }; DST = r; } while (0)
#define ZERO_OUT_MEMORY(DST, LEN) memset(DST, 0, LEN)
#else
#ifdef __aarch64__
#define ASM_COUNT_TZ "ctz %0, %1"
#define ASM_MEM_ZERO "mov x9, %0; __zero: strb wzr, [%0], #1; subs %1, %1, #1; b.ne __zero;"
#define ASM_ZERO_MEM_INPUT(PTR, LEN) "r" (PTR), "r" (LEN)
#define CLOBBER_MEM_ZERO "x9", "x10", "memory"
#elif __amd64__
#define ASM_COUNT_TZ "tzcnt %1, %0"
#define ASM_MEM_ZERO "movq %0, %%r11; xorq %%r12, %%r12; __zero: movb %%r12b, (%%r11); incq %%r11; decq %1; jnz __zero;"
#define ASM_ZERO_MEM_INPUT(PTR, LEN) "g" (PTR), "r" (LEN)
#define CLOBBER_MEM_ZERO "r11", "rbx", "memory"
#else
#error "Please pass -D NO_ASM_USAGE"
#endif
#define GET_LOG_TWO(DST, SRC) do { asm volatile (ASM_COUNT_TZ : "=r" (DST) : "r" (SRC & 0xfe)); } while (0)
#define ZERO_OUT_MEMORY(DST, LEN) do { uintptr_t ptr = (uintptr_t)DST; size_t len = LEN; asm volatile (ASM_MEM_ZERO : : ASM_ZERO_MEM_INPUT(ptr, len) : CLOBBER_MEM_ZERO ); } while (0)
#endif

#define OR_SHR(VAL, SHRN) VAL |= VAL >> SHRN
#define NEAREST_SQUARE(VAL) do { VAL--; OR_SHR(VAL, 1); OR_SHR(VAL, 2); OR_SHR(VAL, 4); OR_SHR(VAL, 8); OR_SHR(VAL, 16); OR_SHR(VAL, 32); VAL++;  } while (0)
#define GET_FIRST_PERIOD(DST, SRC, LEN) do { register char *cpy; for (cpy = &SRC[0]; *cpy && *(cpy - 1) != '.'; ++cpy); DST = (int)(cpy - SRC) == LEN ? -1 : (int)(cpy - SRC); } while (0)

typedef char filename_t[NAME_MAX];
typedef char pathname_t[PATH_MAX];
typedef char *nameptr_t;
typedef int fnmlncls_t;
typedef int namelen_t;
typedef int lenhint_t;

typedef struct {
	unsigned int length : 9;
	unsigned int extpos : 8;
	unsigned int lclass : 4;
	unsigned int 		: 11;
} fnmattrs_t;

typedef struct {
	fnmattrs_t nmattrs;
	filename_t nmrepr;
	nameptr_t nmptr;
} filenm_t;


/*
static inline void init_filenm(filename_t repr, filenm_t *filenm) {
	memset(filenm, 0, sizeof(filenm_t));
	filenm->nmptr = &
	filenm->length = strlen(&filenm->filenm[0]);
	GET_LOG_TWO(filenm->lenclass, filenm->length);
	GET_FIRST_PERIOD(filenm->extbegin, filenm->filenm, filenm->length);
}
*/

int main(int argc, char **argv) {
	char b[] = "1131224";
	ZERO_OUT_MEMORY(&b[0], 8);
}