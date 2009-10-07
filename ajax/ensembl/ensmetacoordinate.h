#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensmetacoordinate_h
#define ensmetacoordinate_h

#include "enstable.h"
#include "enscoordsystem.h"




/* @data EnsPMetaCoordinateadaptor ********************************************
**
** Ensembl Meta-Coordinate Adaptor.
**
** @alias EnsSMetaCoordinateadaptor
** @alias EnsOMetaCoordinateadaptor
**
** @attr Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor.
** @attr CacheByName [AjPTable] Ensembl Feature name (i.e. table name) cache.
** @attr CacheByLength [AjPTable] Maximum length cache.
** @@
******************************************************************************/

typedef struct EnsSMetaCoordinateadaptor
{
    EnsPDatabaseadaptor Adaptor;
    AjPTable CacheByName;
    AjPTable CacheByLength;
} EnsOMetaCoordinateadaptor;

#define EnsPMetaCoordinateadaptor EnsOMetaCoordinateadaptor*




/*
** Prototype definitions
*/

/* Ensembl Meta-Coordinate Adaptor */

EnsPMetaCoordinateadaptor ensMetaCoordinateadaptorNew(EnsPDatabaseadaptor dba);

void ensMetaCoordinateadaptorDel(EnsPMetaCoordinateadaptor *Padaptor);

AjBool ensMetaCoordinateadaptorFetchAllCoordSystems(
    EnsPMetaCoordinateadaptor adaptor,
    const AjPStr name,
    AjPList cslist);

ajuint ensMetaCoordinateadaptorGetMaximumLength(
    EnsPMetaCoordinateadaptor adaptor,
    const EnsPCoordSystem cs,
    const AjPStr name);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
