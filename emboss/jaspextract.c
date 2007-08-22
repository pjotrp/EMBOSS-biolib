/* @source jaspextract application
**
** Copy JASPAR database to correct EMBOSS location
**
** @author Copyright (C) Alan Bleasby (ajb@ebi.ac.uk)
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

#define J_CORE "JASPAR_CORE"
#define J_FAM  "JASPAR_FAM"
#define J_PHY  "JASPAR_PHYLOFACTS"


static AjBool jaspextract_check(const AjPStr directory, const char *sdir);
static void   jaspextract_copy(const AjPStr directory, const char *sdir);




/* @prog jaspextract **********************************************************
**
** Copy JASPAR database to correct EMBOSS location.
**
** Looks for JASPAR_CORE, JASPAR_FAM and JASPAR_PHYLOFACTS directories
** and copies files to EMBOSS_DATA. Avoids usual UNIX
** copy commands.
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPStr directory = NULL;
    AjBool iscor = ajFalse;
    AjBool isfam = ajFalse;
    AjBool isphy = ajFalse;
    

    embInit("jaspextract",argc,argv);

    directory = ajAcdGetDirectoryName("directory");
    
    iscor = jaspextract_check(directory, J_CORE);
    isfam = jaspextract_check(directory, J_FAM);
    isphy = jaspextract_check(directory, J_PHY);

    if(iscor)
	jaspextract_copy(directory, J_CORE);
    if(isfam)
	jaspextract_copy(directory, J_FAM);
    if(isphy)
	jaspextract_copy(directory, J_PHY);

    embExit();

    return 0;
}




/* @funcstatic jaspextract_check ***********************************************
**
** Checks whether a specific subdirectory exists
**
** @param [r] directory [const AjPStr] jaspar directory
** @param [r] sdir      [const char *] subdirectory name
**
** @return [AjBool] ajTrue if the subdirectory exists
** @@
******************************************************************************/

static AjBool jaspextract_check(const AjPStr directory, const char *sdir)
{
    AjBool ret = ajFalse;
    AjPStr subdir = NULL;

    subdir = ajStrNew();
    
    ajFmtPrintS(&subdir,"%S%s",directory,sdir);
    if(!ajFileDir(&subdir))
    {
	ajWarn("Cannot find directory %S",subdir);
	ret = ajFalse;
    }
    else
	ret = ajTrue;

    ajStrDel(&subdir);

    return ret;
}




/* @funcstatic jaspextract_copy ***********************************************
**
** Copy datafile into the EMBOSS  data area
**
** @param [r] directory [const AjPStr] jaspar directory
** @param [r] sdir      [const char *] subdirectory name
**
** @return [void]
** @@
******************************************************************************/

static void jaspextract_copy(const AjPStr directory, const char *sdir)
{
    AjPStr subdir = NULL;
    AjPStr wild   = NULL;
    AjPList flist = NULL;
    AjPStr entry  = NULL;
    AjPStr nbase  = NULL;
    AjPStr dfile  = NULL;
    AjPFile inf   = NULL;
    AjPFile outf  = NULL;
    AjPStr line   = NULL;
    
    subdir = ajStrNew();
    wild   = ajStrNewC("*");
    flist  = ajListNew();
    nbase  = ajStrNew();
    dfile  = ajStrNew();
    line   = ajStrNew();
    
    ajFmtPrintS(&subdir,"%S%s",directory,sdir);
    ajFileScan(subdir,wild,&flist,ajFalse,ajFalse,NULL,NULL,ajFalse,NULL);


    while(ajListPop(flist,(void **)&entry))
    {
	ajStrAssignS(&nbase,entry);
	ajFileNameTrim(&nbase);
	ajFmtPrintS(&dfile,"%s%c%S",sdir,SLASH_CHAR,nbase);

	/* Avoid UNIX copy for portability */
	inf  = ajFileNewIn(entry);
	if(!inf)
	    ajFatal("Cannot open input file: %S",entry);

	ajFileDataNewWrite(dfile,&outf);
	if(!outf)
	    ajFatal("Cannot open output file: %S",dfile);

	while(ajFileReadLine(inf,&line))
	    ajFmtPrintF(outf,"%S\n",line);

	ajFileClose(&inf);
	ajFileClose(&outf);

	ajStrDel(&entry);
    }

    ajListFree(&flist);
    
    ajStrDel(&subdir);
    ajStrDel(&wild);
    ajStrDel(&dfile);
    ajStrDel(&nbase);
    ajStrDel(&line);
    
    return;
}
