/* @source matgen3d application
**
** Generates a 3D-1D scoring matrix from CCF files (clean coordinate files).
**
** @author: Copyright (C) Waqas Awan 
** @author: Copyright (C) Jon Ison (jison@ebi.ac.uk)
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
#include <math.h>





/*Function prototypes*/
float        CalcScoringMatrix(AjPFile calclogf, AjPInt2d CountMat, 
			       AjPFloat2d *ScoringMatrix);


/* Length of AACODES string below */
#define NUMAA   24
/* Max. number of environments that can be returned by an ajResEnvXXX
   function */ /* Jon */
#define MAXENV  24 
ajint   NUMENV  =0;

char AACODES[]="ARNDCQEGHILKMFPSTWYVBZX*";


/* @prog matgen3d *************************************************************
**
** Generates a 3D-1D scoring matrix from CCF files (clean coordinate files).
**
******************************************************************************/

int main(ajint argc, char **argv)
{
    /*Declare acd stuff*/
    AjPDir    ccfddir = NULL;    /* Domain CCF file directory.               */
    AjPDir    ccfpdir = NULL;    /* Protein CCF file directory.              */
    AjPFile   dcfinfile=NULL;    /* Pointer to domain classification file.   */
    AjPFile   coninfile=NULL;    /* Pointer to CON file.                     */
    AjPFile   liginfile=NULL;    /* Pointer to ligand list file.             */
    AjPFile   logf=NULL;         /* matgen3d logfile*/
    AjPFile   Calclogf=NULL;     /* log file for sums for the calculation of 
				    the final matrix.                        */

    AjPStr   *modee=NULL;        /* Holds environment options from acd.      */
    ajint     modeei=0;	         /* modee as an int.                         */

    AjPStr   *mode=NULL;         /* Holds selection option from acd.         */
    ajint     modei=0;	         /* modee as an int.                         */

    AjPStr   *model=NULL;        /* Holds ligand option from acd.            */
    ajint     modeli=0;	         /* model as an int.                         */

    AjPFile   SCMatrixOut=NULL;	 /* final scoring matrix.                    */
    
    AjPStr    msg=NULL;	         /* String for messaging.                    */
    
    AjPList   list_allscop=NULL; /* List to hold entries in domain 
				       classification file.          */
    AjPList   list_cmap=NULL;    /* List to hold entries in CON file.        */
    AjPList   list_lig=NULL;     /* List to hold strings (ligand ids) from 
				    ligand list file.                        */
    AjPStr    lig_tmp=NULL;      /* Temp. string for ligand id.              */
    AjPStr   *lig_arr=NULL;      /* Array of ligand ids (from list_lig)      */
    ajint     lig_n=0;	         /* Size of lig_arr.                         */
    
    AjPCmap  *cmap_arr=NULL;     /* list_cmap as an array.                   */
    ajint     cmap_n=0;	         /* Size of cmap_arr.                        */
    
    AjPInt    sites_arr;         /* Array of residue index numbers of ligand-
				    contact residues.  */
    ajint     nsites = 0;        /* Size of sites_arr.                       */
    

    AjIList   ScopIter=NULL;     /* Iterator for list_allscop.               */
    AjPScop   ScopPtr=NULL;      /* Pointer to SCOP object.                  */
    AjPScop   ScopTmp=NULL;      /* temporary pointer to SCOP object to free 
				    memory.                                  */

    AjIList   CmapIter=NULL;     /* Iterator for list_allscop.               */
    AjPCmap   CmapPtr=NULL;      /* Pointer to SCOP object.                  */
    AjPCmap   CmapTmp=NULL;      /* temporary pointer to SCOP object to free 
				    memory.                                  */
    
    AjPStr    PrevEntry=NULL;    /* Previous SCOP domain ID.                 */
    AjPStr    tmpID=NULL;        /* Temporary string to hold SCOP or PDB ID. */
    AjPList   ListIDs=NULL;      /* List of SCOP or PDB IDs.                 */
    ajint     IDNum=0;		 /* Total number of IDs*/
    AjPFile   DCorFptr=NULL;     /* Pointer to domain coordinate files.      */
    AjPPdb    Pdb=NULL;	         /* Pointer to pdb object.                   */
    ajint     opened=0;	         /* Number of coordinate files opened.       */
    ajint     notopened=0;       /* Number of files not opened.              */
    
    AjIList   ResIter=NULL;      /* Iterator for residues in current 
				    CpdbStride object.                       */
    AjPResidue res=NULL;         /* Pointer to current Residue object in list 
				    iterator.                                */
    
    char      SEnv='\0';         /* Char to hold 3 state secondary structure 
				    assignment.                              */
    AjPStr    OEnv=NULL;         /* Char to hold overall environment class - 
				    one of 18 characters.                    */
/*    ajint     PrevRes=0; */    /* Prev residue Identity - this is the 
                                    domain numbering of the residue.         */
    
    
    AjPStr    IdName=NULL;       /* To temporarily hold domain or protein id.*/
    
    AjPInt2d  CountMatrix=NULL;	 /* Matrix of counts for each environment 
				    type.                                    */
    ajint     i=0;	         /* Row index for matrix, Env class.         */
    ajint     j=0;	         /* Column index for matrix, amino acids.    */
    ajint     count=0;	         /* Counter to load array.                   */
    
    AjPFloat2d SCRMatrix=NULL;        
    ajint     RowEnvIdx=0;       /* Row/Environment index.                   */
    ajint     ColumnResIdx=0;    /* Column/Residue index.                    */
    ajint     Dim=26;	         /* Max Dimension of array.                  */
    float     min=0.0;	         /* Min. value from scoring matrix.          */
    ajint     x=0;               /* Loop counter.                            */

    ajint     y=0;               /* Loop counter.                            */
    AjPStr    label=NULL;        /* Label for matrix.                        */
    ajint     labelcnt1=0;       /* Housekeeping.                            */
    ajint     labelcnt2=0;       /* Housekeeping.                            */
    AjPStr    line=NULL;         /* Housekeeping.                            */
    AjBool    done= ajFalse;     /* Housekeeping.                            */
    
     
    



    /* ACD processing */
    embInitP("matgen3d",argc,argv,"SIGNATURE"); 
    
    ccfddir      = ajAcdGetDirectory("ccfddir");
    ccfpdir      = ajAcdGetDirectory("ccfpdir");
    dcfinfile   = ajAcdGetInfile("dcfinfile");
    coninfile   = ajAcdGetInfile("coninfile");
    liginfile   = ajAcdGetInfile("liginfile");
    logf        = ajAcdGetOutfile("logfile");
    SCMatrixOut = ajAcdGetOutfile("scmatrixfile");
    Calclogf    = ajAcdGetOutfile("calclogfile");
    mode        = ajAcdGetList("mode");
    model       = ajAcdGetList("model");
    modee       = ajAcdGetList("modee");


    /*Assigns the options as integers */
    ajFmtScanS(modee[0], "%d", &modeei);   
    ajFmtScanS(mode[0],  "%d", &modei);   
    ajFmtScanS(model[0], "%d", &modeli);   
    
    
    /* Memory allocation */
    msg         = ajStrNew();
    label       = ajStrNew();
    PrevEntry   = ajStrNew();
    line        = ajStrNew();
    ListIDs     = ajListstrNew();
    OEnv        = ajStrNew();
    
    


    /* mode == 1: All amino acid positions for domains in a DCF file.
       Iterate through list of scop objects, pull out first entry in each 
       family. */

    if(modei == 1)
    {
	/*Read the Scop classification file and create a list of SCOP objects*/
	if(!(list_allscop = ajScopReadAllNew(dcfinfile)))
	    ajFatal("Error reading SCOP classification file\n");

	ScopIter=(ajListIterNew(list_allscop));
	while((ScopPtr=(AjPScop)ajListIterGet(ScopIter)))
	{
	    /*Only first member of each family*/
	    if(ajStrMatchS(PrevEntry, ScopPtr->Family))
		continue;
      
	    tmpID=ajStrNew();
	    ajStrAssignS(&tmpID, ScopPtr->Entry);
	    ajStrFmtLower(&tmpID);
	    ajListstrPushAppend(ListIDs, tmpID);
	    ajStrAssignS(&PrevEntry, ScopPtr->Family);
	}
	ajListIterDel(&ScopIter);  
    }
    /* mode == 2: Ligand-binding positions. */
    else if(modei == 2)
    {
	list_lig = ajListstrNew();
	
	/* Read ligand list file */
	if(modeli == 2)
	{
	    while(ajReadlineTrim(liginfile,&line))
	    {
		lig_tmp = ajStrNew();
		if((ajFmtScanS(line, "%S", lig_tmp) != 1))
		    ajFatal("Ligand id not read from file");
		ajStrFmtUpper(&lig_tmp);
		ajListPushAppend(list_lig, lig_tmp);
	    }	
	    lig_n = ajListToarray(list_lig, (void ***) &lig_arr);
	}	
	

	list_cmap = ajCmapReadAllNew(coninfile);
	cmap_n = ajListToarray(list_cmap, (void ***) &cmap_arr);
		
	
	CmapIter=(ajListIterNew(list_cmap));
	while((CmapPtr=(AjPCmap)ajListIterGet(CmapIter)))
	{
	    for(done=ajFalse, x=0; x<lig_n; x++)
		if(ajStrMatchS(lig_arr[x], CmapPtr->Ligid))
		{
		    done = ajTrue;
		    break;
		}
	    if(!done)
		continue;
	    
	    tmpID=ajStrNew();
	    /* Use domain ID if it is defined (the CON file is for a domain), 
	       otherwise pdb ID (the CON file is for a protein). */
	    if(MAJSTRGETLEN(CmapPtr->Domid))
		ajStrAssignS(&tmpID, CmapPtr->Domid);
	    else if(MAJSTRGETLEN(CmapPtr->Id))
		ajStrAssignS(&tmpID, CmapPtr->Id);
	    else
		ajFatal("Neither domain or pdb id is defined");
	    
	    ajStrFmtLower(&tmpID);
	    ajListstrPushAppend(ListIDs, tmpID);
	}
	
	ajListIterDel(&CmapIter);  
    }
    IDNum=ajListGetLength(ListIDs);
    
    


    /*Initialise NUMENVx26 Matrix*/
    CountMatrix=ajInt2dNew();
    for(i=0; i<MAXENV; i++)	 /* Environments */
	for(j=0; j<Dim; j++)    /* Amino acids */
	    ajInt2dPut(&CountMatrix, i, j, 0);
    


    /* Main loop for calculating environments */
    for(x=0; ajListstrPop(ListIDs, &IdName); x++)
    {     	
	/* PDB code */
	if(MAJSTRGETLEN(IdName)==4)
	{
	    if((!(DCorFptr = ajFileNewListinDirPre(ccfpdir, IdName))))
	    {
		notopened++;
		ajFmtPrintS(&msg, "Could not open for reading %S",
			    IdName);
		ajFmtPrintF(logf, "WARN\tCould not open for reading %S\n",
			    IdName);
		/*	    ajWarn(ajStrGetPtr(msg)); */
		ajStrDel(&IdName);
		continue;
	    }	
	}
	/* Domain code */
	if(MAJSTRGETLEN(IdName)==7)
	{
	    if((!(DCorFptr = ajFileNewListinDirPre(ccfddir, IdName))))
	    {
		notopened++;
		ajFmtPrintS(&msg, "Could not open for reading %S",
			    IdName);
		ajFmtPrintF(logf, "WARN\tCould not open for reading %S\n",
			    IdName);
		/*	    ajWarn(ajStrGetPtr(msg)); */
		ajStrDel(&IdName);
		continue;
	    }	
	}
	else
	    ajFatal("Domain or PDB ID code of unrecognised length");
	
	


	if(!(Pdb = ajPdbReadFirstModelNew(DCorFptr)))
	{
	    ajFmtPrintS(&msg, "Error reading coordinate file");
	    ajFmtPrintF(logf, "WARN\tError reading coordinate file\n");
	    ajWarn(ajStrGetPtr(msg));
	    ajStrDel(&IdName);
	    ajFileClose(&DCorFptr);
	    continue;
	}
	else if(Pdb)
	{
	    opened++;
/*	    ajFmtPrint("%d/%d\n", opened, IDNum);*/
	}

	ajFmtPrintF(logf, "%S\n\n", Pdb->Pdb);


	/* Ligand-binding positions only.  Construct array of residue
	   index numbers of ligand-contact residues */
	if(modei==2)
	{
	    sites_arr = ajIntNew();
	    for(nsites=0, y=0; y<cmap_arr[x]->Dim; y++)
		if(ajUint2dGet(cmap_arr[x]->Mat, x, 0))
		{
		    ajIntPut(&sites_arr, nsites, y+1);
		    nsites++;
		}
	}
		

      
	/*Initialise iterator for domain residues*/
	ResIter=ajListIterNew(Pdb->Chains[0]->Residues);
	while((res=(AjPResidue)ajListIterGet(ResIter)))
	{
	    /*Skip to next atom if residue number same*/
	    /*
	    if(PrevRes == atom->Idx) 
		continue;
		*/


	    /* 2: Select ligands within a CON file */
	    if(modeli == 2)
	    {
		for(done=ajFalse, y=0; y<nsites; y++)
		    if(res->Idx == ajIntGet(sites_arr, y))
		    {
			done = ajTrue;
			break;
		    }
		if(!done)
		    continue;

		AJFREE(sites_arr);
	    }
	    

	    /*Call to function that assigns the secondary structure
              environment class*/
	    if((!ajResidueSSEnv(res, &SEnv, logf)))
	    {
		ajFmtPrintF(logf, "SEnv unassigned for residue %d\n",
			    res->Idx);
		/* PrevRes = res->Idx; */
		continue;
	    }
	    
	    
	    /*Call to function that assigns the overall environment class*/
	    if(modeei == 1)
		NUMENV = ajResidueEnv1(res, SEnv, &OEnv, logf);
	    else if(modeei == 2)
		NUMENV = ajResidueEnv2(res, SEnv, &OEnv, logf);
	    else if(modeei == 3)
		NUMENV = ajResidueEnv3(res, SEnv, &OEnv, logf);
	    else if(modeei == 4)
		NUMENV = ajResidueEnv4(res, SEnv, &OEnv, logf);
	    else if(modeei == 5)
		NUMENV = ajResidueEnv5(res, SEnv, &OEnv, logf);
	    else if(modeei == 6)
		NUMENV = ajResidueEnv6(res, SEnv, &OEnv, logf);
	    else if(modeei == 7)
		NUMENV = ajResidueEnv7(res, SEnv, &OEnv, logf);
	    else if(modeei == 8)
		NUMENV = ajResidueEnv8(res, SEnv, &OEnv, logf);
	    else if(modeei == 9)
		NUMENV = ajResidueEnv9(res, SEnv, &OEnv, logf);
	    else if(modeei == 10)
		NUMENV = ajResidueEnv10(res, SEnv, &OEnv, logf);
	    else if(modeei == 11)
		NUMENV = ajResidueEnv11(res, SEnv, &OEnv, logf);
	    else if(modeei == 12)
		NUMENV = ajResidueEnv12(res, SEnv, &OEnv, logf);
	    else if(modeei == 13)
		NUMENV = ajResidueEnv13(res, SEnv, &OEnv, logf);
	    else if(modeei == 14)
		NUMENV = ajResidueEnv14(res, SEnv, &OEnv, logf);
	    else if(modeei == 15)
		NUMENV = ajResidueEnv15(res, SEnv, &OEnv, logf);
	    else if(modeei == 16)
		NUMENV = ajResidueEnv16(res, SEnv, &OEnv, logf);


	    /*Skip if the overall environment cannot be assigned*/
	    if( (!MAJSTRGETLEN(OEnv)))
	    {
		ajFmtPrintF(logf, "OEnv unassigned for residue %d\n",
			    res->Idx);
		/* PrevRes = res->Idx; */
		continue;
	    }

	    ajFmtPrintF(logf, "\tOEnv %S:%c\n", OEnv, res->Id1);

	    /*Get idices into 2d matrix*/
	    if(MAJSTRGETLEN(OEnv) == 1)
		i = ajStrGetCharFirst(OEnv) - 'A';
	    else if(MAJSTRGETLEN(OEnv) == 2)
		i = ajStrGetCharPos(OEnv, 1) - 'A' + 
		    ((ajStrGetCharFirst(OEnv) - 'A')*26);
	    else
		ajFatal("Matrix label too big in matgen3d");
	    
	   /* i=OEnv-'A'; */		/*Index Env*/     
	    j=res->Id1 - 'A';		/*Index Residue*/
	    /* ajFmtPrint( "%c-%d i: %d j: %d\n", res->Id1, res->Idx, i, j); */
	    /*Increment and load counter in matrix*/
	    count = ajInt2dGet(CountMatrix, i, j);
	    ajInt2dPut(&CountMatrix, i, j, ++count);
	  
	    /* PrevRes=res->Idx; */
	}
       		   
	ajListIterDel(&ResIter);
	ajPdbDel(&Pdb);
	ajFileClose(&DCorFptr);
	ajStrDel(&IdName);
    }
	
	
	
	
  
    /*Print matrix with raw counts*/
    ajFmtPrintF(SCMatrixOut, "# 3D-1D Scoring matrix created by matgen3d\n");
    ajFmtPrintF(SCMatrixOut, "# ajResidueEnv%d\n", modeei);
    ajFmtPrintF(SCMatrixOut, "# Total SCOP entries: %d\n", IDNum);
    ajFmtPrintF(SCMatrixOut, "# No. of files opened: %d\n", opened);
    ajFmtPrintF(SCMatrixOut, "# No. of files not opened: %d\n", notopened);

    /*Initialise the scoring matrix*/
    SCRMatrix=ajFloat2dNew();
/*    for(RowEnvIdx=0; RowEnvIdx<Dim; RowEnvIdx++) */
    for(RowEnvIdx=0; RowEnvIdx<NUMENV; RowEnvIdx++) /* Jon */
    {
	for(ColumnResIdx=0; ColumnResIdx<Dim; ColumnResIdx++)
	{
	    ajFloat2dPut(&SCRMatrix, RowEnvIdx, ColumnResIdx, 0.000);
	}
    }

    min = CalcScoringMatrix(Calclogf, CountMatrix, &SCRMatrix);

    /* ajFmtPrintF(SCMatrixOut, "\n3d1d Score ij\n"); */
    ajFmtPrintF(SCMatrixOut, 
		"    %6s%6s%6s%6s%6s%6s%6s%6s%6s%6s"
		"%6s%6s%6s%6s%6s%6s%6s%6s%6s%6s"
		"%6s%6s%6s%6s\n", 
		"A", "R", "N", "D", "C", "Q", "E", "G", "H", "I", "L", "K",
		"M", "F", "P", "S", "T", "W", "Y", "V", "B", "Z", "X", "*");

    for(labelcnt1=0, labelcnt2=0, RowEnvIdx=0;
	RowEnvIdx<NUMENV;
	RowEnvIdx++, labelcnt2++)
    {	
	ajStrAssignK(&label, (char) labelcnt1+'A');
	ajStrAppendK(&label, (char) labelcnt2+'A');
	if(labelcnt2 == 25)
	{
	    labelcnt1 +=  1;
	    labelcnt2  = -1;
	}
	
	
	/* ajFmtPrintF(SCMatrixOut, "%-6S", RowEnvIdx+'A'); */
	ajFmtPrintF(SCMatrixOut, "%-4S", label);

	

	for(ColumnResIdx=0; ColumnResIdx<NUMAA-1; ColumnResIdx++)
	{
	    /* SCRMatrix includes letters A-Z but does not include '*' */
	    ajFmtPrintF(SCMatrixOut, "%6.2f", 
			ajFloat2dGet(SCRMatrix, RowEnvIdx, 
				     AACODES[ColumnResIdx]-'A'));
	} 
	ajFmtPrintF(SCMatrixOut, "%6.2f\n", min);
    }
    ajFmtPrintF(SCMatrixOut, "%-4c", '*');
    for(x=0; x<NUMAA; x++)
	ajFmtPrintF(SCMatrixOut, "%6.2f", min);
    

  
    /* free scop classification*/
    if(modei == 1)
    {
        while(ajListPop(list_allscop,(void **)&ScopTmp))
	    ajScopDel(&ScopTmp);
	ajListFree(&list_allscop);
    }

    /* free Cmap objects */
    if(modei == 2)
    {
        while(ajListPop(list_cmap,(void **)&CmapTmp))
	    ajCmapDel(&CmapTmp);
	ajListFree(&list_cmap);

	AJFREE(cmap_arr);
    }
    

    ajStrDel(&msg);
    ajStrDel(&label);
    ajStrDel(&PrevEntry);
    ajListstrFree(&ListIDs);
    ajStrDel(&line);
    ajInt2dDel(&CountMatrix);
    ajFloat2dDel(&SCRMatrix);

    /*Free ACD stuff*/
    ajFileClose(&logf);
    ajFileClose(&SCMatrixOut);
    ajFileClose(&dcfinfile);
    ajFileClose(&coninfile);
    ajFileClose(&liginfile);
    ajFileClose(&Calclogf);


    ajDirDel(&ccfddir);
    ajDirDel(&ccfpdir);
    
    AJFREE(modee);
    AJFREE(mode);
    AJFREE(model);
    

    ajListstrFree(&list_lig);
    AJFREE(lig_arr);
    

    ajExit();
    return 0;
}







/* @func CalcScoringMatrix ****************************************************
** Calculates a matrix of 3d-1d scores
** @param [r] CountMat [AjPUint2d]    Matrix of raw counts
** @return [float]
** @@
******************************************************************************/
float   CalcScoringMatrix(AjPFile calclogf, 
			  AjPInt2d CountMat,
			  AjPFloat2d *ScoringMatrix)
{
    ajint       IntNij=0;		/*Integer to hold Nij value
					  i.e. no. of res of type i in env j*/
    ajint       IntSUMiNij=0;		/*Integer to hold single SUM i
					  Nij value*/
    AjPInt      SUMiNijArr=NULL;	/*Array Sum Nij values for any
					  amino acid in environment j*/
    ajint       IntNi=0;		/*Integer to hold single Ni value*/
    AjPInt      NiArr=NULL;		/*Total residues of type i*/
    ajint       IntSUMNi=0;		/*Total number of residues in dataset*/
  
    float       FloatPij=0.0;		/*To hold pij value*/
    AjPFloat2d  PijMatrix=NULL;		/*Matrix of P(i:j) values
					  i.e. probability of finding
					  res i in env j*/
    float       FloatPi=0.0;		/*To hold single Pi value*/
    AjPFloat    PiArr=NULL;		/*Array of Pi values*/

    ajint       RowEnvIdx=0;		/*Row/Environment index*/
    ajint       ColumnResIdx=0;		/*Column/Residue index*/
  
    ajint       Dim=26;			/*Max Dimension of array*/
    ajint       i=0;

    float       Scoreij=0.0;		/*Told single 3d 1d score for ij*/
    float       min=0.0;		/* Min. score from matrix */
  
   
    /*Calculate values for Sum ij - totals for all amino acid in each
      environment (sum for Rows)*/
    /*Create and initialise array of Sum Nij values*/
    SUMiNijArr=ajIntNew();
/*    for(i=0;i<Dim;i++)   */
    for(i=0;i<NUMENV;i++)  /* Jon */
	ajIntPut(&SUMiNijArr, i, 0);

/*    for(RowEnvIdx=0; RowEnvIdx<Dim; RowEnvIdx++)  */ 
    for(RowEnvIdx=0; RowEnvIdx<NUMENV; RowEnvIdx++)  /* Jon */
    {
	IntSUMiNij=0;
	/* ajFmtPrint("%c\t", RowEnvIdx+'A'); */
	for(ColumnResIdx=0; ColumnResIdx<Dim; ColumnResIdx++)
	{
	    IntNij=ajInt2dGet(CountMat, RowEnvIdx, ColumnResIdx);
	    IntSUMiNij+=IntNij;
	}
	ajIntPut(&SUMiNijArr, RowEnvIdx, IntSUMiNij);
    }

    ajFmtPrintF(calclogf, "\nSUMiNijArr\n");
/*    for(i=0; i<Dim; i++) */
    for(i=0; i<NUMENV; i++) /* Jon */
	ajFmtPrintF(calclogf, "%c\t%d\n", i+'A', ajIntGet(SUMiNijArr, i));


    /*Calculate values of Ni - total number of residues of each type
      Sum of columns*/
    /*Create and initialise array of Ni values*/
    NiArr=ajIntNew();
    for(i=0;i<Dim;i++)
	ajIntPut(&NiArr, i, 0);

    for(ColumnResIdx=0; ColumnResIdx<Dim; ColumnResIdx++)
    {
	IntNi=0;
/*	for(RowEnvIdx=0; RowEnvIdx<Dim; RowEnvIdx++) */
	for(RowEnvIdx=0; RowEnvIdx<NUMENV; RowEnvIdx++) /* Jon */
	{
	    IntNij=ajInt2dGet(CountMat, RowEnvIdx, ColumnResIdx);
	    IntNi+=IntNij;
	}
	IntSUMNi+=IntNi;
	ajIntPut(&NiArr, ColumnResIdx, IntNi);
    }
  
    ajFmtPrintF(calclogf, "\nNiArr\n");
    for(i=0; i<Dim; i++)
	ajFmtPrintF(calclogf, "%c\t%d\n", i+'A', ajIntGet(NiArr, i));
	  
    /*Calculate values for P(i:j)*/
    /*Create and initialise 2d matrix for P(i:j) values*/
    PijMatrix=ajFloat2dNew();
/*    for(RowEnvIdx=0; RowEnvIdx<Dim; RowEnvIdx++) */
    for(RowEnvIdx=0; RowEnvIdx<NUMENV; RowEnvIdx++) /* Jon */
    {
	for(ColumnResIdx=0; ColumnResIdx<Dim; ColumnResIdx++)
	{
	    ajFloat2dPut(&PijMatrix, RowEnvIdx, ColumnResIdx, 0.0);
	}
    }
    
/*    for(RowEnvIdx=0; RowEnvIdx<Dim; RowEnvIdx++) */
    for(RowEnvIdx=0; RowEnvIdx<NUMENV; RowEnvIdx++) /* Jon */
    {
	for(ColumnResIdx=0; ColumnResIdx<Dim; ColumnResIdx++)
	{
	    IntNij=ajInt2dGet(CountMat, RowEnvIdx, ColumnResIdx);
	    if(IntNij>0)
	    {
		IntSUMiNij=ajIntGet(SUMiNijArr, RowEnvIdx);
		if(IntSUMiNij)
		    FloatPij=(float)IntNij/(float)IntSUMiNij;
		else
		    FloatPij=0.000;
	    }
	    else if(IntNij==0)
	    {
		IntSUMiNij=ajIntGet(SUMiNijArr, RowEnvIdx);
		if(IntSUMiNij)
		    FloatPij=(float)1/(float)IntSUMiNij;
		else
		    FloatPij=0.000;
	    }
	    ajFloat2dPut(&PijMatrix, RowEnvIdx, ColumnResIdx, FloatPij);
	}
    }



    ajFmtPrintF(calclogf, "\nPijMatrix\n");
/*    for(RowEnvIdx=0; RowEnvIdx<Dim; RowEnvIdx++) */
    for(RowEnvIdx=0; RowEnvIdx<NUMENV; RowEnvIdx++) /* Jon */
    {
	ajFmtPrintF(calclogf, "%c\t", RowEnvIdx+'A');
	for(ColumnResIdx=0; ColumnResIdx<Dim; ColumnResIdx++)
	{
	    ajFmtPrintF(calclogf, "%.3f\t",
			ajFloat2dGet(PijMatrix, RowEnvIdx, ColumnResIdx));
	}
	ajFmtPrintF(calclogf, "\n");
    }

    /*Calculate values for Pi*/
    PiArr=ajFloatNew();
    ajFmtPrintF(calclogf, "\nPi\n");
    for(i=0;i<Dim;i++)
	ajFloatPut(&PiArr, i, 0.0);
  
    for(i=0;i<Dim;i++)
    {
	IntNi=ajIntGet(NiArr, i);
	if(IntSUMNi)
	    FloatPi=(float)IntNi/(float)IntSUMNi;
	else
	    FloatPi=0;
	ajFloatPut(&PiArr, i, FloatPi);
    }
  
    for(i=0; i<Dim; i++)
	ajFmtPrintF(calclogf, "%c\t%.3f\n\n", i+'A', ajFloatGet(PiArr, i));
  
  
    /*Calculate matrix of 3d-1d scores*/


    /* for(RowEnvIdx=0; RowEnvIdx<Dim; RowEnvIdx++) */
    for(RowEnvIdx=0; RowEnvIdx<NUMENV; RowEnvIdx++) /* Jon */
    { 
	for(ColumnResIdx=0; ColumnResIdx<Dim; ColumnResIdx++)
	{
	    FloatPij=ajFloat2dGet(PijMatrix, RowEnvIdx, ColumnResIdx);
	    FloatPi=ajFloatGet(PiArr, ColumnResIdx);


	    /* Jon */
	    if(FloatPij == 0)
		Scoreij = 0;
	    else
	    {
		if(FloatPi)
		    Scoreij=log(FloatPij/FloatPi);
		else
		    Scoreij=log(FloatPij);
	    }
	    
	    ajFloat2dPut(ScoringMatrix, RowEnvIdx, ColumnResIdx, Scoreij);
	  
	}
    }
  


    for(min=10000000, RowEnvIdx=0; RowEnvIdx<NUMENV; RowEnvIdx++)
    { 
	/* Don't want the '*' hence -1 */
	for(ColumnResIdx=0; ColumnResIdx<NUMAA-1; ColumnResIdx++)
	{
	    Scoreij = ajFloat2dGet(*ScoringMatrix, RowEnvIdx,
				   (ajint) AACODES[ColumnResIdx]-'A');
	  
	    if(Scoreij < min)
		min = Scoreij;
	}
    }
  

    ajIntDel(&SUMiNijArr);
    ajIntDel(&NiArr);
    ajFloat2dDel(&PijMatrix);
    ajFloatDel(&PiArr);

    return min;
}
