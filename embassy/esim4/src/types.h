#ifndef SIM_TYPES_H
#define SIM_TYPES_H
/* $Id: types.h,v 1.3 2004/06/16 10:02:28 rice Exp $ */

#define NACHARS 128

typedef int bool;
typedef int ss_t[NACHARS][NACHARS];

/* uchar is already defined in most platforms?  - but not linux at EBI */
/*  how do we test? */

typedef unsigned char uchar;

#endif
