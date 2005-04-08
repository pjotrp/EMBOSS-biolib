/* @source cutgextract application
**
** Create EMBOSS codon usage files from the CUTG database
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

#define CODONS 64
#define TABLE_ESTIMATE 1000

typedef struct CutgSValues
{
    ajint Count[CODONS];
    ajint CdsCount;
    AjPStr Division;
    AjPStr Doc;
    AjPStr Species;
} CutgOValues;
#define CutgPValues CutgOValues*


static char *cutgextract_next(AjPFile inf, AjPStr wildspecies,
			      AjPStr* pspecies, AjPStr* pdoc);
static void cutgextract_readcodons(AjPFile inf, ajint *count);




/* @prog cutgextract **********************************************************
**
** Extract data from CUTG.
**
** Reads all *.codon files in the input, and builds a table for each organism.
** Writes the tables to the EMBOSS_DATA/CODONS directory at the end.
**
******************************************************************************/

int main(int argc, char **argv)
{
    char *codons[]=
    {
	"TAG","TAA","TGA","GCG","GCA","GCT","GCC","TGT", /* 00-07 */
	"TGC","GAT","GAC","GAA","GAG","TTT","TTC","GGT", /* 08-15 */
	"GGG","GGA","GGC","CAT","CAC","ATA","ATT","ATC", /* 16-23 */
	"AAA","AAG","CTA","TTA","TTG","CTT","CTC","CTG", /* 24-31 */
	"ATG","AAT","AAC","CCG","CCA","CCT","CCC","CAA", /* 32-39 */
	"CAG","CGT","CGA","CGC","AGG","AGA","CGG","TCG", /* 40-47 */
	"TCA","AGT","TCT","TCC","AGC","ACG","ACT","ACA", /* 48-55 */
	"ACC","GTA","GTT","GTC","GTG","TGG","TAT","TAC"	 /* 56-63 */
    };

    char *aa=
	"***AAAACCDDEEFFGGGGHHIIIKKLLLLLLMNNPPPPQQRRRRRRSSSSSSTTTTVVVVWYY";

    const char* thiscodon = NULL;

    AjPFile inf     = NULL;
    AjPFile outf    = NULL;
    char *entryname = NULL;
    AjPStr fname    = NULL;
    AjPStr line     = NULL;
    AjPStr key      = NULL;
    AjPStr tmpkey   = NULL;

    AjPTable table  = NULL;
    ajint i = 0;
    ajint j = 0;
    ajint k = 0;
    ajint x = 0;

    AjPStr *array = NULL;
    AjPCod codon  = NULL;
    ajint sum = 0;
    char c;

    AjPList flist = NULL;
    AjPFile logf = NULL;
    AjPStr  entry = NULL;
    AjPStr  wild  = NULL;
    AjPStr division = NULL;
    AjPStr release = NULL;
    AjPStr wildspecies = NULL;
    CutgPValues value = NULL;
    AjPStr docstr = NULL;
    AjPStr species = NULL;
    AjPStr filename = NULL;

    embInit("cutgextract",argc,argv);
    ajint gc[3];
    ajint totgc;

    line   = ajStrNew();
    tmpkey = ajStrNew();
    fname  = ajStrNew();


    table = ajStrTableNew(TABLE_ESTIMATE);


    flist = ajAcdGetDirlist("directory");
    wild  = ajAcdGetString("wildspec");
    release  = ajAcdGetString("release");
    logf = ajAcdGetOutfile("outfile");
    wildspecies = ajAcdGetString("species");
    filename = ajAcdGetString("filename");

    while(ajListPop(flist,(void **)&entry))
    {
	if(!ajStrMatchWild(entry,wild))
	{
	    ajStrDel(&entry);
	    continue;
	}

	inf = ajFileNewIn(entry);
	if(!inf)
	    ajFatal("cannot open file %S",entry);

	ajFmtPrintS(&division, "%F", inf);
	ajFileNameShorten(&division);

	while((entryname = cutgextract_next(inf, wildspecies,
					    &species, &docstr)))
	{
	    if(ajStrLen(filename))
		ajStrAssS(&tmpkey,filename);
	    else
		ajStrAssC(&tmpkey,entryname);

	    /* See if organism is already in the table */
	    value = ajTableGet(table,tmpkey);
	    if(!value)			/* Initialise */
	    {
		key = ajStrNewS(tmpkey);
		AJNEW0(value);
		ajStrAssS(&value->Species,species);
		ajStrAssS(&value->Division, division);
		ajTablePut(table,(const void *)key,(void *)value);
	    }
	    cutgextract_readcodons(inf,value->Count);
	    value->CdsCount++;
	}
	ajStrDel(&entry);
	ajFileClose(&inf);
    }

    array = (AjPStr *) ajTableToarray(table,NULL);

    i = 0;
    while(array[i])
    {
	key   = array[i++];
	value = (CutgPValues) array[i++];
	codon = ajCodNew();
	sum   = 0;
	totgc = 0;
	gc[0] = gc[1] = gc[2] = 0;
	for(j=0;j<CODONS;++j)
	{
	    sum += value->Count[j];
	    x = ajCodIndexC(codons[j]);
	    codon->num[x] = value->Count[j];
	    thiscodon = codons[x];

	    for(k=0;k<3;k++)
	    {
		if(thiscodon[k] == 'C' || thiscodon[k] == 'G')
		{
		    totgc += value->Count[j];
		    gc[k] += value->Count[j];
		}
	    }
	    ajDebug("x:%d thiscodon: '%s' count: %d totgc: %d gc: %d %d %d\n",
		    x, thiscodon, value->Count[j], totgc, gc[0], gc[1], gc[2]);
	    c = aa[j];
	    if(c=='*')
		codon->aa[x] = 27;
	    else
		codon->aa[x] = c-'A';

	}
	ajCodCalculateUsage(codon,sum);

	ajFmtPrintF(logf, "Writing %S.cut\n", key);

	ajFmtPrintS(&fname,"CODONS/%S.cut",key);
	ajFileDataNewWrite(fname,&outf);
	if(!outf)
	    ajFatal("Cannot open output file %S",fname);
	ajFmtPrintF(outf, "#Species: %S\n", value->Species);
	ajFmtPrintF(outf, "#Division: %S\n", value->Division);
	if(ajStrLen(release))
	    ajFmtPrintF(outf, "#Release: %S\n", release);
	ajFmtPrintF(outf, "#CDSs: %d\n", value->CdsCount);
	ajFmtPrintF(outf, "#Codons: %d\n", sum);
	ajFmtPrintF(outf, "\n", value->CdsCount);
	ajFmtPrintF(outf, "#Coding GC: %.2f%%\n",
		    (100.0 * (float)totgc/(float)sum/3.0));
	ajFmtPrintF(outf, "#1st letter GC: %.2f%%\n",
		    (100.0 * (float)gc[0]/(float)sum));
	ajFmtPrintF(outf, "#2nd letter GC: %.2f%%\n",
		    (100.0 * (float)gc[1]/(float)sum));
	ajFmtPrintF(outf, "#3rd letter GC: %.2f%%\n",
		    (100.0 * (float)gc[2]/(float)sum));
	ajFmtPrintF(outf, "\n", value->CdsCount);
	ajFmtPrintF(outf, "#Codon AA Fraction Frequency Number\n");
	ajCodWrite(codon, outf);
	ajFileClose(&outf);


	ajStrDel(&key);
	AJFREE(value);
	ajCodDel(&codon);
    }

    ajTableFree(&table);
    ajListDel(&flist);

    ajExit();

    return 0;
}



/* @funcstatic cutgextract_next ***********************************************
**
** Reads the first line of a CUTG database entry,
** returning the name of the species.
**
** Each entry has one line beginning with '>' followed by fields
** delimited by a backslash:
**
** GenBank entry name
** GenBank accession number
** GenBank feature location
** Length of feature in nucleotides
** Protein ID
** Genus and species
** Genbank entry description and feature qualifiers
**
** The second line is 64 integers giving the number of times each codon
** appears in this coding sequence.
**
** @param [u] inf [AjPFile] Input CUTG database file
** @param [r] wildspecies [AjPStr] Wildcard species to select
** @param [w] pspecies [AjPStr*] Species for this entry
** @param [w] pdoc [AjPStr*] Documentation for this entry
** @return [char*] Undocumented
** @@
******************************************************************************/

static char* cutgextract_next(AjPFile inf, AjPStr wildspecies,
			      AjPStr* pspecies, AjPStr* pdoc)
{
    static AjPStr line = NULL;
    static AjPStr org  = NULL;
    AjPStrTok handle    = NULL;
    AjPStr token = NULL;
    ajint i;
    ajint len;
    char *p = NULL;
    char c;
    AjBool done = ajFalse;

    if(!line)
    {
	line = ajStrNew();
	org  = ajStrNew();
    }

    ajStrAssC(&line,"");
    ajStrAssC(pdoc,"");
    while (!done)
    {

	while(*ajStrStr(line) != '>')
	    if(!ajFileReadLine(inf,&line))
		return NULL;

	handle = ajStrTokenInit(line,"\\\n\t\r");
	for(i=0;i<7;++i) {
	    ajStrToken(&token,&handle,"\\\n\t\r");
	    if(i==5)
	    {
		ajStrAssC(&org,"E");
		ajStrApp(&org, token);
		ajStrAssS(pspecies, token);
		if(ajStrMatchWild(token,wildspecies))
		{
		    done = ajTrue;
		}
	    }

	    switch(i)
	    {
	    case 0:
		ajStrAppC(pdoc, "#ID ");
		ajStrApp(pdoc, token);
		ajStrAppC(pdoc, "\n");
		break;
	    case 1:
		ajStrAppC(pdoc, "#AC ");
		ajStrApp(pdoc, token);
		ajStrAppC(pdoc, "\n");
		break;
	    case 2:
		ajStrAppC(pdoc, "#FT ");
		ajStrApp(pdoc, token);
		ajStrAppC(pdoc, "\n");
		break;
	    case 3:
		ajStrAppC(pdoc, "#FL ");
		ajStrApp(pdoc, token);
		ajStrAppC(pdoc, "\n");
		break;
	    case 4:
		ajStrAppC(pdoc, "#PI ");
		ajStrApp(pdoc, token);
		ajStrAppC(pdoc, "\n");
		break;
	    case 5:
		ajStrAppC(pdoc, "#OS ");
		ajStrApp(pdoc, token);
		ajStrAppC(pdoc, "\n");
		break;
	    case 6:
		ajStrAppC(pdoc, "#DE ");
		ajStrApp(pdoc, token);
		ajStrAppC(pdoc, "\n");
		break;
	    default:
		break;
	    }
	}

	ajStrTokenClear(&handle);
	if(!done)
	    if(!ajFileReadLine(inf,&line))
		return NULL;
    }

    len = ajStrLen(org);
    p   = ajStrStrMod(&org);
    for(i=0;i<len;++i)
    {
	c = p[i];
	if(c=='/' || c==' ' || c=='.' || c=='\'')
	    p[i]='_';
    }

    if(p[strlen(p)-1]=='_')
	p[strlen(p)-1]='\0';
    ajStrDel(&token);

    return p;
}




/* @funcstatic cutgextract_readcodons *****************************************
**
** Reads the codon frequency line from a CUTG database entry.
**
** Codons are reported according to the number of codons per amino acid
** (6, 4, 2, 3, 1, stop) in the order:
** 
** CGA CGC CGG CGU AGA AGG CUA CUC CUG CUU UUA UUG UCA UCC UCG UCU AGC AGU
**  R   R   R   R   R   R   L   L   L   L   L   L   S   S   S   S   S   S
** ACA ACC ACG ACU CCA CCC CCG CCU GCA GCC GCG GCU GGA GGC GGG GGU
**  T   T   T   T   P   P   P   P   A   A   A   A   G   G   G   G
** GUA GUC GUG GUU AAA AAG AAC AAU CAA CAG CAC CAU GAA GAG GAC GAU
**  V   V   V   V   K   K   N   N   Q   Q   H   H   E   E   D   D
** UAC UAU UGC UGU UUC UUU AUA AUC AUU AUG UGG UAA UAG UGA
**  Y   Y   C   C   F   F   I   I   I   M   W   *   *   *
** @param [u] inf [AjPFile] Input CUTG database file
** @param [w] count [ajint*] Codon usage total so far for this species
** @@
******************************************************************************/

static void cutgextract_readcodons(AjPFile inf, ajint *count)
{
    static int cutidx[] =
    {
	42,43,46,41,45,44,26,30,31,29,27,28,48,51,47,50,
	52,49,55,56,53,54,36,38,35,37, 4, 6, 3, 5,17,18,
	16,15,57,59,60,58,24,25,34,33,39,40,20,19,11,12,
	10, 9,63,62, 8, 7,14,13,21,23,22,32,61, 1, 0, 2
    };
    static AjPStr line  = NULL;
    static AjPStr value = NULL;

    AjPStrTok token = NULL;
    ajint i;
    ajint n = 0;


    if(!line)
    {
	line  = ajStrNew();
	value = ajStrNew();
    }

    if(!ajFileReadLine(inf,&line))
	ajFatal("Premature end of file");


    token = ajStrTokenInit(line," \n\t\r");
    for(i=0;i<CODONS;++i)
    {
	ajStrToken(&value,&token," \n\t\r");
	ajStrToInt(value,&n);
	count[cutidx[i]] += n;
    }

    ajStrTokenClear(&token);

    return;
}
