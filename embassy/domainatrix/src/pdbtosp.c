/* @source pdbtosp application
**
** Convert raw swissprot:pdb equivalence file to embl-like format.
**
** @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
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
** 

*******************************************************************************
**  Application name
**  pdbtosp 
**  
**
**
**  Summary
**  Convert raw swissprot:pdb equivalence file to embl-like format.
**
**
**
**  Input and Output
**  pdbtosp parses the swissprot:pdb equivalence table available at URL (1)
**  (1) http://www.expasy.ch/cgi-bin/lists?pdbtosp.txt
**  and writes the data out in embl-like format file (Figure 2). The raw (input)
**  file can be obtained by doing a "save as ... text format" from the web page 
**  (1).   No changes are made to the data other than changing the format in 
**  which it is held.  The input and output files are specified by the user.
**   
**
**  
**  Sister applications
**  scopseqs uses the pdbtosp output file as input.
**
**  
**  
**  Notes
**  pdbtosp is used to create the emboss data file Epdbtosp.dat that is 
**  included in the emboss distribution.
**
**  
**  
**  Known bugs & warnings
**  
**  
**  
**  Description
**  This program is part of a suite of EMBOSS applications that directly or 
**  indirectly make use of the protein structure databases pdb and scop.  
**  This program is part of an experimental analysis pipeline described in an
**  accompanying document.  We provide the software in the hope that it will
**  be useful.  The applications were designed for specific research purposes
**  and may not be useful or reliable in contexts other than the described 
**  pipeline.  The development of the suite was coordinated by Jon Ison to
**  whom enquiries and bug reports should be sent (email jison@hgmp.mrc.ac.uk).
**  
**  Some research applications require knowledge of the database sequence(s)
**  that corresponds to the sequence(s) given in a pdb file.  A 'swissprot:pdb
**  equivalence' file listing accession numbers and swissprot database 
**  identifiers for certain pdb code is available, but is not in a format that
**  is consistent with flat file formats used for protein structural data in 
**  emboss.  pdbtosp parses the swissprot:pdb equivalence in its raw format 
**  and converts it to an embl-like format.
** 
**  
**  
**  Algorithm
**  pdbtosp relies on finding a line beginning with '____  _' in the input file
**  (all lines up to and including this one are ignored). Lines of code data
**  are then parsed, up until the first blank line.
**  
**  
**  
**  Usage 
**  An example of interactive use of pdbtosp is shown below.
**  
**  Unix % pdbtosp
**  Convert raw swissprot:pdb equivalence file to embl-like format
**  Name of raw swissprot:pdb equivalence file (input): /test_data/pdbtosp.txt
**  Name of swissprot:pdb equivalence file (embl format output) [Epdbtosp.dat]: /test_data/Epdbtosp.dat
**  
**  The raw swissprot:pdb equivalence file /test_data/pdbtosp.txt was parsed 
**  and a new swissprot:pdb equivalence file in embl-like format called 
**  /test_data/Epdbtosp.dat was written.
**  
**  The following command line would achieve the same result.
**  pdbtosp /test_data/pdbtosp.txt /test_data/Epdbtosp.dat
**  
**  
**  
**  Input file format
**  Excerpt from the swissprot:pdb equivalence table.
** 
**    ------------------------------------------------------------------------
**    ExPASy Home page   Site Map    Search ExPASy   Contact us    SWISS-PROT
** 
**  Hosted by SIB       Mirror                                        USA[new]
**  Switzerland         sites:      AustraliaCanada China Korea Taiwan
**   ------------------------------------------------------------------------
** 
**  < Other bibliographic information ommitted for clarity > 
**  
**  PDB   Last revision
**  code  date           SWISS-PROT entry name(s)
**  ____  ___________    __________________________________________
**  101M  (08-APR-98)  : MYG_PHYCA   (P02185)
**  102L  (31-OCT-93)  : LYCV_BPT4   (P00720)
**  102M  (08-APR-98)  : MYG_PHYCA   (P02185)
**  103L  (31-OCT-93)  : LYCV_BPT4   (P00720)
** 
**  < Data ommitted for clarity >
**  
**  7EST  (30-APR-94)  : EL1_PIG     (P00772)
**  7FAB  (31-JAN-94)  : HV2G_HUMAN  (P01825), LAC_HUMAN   (P01842),
**                       LV1E_HUMAN  (P01703)
**  9WGA  (15-OCT-90)  : AGI2_WHEAT  (P02876)
**  9XIA  (15-JUL-92)  : XYLA_STRRU  (P24300)
**  9XIM  (15-JUL-93)  : XYLA_ACTMI  (P12851)
**  
**  ----------------------------------------------------------------------------
**  SWISS-PROT is copyright.  It is produced through a collaboration between the
**  Swiss Institute  of  Bioinformatics   and the EMBL Outstation - the European
**
**  < Other bibliographic information ommitted for clarity > 
**  
**  
**  
**  Output file format
**  An excerpt from the swissprot:pdb equivalence file in embl-like format is 
**  shown below.  The records used to describe an entry are as follows.  
**  (1)  EN - PDB identifier code.  This is the 4-character PDB identifier code.
**  (2)  NE - Number of entries.  This is the number of accession numbers that 
**  were given for that pdb code in the equivalence file.  
**  (3)  IN - Code line.  The swissprot database identifier code and accession
**  number are given preceeding ID and ACC respectively.  
**  
**  Excerpt from embl-like format swissprot:pdb equivalence file
**
**  EN   3SDH
**  XX
**  NE   2
**  XX
**  IN   LEU3_THETH ID; P00351 ACC;
**  IN   LEU4_THEFF ID; P02351 ACC;
**  XX
**  //
**  EN   2SDH
**  XX
**  NE   1
**  XX
**  IN   LEU1_FDFTH ID; P11351 ACC;
**  XX
**  //
**  
**  
**  
**  Data files
**  No data files are used by pdbtosp.
**  
**  
**  
**  Diagnostic error messages
**  
**  
**  
**  Authors
**  Jon Ison (jison@hgmp.mrc.ac.uk)
**
**  
**  
**  References
**  Please cite the authors.
**
**  
**  
******************************************************************************/





#include "emboss.h"







/* @prog pdbtosp *************************************************************
**
** Convert raw swissprot:pdb equivalence file to embl-like format
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPFile    inf1    =NULL;
    AjPFile    outf    =NULL;
    AjPStr     pdb     =NULL;   /* PDB identifier */
    AjPStr     spr     =NULL;   /* Swissprot identifier */
    AjPStr     acc     =NULL;   /* Accession number */
    AjPStr     pspr    =NULL;   /* Swissprot identifier pointer */
    AjPStr     pacc    =NULL;   /* Accession number pointer */
    AjPStr     line    =NULL;   /* Line from file */
    AjPStr     token   =NULL;   /* Token from line */
    AjPStr     subtoken=NULL;   /* Token from line */
    AjPList    acclist =NULL;   /* List of accession numbers */
    AjPList    sprlist =NULL;   /* List of swissprot identifiers */
    ajint      n       =0;      /* No. of accession numbers for current pdb code */
    AjBool     ok      =ajFalse;/* True if "____  _" has been found and we can start
				  parsing */
    AjBool     done_1st=ajFalse;/* True if the first line of data has been parsed*/

    
    
    


    /* Read data from acd*/
    ajNamInit("emboss");
    ajAcdInitP("pdbtosp", argc, argv, "DOMAINATRIX");
    inf1  =  ajAcdGetInfile("infilea");
    outf  =  ajAcdGetOutfile("outfile");


    /* Memory allocation */
    line    = ajStrNew();
    token   = ajStrNew();
    subtoken= ajStrNew();
    pdb     = ajStrNew();
    acclist = ajListstrNew();
    sprlist = ajListstrNew();


    /* Read lines from file */
    while(ajFileReadLine(inf1, &line))
    {
	if(ajStrPrefixC(line, "____  _"))
	{
	    ok=ajTrue;
	    continue;
	}
	
	
	if(!ok)
	    continue;

	if(ajStrMatchC(line, ""))
	    break; 
	
	

	/* Read in pdb code first.  Then tokenise by ':', discard the 
	   first token, then tokenise the second token by ',', parsing 
	   out the swisssprot codes and accession numbers from the subtokens*/


	/* Make sure this is a line containing the pdb code */
	if((ajStrFindC(line, ":")!=-1))
	{
	    if(done_1st)
	    {
		/* Print data for last pdb code to file */
		ajFmtPrintF(outf, "%-5s%S\nXX\n%-5s%d\nXX\n", 
			    "EN", pdb, "NE", n);
	    
		while(ajListstrPop(acclist, &pacc))
		{
		    ajListstrPop(sprlist, &pspr);
	    
		    ajFmtPrintF(outf, "%-5s%S ID; %S ACC;\n", 
				"IN", pspr, pacc);

		    ajStrDel(&pspr);
		    ajStrDel(&pacc);
		}
		ajFmtPrintF(outf, "XX\n//\n");

		n=0;
	    }	

	    ajFmtScanS(line, "%S", &pdb);

	    ajStrTokC(line, ":");
	    ajStrAssS(&token, ajStrTokC(NULL, ":"));

	    done_1st=ajTrue;
	}
	else 
	{
	    ajStrAssS(&token, line);
	}
	

	spr  = ajStrNew();
	acc  = ajStrNew();
	ajFmtScanS(token, "%S (%S", &spr, &acc);
	
	if(ajStrSuffixC(acc, "),"))
	{
	    ajStrChop(&acc);
	    ajStrChop(&acc);
	}
	else
       	    ajStrChop(&acc);
	
	
	ajListstrPushApp(acclist, acc);
	ajListstrPushApp(sprlist, spr);
	n++;

	ajStrTokC(token, ",");
	while((subtoken=ajStrTokC(NULL, ",")))
	{
	    spr  = ajStrNew();
	    acc  = ajStrNew();

	    ajFmtScanS(subtoken, "%S (%S", &spr, &acc); 

	    if(ajStrSuffixC(acc, "),"))
	    {
		ajStrChop(&acc);
		ajStrChop(&acc);
	    }
	    else
		ajStrChop(&acc);


	    ajListstrPushApp(acclist, acc);
	    ajListstrPushApp(sprlist, spr);
	    n++;
	}
    }	
    
    
    /* Print data for last pdb code to file */
    ajFmtPrintF(outf, "%-5s%S\nXX\n%-5s%d\nXX\n", 
		"EN", pdb, "NE", n);
	    
    while(ajListstrPop(acclist, &pacc))
    {
	ajListstrPop(sprlist, &pspr);
	
	ajFmtPrintF(outf, "%-5s%S ID; %S ACC;\n", 
		    "IN", pspr, pacc);
	
	ajStrDel(&pspr);
	ajStrDel(&pacc);
    }
    ajFmtPrintF(outf, "XX\n//\n");



    /* Tidy up */
    ajFileClose(&inf1);
    ajFileClose(&outf);
    ajStrDel(&line);
    ajStrDel(&token);
    ajStrDel(&subtoken);
    ajStrDel(&pdb);
    ajListstrDel(&acclist);	
    ajListstrDel(&sprlist);	

    ajExit();
    return 0;
}
