/* @source domainrep application
**
** Reorder DCF file (domain classification file) so that the representative
** structure of each user-specified node is given first.
**  
**
** @author: Copyright (C) Mathew Blades
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

*******************************************************************************
** 
**  DOMAINREP documentation
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
**  The stamp application must be installed on the system that is running 
**  domainrep and correctly configured (see DOMAINREP documentation).
******************************************************************************/






#include "emboss.h"





/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static AjBool domainrep_WriteRmsd(ajint x, ajint y, 
				  AjPFloat2d *scores,  
				  AjPFile fptr);

static void domainrep_writelast(AjPDomain domain, 
				ajint noden, 
				AjPStr *last_node, 
				ajint *last_nodeid);






/* @prog domainrep ************************************************************
**
** Reorder scop classificaiton file so that the representative 
** structure of each family is given first.
**
******************************************************************************/

int main(int argc, char **argv)
{
    ajint      nfam       = 0;	  /* Counter for the families                */
    ajint      x          = 0;    /* Counter                                 */
    ajint      y          = 0;    /* Counter                                 */
    ajint      last_nodeid= 0;    /* SCOP Sunid of last family that was processed */
    AjPStr     last_node  = NULL; /* Last node that was processed */
    AjPStr     exec       = NULL; /* The UNIX command line to be executed*/
    AjPStr     dom        = NULL; /* Name of file containing single domain*/
    AjPStr     set        = NULL; /* Name of file containing set of domains*/
    AjPStr     out        = NULL; /* Name of file containing pairwise rmsd values */
    AjPStr     scan       = NULL; /* Name of temp. file used by STAMP.        */
    AjPStr     name       = NULL; /* Base name of STAMP temp files */
    AjPStr     temp       = NULL; /* A temporary string */


    AjPFile    dcfin      = NULL; /* File pointer for Escop.dat file (input)*/
    AjPFile    dcfout     = NULL; /* File pointer for Escop.dat file (output)*/
    AjPFile    domf       = NULL; /* File pointer for single domain file */
    AjPFile    setf       = NULL; /* File pointer for domain set file */
    AjPFile    outf       = NULL; /* File pointer for file containing pairwise rmsd values */
    

    AjPDomain  domain     = NULL; /* Pointer to domain structure */

    AjPList    famlist    = NULL; /* List of Scopobjects for current family */
    AjPDomain *arr        = NULL; /* Array of pointers to the famlist data */
    
    AjPDomain  tmp        = NULL; /* Temp. pointer for freeing famlist*/
    ajint      famsize    = 0;    /* No. domains in the family */
    
    AjPFloat2d scores     = NULL; /* Array of pairwise rmsd values          */
    AjPFloat   means      = NULL; /* Array of average pairwise rmsd values  */
    float      reso_cnt   = 0.0;  /* Used to calculate means array */
    ajint      div        = 0;    /* Used to calculate means array */
    ajint      ignore     = 0;    /* Used for writing ordered scop output file */
    float      min        = 0.0;  /* Used for writing ordered scop output file */    

   
    AjPStr     node       = NULL; /* Node of redundancy removal */
    ajint      noden      = 0;    /*1: Class (SCOP), 2: Fold (SCOP) etc, see ACD file */
    ajint      type       = 0;    /* Type of domain (ajSCOP or ajCATH) in the DCF file */


    /* Initialise strings etc*/
    last_node = ajStrNew();
    exec     = ajStrNew();

    dom      = ajStrNew();
    set      = ajStrNew();
    name     = ajStrNew();
    temp     = ajStrNew();
    scan     = ajStrNew();

    famlist  = ajListNew();


    /* Read data from acd */
    embInitPV("domainrep",argc,argv,"DOMALIGN",VERSION);

    dcfin     = ajAcdGetInfile("dcfinfile");
    dcfout    = ajAcdGetOutfile("dcfoutfile");
    node      = ajAcdGetListSingle("node");

    
    /* Convert the selected node to an integer. */
    if(!(ajStrToInt(node, &noden)))
	ajFatal("Could not parse ACD node option");

    /* Initialise random number generator for naming of temp. files*/
    ajRandomSeed();
    ajFilenameSetTempname(&name);


    /* Create names for temp. files*/
    ajStrAssignS(&dom, name);	
    ajStrAppendC(&dom, ".dom");
    ajStrAssignS(&set, name);	
    ajStrAppendC(&set, ".set");
    ajStrAssignS(&out, name);	
    ajStrAppendC(&out, ".out");
    ajStrAssignS(&scan, name);	
    ajStrAppendC(&scan, ".scan");


    /* Initialise last_node with something that is not in SCOP. */
    ajStrAssignC(&last_node,"!!!!!");
    
    
    type = ajDomainDCFType(dcfin);

    /* Start of main application loop. */
    while((domain=(ajDomainReadCNew(dcfin, "*", type))))
    {
	/* A new family. */
	if(((domain->Type == ajSCOP) &&
	    (((noden==1) && (last_nodeid !=  domain->Scop->Sunid_Class))      || 
	     ((noden==2) && (last_nodeid !=  domain->Scop->Sunid_Fold))       || 
	     ((noden==3) && (last_nodeid !=  domain->Scop->Sunid_Superfamily))|| 
	     ((noden==4) && (last_nodeid !=  domain->Scop->Sunid_Family))))   ||
	   ((domain->Type == ajCATH) &&
	    (((noden==5) && (last_nodeid !=  domain->Cath->Class_Id))         || 
	     ((noden==6) && (last_nodeid !=  domain->Cath->Arch_Id))          || 
	     ((noden==7) && (last_nodeid !=  domain->Cath->Topology_Id))      || 
	     ((noden==8) && (last_nodeid !=  domain->Cath->Superfamily_Id))   || 
	     ((noden==9) && (last_nodeid !=  domain->Cath->Family_Id)))))
	    
	{
	    /* If we have done the first family*/
	    if(nfam)
	    {
		/* Get family size. */
		famsize = ajListGetLength(famlist);
		

		if(famsize>2)
		{
		    /* Create an array of pointers to the famlist data. */
		    ajListToarray(famlist, (void ***) &(arr));
		    
		    /* Allocate & initialise scores and mean arrays. */
		    scores   = ajFloat2dNewRes(famsize);
		    for(x=0;x<famsize;x++)
			for(y=0;y<famsize;y++)
			    ajFloat2dPut(&scores, x, y, 0);
		    
		    means = ajFloatNewRes(famsize);
		    for(x=0;x<famsize;x++)
			ajFloatPut(&means, x, 0);


		    /* Do pairwise alignments for the family. */
		    for(x=0; x<famsize-1; x++)
		    {
			/* Open, write and close domain file*/
			
			if(!(domf=ajFileNewOutNameS(dom)))
			    ajFatal("Could not open domain file\n");
			ajStrAssignS(&temp, ajDomainGetId(arr[x]));
			ajStrFmtLower(&temp);
			ajFmtPrintF(domf, "%S %S { ALL }\n", temp, temp);
			ajFileClose(&domf);	


			for(y=x+1; y<famsize; y++)
			{
			    /* Open domain set file. */

			    if(!(setf=ajFileNewOutNameS(set)))
				ajFatal("Could not open domain set file\n");

			    
			    /* Write to domain set file*/
			    ajStrAssignS(&temp, ajDomainGetId(arr[x]));
			    ajStrFmtLower(&temp);
			    ajFmtPrintF(setf, "%S %S { ALL }\n", temp, temp);
			    ajStrAssignS(&temp, ajDomainGetId(arr[y]));
			    ajStrFmtLower(&temp);
			    ajFmtPrintF(setf, "%S %S { ALL }\n", temp, temp);
			    

			    /*Close domain set file*/
			    ajFileClose(&setf);	
			    
			    
			    /* Call STAMP. */
			    ajFmtPrintS(&exec,
                                        "%S -l %S -s -n 2 -slide 5 -prefix "
					"%S -d %S > %S\n",
                                        ajAcdGetpathC("stamp"),
                                        dom, name, set, out);
			    ajFmtPrint("%S\n", exec);
			    system(ajStrGetPtr(exec));  

			    
			    /* Open stamp output file. */
			
			    outf = ajFileNewInNameS(out);
			    

			    /* Parse stamp output file and write scores array. */ 
			    domainrep_WriteRmsd(x, y, &scores, outf);
			    
			    
			    /* Close stamp output file. */
			    ajFileClose(&outf);	

			}
		    }
		    
		    /* Calculate the means array 

		       In this code, the mean calculation does NOT include (obviously)
		       the comparison of the domain against itself.  And in cases where
		       the array value = 100 (i.e. no rmsd given in scopalign.out file)
		       the calculation again does not involve this domain.
		       
		       The important thing is that the mean is only calculated from the
		       domains against which the scan domain was scanned and an rmsd 
		       obtaioned. the mean is calculated by dividing by the number of 
		       domains for which acceptable rmsd valuse were available.
		       */
		 
		    for(x=0;x<famsize;x++)
		    {
			reso_cnt = 0;
			div      = 0;
			for(y=0;y<famsize;y++)    
			{       
			    if((y != x) && (ajFloat2dGet(scores, x, y) != 100))
			    {
				reso_cnt += ajFloat2dGet(scores, x, y);
				div++;
			    }           
                        
			    else
				continue;
			}
        
			ajFloatPut(&means, x, (reso_cnt/(float)(div)));
		    }
		    
		    
		    /* Write the scop classificaiton file (output) giving the structure
		       with the lowest mean rmsd first. */
		    for(ignore=0, min = 1000000, x=0;x<famsize;x++)
		    {
			if(ajFloatGet(means, x) < min)
			{
			    min = ajFloatGet(means, x);
			    ignore = x;
			}
		    }
		    ajDomainWrite(dcfout, arr[ignore]);
		    for(x=0;x<famsize;x++)
		    {
			if(x==ignore)
			    continue;
			else
			    ajDomainWrite(dcfout, arr[x]);
		    }


		    /* Delete family array amd family list, create new family list*/
		    AJFREE(arr);
		    arr=NULL;
		    
		    while(ajListPop(famlist,(void**)&tmp))
			ajDomainDel(&tmp);
		    ajListFree(&famlist);
		    famlist    = ajListNew();
		
    
		    /* Delete the scores and means array. */
		    ajFloat2dDel(&scores);  	    	    
		    ajFloatDel(&means);
		}
		/* family only has 1 or 2 members, Write output file with domain 
		   or two domains in family list, delete family list and continue. */
		else
		{
		    while(ajListPop(famlist,(void**)&tmp))
		    {
			ajDomainWrite(dcfout, tmp);
			ajDomainDel(&tmp);
		    }
		    ajListFree(&famlist);
		    famlist    = ajListNew();
		}
	    }
	    	    	    		
		
	    /* Increment family counter*/
	    nfam++;


	    /* Copy current family name to last_node*/
	    domainrep_writelast(domain, noden, &last_node, &last_nodeid);
	}
		
	/* Add the Scop object to the list for the family. */
	ajListPushAppend(famlist, domain);
    }
    /* End of main application loop*/



    /* Start of code to process last family. */
    /* Get family size. */
    famsize = ajListGetLength(famlist);
		

    if(famsize>2)
    {
	/* Create an array of pointers to the famlist data. */
	ajListToarray(famlist, (void ***) &(arr));
	
	/* Allocate & initialise scores and mean arrays. */
	scores   = ajFloat2dNewRes(famsize);
	for(x=0;x<famsize;x++)
	    for(y=0;y<famsize;y++)
		ajFloat2dPut(&scores, x, y, 0);
	
	means = ajFloatNewRes(famsize);
	for(x=0;x<famsize;x++)
	    ajFloatPut(&means, x, 0);
	
	
	/* Do pairwise alignments for the family. */
	for(x=0; x<famsize-1; x++)
	{
	    /* Open, write and close domain file*/

	    if(!(domf=ajFileNewOutNameS(dom)))
		ajFatal("Could not open domain file\n");
	    ajStrAssignS(&temp, ajDomainGetId(arr[x]));
	    ajStrFmtLower(&temp);
	    ajFmtPrintF(domf, "%S %S { ALL }\n", temp, temp);
	    ajFileClose(&domf);	


	    for(y=x+1; y<famsize; y++)
	    {

		/* Open domain set file. */
		if(!(setf=ajFileNewOutNameS(set)))
		    ajFatal("Could not open domain set file\n");

			    
		/* Write to domain set file*/
		ajStrAssignS(&temp, ajDomainGetId(arr[x]));
		ajStrFmtLower(&temp);
		ajFmtPrintF(setf, "%S %S { ALL }\n", temp, temp);
		ajStrAssignS(&temp, ajDomainGetId(arr[y]));
		ajStrFmtLower(&temp);
		ajFmtPrintF(setf, "%S %S { ALL }\n", temp, temp);
			    

		/*Close domain set file*/
		ajFileClose(&setf);	
			    
			    
		/* Call STAMP. */
		ajFmtPrintS(&exec,"stamp -l %S -s -n 2 -slide 5 -prefix "
			    "%S -d %S > %S\n", dom, name, set, out);
		ajFmtPrint("%S\n", exec);
		system(ajStrGetPtr(exec));  

			    
		/* Open stamp output file. */

		outf = ajFileNewInNameS(out);
			    

		/* Parse stamp output file and write scores array. */ 
		domainrep_WriteRmsd(x, y, &scores, outf);
			    
			    
		/* Close stamp output file. */
		ajFileClose(&outf);	
	    }
	}
	
	/* Calculate the means array 
	   
	   In this code, the mean calculation does NOT include (obviously)
	   the comparison of the domain against itself.  And in cases where
	   the array value = 100 (i.e. no rmsd given in scopalign.out file)
	   the calculation again does not involve this domain.
	   
	   The important thing is that the mean is only calculated from the
	   domains against which the scan domain was scanned and an rmsd 
	   obtaioned. the mean is calculated by dividing by the number of 
	   domains for which acceptable rmsd valuse were available.
	   */
	
	for(x=0;x<famsize;x++)
	{
	    reso_cnt = 0;
	    div      = 0;
	    for(y=0;y<famsize;y++)    
	    {       
		if((y != x) && (ajFloat2dGet(scores, x, y) != 100))
		{
		    reso_cnt += ajFloat2dGet(scores, x, y);
		    div++;
		}           
                        
		else
		    continue;
	    }
        
	    ajFloatPut(&means, x, (reso_cnt/(float)(div)));
	}
	
	
	/* Write the scop classificaiton file (output) giving the structure
	   with the lowest mean rmsd first. */
	for(ignore=0, min = 1000000, x=0;x<famsize;x++)
	{
	    if(ajFloatGet(means, x) < min)
	    {
		min = ajFloatGet(means, x);
		ignore = x;
	    }
	}
	ajDomainWrite(dcfout, arr[ignore]);
	for(x=0;x<famsize;x++)
	{
	    if(x==ignore)
		continue;
	    else
		ajDomainWrite(dcfout, arr[x]);
	}
	
	
	/* Delete family array amd family list, create new family list*/
	AJFREE(arr);
	arr=NULL;
	
	while(ajListPop(famlist,(void**)&tmp))
	    ajDomainDel(&tmp);
	ajListFree(&famlist);
	famlist    = ajListNew();
	
	
	/* Delete the scores and means array. */
	ajFloat2dDel(&scores);  	    	    
	ajFloatDel(&means);
    }
    /* family only has 1 or 2 members, Write output file with domain 
       or two domains in family list, delete family list and continue. */
    else
    {
	while(ajListPop(famlist,(void**)&tmp))
	{
	    ajDomainWrite(dcfout, tmp);
	    ajDomainDel(&tmp);
	}
	ajListFree(&famlist);
	famlist    = ajListNew();
    }



    /* Remove all temporary files. */
/*    if(domf)
    {
	ajFmtPrintS(&temp, "rm %S", dom);
	ajSysSystem(temp);
    }
    if(setf)
    {
	ajFmtPrintS(&temp, "rm %S", set);
	ajSysSystem(temp);
    }
    if(outf)
    {    
	ajFmtPrintS(&temp, "rm %S", out);
	ajSysSystem(temp);
    } */


    ajFmtPrintS(&temp, "rm %S", dom);
    ajSysSystem(temp);

    ajFmtPrintS(&temp, "rm %S", set);
    ajSysSystem(temp);

    ajFmtPrintS(&temp, "rm %S", out);
    ajSysSystem(temp);

    ajFmtPrintS(&temp, "rm %S", scan);
    ajSysSystem(temp);

    
    /* Tidy up*/
    while(ajListPop(famlist,(void**)&tmp))
        ajDomainDel(&tmp);
    ajListFree(&famlist);

    ajStrDel(&node);

    ajFileClose(&dcfin);	
    ajFileClose(&dcfout);	
    ajStrDel(&last_node);
    ajStrDel(&exec);
    ajStrDel(&dom);
    ajStrDel(&set);
    ajStrDel(&out);
    ajStrDel(&name);
    ajStrDel(&temp); 
    ajStrDel(&scan); 

    
    /* All done. */
    ajExit();
    return 0;
}











/* @funcstatic domainrep_WriteRmsd  *******************************************
 **
 ** Reads a file containing the redirected stdout output from the scopalign
 ** application and extracts the pairwise rmsd data.  The rmsd values are 
 ** entered into a 2d float array  
 **
 ** @param  [r] x        [ajint]      Index into scoring array
 ** @param  [r] y        [ajint]      Index into scoring array
 ** @param  [w] scores   [AjPFloat2d] 2d float score array 
 ** @param  [r] fptr     [AjPFile]    File pointer to scopalign file
 ** @return              [AjBool]     True on succcess
 ** @@
 *****************************************************************************/
static AjBool domainrep_WriteRmsd(ajint x, ajint y, AjPFloat2d *scores,  
				  AjPFile fptr)
{

    AjPStr      scan          = NULL;   /* scan domain                       */
    AjPStr      dom           = NULL;   /* domain scanned against scan domian*/
    AjPStr      line          = NULL;   /* string to hold file line          */
    AjPStr      skip          = NULL;   /* string                            */
    float       rmsd          = 0.0;    /* rmsd for this pairwise comparison */
    


    /* Assign line string. */
    line = ajStrNew();


    /* Read through file until line starting with "See" is found. */
    while(ajReadlineTrim(fptr, &line) && !(ajStrPrefixC(line, "See")))
    {
        /* Look for line starting Scan. */
        if(ajStrPrefixC(line, "Scan"))
        {
            /* Assign strings. */
            skip = ajStrNew();
            dom  = ajStrNew();
            scan = ajStrNew();
            
            /* Read in important bits. */
            ajFmtScanS(line, "%*s%S%S%S", &scan, &dom, &skip);
            
            /* If line does NOT represent domain scanned against itself . */
            /* then process rmsd value                                   */
            if(!ajStrMatchS(scan, dom))
            {
                /* Check if domain was skipped i.e. no rmsd given. */
                if(ajStrMatchC(skip, "skipped"))
                {
                    /* Assign silly value to array to ensure value is */
                    /* ignored in later averaging calculations        */
                    ajFloat2dPut(scores, x, y, 100);
                    ajFloat2dPut(scores, y, x, 100);
                    
                    /* Delete strings. */
                    ajStrDel(&scan);
                    ajStrDel(&skip);
                    ajStrDel(&dom);             
                }               
                
                /* Else process rmsd value. */
                else
                {
                    /* Read rmsd. */
                    ajFmtScanS(line, "%*s %*s %*s %*d %*f %f", &rmsd);
                    
                    /* Assign rmsd to array. */
                    ajFloat2dPut(scores, x, y, rmsd);
                    ajFloat2dPut(scores, y, x, rmsd);
                    
                    /* Delete strings. */
                    ajStrDel(&scan);
                    ajStrDel(&skip);
                    ajStrDel(&dom);             
                }               
            }
            
            /* Line is domain scanned against itself */
            /* so delete strings and proceed         */
            else
            {
                ajStrDel(&scan);
                ajStrDel(&skip);
                ajStrDel(&dom); 
                continue;
            }
            
        }       
            
        /* Line doesn't start with 'Scan' so continue. */
        else
            continue;
    }

    
    /* Tidy up. */
    ajStrDel(&line);

    /* Return. */
    return ajTrue;
}





/* @funcstatic domainrep_writelast ********************************************
**
** House-keeping function.
**
** @@
****************************************************************************/
static void domainrep_writelast(AjPDomain domain, ajint noden, AjPStr *last_node, 
				ajint *last_nodeid)
{
    if(noden==1) 
    {
	ajStrAssignS(last_node, domain->Scop->Class);
	*last_nodeid = domain->Scop->Sunid_Class;
    }		       
    else if (noden==2)
    {
	ajStrAssignS(last_node, domain->Scop->Fold);
	*last_nodeid = domain->Scop->Sunid_Fold;
    }
    else if (noden==3)
    {
	ajStrAssignS(last_node, domain->Scop->Superfamily);
	*last_nodeid = domain->Scop->Sunid_Superfamily;
    }
    else if (noden==4)
    {
	ajStrAssignS(last_node, domain->Scop->Family);
	*last_nodeid = domain->Scop->Sunid_Family;
    } 	
    else if (noden==5)
    {
	ajStrAssignS(last_node, domain->Cath->Class);
	*last_nodeid = domain->Cath->Class_Id ;
    } 
    else if (noden==6)
    {
	ajStrAssignS(last_node, domain->Cath->Architecture);
	*last_nodeid = domain->Cath->Arch_Id;
    } 
    else if (noden==7)
    {
	ajStrAssignS(last_node, domain->Cath->Topology);
	*last_nodeid = domain->Cath->Topology_Id;
    } 
    else if (noden==8)
    {
	ajStrAssignS(last_node, domain->Cath->Superfamily);
	*last_nodeid = domain->Cath->Superfamily_Id;
    } 
    else if (noden==9)
    {
	/* There is no text describing the CATH families. */
	ajFmtPrintS(last_node, "%d", domain->Cath->Family_Id);
	*last_nodeid = domain->Cath->Family_Id;
    } 
}

