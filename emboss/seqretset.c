#include "emboss.h"


/* @prog seqretset ************************************************************
**
** Reads and writes (returns) a set of sequences all at once
**
******************************************************************************/

int main(int argc, char **argv)
{

  AjPSeqset seqset;
  AjPSeqout seqout;

  embInit ("seqretset", argc, argv);

  seqset = ajAcdGetSeqset ("sequence1");
  seqout = ajAcdGetSeqoutset ("outseq2");

  ajSeqsetWrite (seqout, seqset);

  ajSeqWriteClose (seqout);

  ajExit ();
  return 0;
}
