#include "emboss.h"


/* @prog seqretallfeat ********************************************************
**
** Reads and writes (returns) one or more sequences
**
******************************************************************************/

int main(int argc, char **argv)
{

  AjPSeqout seqout;
  AjPSeqall seqall;
  AjPSeq seq = NULL;

  embInit ("seqretallfeat", argc, argv);
  ajFeatTest();

  seqout = ajAcdGetSeqoutall ("outseq");
  seqall = ajAcdGetSeqall ("sequence");

  while (ajSeqallNext(seqall, &seq)) {
    ajSeqAllWrite (seqout, seq);
    ajSeqTrace (seq);
  }
  ajSeqWriteClose (seqout);

  ajExit ();
  return 0;
}
