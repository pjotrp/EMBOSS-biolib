/* @source crystalball application
**
** Answers every drug discovery question you have about
**  this sequence
**
** @author: Copyright (C) Scott Markel and Darryl Leon
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


/* @prog crystalball **********************************************************
**
** Answers every drug discovery question you have about
**  this sequence
**
******************************************************************************/

int main(int argc, char **argv)
{

    AjPFile outf=NULL;
    AjPSeq seq = NULL;
    AjBool competition;
    AjBool rdtime;
    AjBool rdcost;
    AjBool animalstudies;
    AjBool clinicaltrials;
    AjBool fdaproblems;
    AjBool fdatime;
    AjBool profit;
    AjBool everythingelse;

    ajint mintime=10;
    ajint maxtime=15;
    float fcost;

    embInit ("crystalball", argc, argv);

    seq = ajAcdGetSeq ("sequence");
    outf = ajAcdGetOutfile("outfile");

    competition = ajAcdGetBool("competition");

    rdtime = ajAcdGetBool ("rdtime");
    rdcost = ajAcdGetBool ("rdcost");
    animalstudies = ajAcdGetBool ("animalstudies");
    clinicaltrials = ajAcdGetBool ("clinicaltrials");
    fdaproblems = ajAcdGetBool ("fdaproblems");
    fdatime = ajAcdGetBool ("fdatime");
    profit = ajAcdGetBool ("profit");
    everythingelse = ajAcdGetBool ("everythingelse");

    if (rdtime)
      ajFmtPrintF(outf, "Drug discovery development time: %d-%d years\n",
		  mintime, maxtime);

    if (rdcost) {
      fcost = ((float) (maxtime+mintime)/2.0) * 64;
      ajFmtPrintF (outf, "Drug discovery development cost: $%d million\n",
		   (ajint) fcost);

    }

    if (animalstudies)
      ajFmtPrintF (outf, "Animal studies results: 'Animals are not like humans'\n");
    if (clinicaltrials)
      ajFmtPrintF (outf, "Clinical trials surprises: Unfortunately, not another Viagra\n");

    if (fdaproblems)
      ajFmtPrintF (outf, "FDA issues: List not yet complete\n");

    if (fdatime)
      ajFmtPrintF (outf, "FDA time to decision: Waiting .........\n");
		   
    if (profit)
      ajFmtPrintF (outf, "Profit: Not nearly as much as you'd like!\n");

    if (competition)
      ajFmtPrintF (outf, "The competition: Every EMBOSS user now knows as much as you!\n");

    if (everythingelse)
      ajFmtPrintF (outf, "Everything else: Trust me, you really don't want to know just now!\n");

    ajFileClose (&outf);

    ajExit ();
    return 0;
}
