/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/* alphtype.c */
/*	
	alphtype <alphabet>

	<alphabet>	MAST alphabet string

	Prints "DNA" or "PROTEIN" if the alphabet is one of the
	recognized ones.  Prints an error message to standard error
	otherwise.
*/
	
#define DEFINE_GLOBALS 
#include "macros.h"
#include "hash_alph.h"
#include "meme.h"

extern int main(
  int argc,
  char **argv
)
{
  char *oldalph = argv[1];			/* old alphabet */
  int *dummy[MAXASCII];
  char *newalph = get_alphabet(oldalph, dummy);

  if (strcmp(newalph, DNAB) == 0) {
    ajFmtPrintF(outf,"DNA\n");
  } else if (strcmp(newalph, PROTEINB) == 0) {
    ajFmtPrintF(outf,"PROTEIN\n");
  } else {
    return 1;
  }
  return 0;
}
  
