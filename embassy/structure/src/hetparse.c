/* @source hetparse application
**
** Converts raw dictionary of heterogen groups
** to a file in EMBL-like format.
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
**

*******************************************************************************
** 
**  HETPARSE Documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**  
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@ebi.ac.uk
**  
**  NOTES
**  
******************************************************************************/




#include "emboss.h"





/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static AjBool  hetparse_HetScan(AjPList listfiles,  
				      AjPHet ptr);



/* @prog hetparse *************************************************************
**
** Converts raw dictionary of heterogen groups to a file in EMBL-like format.
**
******************************************************************************/
int main(int argc, char **argv)
{
    AjPFile   fin=NULL;
    AjPFile   fout=NULL;    
    AjPHet dic=NULL;
    AjBool    dogrep=ajFalse;
    AjPList   dirlist = NULL;
    AjPStr    tmp=NULL;
    



    
    /* Get values from acd. */
    embInitP("hetparse", argc, argv, "STRUCTURE");

    fin     = ajAcdGetInfile("infile");
    fout    = ajAcdGetOutfile("outfile");
    dogrep  = ajAcdGetToggle("dogrep");
    dirlist = ajAcdGetDirlist("dirlistpath");
    
    

    /* Parse raw file. */
    if(!(dic=ajHetReadRawNew(fin)))
       ajFatal("ajHetReadRawNew failed\n");
        
    
    /* Search pdb files for heterogens if appropriate. */
    if(dogrep)
	hetparse_HetScan(dirlist, dic);
    
			
    /* Write output file. */
    if(!ajHetWrite(fout, dic, dogrep))
	ajFatal("ajHetWrite failed\n");
    

    /* Tidy up and return. */
    while(ajListPop(dirlist,(void **)&tmp))
	ajStrDel(&tmp);
    ajListFree(&dirlist);
    ajHetDel(&dic);
    ajFileClose(&fin);
    ajFileClose(&fout); 
    
    

    ajExit();
    return 0;    
}






/* @funcstatic  hetparse_HetScan **********************************************
**
** Search a directory of pdb files and count the number of files that each 
** heterogen (from a Het object) appears in.
**
** @param [r] listfiles [AjPList] List of files
** @param [w] ptr  [AjPHet]       Het object
**
** @return [AjBool] True on success
** @@
******************************************************************************/
static AjBool        hetparse_HetScan(AjPList listfiles, 
				      AjPHet ptr)
{

    AjPList     listhet=NULL;     /* List of names of different heterogens 
				     in the current file.                    */
    AjIList     iter=NULL;        /* Iterator for listhet.                   */
    AjPStr      search_term=NULL; /* Temp. string to match against files 
				     in directory.                           */
    AjPStr      fname =NULL;      /* Temp pointer.                           */
    AjPFile     fptr=NULL;        /* File pointer.                           */
    AjPStr      line=NULL;        /* A line from the file.                   */
    AjPStr      het=NULL;         /* 3-character code of heterogen.          */
    AjPStr      hetcopy=NULL;     /* Copy of 3-character code of heterogen 
				     for list.                               */
    AjPStr      hettemp=NULL;     /* Temp. pointer to 3-character code of 
				     heterogen.                              */
    AjBool      foundhet=ajFalse; /* True if current heterogen was found 
				     in listhet.                             */
    ajint       i=0;              /* Counter.                                */
    


    /* Check args. */
    if(!ptr)
    {
	ajWarn("Bad arg's passed to hetparse_HetScan");
	return ajFalse;
    }
    


    /* Allocate memory. */
    search_term = ajStrNew();
    line = ajStrNew();
    het = ajStrNew();
    

    /* Create list of files in alignments directory. */
    /*Loop for reading each file in directory*/
    while(ajListPop(listfiles,(void **)&fname))
    {
        /* Open pdb file. */
        if((fptr=ajFileNewIn(fname))==NULL)
	{
	    ajWarn("Could not open file in hetparse_HetScan\n");
	    continue;
	}
	else 
	    ajFmtPrint("Processing %S\n", fname);
	
	

	/* Create list of heterogens. */
	listhet = ajListstrNew();

	/* PARSE FILE & POPULATE LIST OF HETATM CODES. */
	while(ajFileReadLine(fptr, &line))
	{
	    if(ajStrPrefixC(line,"HETATM"))
	    {
		/* Copy heterogen code from pdb file into <het>. */
		ajStrAssignSubS(&het, line, 17, 19);
		

		/* Initialise iterator to iterate through the list <listhet>. */
		iter=ajListIterNewread(listhet);

		foundhet=ajFalse;

		/* Iterate through the list, make <hettemp> point to the current
		   node. */		
		while((hettemp=(AjPStr)ajListIterGet(iter)))
		{
		    /* If <het> matches the current node, break. */
		    if(ajStrMatchS(hettemp, het))
		    {
			foundhet=ajTrue;
			break;
		    }
		}

		/* <het> was not found in the list ... so add it. */
		if(!foundhet)
		{
		    hetcopy=ajStrNew();
		    ajStrAssignRef(&hetcopy, het);
		    ajListstrPush(listhet, hetcopy);
		}
		
		
		/* Free the list iterator. */
		ajListIterDel(&iter);
	    }
	}
	


	
	/* Compare list of heterogens from this file to our dictionary. 
	   Initialise iterator to iterate through the list <listhet>. */
	iter=ajListIterNewread(listhet);

	/* Iterate through list, make <hettemp> point to the current node. */		
	while((hettemp=(AjPStr)ajListIterGet(iter)))
	{
	    /* Run through each heterogen in our dictionary and 
	       increment the counter if we have a match*/
	    for(i=0; i< ptr->n; i++)
	    {
		if(ajStrMatchS(hettemp, ptr->entries[i]->abv))
		{
		    ptr->entries[i]->cnt++;
		    break;
		}
	    }
	}
	

	/* Close file and tidy up. */
	ajFileClose(&fptr);
	ajStrDel(&fname);
	ajListIterDel(&iter);
	
	/* Free the list and its contents. */
	iter=ajListIterNewread(listhet);

	while((hettemp=(AjPStr)ajListIterGet(iter)))
	    ajStrDel(&hettemp);
	
	ajListstrFree(&listhet);

	/* Free the list iterator. */
	ajListIterDel(&iter);
    }



    /* Tidy up and return. */
    ajStrDel(&search_term);
    ajStrDel(&line);
    ajStrDel(&het);

    return ajTrue;
}





