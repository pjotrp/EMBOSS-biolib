#ifndef _H_CKITTYPES
#define _H_CKITTYPES

#include "seqspec.h"

typedef char* (*CharPtrFunc)(void);  /* Function returning a Char* */
typedef int (*ProcPtr)(void);        /* A pointer to a procedure */
typedef int (*ProcPtrChar)(char* msg);        /* A pointer to a procedure */
typedef Boolean (*ProcPtrSeq)(SeqSpec *spec); /* A pointer to a procedure */
typedef char* Ptr;               /* A pointer to an array    */

#ifndef NULL
#define NULL  (void *)(0)
#endif

#ifndef EOS
#define EOS  '\0'
#endif

#ifndef Boolean
#define Boolean int
#endif

#ifndef Choice
#define Choice int
#endif

#endif
