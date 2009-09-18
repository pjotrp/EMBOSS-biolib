#ifdef __cplusplus
"C"
{
#endif

#ifndef ensintron_h
#define ensintron_h

#include "ensdata.h"




/*
** Prototype definitions
*/

/* Ensembl Intron */

EnsPIntron ensIntronNew(EnsPExon exon1, EnsPExon exon2);

EnsPIntron ensIntronNewObj(const EnsPIntron object);
    
EnsPIntron ensIntronNewRef(EnsPIntron intron);

void ensIntronDel(EnsPIntron* Pintron);

EnsPFeature ensIntronGetFeature(const EnsPIntron intron);

EnsPExon ensIntronGetPreviousExon(const EnsPIntron intron);

EnsPExon ensIntronGetNextExon(const EnsPIntron intron);

AjBool ensIntronTrace(const EnsPIntron intron, ajuint level);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
