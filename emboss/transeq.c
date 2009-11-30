/* @source transeq application
**
** Translate nucleic acid sequences
**
** @author Copyright (C) Gary Williams (gwilliam@hgmp.mrc.ac.uk)
** Mar  4 17:18 1999 (ajb)
** Jul 19 19:24 2000 (ajb)
** Jun 29 16:50 2001 (gww) use new version of ajTrnSeqOrig
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
#include "function/emboss_transeq.h"

/* @prog transeq **************************************************************
**
** Translate nucleic acid sequences
**
******************************************************************************/

int main(int argc, char **argv)
{
  int result;

  embInit("transeq", argc, argv);

  result = transeq_acd(argc, argv);

  embExit();

  return result;
}


