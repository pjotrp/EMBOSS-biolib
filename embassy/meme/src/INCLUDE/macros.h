/* macros.h */
/*
	Handy macros for general C programming
*/

#ifndef macros_h
#define macros_h

#ifdef sgi4d
#define _XOPEN_SOURCE		/* needed for isnan in math.h on sgi */
extern double   cbrt(double); /* _XOPEN_SOURCE turns of definition cbrt */
#endif
#include <math.h>
#define exp10(X) pow(10.0, (X))

#ifdef UNIX

#ifndef SUNOS_CC
  /* this is defined in malloc.h which is in stdlib.h when compiler CC is 
     used under SunOS; otherwise we must declare it 
  */
    typedef void *malloc_t;
#endif

#else
  /* VAX stuff */
    typedef void *malloc_t;

#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#ifdef sgi4d
#include <time.h>
#else
#include <sys/time.h>
#endif
#include <sys/resource.h>

/* random functions */
#ifdef __cplusplus
extern "C" {
#endif

/* flush functions */
extern int	filbuf(void);
extern int	flsbuf(void);

/* random number functions */
double drand48(void);
void srand48(long seedval); 

#ifdef __cplusplus
}
#endif

/* EXTERN and DEXTERN can be used to allow global variables
   to be defined in a .h file.  The .h file can be included
   wherever the variables are used.  It should be preceeded
   by #define DEFINE_GLOBALS in exactly one file, usually the
   main program file.  This causes space to be allocated for
   the variable only once.
*/
#ifdef DEFINE_GLOBALS
#define EXTERN
#define DEXTERN(A,B,C) A B = C
#else
#define EXTERN extern
#define DEXTERN(A,B,C) extern A B
#endif

/* 
  rounding 
*/
/* macro to round to the nearest int value, except halfway cases are rounded
   to the int value larger in magnitude.
*/
#define NINT(x) ((int)((x) >= 0 ? ((x) + 0.5) : ((x) - 0.5)))

/* the largest and smallest double precision numbers */
#define BIG HUGE_VAL
#define LITTLE -BIG
#define MAXPOSLONG 0x7FFFFFFF
#define MAXNEGLONG 0x80000000

/* handy constants */
#define Log2 0.693147				/* log(2) */

/* max and min for all seasons */

#ifndef MAX
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#endif


/* swapping any two items x and y of type "type"*/
#define SWAP(type, x, y) {type tmp; tmp = (x); (x) = (y); (y) = (tmp);}

/* Used for preventing overflow with logarithms */
#define EPSILON 1e-200		/* smallest LOG2(X) = -664.385 */ 
#define LOG(X) log((double)((X) + EPSILON))
#define LOG2(X) (LOG(X)/0.6931471805599452862)
/*#define LOG2(X) log2((double)((X) + EPSILON))*/

/* used for summing logarithms:  log(x + y) where log(x) and log(y) are avail.*/
#define BITS 64.0
#define LOG_SUM1(logx, logy) ( ( ( (logx) - (logy) ) > BITS ) ? \
    (logx) : (logx) + log( 1 + exp((logy) - (logx) ) ) )
#define LOG_SUM(logx, logy) ( ( (logx) > (logy) ) ? \
  LOG_SUM1( (logx), (logy) ) : LOG_SUM1( (logy), (logx) ) )

/* Largest X  L = e^(-1/SCALE_LOGS); larger X yeild INT_LOG(x) = 0 */
/* Smallest X  S = e^(-L/SCALE_LOGS) - EPSILON) */
/* INT_LOG(0) = SCALE_LOGS * log(EPSILON) = ??? */
#define SCALE_LOGS 1e3  	       /* L = .999307, S = 1e-200 */
#define INT_LOG(X) ((int)(SCALE_LOGS * LOG(X)))

/* get the exponent and manitissa of large numbers expressed as logarithms */
#define exp10_logx(logx, m, e) { \
  (e) = logx < 0 ? floor(logx) : ceil(logx); \
  (m) = exp10((logx) - (e)); \
  if ((m) >= 9.9995) { (m) = 1; (e) += 1;} \
}

/* dynamically create or grow an array; P = pointer, N = new size, T = type */
#define Resize(P,N,T) { \
  void *new_P; \
  new_P = (P) ? realloc((malloc_t)(P), (N)*sizeof(T)) : malloc((N)*sizeof(T)); \
  if (!new_P) { \
   fprintf(stderr, "Resize(" #P "," #N "," #T ") failed in file %s line %d!\n", __FILE__, __LINE__);\
    exit(1); \
  } \
  (P) = (T *) new_P; \
}

/* Useful data types and constants */
typedef int BOOLEAN;
#define FALSE 0
#define TRUE 1

/* make a better free function */
# define myfree(x) if (x) free((char *) (x))

/* macros to create and destroy an r x c array; 
   avoids problems with multiply subscripted C array elements 
   behaving differently when passed to subroutines or assigned locally 
*/
#define create_2array(v, typ, r, c) {					\
  int _i, _ir=(r), _ic=(c);		/* avoid side effects */	\
  (v) = (typ **) malloc(_ir * sizeof(typ *));				\
  if (!(v)) printf("malloc failed 1\n");				\
  for (_i=0; _i<_ir; _i++) {						\
    typ *tmp;								\
    (v)[_i] = 0;							\
    tmp = (typ *) malloc(_ic * sizeof(typ));			\
    (v)[_i] = tmp;							\
    if (!(v)[_i]) printf("malloc failed 2\n");				\
  }									\
}
#define free_2array(v, r) {						\
  int _i, _ir=(r);			/* avoid side effects */	\
  for (_i=0; _i<_ir; _i++) myfree((v)[_i]);				\
  myfree(v);								\
}
#endif

/* swap two entities of a given type */
#define swap(x, y, typ) {typ _tmp; _tmp = x; x = y; y = _tmp;}

/* I/O functions */
/* skip until end of line; start of next line to be read when done */
#define Skip_eol(c, file) while ((c) != '\n' && (c) != EOF) (c) = fgetc(file)
/* skip white space; start of next token to be read when done */
#define Skip_whi(c, file) { \
  while ((c) == ' ' || (c) == '\t') (c) = fgetc(file); \
  ungetc((c), (file)); \
}

/* print a debug message once only */
#ifdef DEBUG
#define print_once(msg) {static BOOLEAN ft; if (!ft) printf(msg); ft=1;}
#else
#define print_once(msg) {}
#endif

#ifdef PARALLEL
#include <mp.h>
#define printf if (mpMyID() == 0) printf
#define fprintf if (mpMyID() == 0) fprintf
#endif

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/macros.h,v 1.2 2000/11/05 22:27:29 ajb Exp $ */

