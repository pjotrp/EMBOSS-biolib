/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Bill Grundy 					       *
*								       *
***********************************************************************/
#ifndef MPMYID_H
#define MPMYID_H

extern int mpNodes();
extern int mpMyID();
extern void mpReduceAdd(int *data);
extern void mpBroadcast(void *data, int bytes, int broadcaster);

#endif
