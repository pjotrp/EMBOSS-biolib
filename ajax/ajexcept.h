#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajexcept_h
#define ajexcept_h

#include "ajarch.h"

#include <setjmp.h>


#define T Except_T
typedef struct T {
	char *reason;
} T;
typedef struct Except_Frame Except_Frame;
struct Except_Frame {
	jmp_buf env;
	Except_Frame *prev;
	const char *file;
	ajint line;
	const T *exception;
};
enum { Except_entered=0, Except_raised,
       Except_handled,   Except_finalized };
extern Except_Frame *Except_stack;


extern const Except_T Assert_Failed;

void ajExceptRaise(const T *e, const char *file,int line);
#define AJRAISE(e) ajExceptRaise(&(e), __FILE__, __LINE__)
#define AJRERAISE ajExceptRaise(Except_frame.exception, \
	Except_frame.file, Except_frame.line)
#define AJRETURN switch (Except_stack = Except_stack->prev,0) default: return
#define AJTRY do { \
	volatile ajint Except_flag; \
	Except_Frame Except_frame; \
	Except_frame.prev = Except_stack; \
	Except_stack = &Except_frame;  \
	Except_flag = setjmp(Except_frame.env); \
	if (Except_flag == Except_entered) {
#define AJEXCEPT(e) \
		if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
	} else if (Except_frame.exception == &(e)) { \
		Except_flag = Except_handled;
#define ELSE \
		if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
	} else { \
		Except_flag = Except_handled;
#define FINALLY \
		if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
	} { \
		if (Except_flag == Except_entered) \
			Except_flag = Except_finalized;
#define END_TRY \
		if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
		} if (Except_flag == Except_raised) AJRERAISE; \
} while (0)
#undef T
#endif

#ifdef __cplusplus
}
#endif
