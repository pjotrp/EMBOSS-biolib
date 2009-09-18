#ifdef __cplusplus
extern "C"
{
#endif

#ifndef enstable_h
#define enstable_h

#include "ajax.h"




/*
** Prototype definitions
*/

ajint ensTableCmpUInt(const void *x, const void *y);

ajuint ensTableHashUInt(const void *key, ajuint hashsize);

#define MENSTABLEUINTNEW(size) \
ajTableNewFunctionLen(size, ensTableCmpUInt, ensTableHashUInt)

/*
** End of prototype definitions
*/

#endif

#ifdef __cplusplus
}
#endif
