#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
**
** AJAX regular expression functions
**
******************************************************************************/

#ifndef ajhsreg_h
#define ajhsreg_h

#include "ajax.h"
#include "hsregexp.h"
#include "hsregmagic.h"

/* constructors */

AjPHsRegexp ajHsRegComp (AjPStr exp);
AjPHsRegexp ajHsRegCompC (const char* exp);

/* execute expression match */

AjBool ajHsRegExec (AjPHsRegexp prog, AjPStr str);
AjBool ajHsRegExecC (AjPHsRegexp prog, const char* str);

/* test substrings */

ajint  ajHsRegLenI (AjPHsRegexp rp, ajint isub);
AjBool ajHsRegPost (AjPHsRegexp rp, AjPStr* post);
AjBool ajHsRegPostC (AjPHsRegexp rp, const char** post);

ajint  ajHsRegOffset (AjPHsRegexp rp);
ajint  ajHsRegOffsetI (AjPHsRegexp rp, ajint isub);
ajint  ajHsRegOffsetC (AjPHsRegexp rp);
ajint  ajHsRegOffsetIC (AjPHsRegexp rp, ajint isub);

/* substitute substrings */

void   ajHsRegSub (AjPHsRegexp rp, AjPStr source, AjPStr* dest);
void   ajHsRegSubI (AjPHsRegexp rp, ajint isub, AjPStr* dest);
void   ajHsRegSubC (AjPHsRegexp rp, const char* source, AjPStr* dest);

/* destructor */

void   ajHsRegFree (AjPHsRegexp* pexp);
void   ajHsRegTrace (AjPHsRegexp exp);

void   ajHsRegExit (void);
#endif

#ifdef __cplusplus
}
#endif
