#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MK2X(M) M, M
#define MK4X(M) MK2X(M), MK2X(M)
#define MK6X(M) MK4X(M), MK2X(M)
#define MK8X(M) MK4X(M), MK4X(M)
#define MK16X(M) MK8X(M), MK8X(M)
#define MK32X(M) MK16X(M), MK16X(M)
#define MK60X(M) MK32X(M), MK16X(M), MK8X(M), MK4X(M)

#define DEFAULT_RETURN return TRUE

#define INLINE_LOCAL static inline
#define STORE_REENTRANT static
#define REENTRANT_NOCHANGE static const
#define EXTERNAL_LINKAGE extern
#define EXTERNAL_INLINE extern inline
#define NOARGS void


REENTRANT_NOCHANGE BYTE log2lut[UCHAR_MAX + 1] = {
	-1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	MK16X(4), MK16X(5), MK16X(5), MK16X(6), MK16X(6), MK16X(6), MK16X(6),
	MK16X(7), MK16X(7), MK16X(7), MK16X(7), MK16X(7), MK16X(7), MK16X(7),
	MK16X(7),
};
#define GET_LOG_TWO(DST, SRC) do { UINT32 v = SRC & 0xfe; unsigned r; register UINT32 t, tt; if (tt = v >> 16) {  r = (t = tt >> 8) ? 24 + log2lut[t] : 16 + log2lut[tt]; } else {  r = (t = v >> 8) ? 8 + log2lut[t] : log2lut[v]; }; DST = r; } while (0)
#define ZERO_OUT_MEMORY(DST, LEN) memset(DST, 0, LEN)
#define OR_SHR(VAL, SHRN) VAL |= VAL >> SHRN
#define NEAREST_SQUARE(VAL) do { VAL--; OR_SHR(VAL, 1); OR_SHR(VAL, 2); OR_SHR(VAL, 4); OR_SHR(VAL, 8); OR_SHR(VAL, 16); OR_SHR(VAL, 32); VAL++;  } while (0)
#define GET_FIRST_PERIOD(DST, SRC, LEN) do { register BYTE *cpy; for (cpy = &SRC[0]; *cpy && *(cpy - 1) != '.'; ++cpy); DST = (INT32)(cpy - SRC) == LEN ? -1 : (INT32)(cpy - SRC); } while (0)


typedef BYTE arru8FileName[NAME_MAX];
typedef BYTE arru8PathName[PATH_MAX];
typedef BYTE *ptru8NameStr;
typedef INT32 primi32NameBucket;
typedef INT32 primi32NameLen;
typedef INT32 primi32PosHint;
typedef BOOL boolYield;

typedef struct {
	UINT16 length;
	UINT8 extpos;
	UINT8 lclass;
} mstFileNameAttrs, *mspFileNameAttrs;

typedef struct {
	mstFileNameAttrs nmattrs;
	ptru8NameStr nmptr;
} mstFileNameDesc, *mspFileNameDesc;


INLINE_LOCAL boolYield fniInitFileNameDesc(arru8FileName repr, mspFileNameDesc filenm) {
	ZERO_OUT_MEMORY(filenm, sizeof(mstFileNameDesc));
	filenm->nmptr = &repr[0];
	filenm->nmattrs.length = strlen(&filenm->nmptr[0]);
	GET_LOG_TWO(filenm->nmattrs.lclass, filenm->nmattrs.length);
	GET_FIRST_PERIOD(filenm->nmattrs.extpos, filenm->nmptr, filenm->nmattrs.length);
	DEFAULT_RETURN:
}

#define MAX_REGEX_LEN 64
#define ZERO_OR_MORE_SYMBOL '*'
#define ATLEAST_ONE_SYMBOL '+'
#define ONE_OR_ZERO_SYMBOL '?'
#define EITHER_OR_SYMBOL '|'
#define SENTINEL_SYMBOL 0

typedef BYTE primu8Symbol, *ptru8Symbol, arru8RegexExpr[MAX_REGEX_LEN], *ptru8RegexExpr;

typedef struct {
	UINT32 symbol_b : 8;
	UINT32 symbol_a : 8;
	UINT32 operator : 16;
} bftPartialNFA, *bfpPartialNFA;

INLINE_LOCAL fniInitPartialNFA(bfpPartialNFA fragment, ptru8RegexExpr exprat) {
	REENTRANT_NOCHANGE WORD arri32sOperatorsMap[UCHAR_MAX] = {
		MK32X(SENTINEL_SYMBOL), 
		MK8X(SENTINEL_SYMBOL), 
		MK2X(SENTINEL_SYMBOL),
		ZERO_OR_MORE_SYMBOL, 
		ATLEAST_ONE_SYMBOL, 
		MK16X(SENTINEL_SYMBOL),
		MK2X(SENTINEL_SYMBOL), 
		SENTINEL_SYMBOL, 
		ONE_OR_ZERO_SYMBOL, 
		MK60X(SENTINEL_SYMBOL),
		EITHER_OR_SYMBOL, 
		MK6X(SENTINEL_SYMBOL),
	};
	fragment->symbol_a = *exprat;
	fragment->symbol_b = *(exprat + 2)
	fragment->operator = arri32sOperatorsMap[*(exprat + 1)];
}