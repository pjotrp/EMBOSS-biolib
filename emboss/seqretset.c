#include "emboss.h"

ajint main (ajint argc, char * argv[]) {

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
