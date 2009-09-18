#ifdef __cplusplus
extern "C"
{
#endif

#ifndef enskaryotype_h
#define enskaryotype_h

#include "ensfeature.h"




/* @data EnsPKaryotypeBandAdaptor *********************************************
**
** Ensembl Karyotype Band Adaptor.
**
** @alias EnsSKaryotypeBandAdaptor
** @alias EnsOKaryotypeBandAdaptor
**
** @cc Bio::EnsEMBL::BaseFeatureAdaptor
** @attr Adaptor [EnsPFeatureAdaptor] Ensembl Feature Adaptor.
** @@
******************************************************************************/

typedef struct EnsSKaryotypeBandAdaptor
{
    EnsPFeatureAdaptor Adaptor;
} EnsOKaryotypeBandAdaptor;

#define EnsPKaryotypeBandAdaptor EnsOKaryotypeBandAdaptor*




/* @data EnsPKaryotypeBand ****************************************************
**
** Ensembl Karyotype Band.
**
** @alias EnsSKaryotypeBand
** @alias EnsOKaryotypeBand
**
** @attr Use [ajuint] Use counter.
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key).
** @attr Adaptor [EnsPKaryotypeBandAdaptor] Ensembl Karyotype Band Adaptor.
** @cc Bio::EnsEMBL::KaryotypeBand
** @attr Feature [EnsPFeature] Ensembl Feature.
** @attr Name [AjPStr] Karyotype Band Name.
** @attr Stain [AjPStr] Karyotype Band Stain.
** @@
******************************************************************************/

typedef struct EnsSKaryotypeBand
{
    ajuint Use;
    ajuint Identifier;
    EnsPKaryotypeBandAdaptor Adaptor;
    EnsPFeature Feature;
    AjPStr Name;
    AjPStr Stain;
} EnsOKaryotypeBand;

#define EnsPKaryotypeBand EnsOKaryotypeBand*




/*
** Prototype definitions
*/

/* Ensembl Karyotype Band */

EnsPKaryotypeBand ensKaryotypeBandNew(ajuint identifier,
                                      EnsPFeature feature,
                                      AjPStr name,
                                      AjPStr stain);

EnsPKaryotypeBand ensKaryotypeBandNewObj(EnsPKaryotypeBand object);

EnsPKaryotypeBand ensKaryotypeBandNewRef(EnsPKaryotypeBand kb);

void ensKaryotypeBandDel(EnsPKaryotypeBand* Pkb);

EnsPKaryotypeBandAdaptor ensKaryotypeBandGetAdaptor(const EnsPKaryotypeBand kb);

ajuint ensKaryotypeBandGetIdentifier(const EnsPKaryotypeBand kb);

EnsPFeature ensKaryotypeBandGetFeature(const EnsPKaryotypeBand kb);

AjPStr ensKaryotypeBandGetName(const EnsPKaryotypeBand kb);

AjPStr ensKaryotypeBandGetStain(const EnsPKaryotypeBand kb);

AjBool ensKaryotypeBandSetAdaptor(EnsPKaryotypeBand kb,
                                  EnsPKaryotypeBandAdaptor kba);

AjBool ensKaryotypeBandSetIdentifier(EnsPKaryotypeBand kb, ajuint identifier);

AjBool ensKaryotypeBandSetFeature(EnsPKaryotypeBand kb, EnsPFeature feature);

AjBool ensKaryotypeBandSetName(EnsPKaryotypeBand kb, AjPStr name);

AjBool ensKaryotypeBandSetStain(EnsPKaryotypeBand kb, AjPStr stain);

AjBool ensKaryotypeBandTrace(const EnsPKaryotypeBand kb, ajuint level);

ajuint ensKaryotypeBandGetMemSize(const EnsPKaryotypeBand kb);

/* Ensembl Karyoptype Band Adaptor */

EnsPKaryotypeBandAdaptor ensKaryotypeBandAdaptorNew(EnsPDatabaseAdaptor dba);

void ensKaryotypeBandAdaptorDel(EnsPKaryotypeBandAdaptor* Padaptor);

AjBool ensKaryotypeBandAdaptorFetchAllByChromosomeName(
    EnsPKaryotypeBandAdaptor adaptor,
    const AjPStr name,
    AjPList kblist);

AjBool ensKaryotypeBandAdaptorFetchAllByChromosomeBand(
    EnsPKaryotypeBandAdaptor adaptor,
    const AjPStr name,
    const AjPStr band,
    AjPList kblist);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
