#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensmetacoordinate_h
#define ensmetacoordinate_h

#include "enstable.h"
#include "enscoordsystem.h"




/* @data EnsPMetaCoordinateAdaptor ********************************************
**
** Ensembl Meta-Coordinate Adaptor.
**
** @alias EnsSMetaCoordinateAdaptor
** @alias EnsOMetaCoordinateAdaptor
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor.
** @attr CacheByName [AjPTable] Ensembl Feature name (i.e. table name) cache.
** @attr CacheByLength [AjPTable] Maximum length cache.
** @@
******************************************************************************/

typedef struct EnsSMetaCoordinateAdaptor
{
    EnsPDatabaseAdaptor Adaptor;
    AjPTable CacheByName;
    AjPTable CacheByLength;
} EnsOMetaCoordinateAdaptor;

#define EnsPMetaCoordinateAdaptor EnsOMetaCoordinateAdaptor*




/*
** Prototype definitions
*/

/* Ensembl Meta-Coordinate Adaptor */

EnsPMetaCoordinateAdaptor ensMetaCoordinateAdaptorNew(EnsPDatabaseAdaptor dba);

void ensMetaCoordinateAdaptorDel(EnsPMetaCoordinateAdaptor *Padaptor);

AjBool ensMetaCoordinateAdaptorFetchAllCoordSystems(
    EnsPMetaCoordinateAdaptor adaptor,
    const AjPStr name,
    AjPList cslist);

ajuint ensMetaCoordinateAdaptorGetMaximumLength(
    EnsPMetaCoordinateAdaptor adaptor,
    const EnsPCoordSystem cs,
    const AjPStr name);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
