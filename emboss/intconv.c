/* @source intconv application
**
** Convert ints to ajints and longs to ajlongs
**
** @author: Copyright (C) Alan Bleasby (ableasby@hgmp.mrc.ac.uk)
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
#include <string.h>

void modLineInt(AjPStr *line, ajint pos);
void modLineLong(AjPStr *line, ajint pos);



int main(int argc, char **argv)
{
    AjPFile inf  = NULL;
    AjPFile outf = NULL;
    AjPStr  line = NULL;
    ajint     pos;
    char    *p;
    char    *q;
    char    c;
    char    d='\0';
    
    (void) embInit("intconv", argc, argv);

    inf  = ajAcdGetInfile("infile");
    outf = ajAcdGetOutfile("outfile");

    line = ajStrNew();

    while(ajFileReadLine(inf, &line))
    {
	if(ajStrPrefixC(line,"ajint main"))
	{
	    ajFmtPrintF(outf,"int main(int argc, char **argv)\n");
	    p=ajStrStr(line);
	    if(p[strlen(p)-1]=='{')
		ajFmtPrintF(outf,"{\n");
	}
	else
	    ajFmtPrintF(outf,"%S\n",line);
    }
    
    ajStrDel(&line);
	
    ajExit();
    return 0;
}


void modLineInt(AjPStr *line, ajint pos)
{
    AjPStr t;
    char   *p;
    
    t = ajStrNew();
    p = ajStrStr(*line);
    
    if(pos)
	ajStrAssSubC(&t,p,0,pos-1);
    else
	ajStrAssC(&t,"");
    
    ajStrAppC(&t,"ajint");
    p += (pos+3);
    ajStrAppC(&t,p);

    ajStrAssC(line,ajStrStr(t));


    ajStrDel(&t);
    return;
}


void modLineLong(AjPStr *line, ajint pos)
{
    AjPStr t;
    char   *p;
    
    t = ajStrNew();
    p = ajStrStr(*line);
    

    if(pos)
	ajStrAssSubC(&t,p,0,pos-1);
    else
	ajStrAssC(&t,"");

    ajStrAppC(&t,"ajlong");
    p += (pos+4);
    ajStrAppC(&t,p);

    ajStrAssC(line,ajStrStr(t));

    ajStrDel(&t);
    return;
}

