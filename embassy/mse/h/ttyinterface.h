#ifndef _H_TTYINTERFACE
#define _H_TTYINTERFACE

/***  TTYInterface.h  *******************************************************
**
**
****************************************************************************/

extern     char *GetInput(char *prompt, char *string);
extern  Boolean  GetBoolean(char *prompt, Boolean def);
extern     long  GetInteger(char *prompt, long def, long low, long high);
extern   double  GetFloat(char *prompt, char *format, double def, double low, double high);
extern  Boolean  GetRange(long *Begin, long *End, SeqEntry *);
extern  SeqSpec *GetSeqSpec(char *prompt, char *dclSymbol, char *defaultDB);
extern      int  TTYError(char *ErrMsg);

#endif
