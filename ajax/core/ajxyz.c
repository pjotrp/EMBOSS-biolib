/********************************************************************
** @source AJAX structure functions
**
** @author Copyright (C) 2000 Jon Ison
** @version 1.0 
** @modified Nov 08 jci First version
** @@
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
** 
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
********************************************************************/

#include <math.h>
#include "ajax.h"





AjBool   ajCpdbWriteAll(AjPFile outf, AjPPdb pdb)
{
    int      x;
    int      y;
    int      offset;
    int      done_first_co_line;
    AjIList  iter=NULL;
    AjPAtom  tmp=NULL;
    

    ajFmtPrintF(outf, "%-5s%S\n", "ID", pdb->Pdb);
    ajFmtPrintF(outf, "XX\n");

    ajFmtPrintSplit(outf,pdb->Compnd,"DE   ",75," \t\r\n");
    ajFmtPrintF(outf, "XX\n");

    ajFmtPrintSplit(outf,pdb->Source,"OS   ",75," \t\r\n");
    ajFmtPrintF(outf, "XX\n");

    ajFmtPrintF(outf, "%-5sMETHOD ", "EX");
    if(pdb->Method == XRAY)
	ajFmtPrintF(outf, "xray; ");	
    else
	ajFmtPrintF(outf, "nmr_or_model; ");		
    ajFmtPrintF(outf, "RESO %.2f; NMOD %d; NCHA %d;\n", pdb->Reso,
		pdb->Nmod, pdb->Nchn);

    for(x=0;x<pdb->Nchn;x++)
    { 
	ajFmtPrintF(outf, "XX\n");	
	ajFmtPrintF(outf, "%-5s[%d]\n", "CN", x+1);	
	ajFmtPrintF(outf, "XX\n");	
	ajFmtPrintF(outf, "%-5sID %c; NR %d; NH %d; NW %d;\n", 
		    "IN", 
		    pdb->Chains[x]->Id,
		    pdb->Chains[x]->Nres,
		    pdb->Chains[x]->Nhet,
		    pdb->Chains[x]->Nwat);
	ajFmtPrintF(outf, "XX\n");	
	ajSeqWriteCdb(outf, pdb->Chains[x]->Seq);
    }

    ajFmtPrintF(outf, "XX\n");	

    for(x=1;x<=pdb->Nmod;x++)
    {
	for(y=0;y<pdb->Nchn;y++)
	{
	    done_first_co_line=0;
	    offset=0;	
	    
	    iter=ajListIter(pdb->Chains[y]->Atoms);
	    while(ajListIterMore(iter))
	    {
		tmp=(AjPAtom)ajListIterNext(iter);
		if(tmp->Mod!=x)
		    continue;
		else	
		{
		    if(!done_first_co_line)
		    {
			ajIntPut(&pdb->Chains[y]->Models, x-1, offset);	
			done_first_co_line=1;
		    }
		    offset++;
		    
		    ajFmtPrintF(outf, "%-6s%-6d%-6d%-6c%-6d%-6d%-5c%-6S%-6S"
				"%-9.3f%-9.3f%-9.3f\n", 
				"CO", 
				tmp->Mod, 
				tmp->Chn, 
				tmp->Type, 
				tmp->Idx, 
				tmp->Pdb, 
				tmp->Id1, 
				tmp->Id3,
				tmp->Atm, 
				tmp->X, 
				tmp->Y, 
				tmp->Z);
		}
	    }
	    ajListIterFree(iter);			
	} 	
    }
    ajFmtPrintF(outf, "//\n");    
}


/*
KNOWN FEATURES:
Only ever returns ajTrue.  Needs modifying to return ajFalse
in case of bad file format, failed file read etc.
*/  
AjBool   ajCpdbRead(AjPStr str, AjPPdb *pdb)
{
    AjPFile   inf=NULL;
    AjPStr    fn=NULL;
    AjPStr    line=NULL;
    AjPStr    token=NULL;
    AjPStr    idstr=NULL;
    AjPStr    destr=NULL;
    AjPStr    osstr=NULL;
    AjPStr     xstr=NULL;
    AjPStrTok handle=NULL;
    
    AjPAtom atom=NULL;
    
    float reso=0.0;
    int   nmod=0;
    int   ncha=0;
    int   i=0;
    int   nc=0;
    int   mod=0;
    int   chn=0;
    int   offset=0;        
    int   last_chn=0;
    int   last_mod=0;
    int   val;
    int   x;
    int   done_first_co_line=0;
    

    fn = ajStrNewC(ajStrStr(str));
    ajFileDataNew(fn,&inf);
    if(!inf)
    {
	(void) ajStrAssC(&fn,"CPDB/");
	(void) ajStrAppC(&fn,ajStrStr(str));
	ajFileDataNew(fn,&inf);
	if(!inf)
	{
	    ajStrDel(&fn);
	    return ajFalse;
	}
    }
    
 

    line  = ajStrNew();
    token = ajStrNew();
    
    idstr = ajStrNew();
    destr = ajStrNew();
    osstr = ajStrNew();
    xstr  = ajStrNew();


    while(ajFileReadLine(inf,&line))
    {
	if(ajStrPrefixC(line,"XX"))
	    continue;
	
	if(ajStrPrefixC(line,"ID"))
	{
	    ajStrTokenAss(&handle,line," \n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&idstr,&handle,NULL);
	    continue;
	}

	if(ajStrPrefixC(line,"CN"))
	{
	    ajStrTokenAss(&handle,line," []\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&nc);
	    continue;
	}
	
	if(ajStrPrefixC(line,"DE"))
	{
	    (void) ajStrTokenAss (&handle, line, " ");
	    (void) ajStrToken (&token, &handle, NULL);
	    /* 'DE' */
	    (void) ajStrToken (&token, &handle, "\n\r");
	    /* desc */
	    if (ajStrLen(destr))
	    {
		(void) ajStrAppC (&destr, " ");
		(void) ajStrApp (&destr, token);
	    }
	    else
		(void) ajStrAss (&destr, token);
	    continue;
	}

	if(ajStrPrefixC(line,"OS"))
	{
	    (void) ajStrTokenAss (&handle, line, " ");
	    (void) ajStrToken (&token, &handle, NULL);
	    /* 'OS' */
	    (void) ajStrToken (&token, &handle, "\n\r");
	    /* source */
	    if (ajStrLen(osstr))
	    {
		(void) ajStrAppC (&osstr, " ");
		(void) ajStrApp (&osstr, token);
	    }
	    else
		(void) ajStrAss (&osstr, token);
	    continue;
	}
	

	if(ajStrPrefixC(line,"EX"))
	{
	    ajStrTokenAss(&handle,line," ;\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&xstr,&handle,NULL); /* xray */
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* reso */
	    ajStrToFloat(token,&reso);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* nmod */
	    ajStrToInt(token,&nmod);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* ncha */
	    
	    ajStrToInt(token,&ncha);

	    *pdb = ajPdbNew(ncha, nmod);

	    ajStrAssS(&(*pdb)->Pdb,idstr);
	    ajStrAssS(&(*pdb)->Compnd,destr);
	    ajStrAssS(&(*pdb)->Source,osstr);
	    if(ajStrMatchC(xstr,"xray"))
		(*pdb)->Method = XRAY;
	    else
		(*pdb)->Method = NMR;

	    (*pdb)->Reso = reso;
	    (*pdb)->Nmod = nmod;
	    (*pdb)->Nchn = ncha;

	    /*Set the offests for the first model of each chain to zero */
	    /*
	    for(x=0; x<ncha; x++)
		ajIntPut(&(*pdb)->Chains[x]->Models, 0, 0);	
*/

	}
	

	if(ajStrPrefixC(line,"IN"))
	{
	    ajStrTokenAss(&handle,line," ;\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* id value */
	    (*pdb)->Chains[nc-1]->Id=*ajStrStr(token);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* residues */
	    ajStrToInt(token,&(*pdb)->Chains[nc-1]->Nres);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* hetatm */
	    ajStrToInt(token,&(*pdb)->Chains[nc-1]->Nhet);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* water */
	    ajStrToInt(token,&(*pdb)->Chains[nc-1]->Nwat);
	    continue;
	}
  



	if(ajStrPrefixC(line,"SQ"))
	{
	    while(ajFileReadLine(inf,&line) && !ajStrPrefixC(line,"XX"))
		ajStrAppC(&(*pdb)->Chains[nc-1]->Seq,ajStrStr(line));
	    ajStrCleanWhite(&(*pdb)->Chains[nc-1]->Seq);
	    continue;
	}
    
	if(ajStrPrefixC(line,"CO"))
	{
	    ajStrTokenAss(&handle,line," \t\n\r");
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&mod);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&chn);
	    
	    AJNEW0(atom);
	    atom->Mod = mod;
	    atom->Chn = chn;


	    if(done_first_co_line == 0)
	    {
		last_chn=chn;
		last_mod=mod;
		done_first_co_line=1;
	    }
	    
	    /*Brave but naive ... 
	    if((chn!=last_chn) || (mod!=last_mod))		
	    {
		if(last_mod < nmod)
		{
		    val=ajIntGet((*pdb)->Chains[last_chn-1]->Models,
				 last_mod-1); 	
		    ajIntPut(&(*pdb)->Chains[last_chn-1]->Models, last_mod,
			     offset+val);
		}
		offset=0;
		last_chn=chn;
		last_mod=mod;
	    }
	    else	
	    {
		offset++;	    
	    }
	    */


	    ajStrToken(&token,&handle,NULL);
	    atom->Type = *ajStrStr(token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->Idx);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->Pdb);

	    ajStrToken(&token,&handle,NULL);
	    atom->Id1 = *ajStrStr(token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Id3,token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Atm,token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->X);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->Y);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->Z);

	    ajListPushApp((*pdb)->Chains[chn-1]->Atoms,(void *)atom);
	}
	
    }
    ajStrTokenClear(&handle);

    ajStrDel(&line);
    ajStrDel(&token);
    ajStrDel(&idstr);
    ajStrDel(&destr);
    ajStrDel(&osstr);
    ajStrDel(&xstr);

    ajFileClose(&inf);

    return ajTrue;
}








AjPAtom ajAtomNew(void)
{
    AjPAtom ret=NULL;

    AJNEW0(ret);
    
    ret->Id3 = ajStrNew();
    ret->Atm = ajStrNew();

    return ret;
}




void ajAtomDel(AjPAtom *thys)
{
    ajStrDel(&(*thys)->Id3);
    ajStrDel(&(*thys)->Atm);

    AJFREE(*thys);

    return;
}




AjPChain ajChainNew(int n)
{
  AjPChain ret=NULL;
  int val;
  
  AJNEW0(ret);

  ret->Seq    = ajStrNewC("");
  ret->Atoms  = ajListNew();
  ret->Models = ajIntNewL(n);  

  return ret;
}




void ajChainDel(AjPChain *thys)
{
    AjPAtom atm=NULL;
    
    while(ajListPop((*thys)->Atoms,(void **)&atm))
	ajAtomDel(&atm);

    ajListDel(&(*thys)->Atoms);

    ajIntDel(&(*thys)->Models);  
    
    AJFREE(*thys);

    return;
}



AjPPdb ajPdbNew(int n, int m)
{
    AjPPdb ret=NULL;
    int i;
    
    AJNEW0(ret);
  

    ret->Pdb = ajStrNew();
    ret->Compnd = ajStrNew();
    ret->Source = ajStrNew();

    AJCNEW0(ret->Chains,n);
    for(i=0;i<n;++i)
	ret->Chains[i] = ajChainNew(m);
    
    return ret;
}



void ajPdbDel(AjPPdb *thys)
{
    int nc=0;
    int i=0;
    
    nc = (*thys)->Nchn;

    ajStrDel(&(*thys)->Pdb);
    ajStrDel(&(*thys)->Compnd);
    ajStrDel(&(*thys)->Source);
    
    for(i=0;i<nc;++i)
	ajChainDel(&(*thys)->Chains[i]);
    AJFREE(*thys);

    return;
}
