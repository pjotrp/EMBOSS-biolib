#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajbase_h
#define ajbase_h

#include "ajax.h"
#include <ctype.h>

typedef struct AjIUB AjIUB;
struct AjIUB
{
    AjPStr code;
    AjPStr list;
};




/*
** Prototype definitions
*/

const AjPStr  ajBaseGetCodes(ajint ibase);
char          ajBaseAlphacharComp(char base);
void          ajBaseExit(void);
float         ajBaseAlphaCompare(ajint base1, ajint base2);

ajint         ajBaseAlphaToBin(ajint c);
char          ajBaseBinToAlpha(ajint c);

char          ajBaseAlphacharToBin(char c);
float         ajBaseAlphacharCompare(char c, char c2);

ajint         ajBasecodeToInt(ajint c);
ajint         ajBasecodeFromInt(ajint n);

ajint         ajResidueAlphaToBin(ajint c);
char          ajResidueBinToAlpha(ajint c);
const AjPStr  ajResidueGetCodes(ajint ires);
AjBool        ajResidueFromTriplet(const AjPStr aa3, char *Pc);
AjBool        ajResidueToTriplet(char c, AjPStr *Paa3);

/*
** deprecated prototype  definitions
*/

__deprecated AjBool        ajBaseAa1ToAa3(char aa1, AjPStr *aa3);
__deprecated AjBool        ajBaseAa3ToAa1(char *aa1, const AjPStr aa3);
__deprecated const AjPStr  ajBaseCodes(ajint ibase);
__deprecated char          ajSeqBaseComp (char base);
__deprecated ajint         ajAZToInt(ajint c);
__deprecated ajint         ajAZToBin(ajint c);
__deprecated char          ajAZToBinC(char c);
__deprecated char          ajBinToAZ(ajint c);
__deprecated ajint         ajIntToAZ(ajint n);
__deprecated char          ajBaseComp(char base);
__deprecated float         ajBaseProb(ajint base1, ajint base2);

/*
** End of prototype definitions
*/

#endif


#ifdef __cplusplus
}
#endif
