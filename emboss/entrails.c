#include "emboss.h"

/* @prog entrails *******************************************************
**
** Reports internal data structures as a guide for user interface developers
**
******************************************************************************/

int main(int argc, char **argv)
{

  AjPFile outf;
  AjBool full;

  embInit ("entrails", argc, argv);

  outf = ajAcdGetOutfile ("outfile");
  full = ajAcdGetBool ("fullreport");

  ajAcdPrintType (outf, full);
  ajSeqPrintInFormat (outf, full);
  ajSeqPrintOutFormat (outf, full);

  ajGraphPrintType (outf, full);
  
  ajExit ();
  return 0;
}
