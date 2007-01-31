#ifndef  _H_SEQSPEC
#define _H_SEQSPEC

/***  SeqSpec.h  **********************************************************
**
** Sequence specification - Composed of individual parts.  Flags will control 
** processing.
**
**  William A. Gilbert, Whitehead Institute
**
****************************************************************************/
#include "boolean.h"
#include "seqdef.h"

typedef struct SeqSpec SeqSpec;
struct SeqSpec {
     char *code;          /* Code word, if used by the foramt               */
     char *file;          /* File with pathname or database name            */
     char *frag;          /* Fragment spec                                  */
     char *options;       /* Options spec                                   */
  Boolean isUser;         /* TRUE if this is a user-entry sequence          */
  Boolean isWildCode;     /* TRUE if CODE contains Wild card characters     */
  Boolean isWildFile;     /* TRUE if FILE contains Wild card characters     */
   Choice format;         /* Sequence format for user-entry file            */
};
#include "ckittypes.h"


#endif


