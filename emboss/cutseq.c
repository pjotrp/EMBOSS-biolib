/* @source cutseq application
**
** Removes a specified section from a sequence
**
** @author: Copyright (C) Gary Williams (gwilliam@hgmp.mrc.ac.uk)
** @@
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/

#include "emboss.h"

int main (int argc, char * argv[]) {

  AjPSeq seq;
  AjPSeqout seqout;
  int from, to;
  AjPStr str=NULL; /* NULL definition is required for the ajStrAss() call */
  int beg, end;

  embInit ("cutseq", argc, argv);

  seq = ajAcdGetSeq ("sequence");
  from = ajAcdGetInt ("from")-1;
  to = ajAcdGetInt ("to")-1;
  seqout = ajAcdGetSeqout ("outseq");

  beg = ajSeqBegin(seq)-1;
  end = ajSeqEnd(seq)-1;
        
/* get a COPY of the sequence string */
  (void) ajStrAssSub (&str, ajSeqStr(seq), beg, end);
  (void) ajStrCut(&str, from-beg, to-beg);

  (void) ajSeqReplace(seq, str);

  (void) ajSeqWrite (seqout, seq);
  (void) ajSeqWriteClose (seqout);

  ajExit ();
  return 0;
}

