/*!***  common  ************************************************************ */ 
/*!*   */ 
/*!*  	common block for pk23 */ 
/*!*   */ 
/*!****************************************************************************** */ 
 
/*#include "sequence.h"*/
#define maxseqlen 350000	/* longest sequence length*/
#define maxpathlen 1024
#define maxseqname 21	/* length of sequence names*/
#define maxaccnum  8	/* length of acc. numbers*/
#define maxdocstring  256/* length of seq. definition*/
                                /* (actual limit is 1 less)*/

typedef struct sequence {
  int check                 ;/* checksum [for the whole entry]*/
  int len                   ;/* entry length*/
  int rev                   ;/* strand of interest (.false.="+")*/
  int format                ;/* sqascii,sq2bit,sq4bit*/
  int type                  ;/* sqprotein, sqnucleic, sq_type_unset*/
  int isnbrf                ;/* true if from nbrf data lib.*/
  int ismsf                 ;/* true if from msf file.*/
  int circular		;/* true if sequence is circular*/
  int begin			;/* begin position*/
  int end			;/*end subroutine*/
  int join			;/* sq_join_unset,sqnojoin,sqjoin*/
  int offset                ;/* offset to sequence*/
  int direction		;/* strand of stored seq*/
  float weight                   ;/* relative importance of seq.*/
  char name[maxseqname+    /* the complete sequence name,*/
	   maxpathlen+1]  ;/* including the library/direct.*/
  char joinname[maxseqname+1]; /* join sequence name.*/
  char number[maxaccnum+1] ;/* accession number for sequence*/
  char doc[maxdocstring+1] ;/* sequence definition*/
  char seq[maxseqlen+1]    ;/* current sequence*/
} sequence;

/*!end */ 
