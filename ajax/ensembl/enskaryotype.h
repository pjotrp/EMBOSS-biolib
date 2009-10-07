#ifdef __cplusplus
extern "C"
{
#endif

#ifndef enskaryotype_h
#define enskaryotype_h

#include "ensfeature.h"




/* @data EnsPKaryotypeBandadaptor *********************************************
**
** Ensembl Karyotype Band Adaptor.
**
** @alias EnsSKaryotypeBandadaptor
** @alias EnsOKaryotypeBandadaptor
**
** @cc Bio::EnsEMBL::BaseFeatureadaptor
** @attr Adaptor [EnsPFeatureadaptor] Ensembl Feature Adaptor.
** @@
******************************************************************************/

typedef struct EnsSKaryotypeBandadaptor
{
    EnsPFeatureadaptor Adaptor;
} EnsOKaryotypeBandadaptor;

#define EnsPKaryotypeBandadaptor EnsOKaryotypeBandadaptor*




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
** @attr Adaptor [EnsPKaryotypeBandadaptor] Ensembl Karyotype Band Adaptor.
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
    EnsPKaryotypeBandadaptor Adaptor;
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

EnsPKaryotypeBandadaptor ensKaryotypeBandGetadaptor(const EnsPKaryotypeBand kb);

ajuint ensKaryotypeBandGetIdentifier(const EnsPKaryotypeBand kb);

EnsPFeature ensKaryotypeBandGetFeature(const EnsPKaryotypeBand kb);

AjPStr ensKaryotypeBandGetName(const EnsPKaryotypeBand kb);

AjPStr ensKaryotypeBandGetStain(const EnsPKaryotypeBand kb);

AjBool ensKaryotypeBandSetadaptor(EnsPKaryotypeBand kb,
                                  EnsPKaryotypeBandadaptor kba);

AjBool ensKaryotypeBandSetIdentifier(EnsPKaryotypeBand kb, ajuint identifier);

AjBool ensKaryotypeBandSetFeature(EnsPKaryotypeBand kb, EnsPFeature feature);

AjBool ensKaryotypeBandSetName(EnsPKaryotypeBand kb, AjPStr name);

AjBool ensKaryotypeBandSetStain(EnsPKaryotypeBand kb, AjPStr stain);

AjBool ensKaryotypeBandTrace(const EnsPKaryotypeBand kb, ajuint level);

ajuint ensKaryotypeBandGetMemSize(const EnsPKaryotypeBand kb);

/* Ensembl Karyoptype Band Adaptor */

EnsPKaryotypeBandadaptor ensKaryotypeBandadaptorNew(EnsPDatabaseadaptor dba);

void ensKaryotypeBandadaptorDel(EnsPKaryotypeBandadaptor* Padaptor);

AjBool ensKaryotypeBandadaptorFetchAllByChromosomeName(
    EnsPKaryotypeBandadaptor adaptor,
    const AjPStr name,
    AjPList kblist);

AjBool ensKaryotypeBandadaptorFetchAllByChromosomeBand(
    EnsPKaryotypeBandadaptor adaptor,
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
