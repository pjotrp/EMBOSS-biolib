#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensstorable_h
#define ensstorable_h

#include "ajax.h"
#include "ensdatabaseconnection.h"




/******************************************************************************
**
** Ensembl Storable Type enumeration.
**
******************************************************************************/

enum EnsEStorableType
{
    ensEStorableTypeNULL,
    ensEStorableTypeAnalysis,
    ensEStorableTypeRepeatConsensus
};




/*
** Including an object Adaptor is a problem, as object adaptors
** needed correct typing.
** Object Adaptors need to be part of the object structures.
*/

/* @data EnsPStorable *********************************************************
**
** Ensembl Storable.
**
** @alias EnsSStorable
** @alias EnsOStorable
**
** @attr Use [ajuint] Use counter.
** @attr Identifier [ajuint] Internal SQL database identifier (primary key).
** @attr Adaptor [void*] Ensembl Object Adaptor.
** @attr Type [AjEnum] Ensembl Storable Object Type.
** @attr Padding [ajuint] Padding to alignment boundary.
**
** @@
******************************************************************************/

typedef struct EnsSStorable
{
    ajuint Use;
    ajuint Identifier;
    void *Adaptor;
    AjEnum Type;
    ajuint Padding;
} EnsOStorable;

#define EnsPStorable EnsOStorable*




/*
** Prototype definitions
*/

EnsPStorable ensStorableNew(AjEnum type, ajuint identifier, void* adaptor);

EnsPStorable ensStorableNewObj(const EnsPStorable object);

EnsPStorable ensStorableNewRef(EnsPStorable storable);

void ensStorableDel(EnsPStorable* Pstorable);

AjEnum ensStorableGetType(const EnsPStorable storable);

void *ensStorableGetadaptor(const EnsPStorable storable);

ajuint ensStorableGetIdentifier(const EnsPStorable storable);

AjBool ensStorableSetIdentifier(EnsPStorable storable, ajuint identifier);

AjBool ensStorableSetadaptor(EnsPStorable storable, void* adaptor);

AjBool ensStorableIsStored(const EnsPStorable storable,
                           const EnsPDatabaseConnection dbc);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif

/*
** FIXME: We could include just a Database Adaptor as the generic adaptor and a
** enum for the Adaptor type. With this information it is always possible to
** get the required adaptor via the registry.
*/
