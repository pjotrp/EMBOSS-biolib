/* @source matgen3d application
 **
 ** Generates a 3D-1D scoring matrix from CCF files (clean coordinate files).
 **
 ** @author: Copyright (C) Waqas Awan (wawan@hgmp.mrc.ac.uk)
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
 ******************************************************************************/

#include "emboss.h"
#include <math.h>





/*Function prototypes*/
float        CalcScoringMatrix(AjPFile calclogf, AjPInt2d CountMat, 
			       AjPFloat2d *ScoringMatrix);



#define NUMAA   24   /* Length of AACODES string below */
#define MAXENV  24   /* Max. number of environment that can be returned by an ajAtomEnvXXX function */  /* Jon */
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
    AjPDir    ccfdir = NULL;		/* CCF file directory.                      */
    AjPFile   ClassFptr=NULL;		/* Pointer to domain classification file.   */
    AjPFile   logf=NULL;		/* matgen3d logfile*/
    AjPFile   Calclogf=NULL;		/* log file for sums for the calculation of 
					   the final matrix.                        */
    AjPStr   *mode=NULL;		/* Holds environment options from acd.      */
    ajint     option=0;			/* option as an int.                        */
    AjPFile   SCMatrixOut=NULL;		/* final scoring matrix.                    */
    
    AjPStr    msg=NULL;			/* String for messaging.                    */
    
    AjPList   list_allscop=NULL;	/* List to hold entries in domain 
					   classification file.                     */
    AjIList   ScopIter=NULL;		/* Iterator for list_allscop.               */
    AjPScop   ScopPtr=NULL;		/* Pointer to SCOP object.                  */
    AjPScop   scoptemp=NULL;		/* temporary pointer to SCOP object to free 
					   memory.                                  */
    
    AjPStr    PrevEntry=NULL;		/* Previous SCOP domain ID.                 */
    AjPStr    tmpID=NULL;		/* Temporary string to hold SCOP ID.        */
    AjPList   ListScopIDs=NULL;		/* List of SCOP ID's, one ID from each 
					   family in the classification file.       */
    ajint     IDNum=0;			/* Total number of scop ids*/
    AjPFile   DCorFptr=NULL;		/* Pointer to domain coordinate files.      */
    AjPPdb    Pdb=NULL;			/* Pointer to pdb object.                   */
    ajint     opened=0;			/* Number of coordinate files opened.       */
    ajint     notopened=0;		/* Number of files not opened.              */
    
    AjIList   AtmIter=NULL;		/* Iterator for atoms in current CpdbStride 
					   object.                                  */
    AjPAtom   atom=NULL;		/* Pointer to current Atom object in list 
					   iterator.                                */
    
    AjPStr    BEnv=NULL;		/* String to hold basic environment class - 
					   letter and digit.                        */
    char      SEnv='\0';		/* Char to hold 3 state secondary structure 
					   assignment.                              */
    AjPStr    OEnv=NULL;		/* Char to hold overall environment class - 
					   one of 18 characters.                    */
    ajint     PrevRes=0;		/* Prev residue Identity - this is the 
					   domain numbering of the residue.         */
    
    
    AjPStr    ScopName=NULL;		/* To temporarily hold SCOP name.           */
    
    AjPInt2d  CountMatrix=NULL;		/* Matrix of counts for each environment 
					   type.                                    */
    ajint     i=0;			/* Row index for matrix, Env class.         */
    ajint     j=0;			/* Column index for matrix, amino acids.    */
    ajint     count=0;			/* Counter to load array.                   */
    
    AjPFloat2d SCRMatrix=NULL;        
    ajint     RowEnvIdx=0;		/* Row/Environment index.                   */
    ajint     ColumnResIdx=0;		/* Column/Residue index.                    */
    ajint     Dim=26;			/* Max Dimension of array.                  */
    float     min=0.0;			/* Min. value from scoring matrix.          */
    ajint     x=0;			/* Loop counter.                            */ 
    AjPStr    label=NULL;               /* Label for matrix.                        */
    ajint     labelcnt1=0;              /* Housekeeping.                            */
    ajint     labelcnt2=0;              /* Housekeeping.                            */
    
    
    /* ACD processing */
    ajNamInit("emboss");
    ajAcdInitP("matgen3d",argc,argv,"DOMAINATRIX"); 
    
    ccfdir = ajAcdGetDirectory("ccfdir");
    ClassFptr=ajAcdGetInfile("dcfinfile");
    logf=ajAcdGetOutfile("logf");
    SCMatrixOut=ajAcdGetOutfile("scmatrix");
    Calclogf=ajAcdGetOutfile("calclogf");
    mode=ajAcdGetList("mode");
    
    /*Assigns the 1d/3d option*/
    ajFmtScanS(mode[0], "%d", &option);
    
    msg=ajStrNew();
    label=ajStrNew();

    
    /*Read the Scop classification file and create a list of SCOP objects*/
    
    if(!(list_allscop = ajScopReadAllNew(ClassFptr)))
    {
	ajFatal("Error reading SCOP classification file\n");
    }
    
    ajFmtPrintF(SCMatrixOut, "# 3D-1D Scoring matrix created by matgen3d\n");
    ajFmtPrintF(SCMatrixOut, "# ajAtomEnv%d\n", option);
    
    PrevEntry=ajStrNew();
    ListScopIDs=ajListstrNew();
    
    /*Iterate through list of scop objects, pull out first entry in each 
      family*/
    ScopIter=(ajListIter(list_allscop));
    while((ScopPtr=(AjPScop)ajListIterNext(ScopIter)))
    {
	/*Only first member of each family*/
	if(ajStrMatch(PrevEntry, ScopPtr->Family))
	    continue;
      
	tmpID=ajStrNew();
	ajStrAssS(&tmpID, ScopPtr->Entry);
	ajStrToLower(&tmpID);
	ajListstrPushApp(ListScopIDs, tmpID);
	ajStrAssS(&PrevEntry, ScopPtr->Family);
    }
    ajListIterFree(&ScopIter);  
    
    /*Initialise NUMENVx26 Matrix*/


    /*Dummy call to function just to get the number of environments. */

    CountMatrix=ajInt2dNew();
/*    for(i=0; i<26; i++) */		/*Environments*/
    for(i=0; i<MAXENV; i++)			/* Jon */
    {
/*	for(j=0; j<26; j++);	*/	/*Amino acids*/
	for(j=0; j<Dim; j++);    /* Jon */
	{
	    ajInt2dPut(&CountMatrix, i, j, 0);
	}
    }
    
    ajFmtPrintF(SCMatrixOut, "# Total SCOP entries: %d\n", 
		ajListLength(ListScopIDs));
    IDNum=ajListLength(ListScopIDs);
    
    
    /*Use while(ajListstrPop(ListScopIDs, &ScopName)) only wanting to consider 
      the first member of each family. */
    while(ajListstrPop(ListScopIDs, &ScopName))
    {     
	if((!(DCorFptr = ajFileNewDirF(ccfdir, ScopName))))
	{
	    notopened++;
	    ajFmtPrintS(&msg, "Could not open for reading %S", ScopName);
	    ajFmtPrintF(logf, "WARN\tCould not open for reading %S\n", ScopName);
	    ajWarn(ajStrStr(msg));
	    ajStrDel(&ScopName);
	    continue;
	}	

	if(!(Pdb = ajPdbReadNew(DCorFptr)))
	{
	    ajFmtPrintS(&msg, "Error reading coordinate file");
	    ajFmtPrintF(logf, "WARN\tError reading coordinate file\n");
	    ajWarn(ajStrStr(msg));
	    ajFileClose(&DCorFptr);
	    continue;
	}
	else if(Pdb)
	{
	    opened++;
/*	    ajFmtPrint("%d/%d\n", opened, IDNum);*/
	}

	BEnv=ajStrNew();
	ajFmtPrintF(logf, "%S\n\n", Pdb->Pdb);
      
	/*Initialise iterator for domain atoms*/
	AtmIter=ajListIter(Pdb->Chains[0]->Atoms);
	while((atom=(AjPAtom)ajListIterNext(AtmIter)))
	{
	    /*Skip to next atom if residue number same*/
	    if(PrevRes == atom->Idx) 
		continue;

	    /*Call to function that assigns the secondary structure environment class*/
	    if((!ajAtomSSEnv(atom, &SEnv, logf)))
	    {
		ajFmtPrintF(logf, "SEnv unassigned for residue %d\n", atom->Idx);
		PrevRes = atom->Idx;
		continue;
	    }
	    
	    
	    /*Call to function that assigns the overall environment class*/
	    if(option == 1)
		NUMENV = ajAtomEnv1(atom, SEnv, &OEnv, logf);
	    else if(option == 2)
		NUMENV = ajAtomEnv2(atom, SEnv, &OEnv, logf);
	    else if(option == 3)
		NUMENV = ajAtomEnv3(atom, SEnv, &OEnv, logf);
	    else if(option == 4)
		NUMENV = ajAtomEnv4(atom, SEnv, &OEnv, logf);
	    else if(option == 5)
		NUMENV = ajAtomEnv5(atom, SEnv, &OEnv, logf);
	    else if(option == 6)
		NUMENV = ajAtomEnv6(atom, SEnv, &OEnv, logf);
	    else if(option == 7)
		NUMENV = ajAtomEnv7(atom, SEnv, &OEnv, logf);
	    else if(option == 8)
		NUMENV = ajAtomEnv8(atom, SEnv, &OEnv, logf);
	    else if(option == 9)
		NUMENV = ajAtomEnv9(atom, SEnv, &OEnv, logf);
	    else if(option == 10)
		NUMENV = ajAtomEnv10(atom, SEnv, &OEnv, logf);
	    else if(option == 11)
		NUMENV = ajAtomEnv11(atom, SEnv, &OEnv, logf);
	    else if(option == 12)
		NUMENV = ajAtomEnv12(atom, SEnv, &OEnv, logf);
	    else if(option == 13)
		NUMENV = ajAtomEnv13(atom, SEnv, &OEnv, logf);
	    else if(option == 14)
		NUMENV = ajAtomEnv14(atom, SEnv, &OEnv, logf);
	    else if(option == 15)
		NUMENV = ajAtomEnv15(atom, SEnv, &OEnv, logf);
	    else if(option == 16)
		NUMENV = ajAtomEnv16(atom, SEnv, &OEnv, logf);


	    /*  ajFmtPrintF(logf, "%S %c %S\n", BEnv, SEnv, OEnv); */
	  
	    /*Skip if the overall environment cannot be assigned*/
	    if( (!MAJSTRLEN(OEnv)))
	    {
		ajFmtPrintF(logf, "OEnv unassigned for residue %d\n", atom->Idx);
		PrevRes = atom->Idx;
		continue;
	    }

	    ajFmtPrintF(logf, "\tOEnv %S:%c\n", OEnv, atom->Id1);

	    /*Get idices into 2d matrix*/
	    if(MAJSTRLEN(OEnv) == 1)
		i = ajStrChar(OEnv, 0) - 'A';
	    else if(MAJSTRLEN(OEnv) == 2)
		i = ajStrChar(OEnv, 1) - 'A' + 
		    ((ajStrChar(OEnv, 0) - 'A')*26);
	    else
		ajFatal("Matrix label too big in matgen3d");
	    
	   /* i=OEnv-'A'; */		/*Index Env*/     
	    j=atom->Id1 - 'A';		/*Index Residue*/
	    /* ajFmtPrint( "%c-%d i: %d j: %d\n", atom->Id1, atom->Idx, i, j); */
	    /*Increment and load counter in matrix*/
	    count = ajInt2dGet(CountMatrix, i, j);
	    ajInt2dPut(&CountMatrix, i, j, ++count);
	  
	    PrevRes=atom->Idx;
	}
       		   
	ajListIterFree(&AtmIter);
	ajPdbDel(&Pdb);
	ajFileClose(&DCorFptr);
	ajStrDel(&ScopName);
    }
	
	
	
	
  
    /*Print matrix with raw counts*/
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

    for(labelcnt1=0, labelcnt2=0, RowEnvIdx=0; RowEnvIdx<NUMENV; RowEnvIdx++, labelcnt2++)
    {	
	ajStrAssK(&label, (char) labelcnt1+'A');
	ajStrAppK(&label, (char) labelcnt2+'A');
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
    while(ajListPop(list_allscop,(void **)&scoptemp))
    {
	ajScopDel(&scoptemp);
    }
    ajListDel(&list_allscop);

    ajStrDel(&msg);
    ajStrDel(&label);
    ajListstrDel(&ListScopIDs);
    ajStrDel(&tmpID);
    ajStrDel(&BEnv);
    ajInt2dDel(&CountMatrix);
    ajFloat2dDel(&SCRMatrix);

    /*Free ACD stuff*/
    ajFileClose(&logf);
    ajFileClose(&SCMatrixOut);
  
    ajExit();
    return 0;
}







/* @func CalcScoringMatrix ****************************************************
 ** Calculates a matrix of 3d-1d scores
 ** @param [r] CountMat [AjPInt2d]    Matrix of raw counts
 ** @return [float]
 ** @@
 ******************************************************************************/
float   CalcScoringMatrix(AjPFile calclogf, 
			  AjPInt2d CountMat,
			  AjPFloat2d *ScoringMatrix)
{
    ajint       IntNij=0;		/*Integer to hold Nij value i.e. no. of res of type i in env j*/
    ajint       IntSUMiNij=0;		/*Integer to hold single SUM i Nij value*/
    AjPInt      SUMiNijArr=NULL;	/*Array Sum Nij values for any amino acid in environment j*/
    ajint       IntNi=0;		/*Integer to hold single Ni value*/
    AjPInt      NiArr=NULL;		/*Total residues of type i*/
    ajint       IntSUMNi=0;		/*Total number of residues in dataset*/
  
    float       FloatPij=0.0;		/*To hold pij value*/
    AjPFloat2d  PijMatrix=NULL;		/*Matrix of P(i:j) values i.e. probability of finding res i in env j*/
    float       FloatPi=0.0;		/*To hold single Pi value*/
    AjPFloat    PiArr=NULL;		/*Array of Pi values*/

    ajint       RowEnvIdx=0;		/*Row/Environment index*/
    ajint       ColumnResIdx=0;		/*Column/Residue index*/
  
    ajint       Dim=26;			/*Max Dimension of array*/
    ajint       i=0;

    float       Scoreij=0.0;		/*Told single 3d 1d score for ij*/
    float       min=0.0;		/* Min. score from matrix */
  
   
    /*Calculate values for Sum ij - totals for all amino acid in each environment (sum for Rows)*/
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


    /*Calculate values of Ni - total number of residues of each type Sum of columns*/
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
	    ajFmtPrintF(calclogf, "%.3f\t", ajFloat2dGet(PijMatrix, RowEnvIdx, ColumnResIdx));
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
	    Scoreij = ajFloat2dGet(*ScoringMatrix, RowEnvIdx, (ajint) AACODES[ColumnResIdx]-'A');
	  
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




