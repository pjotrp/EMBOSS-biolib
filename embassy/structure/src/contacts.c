/* @source contacts application
**
** Reads CCF files (clean coordinate files) and writes CON files 
** (contact files) of intra-chain residue-residue contact data.
**
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
**
** 
*******************************************************************************
** 
**  CONTACTS Documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**  
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@ebi.ac.uk.
**  
**  NOTES
**  
******************************************************************************/





#include "emboss.h"
#include "math.h"




/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static AjBool contacts_WriteFile(AjPFile logf,
				 AjPFile outf, 
				 float thresh, 
				 float ignore,
				 AjPPdb pdb,
				 AjPVdwall vdw, 
				 AjBool skip);

static AjBool contacts_ContactMapWrite(AjPFile outf, 
				       AjPInt2d mat,
				       char *txt, 
				       ajint mod,
				       ajint chn,
				       AjPPdb pdb);

static AjBool contacts_ContactMapCalc(AjPInt2d *mat, 
				      ajint *ncon,
				      ajint dim, 
				      float thresh,
				      float ignore,
				      ajint mod, 
				      ajint chn, 
				      AjPPdb pdb,
				      AjPVdwall vdw, 
				      AjBool skip);





/* @prog contacts *******************************************************
**
** Reads coordinate files and writes files of intra-chain residue-residue 
** contact data.
**
******************************************************************************/

int main(ajint argc, char **argv)
{
    AjPList    cpdb_path     = NULL;	/* Directory of ccf files.     */
    AjPStr     cpdb_name     = NULL;	/* Name of ccf file.           */
    AjPDir     con_path      = NULL;	/* Directory of contact files. */
    AjPStr     con_name      = NULL;	/* Name of contact file.       */
    AjPStr     msg           = NULL;	/* Error message.              */
    AjPStr     temp          = NULL;	/* Temp string.                */

    AjPFile    cpdb_inf      = NULL;    /* cpdb input file pointer.    */
    AjPFile    con_outf      = NULL;    /* contact output file pointer.*/
    AjPFile    logf          = NULL;    /* log file pointer.           */
    AjPFile    vdwf          = NULL;    /* van der Waals file pointer. */
    
    AjPPdb     pdb           = NULL;

    float      thresh        = 0;
    float      ignore        = 0;
    AjBool     skip          = ajFalse; /* Whether to calculate contacts 
					   between residue adjacenet in 
					   sequence.                   */
    AjPVdwall  vdw           = NULL;    /* Structure for vdw radii.    */
    
    AjBool     ccfnaming     = ajFalse;   
    /* True == use the pdbid code to name the output file, 
       False== use the name of the original pdb file*/

    /* DEBUG 
    AjBool found=ajFalse; */
    
    
    
    /* Initialise strings. */
    temp          = ajStrNew();
    msg           = ajStrNew();
    cpdb_name     = ajStrNew();
    con_name      = ajStrNew();
    





    /* Read data from acd. */
    embInitP("contacts",argc,argv,"STRUCTURE");

    cpdb_path     = ajAcdGetDirlist("cpdbdir");
    con_path      = ajAcdGetOutdir("conoutdir");
    logf          = ajAcdGetOutfile("conlogfile");
    thresh        = ajAcdGetFloat("threshold");
    ccfnaming     = ajAcdGetBool("ccfnaming");
    skip          = ajAcdGetBool("skip");
    ignore        = ajAcdGetFloat("ignore");
    vdwf          = ajAcdGetDatafile("vdwfile");



    
    /* Allocate and read Vdwall object. */
    if(!(vdw=ajVdwallReadNew(vdwf)))
	ajFatal("Error reading vdw radii file\n");



    /* Start of main application loop. */
    while(ajListPop(cpdb_path,(void **)&temp))
    {

	/* DEBUG start
	if(ajStrFindC(temp, "d1bd7a_")!=-1)  BUILD1_1 
	if(ajStrFindC(temp, "d1du1a_")!=-1)  BUILD3_1 
	if(ajStrFindC(temp, "d1f02t_")!=-1)  BUILD3_2 
	if(ajStrFindC(temp, "d1du1a_")!=-1)  BUILD3_3 
	if(ajStrFindC(temp, "d1f3mb_")!=-1)  BUILD3_3 
	if(ajStrFindC(temp, "d1dt7x_")!=-1)  BUILD3_3 
	{                       
	    found=ajTrue;
	    continue;
	}
	

	if(!found)
	    continue;

	if((ajStrFindC(temp, "d1bd7a_")!=-1)||
	   (ajStrFindC(temp, "d1bd7b_")!=-1)||
	   (ajStrFindC(temp, "d1f02t_")!=-1)||
	   (ajStrFindC(temp, "d1du1a_")!=-1)||
	   (ajStrFindC(temp, "d1f3mb_")!=-1)||
	   (ajStrFindC(temp, "d1dt7x_")!=-1))
	    continue;
	
	DEBUG end*/


	/* Read clean coordinate file. */
	if((cpdb_inf=ajFileNewIn(temp))==NULL)
	{
	    ajFmtPrintS(&msg, "Could not open for reading %S", 
			temp);
	    ajWarn(ajStrGetPtr(msg));
	    ajFmtPrintF(logf, "WARN  Could not open for reading %S\n", 
			temp);
	    ajFileClose(&cpdb_inf);
	    ajStrDel(&temp);	
	    continue;	    
	}


	/* Read pdb structure. */
	if(!(pdb=ajPdbReadAllModelsNew(cpdb_inf)))
	{
	    ajFmtPrintS(&msg, "ERROR file read error %S", temp);
	    ajWarn(ajStrGetPtr(msg));
	    ajFmtPrintF(logf, "ERROR  file read error %S\n", temp);
	    ajFileClose(&cpdb_inf);
	    ajPdbDel(&pdb);
	    ajStrDel(&temp);	
	    continue;
	}

	/* Write diagnostic. */
	ajFmtPrint("%S\n", pdb->Pdb);   
	fflush(stdout);
	ajFmtPrintF(logf, "%S\n", pdb->Pdb);   



	/* Open contact file for writing. */
	if(ccfnaming)
	    ajStrAssignS(&con_name, pdb->Pdb);
	else
	{
	    ajStrAssignS(&con_name, temp);	
	    ajFileDirExtnTrim(&con_name);
	}
		
	       
	ajStrFmtLower(&con_name);
	if(!(con_outf=ajFileNewOutDir(con_path, con_name)))
	{
	    ajFmtPrintS(&msg, "ERROR file open error %S", 
			con_name);
	    ajWarn(ajStrGetPtr(msg));
	    ajFmtPrintF(logf, "ERROR file open error %S\n", con_name);
	    ajFileClose(&cpdb_inf);
	    ajFileClose(&con_outf);
	    ajPdbDel(&pdb);
	    ajStrDel(&temp);	
	    continue;
	}  

	
	/* Write contacts file. */
	if(!contacts_WriteFile(logf, con_outf, thresh, ignore, pdb, vdw, 
			       skip))
	{
	    ajFmtPrintS(&msg, "ERROR  file write error %S", con_name);
	    ajWarn(ajStrGetPtr(msg));
	    ajFmtPrintF(logf, "ERROR  file write error %S\n", con_name);

	    ajFmtPrintS(&temp, "rm %S", con_name);
	    ajSysSystem(temp);

	    ajFileClose(&cpdb_inf);
	    ajFileClose(&con_outf);
	    ajPdbDel(&pdb);
	    ajStrDel(&temp);	
	    continue;
	}


	/* Tidy up. */
	ajFileClose(&cpdb_inf);
	ajFileClose(&con_outf);
	ajPdbDel(&pdb);
	ajStrDel(&temp);	
    }
    /*End of main application loop*/    



    /* Tidy up. */
    ajListFree(&cpdb_path);
    ajStrDel(&cpdb_name);
    ajDirDel(&con_path);
    ajStrDel(&con_name);
    ajStrDel(&msg);
    ajFileClose(&logf);
    ajFileClose(&vdwf);

    ajVdwallDel(&vdw);
    

    /* Return. */
    ajExit();
    return 0;
}	





/* @funcstatic contact_WriteFile *****************************************
**
** Writes a file of INTRA-chain residue contact data in embl-like format 
** for a pdb structure.
**
** @param [w] logf   [AjPFile] Output file stream (log file)
** @param [w] outf   [AjPFile] Output file stream (contact file)
** @param [r] thresh [float]   Threshold contact distance
** @param [r] ignore [float]   Threshold ignore distance
** @param [r] pdb    [AjPPdb]  Pdb object
** @param [r] vdw    [AjPVdwall]   Vdwall object
** @param [r] skip   [AjBool]     Whether to calculate contacts between 
**                                residue adjacenet in sequence.
**
** @return [AjBool] True on success
** @@
**
******************************************************************************/

static AjBool contacts_WriteFile(AjPFile logf,
				 AjPFile outf,
				 float thresh, 
				 float ignore, 
				 AjPPdb pdb,
				 AjPVdwall vdw, 
				 AjBool skip)
{
    AjPInt2d   mat    = NULL;		/* Contact map.                  */
    ajint      x,y,z;			/* Loop counters.                */
    ajint      ncon   = 0;		/* No. contacts (1's) in matrix. */
    ajint      entry  = 0;    
    AjPStr     pdbid  = NULL;
    AjPStr     domid  = NULL;
    AjPSeqout  outseq = NULL;


    /* Error checking on args. */
    if(!logf || !outf || !pdb)
	return ajFalse;
    
    /* Memory management. */
    pdbid=ajStrNew();
    domid=ajStrNew();

    
    /* Print out header data. */
    ajFmtPrintF(outf, "%-5s%s\n", 
		"XX", "Intra-chain residue-residue contact data.");
    ajFmtPrintF(outf, "XX\n");

    
    /* TY */
    ajFmtPrintF(outf, "%-5s%s\n", 
		"TY", "INTRA");
    ajFmtPrintF(outf, "XX\n");


    /* EX */
    ajFmtPrintF(outf, "%-5s%s%.1f; %s%.1f; NMOD %d; NCHA %d\n", 
		"EX", "THRESH ", thresh, "IGNORE ",
		ignore, pdb->Nmod, pdb->Nchn);
    ajFmtPrintF(outf, "XX\n");


    /* NE */
    ajFmtPrintF(outf, "%-5s%d\n", 
		"NE", (pdb->Nmod * pdb->Nchn));
    ajFmtPrintF(outf, "XX\n");


    /* Start of loop to print out data for each entry (chain). */
    for(x=0;x<pdb->Nmod;x++)
    {
	for(y=0;y<pdb->Nchn;y++)
	{
	    /* EN */
	    ajFmtPrintF(outf, "%-5s[%d]\n", "EN", ++entry);
	    ajFmtPrintF(outf, "XX\n");  


	    /* ID */
	    if(MAJSTRGETLEN(pdb->Pdb) > 4)
	    {
		ajStrAssignS(&domid, pdb->Pdb);
		embScopToPdbid(pdb->Pdb, &pdbid);
	    }
	    else
	    {
		ajStrAssignC(&domid, ".");
		ajStrAssignS(&pdbid, pdb->Pdb);
	    }
	    ajFmtPrintF(outf, "%-5sPDB %S; DOM %S; LIG .\n", 
			"ID", pdbid, domid);
	    ajFmtPrintF(outf, "XX\n");  

	    
	    /* CN */
	    ajFmtPrintF(outf, "%-5sMO %d; CN1 %d; CN2 .; ID1 %c; ID2 .; "
			"NRES1 %d; NRES2 .\n",
			"CN", x+1, y+1,
			pdb->Chains[y]->Id, pdb->Chains[y]->Nres);
	    ajFmtPrintF(outf, "XX\n");  
	    

	    /* S1 */
	    if(pdb->Chains[y]->Nres != 0)
	    {
		outseq = ajSeqoutNewFile(outf);
		ajSeqoutDumpSwisslike(outseq, pdb->Chains[y]->Seq, "S1");
		ajSeqoutDel(&outseq);
		ajFmtPrintF(outf, "XX\n");	
	    }
	    
	    /* NC */
	    if(pdb->Chains[y]->Nres == 0)
	    {
		ajFmtPrintF(outf, "%-5sSM 0; LI .\n", "NC");
		ajFmtPrintF(outf, "XX\n");  
	    }
	    else
	    {
		/* Allocate memory for the contact map (SQUARE 2d int array) */
		mat = ajInt2dNewL((ajint)pdb->Chains[y]->Nres);   	

		for(z=0;z<pdb->Chains[y]->Nres;++z)
		    ajInt2dPut(&mat, z, pdb->Chains[y]->Nres-1, (ajint) 0);
	    
		/* Calculate the contact map. */
		if(!contacts_ContactMapCalc(&mat, &ncon, pdb->Chains[y]->Nres, 
					    thresh, ignore, x+1, y+1,
					    pdb, vdw, skip))
		{
		    ajFmtPrintF(logf, "ERROR  Calculating contact map\n");
		    ajInt2dDel(&mat);
		    ajStrDel(&pdbid);
		    ajStrDel(&domid);
		    return ajFalse;
		}

		ajFmtPrintF(outf, "%-5sSM %d; LI .\n", "NC", ncon);
		ajFmtPrintF(outf, "XX\n");  
	    }
	    

	    /* SM */
	    if(ncon)
	    {
		/* Matrix written OK and has at least 1 contact. */
		contacts_ContactMapWrite(outf, mat, "SM", x+1, y+1, pdb);   
	    }
	    else 
	    {
		/* Empty matrix !. */
		/* ajFmtPrintF(outf, ".\nXX\n");	*/
	    }
	    /* Free memory for matrix. */
	    ajInt2dDel(&mat);
	}
    }
    /* End of loop to print out data for each model / chain. */
    ajFmtPrintF(outf, "//\n");    


    /* Return. */
    ajStrDel(&pdbid);
    ajStrDel(&domid);
    return ajTrue;
}





/* @funcstatic contacts_ContactMapWrite **************************************
**
** Writes data in a contact map to file for a certain model and chain in a pdb 
** structure. The contact map must contain intra-chain contacts. Rows and
** columns in the square contact map correspond to residues in the chain.
**
** @param [w] outf [AjPFile]     Output file stream (contact file)
** @param [r] mat  [AjPInt2d]    Contact map
** @param [r] txt  [char*]       Text to print at start of each line
** @param [r] mod  [ajint]       Model number
** @param [r] chn  [ajint]       Chain number
** @param [r] pdb  [AjPPdb]      Pdb object
** 
** It writes a line to the output file for each contact. In the example below, 
** txt has a value of "SM":
** 
** SM   His 2 ; Val 8
** SM   Ala 6 ; Trp 21
** 
** @return [AjBool] True if file was succesfully written.
** @@
**
****************************************************************************/
static AjBool contacts_ContactMapWrite(AjPFile outf, 
				       AjPInt2d mat, 
				       char *txt,
				       ajint mod,
				       ajint chn,
				       AjPPdb pdb)
{
    ajint    x,y;			/* Loop counters.   */    
    AjPStr   res1 = NULL;		/* ID of residue 1. */
    AjPStr   res2 = NULL;		/* ID of residue 2. */


    /* Error checking on args. */
    if(!outf || !mat || !pdb || !txt)
    {
	ajWarn("Null arg. passed to contacts_ContactMapWrite");
	return ajFalse;
    }
    

    /* Allocate strings. */
    res1 = ajStrNew();
    res2 = ajStrNew();


    /* Start of loop for printing out contacts. */
    for(x=0; x<pdb->Chains[chn-1]->Nres; x++)
	for(y=x+1; y<pdb->Chains[chn-1]->Nres; y++)
	{
	    if((ajInt2dGet(mat, x, y)==1))
	    {
		/* Assign residue id. */
		if(!ajBaseAa1ToAa3(ajStrGetCharPos(pdb->Chains[chn-1]->Seq, x), 
				   &res1))
		{
		    ajStrDel(&res1);
		    ajStrDel(&res2);  
		    ajWarn("Index out of range in "
			   "contacts_PrintPdbSeqresChain");		
		    return ajFalse;
		}
		if(!ajBaseAa1ToAa3(ajStrGetCharPos(pdb->Chains[chn-1]->Seq, y), 
				   &res2))
		{
		    ajStrDel(&res1);
		    ajStrDel(&res2);  
		    ajWarn("Index out of range in "
			   "contacts_PrintPdbSeqresChain");		
		    return ajFalse;
		}
 		/* Print out the contact. */
		ajFmtPrintF(outf, "%-5s%S %d ; %S %d\n", txt, res1, x+1, 
			    res2, y+1);	
	    }
	}

    
    /*Tidy up. */
    ajStrDel(&res1);
    ajStrDel(&res2);  


    /* Return. */
    return ajTrue;
}	








/* @funcstatic contacts_ContactMapCalc ****************************************
**
** Write a contact map for a certain model and chain in a pdb structure. The
** contact map must contain intra-chain contacts. Rows and columns in the 
** square contact map correspond to residues in the chain.
**
** @param [w] mat    [AjPInt2d *] Contact map
** @param [w] ncon   [ajint *]    No. contacts
** @param [r] dim    [ajint]      Dimension of matrix (no. residues) 
** @param [r] thresh [float]      Threshold distance at which contact between 
**                                two residues is defined.
** @param [r] ignore [float]      Threshold "ignore" distance - this is a 
**                                speed-up.  Contact is not checked for
**                                between residues with CA atoms a further 
**                                distance apart than this.
** @param [r] mod    [ajint]      Model number
** @param [r] chn    [ajint]      Chain number
** @param [r] pdb    [AjPPdb]     Pdb object
** @param [r] vdw    [AjPVdwall]  Vdwall object
** @param [r] skip   [AjBool]     Whether to calculate contacts between 
**                                residues adjacent in sequence.
** 
** @return [AjBool] True if file was succesfully written.
** @@
**
******************************************************************************/
static AjBool contacts_ContactMapCalc(AjPInt2d *mat, 
				      ajint *ncon, 
				      ajint dim, 
				      float thresh, 
				      float ignore, 
				      ajint mod, 
				      ajint chn,
				      AjPPdb pdb, 
				      AjPVdwall vdw,
				      AjBool skip)
{	
    /* Contact is checked for between two residues, residue 1 and residue 2. */
    
    
    AjPAtom *arr      = NULL; /* Array of AjPAtom objects from list of 
				 AjPAtom objects for chain <chn> from <pdb>  */
    ajint    siz      = 0;    /* Size of <arr>.                              */
    
    ajint    idxfirst = 0;    /* Index in <arr> of first atom belonging to 
				 model <mod>.                                */
    ajint    idxlast  = 0;    /* Index in <arr> of last atom belonging to
				 model <mod>.                                */
    
    ajint    resfirst = 0;    /* Residue number of first atom belonging 
				 to model <mod>.                             */
    ajint    reslast  = 0;    /* Residue number of last atom belonging 
				 to model <mod>. */
    
    ajint    idx1     = 0;    /* Index in <arr> for atom from residue 1.     */
    ajint    idx2     = 0;    /* Index in <arr> for atom from residue 2.     */
    
    ajint    idx1first= 0;    /* Index in <arr> of first atom from residue 1.*/
    ajint    idx2first= 0;    /* Index in <arr> of first atom from residue 2.*/
    
    ajint    res1     = 0;    /* Residue number of residue 1.                */
    ajint    res2     = 0;    /* Residue number of residue 2.                */
    
    AjBool   done     = ajFalse; /* Flag.                                    */
    
    float    dis      = 0.0;  /* Inter-atomic distance.                      */
    ajint    offset   = 0;
    

    /* Error checking on args. */
    if(!mat || !pdb || !chn || !mod)
    {	
	ajWarn("Null or zero arg. passed to contacts_ContactMapCalc");
	return ajFalse;
    }
    

    /* Initialise no. contacts to zero. */
    (*ncon)=0;
    
    
    /*Convert the AjPList of atoms to an array of AjPAtom*/
    if(!(siz=ajListToarray((AjPList)pdb->Chains[chn-1]->Atoms,
			   (void ***)&arr)))
    {
	ajWarn("Zero sized list of sequences passed into "
	       "contacts_ContactMapCalc");
	return ajFalse;
    }


    if(skip) 
	offset = 2;
    else	
        offset = 1;



    
    /* Find index in <arr> of first atom belonging to correct model. */
    for(done=ajFalse, idxfirst=0; idxfirst<siz; idxfirst++) 
	/* Find the correct model. */
	if(arr[idxfirst]->Mod==mod && arr[idxfirst]->Type=='P')
	{
	    done=ajTrue;
	    resfirst=arr[idxfirst]->Idx;
	    break;
	}
    if(!done)
    {
	ajWarn("Model not found in contacts_ContactMapCalc");
	AJFREE(arr);
	return ajFalse;
    } 
    
    
    /* Find index in <arr> of last atom belonging to correct model. */
    for(idxlast=idxfirst; idxlast<siz; idxlast++)
    {
	if(arr[idxlast]->Mod!=mod  || arr[idxlast]->Type!='P')
	{
	    idxlast--;
	    break;	
	}
    }
    

    
    if(idxlast==siz)
	idxlast--;
    if(idxlast == idxfirst)
    {
	ajWarn("No atoms for the chain in contacts_ContactMapCalc");
	AJFREE(arr);
	return ajFalse;
    } 
    reslast=arr[idxlast]->Idx;
    



    
    /*Loop for first residue. */
    for(res1=resfirst,  idx1=idxfirst; res1<reslast; res1++)
    {
	/*Assign position of first atom of res1. */
	for(done=ajFalse, idx1first=idx1; idx1first<idxlast; idx1first++)
	    if(arr[idx1first]->Idx == res1)
	    {
		done=ajTrue;
		break;
	    }
	if(!done)
	    continue;
		
	/* Loop for second residue. 
	   Offset is normally 1 but will be 2 if contacts between adjacent
	   residue are not calculated. */
	for(res2=res1+offset, idx2=idx1; res2<=reslast; res2++)
	{
	    /*Assign position of first atom of res2. */
	    for(done=ajFalse, idx2first=idx2; idx2first<idxlast; idx2first++)
		if(arr[idx2first]->Idx == res2)
		{
		    done=ajTrue;
		    break;
		}
	    if(!done)
		continue;
	
	    /* Loop for atoms in first residue. */
	    for(done=ajFalse, idx1=idx1first; idx1<siz; idx1++)
	    {
		if(arr[idx1]->Idx != res1)
		    break;
		
		/* Loop for atoms in second residue. */
		for(idx2=idx2first; idx2<siz; idx2++)
		{
		    if(arr[idx2]->Idx != res2)
			break;

		    dis = embAtomDistance(arr[idx1], arr[idx2], vdw);
		    if(dis<=thresh)
		    {
			/* Increment no. contacts and write contact map. */
			(*ncon)++;

			if((arr[idx1]->Idx==0)||(arr[idx2]->Idx==0))
			{
			    ajWarn("Indexing error, contact not written !\n");
			}
			else
			{
			    ajInt2dPut(mat, arr[idx1]->Idx-1, 
				       arr[idx2]->Idx-1, 1);
			    ajInt2dPut(mat, arr[idx2]->Idx-1, 
				       arr[idx1]->Idx-1, 1); 
			}

			done=ajTrue;
			break;
		    }
		    else if(dis >= ignore)
			{
			    /* The atoms are so far apart we should not bother 
			       finding any more distances for this pair of 
			       atoms. */
			    done=ajTrue;
			    break;
			}
		    
		    /* ajFmtPrintF(xxxtemp, "NO CONTACT\n");  */

		}	
		if(done)
		    break;
	    }
	}
    }


    
    /* Tidy up. */
    AJFREE(arr);
    

    /* Return. */
    return ajTrue;
}











    
    

