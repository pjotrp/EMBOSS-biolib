/* @source scopparse application
**
** Converts raw scop classification files to a file in embl-like format.
**
** @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
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
** 
**  
*******************************************************************************
**  Application name
**  scopparse 
**  
**  
**  
**  Summary
**  Converts raw scop classification files to a file in embl-like format.
**
**  
**
**  Input and Output
**  scopparse parses the dir.cla.scop.txt and dir.des.scop.txt scop 
**  classification files (e.g. available at URL (1).)
**  (1) http://scop.mrc-lmb.cam.ac.uk/scop/parse/dir.cla.scop.txt_1.57
**      http://scop.mrc-lmb.cam.ac.uk/scop/parse/dir.des.scop.txt_1.57
**  The format of these files is explained at URL (2).
**  (2) http://scop.mrc-lmb.cam.ac.uk/scop/release-notes-1.55.html
**  
**  scopparse writes the classification to a single scop classification file in
**  embl-like format.  No changes are made to the data other than changing the 
**  format in which it is held.  The file does not include domain sequence 
**  information.  The input and output files are specified by the user.
**
**  
** 
**  Sister applications
**  domainer, funky, scopreso, scopseqs, scopnr, scoprep, scopalign, seqsearch, 
**  seqnr and seqalign use a scop classification file as input.  However scopnr, 
**  seqnr and seqalign require the file to contain domain sequence information, 
**  which can be added by using scopseqs.
**  
**  
**  
**  Notes
**  Some SCOP domains are comprised of more than one segments of polypeptide 
**  chain, these segments belonging to a single or more than one polypeptide 
**  chains.  It is debatable whether a domain (using the widely accepted 
**  definition) can truly consist of regions from more than polypeptide. 
**  Accordingly, scopparse gives the option of omitting from the output file 
**  domains that consist of segments from different chains.
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
**  The raw scop classification files are inconvenient for some uses because 
**  the text describing the domain classification is given in a different file
**  to the classification itself, and the file format is not easily extended.  
**  scopparse reads the raw scop classificaiton files and writes a single file
**  in embl-like format, which is an easier format to work with, is more 
**  human-readable and is more extensible than the native scop database format.
**  
**  
**  
**  Algorithm
**  
**  
**  
**  Usage 
**  An example of interactive use of scopparse is shown below.
**  
**  Unix % 
**  Unix % scopparse
**  Converts raw scop classification files to a file in embl-like format.
**  Name of scop classification file (raw format dir.cla.scop.txt_X.XX for input): /test_data/scop.cla.raw
**  Name of scop description file (raw format dir.des.scop.txt_X.XX for input): /test_data/scop.des.raw
**  Write multi-chain domains [N]: 
**  Name of scop classification file (embl format output) [test.scop]: /test_data/all.scop.new
**  Unix % 
**  
**  The raw scop classification files /test_data/scop.cla.raw and 
**  /test_data/scop.des.raw and a scop classification file in embl-like format
**  called /test_data/all.scop.new was written. The output file does not
**  contain domains that comprise segments from more than one chain.
**  
**  The following command line would achieve the same result.
**  scopparse /test_data/scop.cla.raw /test_data/scop.des.raw -outputall N
**  /test_data/all.scop.new
**  
**  
**  
**  Input file format
**  An excerpt from the dir.cla.scop.txt and dir.des.scop.txt scop 
**  input files is shown below.  The format of these files is explained
**  on the scop website:
**  http://scop.mrc-lmb.cam.ac.uk/scop/parse/dir.cla.scop.txt_1.57
**  
**  Excerpt from dir.cla.scop.txt 
**  # dir.cla.scop.txt 
**  # SCOP release 1.57 (January 2002)  [File format version 1.00]
**  # http://scop.mrc-lmb.cam.ac.uk/scop/
**  # Copyright (c) 1994-2002 the scop authors; see http://scop.mrc-lmb.cam.ac.uk/scop/lic/copy.html
**  d1cs4a_	1cs4	A:	d.58.29.1	39418	cl=53931,cf=54861,sf=55073,fa=55074,dm=55077,sp=55078,px=39418
**  d1ii7a_	1ii7	A:	d.159.1.4	62415	cl=53931,cf=56299,sf=56300,fa=64427,dm=64428,sp=64429,px=62415
**  
**  Excerpt from dir.des.scop.txt
**  # dir.des.scop.txt 
**  # SCOP release 1.57 (January 2002)  [File format version 1.00]
**  # http://scop.mrc-lmb.cam.ac.uk/scop/
**  # Copyright (c) 1994-2002 the scop authors; see http://scop.mrc-lmb.cam.ac.uk/scop/lic/copy.html
**  53931	cl	d	-	Alpha and beta proteins (a+b)
**  54861	cf	d.58	-	Ferredoxin-like
**  55073	sf	d.58.29	-	Adenylyl and guanylyl cyclase catalytic domain
**  55074	fa	d.58.29.1	-	Adenylyl and guanylyl cyclase catalytic domain
**  55077	dm	d.58.29.1	-	Adenylyl cyclase VC1, domain C1a
**  55078	sp	d.58.29.1	-	Dog (Canis familiaris)
**  39418	px	d.58.29.1	d1cs4a_	1cs4 A:
**  56299	cf	d.159	-	Metallo-dependent phosphatases
**  56300	sf	d.159.1	-	Metallo-dependent phosphatases
**  64427	fa	d.159.1.4	-	DNA double-strand break repair nuclease
**  64428	dm	d.159.1.4	-	Mre11
**  64429	sp	d.159.1.4	-	Archaeon Pyrococcus furiosus
**  62415	px	d.159.1.4	d1ii7a_	1ii7 A:
**  
**  
**  
**  Output file format
**  An example of the scop classification output file is shown below.
**  The records used to describe an entry are as follows.  Records (4) to (9) 
**  are used to describe the position of the domain in the scop hierarchy.
**
**  (1)  ID - Domain identifier code.  This is a 7-character code that uniquely
**  identifies the domain in scop.  It is identical to the first 7 characters 
**  of a line in the scop classification file.  The first character is always 
**  'D', the next four characters are the PDB identifier code, the fifth 
**  character is the PDB chain identifier to which the domain belongs (a '.' is
**  given in cases where the domain is composed of multiple chains, a '_' is 
**  given where a chain identifier was not specified in the PDB file) and the 
**  final character is the number of the domain in the chain (for chains 
**  comprising more than one domain) or '_' (the chain comprises a single 
**  domain only).
**  (2)  EN - PDB identifier code.  This is the 4-character PDB identifier code
**  of the PDB entry containing the domain.
**  (3)  SI - SCOP Sunid's.  The integers preceeding the codes CL, FO, SF, FA,
**  DO, SO and DD are the SCOP sunids for Class, Fold, SuPerfamily, Family, 
**  Domain, Source and domain data respectively. These numbers uniquely 
**  identify the appropriate node in the SCOP parsable files.
**  (4)  CL - Domain class.  It is identical to the text given after 'Class' in 
**  the scop classification file.
**  (5)  FO - Domain fold.  It is identical to the text given after 'Fold' in 
**  the scop classification file.
**  (6)  SF - Domain superfamily.  It is identical to the text given after 
**  'Superfamily' in the scop classification file.
**  (7)  FA - Domain family. It is identical to the text given after 'Family' 
**  in the scop classification file.
**  (8)  DO - Domain name. It is identical to the text given after 'Protein' in 
**  the scop classification file.
**  (9)  OS - Source of the protein.  It is identical to the text given after 
**  'Species' in the scop classification file.
**  (10) DS - Sequence of the domain according to the pdb file.  This sequence
**  is taken from the domain coordinate file generated by domainer.  The DS    
**  record will only be present if the scop classification file has been 
**  processed using scopseqs.
**  (11) AC - Accession number of the domain sequence.  This record will only 
**  be present if the scop classification file has been processed using 
**  scopseqs and if an accession number for the pdb file corresponding to the 
**  domain is given in the swissprot:pdb equivalence file (generated by 
**  pdbtosp) that scopseqs makes use of.
**  (12) SP - Swissprot code of the domain sequence.  This record will only 
**  be present if the scop classification file has been processed using 
**  scopseqs and if an swissprot code for the pdb file corresponding to the 
**  domain is given in the swissprot:pdb equivalence file (generated by 
**  pdbtosp) that scopseqs makes use of.
**  (13) RA - Position of domain in swissprot sequence.  The integers 
**  preceeding START and END give the start and end points respectively of the
**  domain sequence relative to the full-length swissprot sequence.
**  (14) SQ - Sequence of the domain according to swissprot.  This sequence
**  is taken from the swissprot database.  The SQ record will only be 
**  present if the scop classification file has been processed using 
**  scopseqs and if an accession number for the pdb file corresponding to the 
**  domain is given in the swissprot:pdb equivalence file (generated by 
**  pdbtosp) that scopseqs makes use of.
**  (15) NC - Number of chains comprising the domain, or number of segments 
**  from the same chain that the domain is comprised of.  NC is usually 1.  If
**  the number of chains is greater than 1, then the domain entry will have a 
**  section  containing a CN and a CH record (see below) for each chain.
**  (16) CN - Chain number.  The number given in brackets after this record 
**  indicates the start of the data for the relevent chain.
**  (17) CH - Domain definition.  The character given before CHAIN is the PDB 
**  chain identifier (a '.' is given in cases where a chain identifier was not 
**  specified in the scop classification file), the strings before START and 
**  END give the start and end positions respectively of the domain in the PDB 
**  file (a '.' is given in cases where a position was not specified).  Note 
**  that the start and end positions refer to residue numbering given in the 
**  original pdb file and therefore must be treated as strings.
**  (18) XX - used for spacing.
**  (19) // - used to delimit records for a domain.
**  
**  Excerpt from embl-like format scop classification file
**  ID   D3SDHA_
**  XX
**  EN   3SDH
**  XX
**  SI   46456 CL; 46457 FO; 46458 SF; 46463 FA; 46464 DO; 46465 SO; 14984 DD;
**  XX
**  CL   All alpha proteins
**  XX
**  FO   Globin-like
**  XX
**  SF   Globin-like
**  XX
**  FA   Globins
**  XX
**  DO   Hemoglobin I
**  XX
**  OS   Ark clam (Scapharca inaequivalvis)
**  XX
**  DS   SEQUENCE   146 AA;  15946 MW;  B7D52605 CRC32;
**       PSVYDAAAQL TADVKKDLRD SWKVIGSDKK GNGVALMTTL FADNQETIGY FKRLGNVSQG
**       MANDKLRGHS ITLMYALQNF IDQLDNPDDL VCVVEKFAVN HITRKISAAE FGKINGPIKK
**       VLASKNFGDK YANAWAKLVA VVQAAL
**  XX
**  AC   P02213
**  XX
**  SP   GLB1_SCAIN
**  XX
**  RA   1 START; 146 END;
**  XX
**  SQ   SEQUENCE   146 AA;  15947 MW;  5868B4E5 CRC32;
**       PSVYDAAAQL TADVKKDLRD SWKVIGSDKK GNGVALMTTL FADNQETIGY FKRLGDVSQG
**       MANDKLRGHS ITLMYALQNF IDQLDNPDDL VCVVEKFAVN HITRKISAAE FGKINGPIKK
**       VLASKNFGDK YANAWAKLVA VVQAAL
**  XX
**  NC   1
**  XX
**  CN   [1]
**  XX
**  CH   A CHAIN; . START; . END;
**  //
**  ID   D3SDHB_
**  XX
**  EN   3SDH
**  XX
**  SI   46456 CL; 46457 FO; 46458 SF; 46463 FA; 46464 DO; 46465 SO; 14985 DD;
**  XX
**  CL   All alpha proteins
**  XX
**  FO   Globin-like
**  XX
**  SF   Globin-like
**  XX
**  FA   Globins
**  XX
**  DO   Hemoglobin I
**  XX
**  OS   Ark clam (Scapharca inaequivalvis)
**  XX
**  DS   SEQUENCE   146 AA;  15946 MW;  B7D52605 CRC32;
**       PSVYDAAAQL TADVKKDLRD SWKVIGSDKK GNGVALMTTL FADNQETIGY FKRLGNVSQG
**       MANDKLRGHS ITLMYALQNF IDQLDNPDDL VCVVEKFAVN HITRKISAAE FGKINGPIKK
**       VLASKNFGDK YANAWAKLVA VVQAAL
**  XX
**  AC   P02213
**  XX
**  SP   GLB1_SCAIN
**  XX
**  RA   1 START; 146 END; 
**  XX
**  SQ   SEQUENCE   146 AA;  15947 MW;  5868B4E5 CRC32;
**       PSVYDAAAQL TADVKKDLRD SWKVIGSDKK GNGVALMTTL FADNQETIGY FKRLGDVSQG
**       MANDKLRGHS ITLMYALQNF IDQLDNPDDL VCVVEKFAVN HITRKISAAE FGKINGPIKK
**       VLASKNFGDK YANAWAKLVA VVQAAL
**  XX
**  NC   1
**  XX
**  CN   [1]
**  XX
**  CH   B CHAIN; . START; . END;
**  //
**  
**  
**  
**  Data files
**  No data files are used.
**  
**  
**  
**  Diagnostic error messages
**  
**  
**  
**  Authors
**  Jon Ison (jison@hgmp.mrc.ac.uk)
**  Alan Bleasby (ableasby@hgmp.mrc.ac.uk)
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






/* @prog scopparse **********************************************************
**
** Converts raw scop classification files to a file in embl-like format.
**
*****************************************************************************/

int main(int argc, char **argv)
{
    AjPFile inf1=NULL;
    AjPFile inf2=NULL;
    AjPFile outf=NULL;

    AjPScopcla cla=NULL;   
    AjPScopdes des=NULL;  
    AjPScopdes *desarr=NULL;
    
    AjPList  clalist=NULL;
    AjPList  deslist=NULL;

    AjBool   outputall=ajFalse;
    AjBool   nooutput=ajFalse;
    char     chn;
    
    
    ajint  dim=0;  /* Dimension of array */
    ajint  idx=0;  /* Index into array */
    ajint  i=0;
    

    clalist = ajListNew();
    deslist = ajListNew();

    /* Read data from acd*/
    ajNamInit("emboss");
    ajAcdInitP("scopparse", argc, argv, "DOMAINATRIX");
    
    inf1  =  ajAcdGetInfile("infilea");
    inf2  =  ajAcdGetInfile("infileb");
    outf  =  ajAcdGetOutfile("outfile");
    outputall  =  ajAcdGetBool("outputall");
    


    /* Read the dir.cla.scop.txt file */ 
    while(ajXyzScopclaReadC(inf1, "*", &cla))
    {
	ajListPushApp(clalist, cla);
/*	ajFmtPrint(" %d ", cla->Domdat); */
    }
    
    ajFileClose(&inf1);
    
    
    /* Read the dir.des.scop.txt file, sort the list by Sunid
       and convert to an array */
    while(ajXyzScopdesReadC(inf2, "*", &des))
    {
	ajListPush(deslist, des);
/*	ajFmtPrint("%d\n", des->Sunid); */
    }
    
    ajFileClose(&inf2);


    ajListSort(deslist, ajXyzScopdesCompSunid);
    dim=ajListToArray(deslist, (void ***) &desarr);
    

    while(ajListPop(clalist, (void **)&cla))
    {
	if(!outputall)
	{
	    if(cla->N > 1)
	    {
		chn=cla->Chain[0];
		for(nooutput=ajFalse, i=1;i<cla->N;i++)
		    if(chn != cla->Chain[i])
		    {
			nooutput=ajTrue;
			break;
		    }
		if(nooutput)
		    continue;
	    }
	}
	

	ajFmtPrintF(outf,"ID   %S\nXX\n",cla->Entry);
	ajFmtPrintF(outf,"EN   %S\nXX\n",cla->Pdb);
	ajFmtPrintF(outf,"SI   %d CL; %d FO; %d SF; %d FA; %d DO; %d SO; %d DD;\nXX\n",	
		    cla->Class,cla->Fold, cla->Superfamily, 
		    cla->Family,cla->Domain, cla->Source,
		    cla->Domdat);
	/*ajFmtPrintF(outf,"SI   %d\nXX\n",cla->Domdat);*/
	idx = ajXyzScopdesBinSearch(cla->Class,  desarr, dim);
	ajFmtPrintF(outf,"CL   %S\n",desarr[idx]->Desc);

	idx = ajXyzScopdesBinSearch(cla->Fold,  desarr, dim);
	ajFmtPrintF(outf,"XX\n");
	ajFmtPrintSplit(outf,desarr[idx]->Desc,"FO   ",75," \t\n\r");


	idx = ajXyzScopdesBinSearch(cla->Superfamily,  desarr, dim);
	ajFmtPrintF(outf,"XX\n");
	ajFmtPrintSplit(outf,desarr[idx]->Desc,"SF   ",75," \t\n\r");

	idx = ajXyzScopdesBinSearch(cla->Family,  desarr, dim);
	ajFmtPrintF(outf,"XX\n");
	ajFmtPrintSplit(outf,desarr[idx]->Desc,"FA   ",75," \t\n\r");

	idx = ajXyzScopdesBinSearch(cla->Domain,  desarr, dim);
	ajFmtPrintF(outf,"XX\n");
	ajFmtPrintSplit(outf,desarr[idx]->Desc,"DO   ",75," \t\n\r");

	idx = ajXyzScopdesBinSearch(cla->Source,  desarr, dim);
	ajFmtPrintF(outf,"XX\n");
	ajFmtPrintSplit(outf,desarr[idx]->Desc,"OS   ",75," \t\n\r");

	ajFmtPrintF(outf,"XX\nNC   %d\n",cla->N);

	for(i=0;i<cla->N;++i)
	{
	    ajFmtPrintF(outf,"XX\nCN   [%d]\n",i+1);
	    ajFmtPrintF(outf,"XX\nCH   %c CHAIN; %S START; %S END;\n",
			cla->Chain[i],
			cla->Start[i],
			cla->End[i]);
	}
	ajFmtPrintF(outf,"//\n");

	ajXyzScopclaDel(&cla);
    
    }

    while(ajListPop(deslist, (void **)&des))
	ajXyzScopdesDel(&des);
    
    /* Tidy up */
    ajFileClose(&outf);
    AJFREE(desarr);
    ajListDel(&clalist);
    ajListDel(&deslist);
    

    ajExit();
    return 0;
}












