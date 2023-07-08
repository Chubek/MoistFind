#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdINT32.h>
#include <string.h>

#ifdef NO_ASM_USAGE
// taken from https://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup
static const BYTE log2lut[UCHAR_MAX + 1] = 
{
#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
    -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
    LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
    LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
};
#define GET_LOG_TWO(DST, SRC) do { UINT32 v = SRC & 0xfe; unsigned r; register UINT32 t, tt; if (tt = v >> 16) {  r = (t = tt >> 8) ? 24 + log2lut[t] : 16 + log2lut[tt]; } else {  r = (t = v >> 8) ? 8 + log2lut[t] : log2lut[v]; }; DST = r; } while (0)
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
#define GET_LOG_TWO(DST, SRC) do { uINT3264_t dst = 0, src = SRC & 0xfe; asm volatile (ASM_COUNT_TZ : "=g" (dst) : "g" (src)); DST = dst; } while (0)
#define ZERO_OUT_MEMORY(DST, LEN) do { uINT32ptr_t ptr = (uINT32ptr_t)DST; size_t len = LEN; asm volatile (ASM_MEM_ZERO : : ASM_ZERO_MEM_INPUT(ptr, len) : CLOBBER_MEM_ZERO ); } while (0)
#endif

#define OR_SHR(VAL, SHRN) VAL |= VAL >> SHRN
#define NEAREST_SQUARE(VAL) do { VAL--; OR_SHR(VAL, 1); OR_SHR(VAL, 2); OR_SHR(VAL, 4); OR_SHR(VAL, 8); OR_SHR(VAL, 16); OR_SHR(VAL, 32); VAL++;  } while (0)
#define GET_FIRST_PERIOD(DST, SRC, LEN) do { register BYTE *cpy; for (cpy = &SRC[0]; *cpy && *(cpy - 1) != '.'; ++cpy); DST = (INT32)(cpy - SRC) == LEN ? -1 : (INT32)(cpy - SRC); } while (0)

#define DEFAULT_RETURN return TRUE

#define INLINE static inline

typedef BYTE arri8FileName[NAME_MAX];
typedef BYTE arri8PathName[PATH_MAX];
typedef BYTE *ptri8NameStr;
typedef INT32 primi32NameBucket;
typedef INT32 primi32NameLen;
typedef INT32 primi32PosHint;
typedef BOOL boolYield;

typedef struct {
	UINT32 length : 9;
	UINT32 extpos : 8;
	UINT32 lclass : 4;
	UINT32 		: 11;
} bftFileNameAttrs, *bfpFileNameAttrs;

typedef struct {
	bftFileNameAttrs nmattrs;
	ptri8NameStr nmptr;
} mstFileNameDesc, *mspFileNameDesc;



INLINE boolYield fniInitFileNameDesc(arri8FileName repr, mspFileNameDesc filenm) {
	ZERO_OUT_MEMORY(filenm, sizeof(mstFileNameDesc));
	filenm->nmptr = &repr[0];
	filenm->nmattrs.length = strlen(&filenm->nmptr[0]);
	GET_LOG_TWO(filenm->nmattrs.lclass, filenm->nmattrs.length);
	GET_FIRST_PERIOD(filenm->nmattrs.extpos, filenm->nmptr, filenm->nmattrs.length);
	DEFAULT_RETURN:
}

static 