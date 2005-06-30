/* @source domainreso application
**
** Removes low resolution domains from a DCF file (domain
** classification file).
**
** @author: Copyright (C) Matt Blades (Matthew.Blades@astrazeneca.com)
** @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
** @@
**
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
**  DOMAINRESO documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**  
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@rfcgr.mrc.ac.uk.
**  
**  NOTES
**  
******************************************************************************/




#include <math.h>

#include "emboss.h"




/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static ajint domainreso_StrBinSearchDomain(AjPStr id, 
					   AjPStr *arr, 
					   ajint siz);

static ajint domainreso_StrComp(const void *str1, 
				const void *str2);





/* @prog domainreso ***********************************************************
**
** Removes low resolution domains from a DCF file (domain
** classification file).
**
******************************************************************************/
int main(ajint argc, char **argv)
{

    AjPList     cpdb_path  = NULL; /* Location of coordinate files for input */
    AjPStr      cpdb_name  = NULL; /* Name of coordinate file                */
    AjPStr      temp       = NULL; /* temp string                            */
    AjPStr      temp2      = NULL; /* temp string                            */
    AjPList     entry      = NULL; /* List of pdb codes with resolution      */
                                   /* ABOVE the threshold                    */
    AjPStr     *entryarr   = NULL; /* entry as an array                      */
    

    AjPFile     fptr_cpdb  = NULL; /* Pointer to current coordinate file     */
    AjPFile     dcfin      = NULL; /* DCF input file                         */
    AjPFile     dcfout     = NULL; /* DCF output file                        */

    AjPPdb      pdb        = NULL; /* Pdb object pointer                     */    
    AjPDomain   domain     = NULL; /* Domain structure                       */
 

    float       threshold  = 0.0;  /* Resolution threshold                   */
    ajint       num        = 0;    /* number of nodes in list                */

    ajint       type       = 0;    /* Type of domain (ajSCOP or ajCATH) in 
				      the DCF file                           */


    
    
                                           
    /* Read data from acd */
    ajNamInit("emboss");
    ajAcdInitP("domainreso",argc,argv,"DOMAINATRIX"); 
    cpdb_path     = ajAcdGetDirlist("cpdbpath");    
    threshold     = ajAcdGetFloat("threshold");
    dcfin         = ajAcdGetInfile("dcfinfile");
    dcfout        = ajAcdGetOutfile("dcfoutfile");




    
    /* Allocate strings etc. */
    cpdb_name     = ajStrNew();
    temp          = ajStrNew();

    /* Create list . */
    entry    = ajListNew();


    /* Create list of files in CPDB directory. */
    

    /* Determine number of nodes on list    */
    num = ajListLength(cpdb_path);



/*
 domainreso reads a directory of clean coordinate files file, creates a list 
 of the files, then reads every list entry and extracts the resolution of the 
 structure.  If the value is less than a threshold (user defined) then the 
 domain identifier is pushed onto a list.  The DCF file (domain classification 
 file) is then read and domain identifiers compared to those on the list, if 
 found then the domain structure data is written the new DCF file.    
*/


    type = ajDomainDCFType(dcfin);


    /* Start of main application loop                         */
    /* Produce list of pdb codes with resolution              */
    /* ABOVE the threshold.                                   */
    while(ajListPop(cpdb_path,(void **)&temp))
    {
        /* Open coordinate file. */
        if((fptr_cpdb=ajFileNewIn(temp))==NULL)
        {
	    ajWarn("Could not open cpdb file");
            ajStrDel(&temp);
            continue;       
        }
	ajFmtPrint("%S\n", temp);
	fflush(stdout);
	
	

        /* Read coordinate data file. */ 
	pdb = ajPdbReadFirstModelNew(fptr_cpdb);
        

        /* Check if resolution is above threshold. */
        if(pdb->Reso > threshold)
	{
	    /* assign ID to list. */
	    temp2=ajStrNew();
	    ajStrAssS(&temp2, pdb->Pdb);
	    ajListPush(entry, (AjPStr) temp2);
	}        
	
        /* Close coordinate file and tidy up*/
        ajPdbDel(&pdb);
        ajFileClose(&fptr_cpdb);
	ajStrDel(&temp);
    }
    num = ajListLength(entry);
    

    /* Sort the list of pdb codes & convert to an array. */
    ajListSort(entry, domainreso_StrComp);
    ajListToArray(entry, (void ***)&entryarr);
    
    
    /* Read DCF file and compare IDs to those in list          
     if not present then write domain structure data to output. . */
    while((domain=(ajDomainReadCNew(dcfin, "*", type))))
    {
	/* DOMAIN id not found in the list of domains with resolution 
	   above the threshold, so include it in the output file. */
	if((domainreso_StrBinSearchDomain(ajDomainGetId(domain), 
					  entryarr, num))==-1)
	    ajDomainWrite(dcfout, domain);

        /* Delete domain structure. */
        ajDomainDel(&domain);
    }


    /* Tidy up. */
    ajStrDel(&cpdb_name);
    ajFileClose(&dcfout);
    ajFileClose(&dcfin);
    ajListDel(&cpdb_path);
    AJFREE(entryarr);
    
  
    /* Return. */
    ajExit();
    return 0;
}





/* @funcstatic domainreso_StrBinSearchDomain ********************************
**
** Performs a binary search for a DOMAIN domain code over an array of AjPStr
** (which of course must first have been sorted, e.g. by domainreso_StrComp). 
** This is a case-insensitive search.
**
** @param [r] id  [AjPStr]      Search term
** @param [r] arr [AjPStr*]     Array of AjPStr objects
** @param [r] siz [ajint]       Size of array
**
** @return [ajint] Index of first AjPStr object found with an PDB code
** matching id, or -1 if id is not found.
** @@
******************************************************************************/
static ajint domainreso_StrBinSearchDomain(AjPStr id, 
					   AjPStr *arr,
					   ajint siz)	
{
    int l;
    int m;
    int h;
    int c;


    l = 0;
    h = siz-1;
    while(l<=h)
    {
        m = (l+h)>>1;

        if((c=ajStrCmpCase(id, arr[m])) < 0)
	    h = m-1;
        else if(c>0) 
	    l = m+1;
        else 
	    return m;
    }

    return -1;
}




/* @funcstatic domainreso_StrComp *********************************************
**
** Function to sort strings.
**
** @param [r] str1  [const void*] AjPStr 1
** @param [r] str2  [const void*] AjPStr 2
**
** @return [ajint] -1 if str1 should sort before str2, +1 if the str2 should 
** sort first. 0 if they are identical in length and content. 
** @@
******************************************************************************/

static ajint domainreso_StrComp(const void *str1,
				const void *str2)
{
    AjPStr p = NULL;
    AjPStr q = NULL;

    p = (*(AjPStr*)str1);
    q = (*(AjPStr*)str2);
    
    return ajStrCmpO(p, q);
}








