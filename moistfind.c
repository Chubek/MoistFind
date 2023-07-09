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
#define RETURN_ERROR return FALSE
#define RETURN_SUCCESS return TRUE

#define INLINE_LOCAL static inline
#define STORE_REENTRANT static
#define REENTRANT_NOCHANGE static const
#define EXTERNAL_LINKAGE extern
#define EXTERNAL_INLINE extern inline
#define NOARGS void
#define NORETURN void

REENTRANT_NOCHANGE BYTE log2lut[UCHAR_MAX + 1] = {
	-1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	MK16X(4), MK16X(5), MK16X(5), MK16X(6), MK16X(6), MK16X(6), MK16X(6),
	MK16X(7), MK16X(7), MK16X(7), MK16X(7), MK16X(7), MK16X(7), MK16X(7),
	MK16X(7),
};
#define GET_LOG_TWO(DST, SRC) do { UINT32 v = SRC & 0xfe; unsigned r; register UINT32 t, tt; if (tt = v >> 16) {  r = (t = tt >> 8) ? 24 + log2lut[t] : 16 + log2lut[tt]; } else {  r = (t = v >> 8) ? 8 + log2lut[t] : log2lut[v]; }; DST = r; } while (0)
#define ZERO_OUT_MEMORY(DST, LEN) memset(DST, 0, LEN)
#define COPY_MEMORY(DST, SRC, LEN) memmove(DST, SRC, LEN)
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
	UINT8 currpos;
} mstFileNameAttrs, *mspFileNameAttrs;

typedef struct {
	mstFileNameAttrs nmattrs;
	ptru8NameStr nmptr;
} mstFileNameDesc, *mspFileNameDesc;


INLINE_LOCAL boolYield fsiInitFileNameDesc(arru8FileName sstr_fnmrepr, mspFileNameDesc mem_fnmdesc) {
	ZERO_OUT_MEMORY(mem_fnmdesc, sizeof(mstFileNameDesc));
	mem_fnmdesc->nmptr = &sstr_fnmrepr[0];
	mem_fnmdesc->nmattrs.length = strlen(&mem_fnmdesc->nmptr[0]);
	GET_LOG_TWO(mem_fnmdesc->nmattrs.lclass, mem_fnmdesc->nmattrs.length);
	GET_FIRST_PERIOD(mem_fnmdesc->nmattrs.extpos, mem_fnmdesc->nmptr, mem_fnmdesc->nmattrs.length);
	DEFAULT_RETURN:
}

#define STACK_SIZE 2048

typedef BYTE arru8PushdownStack[STACK_SIZE], *ptru8PushdownStack, primu8Symbol, *ptru8Symbol;
typedef INT32 primi32StackTop, *ptru16StackTop;
typedef struct {
	arru8PushdownStack arena;
	primi32StackTop top;
} mstPushdownStack, *mspPushdownStack;

INLINE_LOCAL boolYield fsiInitArenaStack(mspPushdownStack mem_stack) {
	return ZERO_OUT_MEMORY(mem_stack, sizeof(*mem_stack));
}

INLINE_LOCAL boolYield fsiPushToStack(mspPushdownStack mem_stack, primu8Symbol ub_symbol) {
	if (mem_stack->top + 1 >= STACK_SIZE)
		RETURN_ERROR;
	else
		mem_stack->arena[++mem_stack->top] = ub_symbol;
	RETURN_SUCCESS;
}

INLINE_LOCAL boolYield fsiPopFromStack(mspPushdownStack mem_stack, ptru8Symbol ub_symbol) {
	if (mem_stack->top < 0)
		RETURN_ERROR;
	else
		*ub_symbol = mem_stack->arena[mem_stack->top--];
	RETURN_SUCCESS;
}

INLINE_LOCAL primi32StackTop fsiGetStackTop(mspPushdownStack mem_stack) {
	return mem_stack->top;
}