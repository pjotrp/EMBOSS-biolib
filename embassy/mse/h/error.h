#ifndef _H_ERROR
#define _H_ERROR

#include "ckittypes.h"

#define DISPLAY 1
#define WARNING 2
#define FATAL   3


/***  error.h
***********************************************************
**
**
****************************************************************************/

extern void Notify(ProcPtr pNotify, ProcPtr pError, ProcPtr pFatal);
extern void PostError(Choice severity, char *errMsg);
extern  int ClearErrors(void);
extern  int Errors(void);

#endif

