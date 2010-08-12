#ifdef __cplusplus
extern "C"
{
#endif

#ifndef enstable_h
#define enstable_h

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ajax.h"




/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */




/* ==================================================================== */
/* ========================== public data ============================= */
/* ==================================================================== */




/* ==================================================================== */
/* ======================= public functions =========================== */
/* ==================================================================== */

/*
** Prototype definitions
*/

ajint ensTableCmpUint(const void *x, const void *y);

ajuint ensTableHashUint(const void *key, ajuint hashsize);

/*
** End of prototype definitions
*/

#define MENSTABLEUINTNEW(size)                                          \
ajTableNewFunctionLen(size, ensTableCmpUint, ensTableHashUint)

#endif /* enstable_h */

#ifdef __cplusplus
}
#endif