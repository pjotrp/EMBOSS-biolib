#include "ajax.h"
AjBool vienna_GetConstraints(AjPFile confile, AjPStr *constring);

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
    

    ajStrAssignC(constring,"");
    

    while(ajFileReadLine(confile,&line))
    {
	if(ajStrPrefixC(line,"#") || ajStrPrefixC(line,"*") || !ajStrGetLen(line))
	    continue;
	len = ajStrGetLen(line);

	ajStrAssignC(&constr,"");
	ajStrAssignC(&conline,"");

	cline = MAJSTRGETPTR(line);
	
	if(strspn(cline,rna_accept) == len)
	{
	    ajStrAssignC(&seqstr,"");	    
	    ajStrAssignS(&seqstr,line);
	    seqlen = ajStrGetLen(seqstr);

	    if(!ajFileReadLine(confile,&line))
		ajFatal("Missing constraint line in file (%F)\n",confile);
	    if(ajStrPrefixC(line,"#") || ajStrPrefixC(line,"*") ||
	       !ajStrGetLen(line))
		continue;

	    len = ajStrGetLen(line);
	}
	else
	    seqlen = 0;


	if(strspn(cline,con_accept) == len)
	    ajStrAssignS(&conline,line);
	else
	    ajFatal("Invalid line [%S]\n",line);


	if((seqlen))
	{
	    ajStrAssignS(&cleanstr,seqstr);
	    ajStrRemoveWhite(&cleanstr);
	}

	conlen = ajStrGetLen(conline);
	if(!conlen)
	    ajFatal("Missing constraint line\n");
	
	if(seqlen && conlen > seqlen)
	    ajFatal("Constraint line longer than sequence line\n");

	if(seqlen)
	{
	    for(i=0;i<conlen;++i)
	    {
		cs = MAJSTRGETPTR(seqstr)[i];
		cc = MAJSTRGETPTR(conline)[i];
		
		if(cs == ' ')
		{
		    if(cc != ' ')
			ajFatal("Constraint character given for a sequence "
				"space character\n");
		    else
			continue;
		}
		ajStrAppendK(&constr,cc);
	    }

	    padding = ajStrGetLen(cleanstr) - ajStrGetLen(constr);
	    for(i=0; i<padding; ++i)
		ajStrAppendK(&constr,' ');
	    ajStrAppendS(constring,constr);
	}
	else
	    ajStrAppendS(constring,conline);


    }
    
    ajStrDel(&line);
    ajStrDel(&seqstr);
    ajStrDel(&constr);
    ajStrDel(&cleanstr);
    ajStrDel(&conline);

    return ajTrue;
}
