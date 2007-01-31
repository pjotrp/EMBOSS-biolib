
/****  NEXT.c  ***************************************************************
**
**  NEXT is used as a model for programs which use the NextSeqEntry routine
**  to process wildcard sequence specifications.
**
**  It is designed to behave inteactively if given a discrete (non-Wildcard)
**  sequence specification.  If a wildcard SeqSpec is detected only a minimal
**  about of output is given.
**
** Description:
** ----------- 
**  Instantiate "wildSpec".
**  
**  Get a sequence spec, use PIR PROTEIN as default database.  "TestSpec"
**  contains the last SeqSpec used during this process.
**
**  NEXT will continue to return valid SeqSpecs until wildSpec is exhausted.
**  If wildSpec is discreet (i.e. not wild) NextSeqEntry will exhaust after
**  the first call.
**
**  NEXT does the following:
**     1) Set up an error notification routine, here use a generic one from
**        the TTYInterface.  If Notify is not called no error will be reported.
**     2) Echo the current Sequence Name, Title and Description.
**     3) For non-wild specs ask for a subrange. Use the return value
**        to determine if the strand was reversed or not.  Reversing
**	  is done on nucleic acid sequences only.
**     4) Asks for any input parameters. For wild specs, the parameters
**        are only asked once.
**     5) Do whatever processing needs to be done on each spec.
**    
**    William Gilbert, Whitehead Institute
**    April, 1988
**
******************************************************************************/

#include <string.h>
#include "ckit.h"
#include "ttyinterface.h"

int main(int argc, char* argv[])
{
SeqSpec *wildSpec;
SeqEntry *seq;
Boolean ask = 1;

/* 
** These variables are used by the NEXT program for reporting
*/


	Notify(TTYError,TTYError,TTYError);

ReTry:	wildSpec = GetSeqSpec("NEXT on which sequence(s)","TestSpec", "PIR1");

	while ( (seq = NextSeqEntry(wildSpec)) ) {

	  printf("\n\n %s\n %s\n %s\n", seq->name, seq->title, seq->desc);

#if 0
	  if ( oneSpec ) reverse = GetRange(&begin, &end, seq);
printf("\n%ld,%ld",begin,end);
#endif
          /*
	  **  Ask questions which would like here, use the variable "ask"
	  **  to only ask these questions once.  Presumable, when using
	  **  wildcard the answers to the question would apply to all.
	  */

	  if ( ask ) {
	    ;
	    ;
	    ask = 0;
	  }

          /*
          ** The work of each program goes here.  In the case of NEXT we'll
          ** just report some interesting facts about the sequence structure
          */
#if 0

	  seq->circular ? strcpy(cString, "Circular ") : strcpy(cString,"");

	  printf("\n\n \"%s\", %s%s, %s\n Seq. Memory size: %6ld, starting at %7lX", 
	    seq->name, cString, DecodeType(seq->type), 
	    DecodeFormat(seq->spec->format), seq->size, seq->mem);
	  printf("\n Text Memory size: %6ld, starting at %7lX\n", 
	    seq->tSize, seq->text);
	  printf("\n SeqSpec starting at %7lX\n", 
	     seq->spec);

	  if ( seq->mem ) {
	    printf("\n------\n%-50.50s", seq->mem);
	    printf("\n M.W. = %lf",Weight(seq->mem,seq->type) );
	  }
	  if ( seq->text ) 
	    printf("\n-----\n%-80.80s\n-------\n", seq->text);
#endif
	  DeleteSeqEntry(seq);
        }

	printf("\n\n List exhausted...\n\n");

	ClearErrors();
	goto ReTry;
        return 0;
}
