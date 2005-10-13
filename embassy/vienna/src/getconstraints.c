#include "ajax.h"

AjBool vienna_GetConstraints(AjPFile confile, AjPStr *constring)
{
    AjPStr line = NULL;
    static char *rna_accept="aAcCgGtTuU ";
    static char *con_accept=" |.x<>()";
    ajint len;
    char *cline = NULL;
    AjPStr seqstr = NULL;
    AjPStr constr = NULL;
    AjPStr cleanstr = NULL;
    AjPStr conline = NULL;
    
    ajint i;
    ajint conlen = 0;
    char cs;
    char cc;
    ajint padding;
    ajint seqlen = 0;
    
    

    line = ajStrNew();
    seqstr = ajStrNew();
    constr = ajStrNew();
    cleanstr = ajStrNew();
    conline  = ajStrNew();
    

    ajStrAssC(constring,"");
    

    while(ajFileReadLine(confile,&line))
    {
	if(ajStrPrefixC(line,"#") || ajStrPrefixC(line,"*") || !ajStrLen(line))
	    continue;
	len = ajStrLen(line);

	ajStrAssC(&constr,"");
	ajStrAssC(&conline,"");

	cline = MAJSTRSTR(line);
	
	if(strspn(cline,rna_accept) == len)
	{
	    ajStrAssC(&seqstr,"");	    
	    ajStrAssS(&seqstr,line);
	    seqlen = ajStrLen(seqstr);

	    if(!ajFileReadLine(confile,&line))
		ajFatal("Missing constraint line in file (%F)\n",confile);
	    if(ajStrPrefixC(line,"#") || ajStrPrefixC(line,"*") ||
	       !ajStrLen(line))
		continue;

	    len = ajStrLen(line);
	}
	else
	    seqlen = 0;


	if(strspn(cline,con_accept) == len)
	    ajStrAssS(&conline,line);
	else
	    ajFatal("Invalid line [%S]\n",line);


	if((seqlen))
	{
	    ajStrAssS(&cleanstr,seqstr);
	    ajStrCleanWhite(&cleanstr);
	}

	conlen = ajStrLen(conline);
	if(!conlen)
	    ajFatal("Missing constraint line\n");
	
	if(seqlen && conlen > seqlen)
	    ajFatal("Constraint line longer than sequence line\n");

	if(seqlen)
	{
	    for(i=0;i<conlen;++i)
	    {
		cs = MAJSTRSTR(seqstr)[i];
		cc = MAJSTRSTR(conline)[i];
		
		if(cs == ' ')
		{
		    if(cc != ' ')
			ajFatal("Constraint character given for a sequence "
				"space character\n");
		    else
			continue;
		}
		ajStrAppK(&constr,cc);
	    }

	    padding = ajStrLen(cleanstr) - ajStrLen(constr);
	    for(i=0; i<padding; ++i)
		ajStrAppK(&constr,' ');
	    ajStrApp(constring,constr);
	}
	else
	    ajStrApp(constring,conline);


    }
    
    ajStrDel(&line);
    ajStrDel(&seqstr);
    ajStrDel(&constr);
    ajStrDel(&cleanstr);
    ajStrDel(&conline);

    return ajTrue;
}
