/****  Macros.h  **********************************************************
**
**  ABS    - return the absolute value of x.
**  MAX    - return the larger of x and y.
**  MIN    - return the smaller of x and y.
**  INT    - return the integer part of x.
**  FRAC   - return the fraction part of x.
**  LIMIT  - return x if it is in the range of min and max, otherwise 
**            return min or max, if x is too low or too high.
**  SWAP   - exchange the values of x and y without a temporary variable.
**  MALLOC - easier to use implementation of "malloc".
**  CALLOC - easier to use implementation of "calloc".
**  REALLOC- easier to use implementation of "realloc".
**  new    - same as MALLOC, but C++ compatible
**  dispose- free up and space an don't forget to set pointer
**
**  ABS, MAX, MIN, INT, FRAC, LIMIT and SWAP should work with int, long, 
**  unsigned, float, double, char and pointer data types.
**
**  MALLOC, CALLOC and REALLOC will work with all data types.
*/
#ifndef _H_MACROS
#define _H_MACROS 1
/*
#define ABS(x)             ((x)<0 ? -(x) : (x))
#define MAX(x,y)           ((x)<(y) ? (y) : (x))
#define MIN(x,y)           ((x)<(y) ? (x) : (y))
*/
#define INT(x)             ((int)(x))
#define FRAC(x)            ((int)(x)-(x))
#define LIMIT(min,x,max)   ((x)<(min) ? (min) : ((x)>(max) ? (max) : (x))) 
#define SWAP(x,y)          {(x)^=(y); (y)^=(x); (x)^=(y);}
#define MALLOC(x)          ((x *) malloc(sizeof(x)))
#define CALLOC(n,x)        ((x *) calloc((n), sizeof(x)))
#define REALLOC(p,n,x)     ((x *) realloc((x *)(p), (n)*sizeof(x)))

#define new(x)             ((x *) malloc(sizeof(x)))
#define dispose(x)         {free((x));(x)=NULL;}    

#endif
