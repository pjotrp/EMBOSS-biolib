#ifndef SIM_TYPES_H
#define SIM_TYPES_H
/* $Id: types.h,v 1.4 2004/08/05 16:02:32 rice Exp $ */

#define NACHARS 128

typedef int bool;
typedef int ss_t[NACHARS][NACHARS];

/* uchar is already defined in most platforms?  - but not linux at EBI */
/*  how do we test? */

typedef unsigned char sim4_uchar;

#endif
