/* @source siggenlig application
**
** Generates ligand-binding signatures from a CON file (contacts file) of 
** residue-ligand contacts.
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
**
*****************************************************************************
** 
**  SIGGENLIG documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**
**  Email Waqas Awan (wawan@rfcgr.mrc.ac.uk) or 
**        Jon Ison (jison@rfcgr.mrc.ac.uk)
**  
****************************************************************************/







#include "emboss.h"



/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
AjBool siggenlig_assign_env(AjPResidue residue, AjPStr *OEnv, ajint envdefi, AjPFile logf);
void siggenlig_new_sig_from_cmap(AjPSignature *sig, AjPCmap cmap, ajint patchsize, ajint gapdistance, ajint typei);
AjPPdb siggenlig_read_ccf(AjPCmap cmap, AjPDir ccfd, AjPDir ccfp);




/* @prog siggenlig *************************************************************
**
** Generates ligand-binding signatures from a CON file (contacts file) of 
** residue-ligand contacts.
**
******************************************************************************/

int main(ajint argc, char **argv)
{
    /*************************/
    /* Variable declarations */
    /*************************/
    AjPFile logf       = NULL;  /* Log file.                                  */
    AjPFile conf         = NULL;  /* CON file (input).                          */
    AjPDir ccfp          = NULL;  /* CCF file - protein (input).                */
    AjPDir ccfd          = NULL;  /* CCF file - domain (input).                 */
    AjPPdb  pdb          = NULL;  /* PDB object.                                */
    AjIList iter_residue    = NULL;  /* Iterator for residues in PDB object.          */
    AjPResidue residue         = NULL;  /* Temp. object.                              */
    
    
    AjPStr *mode         = NULL;  /* Mode, 1: Full-length signatures, 
				     2: Patch signatures.                       */
    ajint   modei        = 0;     /* Selected mode as integer.                  */
    
    AjPStr *type         = NULL;  /* Type, 1: 1D (sequence) signature, 
				     2: 3D (structural) signature.              */
    ajint   typei        = 0;     /* Selected mode as integer.                  */
    
    AjPStr  *envdef      = NULL;  /* Holds environment options from acd.        */
    ajint   envdefi      = 0;     /* envdef as an int.                          */
    ajint   patchsize    = 0;     /* Minimum patch size.                        */
    ajint   gapdistance  = 0;     /* Maximum gap distance.                      */
    ajint   wsiz         = 0;     /* Window size.                               */
    AjPDir  sigdir       = NULL;  /* Directory of signature files (output).     */
    
    AjPCmap cmap         = NULL;  /* Contact map for entry from CON file (input)*/
    AjPList cmap_list    = NULL;  /* List of contact maps from CON file (input).*/
    AjIList iter         = NULL;  /* Iterator for cmap_list.                    */
    
    AjPSignature    sig  = NULL;  /* Full-length or patch signature.            */
    AjPList    sig_list  = NULL;  /* List of signatures.                        */
    AjPSigdat    sigdat  = NULL;  /* Raw signature position.                    */
    AjPSigdat sigdat_tmp = NULL;  /* Raw signature position.                    */
    AjPList sigdat_list  = NULL;  /* List of signature positions.               */
    
    ajint             x  = 0;     /* Loop counter.                              */
    ajint       prevpos  = 0;     /* Previous contact position.                 */
    ajint       patchno  = 0;     /* Patch number.                              */
    ajint   num_patches  = 0;     /* Number of patches.                         */ 
    
    AjPStr     sigfname  = NULL;  /* Name of signature file.                    */
    AjPFile    sigoutf   = NULL;  /* Signature output file.                     */
    AjPStr   OEnv        = NULL;  /* Residue environment.                          */
    AjBool   firstpos  = ajTrue;  /* Housekeeping.                              */
    AjBool iscontact   = ajFalse; /* Housekeeping.                              */
    AjBool   foundresidue = ajFalse; /* Housekeeping.                              */
    
    
    
    /*************************/
    /* Read data from acd.   */
    /*************************/
    ajNamInit("emboss");
    ajAcdInitP("siggenlig",argc,argv,"SIGNATURE"); 
    conf        = ajAcdGetInfile("confile");
    ccfd        = ajAcdGetDirectory("ccfddir");
    ccfp        = ajAcdGetDirectory("ccfpdir");
    mode        = ajAcdGetList("mode");
    type        = ajAcdGetList("type");
    envdef      = ajAcdGetList("environment");
    patchsize   = ajAcdGetInt("patchsize");
    gapdistance = ajAcdGetInt("gapdistance");
    wsiz        = ajAcdGetInt("wsiz");
    sigdir      = ajAcdGetOutdir("sigoutdir");
    logf      = ajAcdGetOutfile("logfile");
    
    
    
    modei       = (ajint) ajStrChar(*mode,0)-48;
    typei       = (ajint) ajStrChar(*type,0)-48;
    envdefi     = (ajint) ajStrChar(*envdef,0)-48;
    
    
    
    
    
    /* Memory management */
    sigfname = ajStrNew();
    OEnv = ajStrNew();
    
    
    /*************************/
    /* Main application loop */
    /*************************/
    
    /* Read contact maps into list */
    cmap_list = ajCmapReadAllNew(conf);
    iter      = ajListIter(cmap_list);
    
    
    
    
    
    /* WRITE FULL-LENGTH SIGNATURE */
    if(modei == 1)
    {
	/* Process each contact map (protein-ligand interaction site) in turn */
	while( (cmap=(AjPCmap)ajListIterNext(iter)))
	{
	    /* 3D signature */
	    if(typei==2)
		/* Read CCF file, continue if not found */
		if((!(pdb=siggenlig_read_ccf(cmap, ccfd, ccfp))))
	    continue;
	    
	    /* Allocate intitial signature object (w/o position data) and copy
	       data from Cmap object / ACD. */
	    siggenlig_new_sig_from_cmap(&sig, cmap, patchsize, gapdistance, typei);

	    /* Create list of for putative signature positions. */
	    sigdat_list = ajListNew();

	    for(firstpos = ajTrue, prevpos=0, x=0; x<cmap->Dim; x++)
	    {
		if((ajInt2dGet(cmap->Mat, 0, x)))
		{
		    /* Always 1 residue and 1 gap for these signature positions. */
		    sigdat = embSigdatNew(1,1);
		  
		    /* 1D signature */
		    if(typei==1)
		    {
			ajChararrPut(&sigdat->rids, 0, ajStrChar(cmap->Seq1, x));
			ajIntPut(&sigdat->rfrq, 0, 1);
		    }
		    /* 3D signature */
		    else
		    {
			foundresidue = ajFalse;
			iter_residue = ajListIter(pdb->Chains[cmap->Chn1-1]->Residues);
			while((residue = (AjPResidue) ajListIterNext(iter_residue)))
			{
			    if(residue->Idx == x+1)
			    {
				if((!siggenlig_assign_env(residue, &OEnv, envdefi, logf)))
				    ajStrAssC(&OEnv, "*");
				
				ajStrAssS(&sigdat->eids[0], OEnv);
				ajIntPut(&sigdat->efrq, 0, 1);
				foundresidue=ajTrue;
				break;
			    } /* if(residue->Idx == x+1) */
			}
			if(!foundresidue)
			    ajFatal("Residue not found in siggenlig");
			ajListIterFree(&iter_residue);
		    }
		    if(firstpos)
			ajIntPut(&sigdat->gsiz, 0, x);
		    else
			ajIntPut(&sigdat->gsiz, 0, x-prevpos-1);

		    ajIntPut(&sigdat->gfrq, 0, 1);

		    ajListPushApp(sigdat_list, sigdat);
		    prevpos=x;
		    firstpos = ajFalse;
		}
	    }
	    
	    if(typei==2)
		ajPdbDel(&pdb);
	    
		
		
	    /* Convert list of Sigdata objects into array in signature */
	    sig->npos = ajListToArray(sigdat_list, (void***)&sig->dat);
	    ajListDel(&sigdat_list);
	    
		
	    /* WRITE SIGNATURE FILE */
	    ajFmtPrintS(&sigfname, "%S.%d.F.#.%S.", sig->Ligid, sig->sn, sig->Id);
	    if(MAJSTRLEN(sig->Domid))
		ajStrApp(&sigfname, sig->Domid);
	    else
		ajStrAppK(&sigfname, '#');
	    ajStrAppC(&sigfname, ".sig");
	    
	    sigoutf = ajFileNewOutDir(sigdir, sigfname);
	    embSignatureWrite(sigoutf, sig);
	    embSignatureDel(&sig);
	    ajFileClose(&sigoutf);
	}
    }
    /* WRITE PATCH SIGNATURE */
    else 
    {
	/* Process each contact map (protein-ligand interaction site) in turn */
	while( (cmap=(AjPCmap)ajListIterNext(iter)))
	{
	    /* 3D signature */
	    if(typei==2)
		/* Read CCF file, continue if not found */
		if((!(pdb=siggenlig_read_ccf(cmap, ccfd, ccfp))))
		    continue;
	    


	    /* Create list of for putative signature positions. */
	    sigdat_list = ajListNew();
	    sig_list    = ajListNew();
	    
	    for(patchno = 0, firstpos = ajTrue, prevpos=0, x=0; x<cmap->Dim; x++)
	    {
		iscontact = ajInt2dGet(cmap->Mat, 0, x);

		if(iscontact)
		{
		    /* Always 1 residue and 1 gap for these signature positions. */
		    sigdat = embSigdatNew(1,1);
		    
		    /* 1D signature */
		    if(typei==1)
		    {
			ajChararrPut(&sigdat->rids, 0, ajStrChar(cmap->Seq1, x));
			ajIntPut(&sigdat->rfrq, 0, 1);
		    }
		    /* 3D signature */
		    else
		    {
			foundresidue = ajFalse;
			iter_residue = ajListIter(pdb->Chains[cmap->Chn1-1]->Residues);
			while((residue = (AjPResidue) ajListIterNext(iter_residue)))
			{
			    if(residue->Idx == x+1)
			    {
				if((!siggenlig_assign_env(residue, &OEnv, envdefi, logf)))
				    ajStrAssC(&OEnv, "*");
				ajStrAssS(&sigdat->eids[0], OEnv);
				ajIntPut(&sigdat->efrq, 0, 1);

				foundresidue=ajTrue;
				break;
			    } 
			}
			if(!foundresidue)
			    ajFatal("Residue not found in siggenlig");
			ajListIterFree(&iter_residue);
		    }
		    
		    
		    if(firstpos)
			ajIntPut(&sigdat->gsiz, 0, 0);
		    else
			ajIntPut(&sigdat->gsiz, 0, x-prevpos-1);
		    ajIntPut(&sigdat->gfrq, 0, 1);
		  


		    if(((x - prevpos -1) <= gapdistance) || (firstpos))
			ajListPushApp(sigdat_list, sigdat);
		    else
		    {
			/* Either create new signature or delete list of Sigdat objects */
			if((ajListLength(sigdat_list) >= patchsize))
			{
			    /* Allocate signature object (w/o position data) and copy
			       data from Cmap object / ACD. */
			    siggenlig_new_sig_from_cmap(&sig, cmap, patchsize, gapdistance, typei);
			    sig->pn = ++patchno;

			    /* Convert list of Sigdata objects into array in signature */
			    sig->npos = ajListToArray(sigdat_list, (void***)&sig->dat);
			    ajListPushApp(sig_list, sig);
			  

			    /* Set gap distance for current position, i.e. the first position in 
			       the next putative signature, to 0 */
			    ajIntPut(&sigdat->gsiz, 0, 0);
			}
			else
			{
			    while(ajListPop(sigdat_list, (void **) &sigdat_tmp))
				embSigdatDel(&sigdat_tmp);
			}

			/* Push current position, i.e. the first position in the next putative signature. */
			ajListDel(&sigdat_list);
			sigdat_list = ajListNew();
			ajListPushApp(sigdat_list, sigdat);
		    }

		    prevpos=x;
		    firstpos = ajFalse;
		}
	    }
	    
	    if(typei==2)
		ajPdbDel(&pdb);



	    /* PROCESS LAST SIGNATURE */

	    /* Either create new signature or delete list of Sigdat objects */
	    if((ajListLength(sigdat_list) >= patchsize))
	    {
		/* Allocate signature object (w/o position data) and copy
		   data from Cmap object / ACD. */
		siggenlig_new_sig_from_cmap(&sig, cmap, patchsize, gapdistance, typei);
		sig->pn = ++patchno;

	      
		/* Convert list of Sigdata objects into array in signature */
		sig->npos = ajListToArray(sigdat_list, (void***)&sig->dat);
		ajListPushApp(sig_list, sig);
	    }
	    else
	    {
		while(ajListPop(sigdat_list, (void **) &sigdat_tmp))
		    embSigdatDel(&sigdat_tmp);
	    }
	    ajListDel(&sigdat_list);



	    /* WRITE SIGNATURE FILE */
	    /* Write number of patches in signature object */
	    num_patches = ajListLength(sig_list);
	    while(ajListPop(sig_list, (void **) &sig))
	    {
		sig->np = num_patches;

		ajFmtPrintS(&sigfname, "%S.%d.P.%d-%d.%S.", sig->Ligid, sig->sn, sig->pn, sig->np, sig->Id);
		if(MAJSTRLEN(sig->Domid))
		    ajStrApp(&sigfname, sig->Domid);
		else
		    ajStrAppK(&sigfname, '#');
		ajStrAppC(&sigfname, ".sig");

		sigoutf = ajFileNewOutDir(sigdir, sigfname);

		embSignatureWrite(sigoutf, sig);
		embSignatureDel(&sig);
		ajFileClose(&sigoutf);
	    }
	    ajListDel(&sig_list);
	}
    }
    
    /*************************/
    /* Memory management     */
    /*************************/
    ajDirDel(&ccfp);
    ajDirDel(&ccfd);
    ajFileClose(&conf);
    ajFileClose(&logf);
    while(ajListPop(cmap_list, (void **) &cmap))
	ajCmapDel(&cmap);
    ajListDel(&cmap_list);
    AJFREE(mode);
    AJFREE(type);
    AJFREE(envdef);
    ajListIterFree(&iter);
    ajDirDel(&sigdir);
    ajStrDel(&sigfname);
    ajStrDel(&OEnv);
    
    ajExit();
    return 0;
}



/* ccfd    CCF file - domain (input). */

AjPPdb siggenlig_read_ccf(AjPCmap cmap,  AjPDir ccfd,  AjPDir ccfp)
{
    AjPPdb  pdb          = NULL;  /* PDB object.                                */
    AjPFile ccffptr      = NULL;  /* CCF file pointer.                          */

    if(MAJSTRLEN(cmap->Domid))
	ccffptr = ajFileNewDirF(ccfd, cmap->Domid);
    else if((!MAJSTRLEN(cmap->Id)))
	ajFatal("No Id!");
    else
	ccffptr = ajFileNewDirF(ccfp, cmap->Id);
    
    if(!ccffptr)
    {
	if(MAJSTRLEN(cmap->Domid))
	    ajWarn("Coordinate file %S not found", cmap->Domid);
	else if((!MAJSTRLEN(cmap->Id)))
	    ajWarn("Coordinate file %S not found", cmap->Id);
	return NULL;
    }
    
    pdb = ajPdbReadFirstModelNew(ccffptr);

    if(!pdb)
    {
	ajWarn("Pdb object write failed");
	return NULL;
    }
    ajFileClose(&ccffptr);

    return pdb;
}	








void siggenlig_new_sig_from_cmap(AjPSignature *sig, AjPCmap cmap, ajint patchsize, ajint gapdistance, ajint typei)
{
    /* Allocate intitial signature object (w/o position data) and copy
       data from Cmap object / ACD. */
    *sig = embSignatureNew(0);

    (*sig)->Type = cmap->Type;
    ajStrAssS(&(*sig)->Id, cmap->Id);
    ajStrAssS(&(*sig)->Domid, cmap->Domid);
    ajStrAssS(&(*sig)->Ligid, cmap->Ligid);
    ajStrAssS(&(*sig)->Desc, cmap->Desc);
    (*sig)->ns = cmap->ns;
    (*sig)->sn = cmap->sn;
    (*sig)->np = 0;
    (*sig)->pn = 0;
    (*sig)->minpatch = patchsize;
    (*sig)->maxgap   = gapdistance;  


    if(typei==1)
	(*sig)->Typesig = aj1D;
    if(typei==2)
	(*sig)->Typesig = aj3D;


    return;
}





AjBool siggenlig_assign_env(AjPResidue residue, AjPStr *OEnv, ajint envdefi, AjPFile logf)
{
    char     SEnv = '\0';
    ajint    NUMENV = 0;

    /*Call to function that assigns the secondary structure environment class.
      Default to type 'C' (open coil). */
    if(!ajResidueSSEnv(residue, &SEnv, logf))
	SEnv='C'; 
    
    
    switch(envdefi)
    {
    case 1:
	/*Call to function that assigns the overall environment class*/
	NUMENV = ajResidueEnv1(residue, SEnv, OEnv, logf);
	break;
    case 2:
	NUMENV = ajResidueEnv2(residue, SEnv, OEnv, logf);
	break;
    case 3:
	NUMENV = ajResidueEnv3(residue, SEnv, OEnv, logf);
	break;
    case 4:
	NUMENV = ajResidueEnv4(residue, SEnv, OEnv, logf);
	break;
    case 5:
	NUMENV = ajResidueEnv5(residue, SEnv, OEnv, logf);
	break;
    case 6:
	NUMENV = ajResidueEnv6(residue, SEnv, OEnv, logf);
	break;
    case 7:
	NUMENV = ajResidueEnv7(residue, SEnv, OEnv, logf);
	break;
    case 8:
	NUMENV = ajResidueEnv8(residue, SEnv, OEnv, logf);
	break;
    case 9:
	NUMENV = ajResidueEnv9(residue, SEnv, OEnv, logf);
	break; 
    case 10:
	NUMENV = ajResidueEnv10(residue, SEnv, OEnv, logf);
	break; 
    case 11:
	NUMENV = ajResidueEnv11(residue, SEnv, OEnv, logf);
	break; 
    case 12:
	NUMENV = ajResidueEnv12(residue, SEnv, OEnv, logf);
	break; 
    case 13:
	NUMENV = ajResidueEnv13(residue, SEnv, OEnv, logf);
	break; 
    case 14:
	NUMENV = ajResidueEnv14(residue, SEnv, OEnv, logf);
	break; 
    case 15:
	NUMENV = ajResidueEnv15(residue, SEnv, OEnv, logf);
	break;
    case 16:
	NUMENV = ajResidueEnv16(residue, SEnv, OEnv, logf);
	break;
    default:
	ajFatal("Unknown environment definition in siggenlig_assign_env");
    } 

    /* Unrecnogised environment */
    if(!NUMENV)
	return ajFalse;
    

    return ajTrue;
}
				
				    
			      
