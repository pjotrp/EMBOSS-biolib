/***********************************************************************
*                                                                      *
*       MEME++                                                         *
*       Copyright 1994, The Regents of the University of California    *
*       Author: Timothy L. Bailey                                      *
*                                                                      *
***********************************************************************/
/* clock.c */

#if defined(crayc90) || defined(crayt3e)

  #include <time.h>
  extern double myclock() {return((double)clock());}

#else

#include "meme.h"

/**********************************************************************/
/*
	myclock
	Return number of CPU microseconds since first call to clock(). 
	This corrects the bug in the system version of clock. 
*/
/**********************************************************************/

#include <sys/time.h>
#include <sys/resource.h>

#ifdef __cplusplus
extern "C" {
#endif
int getrusage(int who, struct rusage *rusage);
#ifdef __cplusplus
}
#endif

extern double myclock()
{
  static int first_time = 1;
  static double start_time;
  double elapsed;
  struct rusage ru;
  if (first_time) {
    getrusage(RUSAGE_SELF,&ru);
    start_time = ru.ru_utime.tv_sec*1.0E6 + ru.ru_utime.tv_usec;
    first_time = 0;
    return 0;
  } else {
    getrusage(RUSAGE_SELF,&ru);
    elapsed = ru.ru_utime.tv_sec*1.0E6 + ru.ru_utime.tv_usec - start_time;
    return elapsed;
  }
}
#endif

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/clock.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
