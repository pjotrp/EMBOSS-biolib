#ifndef _H_CKITTYPES
#define _H_CKITTYPES

typedef char* (*CharPtrFunc)();  /* Function returning a Char* */
typedef int (*ProcPtr)();        /* A pointer to a procedure */
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

#ifndef false
#define false     0
#endif

#ifndef true
#define true      1
#endif

/* typedef enum {false, true} Boolean; */

#ifndef Choice
#define Choice int
#endif

#endif
