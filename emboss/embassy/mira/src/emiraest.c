/* @source emiraest application
**
** Wrapper for MIRA by Bastien Chevreux
**
** @author Copyright (C) Alan Bleasby
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


#define MIRATAB_GUESS 200


/* @datastatic MiraPrefix **************************************************
**
** MIRA qualifier names and their prefixes
**
** @alias MiraSPrefix
** @alias MiraOPrefix
**
** @attr qname [const char*] emboss qualifier name
** @attr prefix [const char*] mira prefix
** @@
******************************************************************************/

typedef struct MiraSPrefix
{
    const char* qname;
    const char* prefix;
} MiraOPrefix;
#define MiraPPrefix MiraOPrefix*

static MiraOPrefix miraprefix[] = {
    {"project", "GE:"},
    {"lj", "GE:"},
    {"fo", "GE:"},
    {"mxti", "GE:"},
    {"rns", "GE:"},
    {"eq", "GE:"},
    {"eqo", "GE:"},
    {"droeqe", "GE:"},
    {"uti", "GE:"},
    {"ess", "GE:"},
    {"ps", "GE:"},

    {"lsd", "SB:"},
    {"lb", "SB:"},
    {"sbuip", "SB:"},
    {"bsn", "SB:"},
    {"bft", "SB:"},
    {"brl", "SB:"},
    {"bbq", "SB:"},
    {"abnc", "SB:"},

    {"mrl", "AS:"},
    {"nop", "AS:"},
    {"sep", "AS:"},
    {"rbl", "AS:"},
    {"sd", "AS:"},
    {"sdlpo", "AS:"},
    {"bdq", "AS:"},
    {"ugpf", "AS:"},
    {"uess", "AS:"},
    {"esspd", "AS:"},
    {"umcbt", "AS:"},
    {"bts", "AS:"},

    {"ure", "DP:"},
    {"rewl", "DP:"},
    {"rewme", "DP:"},
    {"feip", "DP:"},
    {"leip", "DP:"},
    {"tpae", "DP:"},
    {"pbwl", "DP:"},
    {"pbwme", "DP:"},
    {"pbwgd", "DP:"},

    {"pvc", "CL:"},
    {"pvcmla", "CL:"},
    {"qc", "CL:"},
    {"qcmq", "CL:"},
    {"qcwl", "CL:"},
    {"mbc", "CL:"},
    {"mbcgs", "CL:"},
    {"mbcmfg", "CL:"},
    {"mbcmeg", "CL:"},
    {"emlc", "CL:"},
    {"mlcr", "CL:"},
    {"smlc", "CL:"},

    {"bph", "SK:"},
    {"hss", "SK:"},
    {"pr", "SK:"},
    {"mhpr", "SK:"},

    {"bip", "AL:"},
    {"bmin", "AL:"},
    {"bmax", "AL:"},
    {"mo", "AL:"},
    {"ms", "AL:"},
    {"mrs", "AL:"},
    {"egp", "AL:"},
    {"egpl", "AL:"},
    {"megpp", "AL:"},

    {"np", "CO:"},
    {"an", "CO:"},
    {"rodirs", "CO:"},
    {"dmer", "CO:"},
    {"mr", "CO:"},
    {"asir", "CO:"},
    {"mrpg", "CO:"},
    {"mgqrt", "CO:"},
    {"emea", "CO:"},
    {"amgb", "CO:"},
    {"amgbemc", "CO:"},
    {"amgbnbp", "CO:"},
    {"dismin", "CO:"},
    {"dismax", "CO:"},

    {"ace", "ED:"},
    {"sem", "ED:"},
    {"ct", "ED:"},

    {"gapfda", "DIR:"},
    {"exp", "DIR:"},
    {"scf", "DIR:"},
    {"log", "DIR:"},

    {"fei", "FI:"},
    {"fpi", "FI:"},
    {"pi", "FI:"},
    {"fai", "FI:"},
    {"fqi", "FI:"},
    {"ci", "FI:"},
    {"sdi", "FI:"},
    {"xtii", "FI:"},
    {"co", "FI:"},

    {"orc", "OUT:"},
    {"org", "OUT:"},
    {"orf", "OUT:"},
    {"ora", "OUT:"},
    {"ort", "OUT:"},
    {"ors", "OUT:"},
    {"orh", "OUT:"},
    {"otc", "OUT:"},
    {"otg", "OUT:"},
    {"otf", "OUT:"},
    {"ota", "OUT:"},
    {"ott", "OUT:"},
    {"ots", "OUT:"},
    {"oth", "OUT:"},
    {"oetc", "OUT:"},
    {"oetg", "OUT:"},
    {"oetf", "OUT:"},
    {"oeta", "OUT:"},
    {"oett", "OUT:"},
    {"oeth", "OUT:"},

    {"tcpl", "OUT:"},
    {"hcpl", "OUT:"},
    {"tegfc", "OUT:"},
    {"hegfc", "OUT:"},
    {NULL, NULL}
};





/* @datastatic MiraQuals **************************************************
**
** MIRA qualifier names and their types
**
** @alias MiraSQuals
** @alias MiraOQuals
**
** @attr qname [const char*] emboss qualifier name
** @attr mname [const char*] mira qualifier name
** @attr def [const char*] default ACD value
** @@
******************************************************************************/

typedef struct MiraSQuals
{
    const char* qname;
    const char* mname;
    const char* def;
} MiraOQuals;
#define MiraPQuals MiraOQuals*




static MiraOQuals mirainfiles[] = {
    {"paramsfile", "params", ""},
    {"feifile", "fei", "mira_in.fofn"},
    {"fpifile", "fpi", "mira_in.fofn"},
    {"pifile", "pi", "mira_in.phd"},
    {"faifile", "fai", "mira_in.fasta"},
    {"fqifile", "fqi", "mira_in.fasta.qual"},
    {"cifile", "ci", "mira_in.caf"},
    {"sdifile", "sdi", "mira_straindata_in.txt"},
    {"xtiifile", "xtii", "mira_xmltraceinfo_in.xml"},
    {NULL, NULL, NULL}
};

static MiraOQuals mirastrings[] = {
    {"project", "project", "mira"},
    {"bsn", "bsn", ""},
    {"np", "np", "mira"},
    {"gapfda", "gap4da", "gap4da"},
    {"log", "log", "miralog"},
    {"co", "co", "mira_out.caf"},
#if 0
    /* Can't use these until ACD can cope with a default string of " " */
    {"tegfc", "tegfc", " "},
    {"hegfc", "hegfc", " "},
#endif
    {NULL, NULL, NULL}
};



static MiraOQuals miradirectories[] = {
    {"expdir", "exp", "./"},
    {"scfdir", "scf", "./"},
    {NULL, NULL, NULL}
};




static MiraOQuals mirabools[] = {
    {"highlyrepetitive", "highlyrepetitive", "N"},
    {"highqualitydata", "highqualitydata", "N"},
    {"estmode", "estmode", "N"},
    {"horrid", "horrid", "N"},
    {"borg", "borg", "N"},
    {"fo", "fo", "N"},
    {"mxti", "mxti", "N"},
    {"eqo", "eqo", "N"},
    {"droeqe", "droeqe", "Y"},
    {"uti", "uti", "Y"},
    {"ps", "ps", "Y"},
    {"lsd", "lsd", "N"},
    {"lb", "lb", "N"},
    {"abnc", "abnc", "Y"},
    {"sep", "sep", "Y"},
    {"sd", "sd", "N"},
    {"sdlpo", "sdlpo", "Y"},
    {"ugpf", "ugpf", "Y"},
    {"uess", "uess", "Y"},
    {"umcbt", "umcbt", "N"},
    {"ure", "ure", "Y"},
    {"tpae", "tpae", "N"},
    {"pvc", "pvc", "Y"},
    {"qc", "qc", "N"},
    {"mbc", "mbc", "Y"},
    {"emlc", "emlc", "Y"},
    {"egp", "egp", "N"},
    {"mr", "mr", "Y"},
    {"asir", "asir", "N"},
    {"amgb", "amgb", "Y"},
    {"amgbemc", "amgbemc", "Y"},
    {"amgbnbs", "amgbnbs", "Y"},
    {"ace", "ace", "N"},
    {"sem", "sem", "Y"},
    {"orc", "orc", "Y"},
    {"org", "org", "Y"},
    {"orf", "orf", "Y"},
    {"ora", "ora", "N"},
    {"ort", "ort", "Y"},
    {"ors", "ors", "Y"},
    {"orh", "orh", "N"},
    {"otc", "otc", "N"},
    {"otg", "otg", "N"},
    {"otf", "otf", "N"},
    {"ota", "ota", "N"},
    {"ott", "ott", "N"},
    {"ots", "ots", "N"},
    {"oth", "oth", "N"},
    {"oetc", "oetc", "N"},
    {"oetg", "oetg", "N"},
    {"oetf", "oetf", "N"},
    {"oeta", "oeta", "N"},
    {"oett", "oett", "N"},
    {"oeth", "oeth", "N"},
    {NULL, NULL, NULL}
};




static MiraOQuals miralistsingles[] = {
    {"lj", "lj", "fofnexp"},
    {"rns", "rns", "sanger"},
    {"eq", "eq", "SCF"},
    {"bft", "bft", "fasta"},
    {"egpl", "egpl", "low"},
    {"an", "an", "signal"},
    {NULL, NULL, NULL}
};



static MiraOQuals miraintegers[] = {
    {"ess", "ess", "1"},
    {"sbuip", "sbuip", "3"},
    {"brl", "brl", "2500"},
    {"bbq", "bbq", "-1"},
    {"mrl", "mrl", "40"},
    {"nop", "nop", "3"},
    {"rbl", "rbl", "2"},
    {"bdq", "bdq", "10"},
    {"esspd", "esspd", "500"},
    {"bts", "bts", "10000"},
    {"rewl", "rewl", "30"},
    {"rewme", "rewme", "2"},
    {"feip", "feip", "0"},
    {"leip", "leip", "0"},
    {"pbwl", "pbwl", "7"},
    {"pbwme", "pbwme", "2"},
    {"pbwgd", "pbwgd", "9"},
    {"pvcmla", "pvcmla", "18"},
    {"qcmq", "qcmq", "20"},
    {"qcwl", "qcwl", "30"},
    {"mbcgs", "mbcgs", "20"},
    {"mbcmfg", "mbcmfg", "40"},
    {"mbcmeg", "mbcmeg", "60"},
    {"mlcr", "mlcr", "25"},
    {"smlc", "smlc", "30"},
    {"bph", "bph", "14"},
    {"hss", "hss", "4"},
    {"pr", "pr", "50"},
    {"mhpr", "mhpr", "200"},
    {"bip", "bip", "15"},
    {"bmin", "bmin", "25"},
    {"bmax", "bmax", "50"},
    {"mo", "mo", "15"},
    {"ms", "ms", "15"},
    {"mrs", "mrs", "65"},
    {"megpp", "megpp", "100"},
    {"rodirs", "rodirs", "15"},
    {"dmer", "dmer", "1"},
    {"mrpg", "mrpg", "2"},
    {"mgqrt", "mgqrt", "30"},
    {"emea", "emea", "15"},
    {"dismin", "dismin", "500"},
    {"dismax", "dismax", "5000"},
    {"ct", "ct", "50"},
    {"tcpl", "tcpl", "60"},
    {"hcpl", "hcpl", "60"},
    {NULL, NULL, NULL}
};




static void emiraest_dobools(AjPStr *cl, AjPTable table);
static void emiraest_dostrings(AjPStr *cl, AjPTable table);
static void emiraest_doinfiles(AjPStr *cl, AjPTable table);
static void emiraest_dodirectories(AjPStr *cl, AjPTable table);
static void emiraest_dolistsingles(AjPStr *cl, AjPTable table);
static void emiraest_dointegers(AjPStr *cl, AjPTable table);
static AjPTable emiraest_makepreftab(void);




/* @prog emiraest ************************************************************
**
** Wrapper for MIRAest
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPStr cl    = NULL;
    AjPStr stmp  = NULL;
    AjPStr squal = NULL;
    /* AjBool bqual = ajFalse; */
    AjPTable preftab = NULL;



    embInitPV("emiraest", argc, argv, "MIRA",VERSION);


    cl = ajStrNewS(ajAcdGetpathC("miraEST"));

    stmp = ajStrNew();

    preftab = emiraest_makepreftab();

    if(ajAcdIsUserdefinedC("genome"))
    {
	squal = ajAcdGetListSingle("genome");
	ajFmtPrintAppS(&cl," -genome%S",squal);
	ajStrDel(&squal);
    }

    if(ajAcdIsUserdefinedC("mapping"))
    {
	squal = ajAcdGetListSingle("mapping");
	ajFmtPrintAppS(&cl," -mapping%S",squal);
	ajStrDel(&squal);
    }

    if(ajAcdIsUserdefinedC("clipping"))
    {
	squal = ajAcdGetListSingle("clipping");
	ajFmtPrintAppS(&cl," -clipping%S",squal);
	ajStrDel(&squal);
    }

    if(ajAcdIsUserdefinedC("setparam"))
    {
	squal = ajAcdGetListSingle("setparam");
	if(!ajStrMatchC(squal,"unspecified"))
	    ajFmtPrintAppS(&cl," -%S",squal);
	ajStrDel(&squal);
    }


    emiraest_dostrings(&cl, preftab);
    emiraest_doinfiles(&cl, preftab);
    emiraest_dodirectories(&cl, preftab);
    emiraest_dobools(&cl, preftab);
    emiraest_dointegers(&cl, preftab);
    emiraest_dolistsingles(&cl, preftab);
    
    ajDebug("Constructed command line: %S\n",cl);

#if 0
   ajFmtPrint("%S\n",cl);
#endif

#if 1
    system(ajStrGetPtr(cl));
#endif

    ajStrDel(&stmp);
    ajStrDel(&cl);

    ajTablestrFree(&preftab);

    embExit();

    return 0;
}




/* @funcstatic emiraest_dobools *********************************************
**
** Get ACD bools.
** Only add to command line if value different from the default
**
** @param [w] cl [AjPStr*] command line
** @param [r] table [AjPTable] table relating qualifiers to prefixes
**
** @return [void]
** @@
******************************************************************************/

static void emiraest_dobools(AjPStr *cl, AjPTable table)
{
    ajuint i;
    AjBool bqual = ajFalse;
    AjBool def   = ajFalse;
    AjPStr prefix = NULL;
    AjPStr key    = NULL;
    AjPStr value  = NULL;
    
    prefix = ajStrNew();
    key    = ajStrNew();
    
    i = 0;

    while(mirabools[i].qname)
    {
	bqual = ajAcdGetBoolean(mirabools[i].qname);
	ajStrAssignC(&key,mirabools[i].qname);
	ajStrAssignC(&prefix,"");

	value = ajTableFetch(table, key);
	if(value)
	    ajStrAssignS(&prefix,value);

	def = ajCharMatchC(mirabools[i].def,"N") ? ajFalse : ajTrue;
	if(bqual != def)
	{
	    ajFmtPrintAppS(cl," -%S%s=",prefix,mirabools[i].mname);
	    if(bqual)
		ajFmtPrintAppS(cl,"yes");
	    else
		ajFmtPrintAppS(cl,"no");
	}
	
	++i;
    }

    ajStrDel(&key);
    ajStrDel(&prefix);
    
    return;
}




/* @funcstatic emiraest_dostrings *********************************************
**
** Get ACD strings.
** Only add to command line if value different from the default
**
** @param [w] cl [AjPStr*] command line
** @param [r] table [AjPTable] table relating qualifiers to prefixes
**
** @return [void]
** @@
******************************************************************************/

static void emiraest_dostrings(AjPStr *cl, AjPTable table)
{
    ajuint i;
    AjPStr squal = NULL;
    AjPStr prefix = NULL;
    AjPStr key    = NULL;
    AjPStr value  = NULL;
    
    prefix = ajStrNew();
    key    = ajStrNew();
    
    i = 0;

    while(mirastrings[i].qname)
    {
	squal = ajAcdGetString(mirastrings[i].qname);
	ajStrAssignC(&key,mirastrings[i].qname);
	ajStrAssignC(&prefix,"");

	value = ajTableFetch(table, key);
	if(value)
	    ajStrAssignS(&prefix,value);

	if(!ajStrMatchC(squal,mirastrings[i].def))
	    ajFmtPrintAppS(cl," -%S%s=%S",prefix,mirastrings[i].mname,squal);
	ajStrDel(&squal);
	++i;
    }

    ajStrDel(&key);
    ajStrDel(&prefix);
    
    return;
}




/* @funcstatic emiraest_doinfiles *********************************************
**
** Get ACD input files.
** Only add to command line if value different from the default
**
** @param [w] cl [AjPStr*] command line
** @param [r] table [AjPTable] table relating qualifiers to prefixes
**
** @return [void]
** @@
******************************************************************************/

static void emiraest_doinfiles(AjPStr *cl, AjPTable table)
{
    ajuint i;
    AjPStr squal = NULL;
    AjPStr prefix = NULL;
    AjPStr key    = NULL;
    AjPStr value  = NULL;
    AjPFile infile = NULL;
    
    prefix = ajStrNew();
    key    = ajStrNew();
    
    i = 0;

    while(mirainfiles[i].qname)
    {
	infile = ajAcdGetInfile(mirainfiles[i].qname);
	if(infile)
	   squal = ajStrNewS(ajFileGetName(infile));
	else
	    squal = ajStrNewS(ajAcdGetValueDefault(mirainfiles[i].qname));
	ajStrAssignC(&key,mirainfiles[i].qname);
	ajStrAssignC(&prefix,"");

	value = ajTableFetch(table, key);
	if(value)
	    ajStrAssignS(&prefix,value);

	if(!ajStrMatchC(squal,mirainfiles[i].def))
	    ajFmtPrintAppS(cl," -%S%s=%S",prefix,mirainfiles[i].mname,
			   squal);
	ajStrDel(&squal);
	ajFileClose(&infile);
	++i;
    }

    ajStrDel(&key);
    ajStrDel(&prefix);
    
    return;
}




/* @funcstatic emiraest_dodirectories ****************************************
**
** Get ACD directories.
** Only add to command line if value different from the default
**
** @param [w] cl [AjPStr*] command line
** @param [r] table [AjPTable] table relating qualifiers to prefixes
**
** @return [void]
** @@
******************************************************************************/

static void emiraest_dodirectories(AjPStr *cl, AjPTable table)
{
    ajuint i;
    AjPStr squal = NULL;
    AjPStr prefix = NULL;
    AjPStr key    = NULL;
    AjPStr value  = NULL;
    
    prefix = ajStrNew();
    key    = ajStrNew();
    
    i = 0;

    while(miradirectories[i].qname)
    {
	squal = ajAcdGetDirectoryName(miradirectories[i].qname);
	ajStrAssignC(&key,miradirectories[i].qname);
	ajStrAssignC(&prefix,"");

	value = ajTableFetch(table, key);
	if(value)
	    ajStrAssignS(&prefix,value);

	if(!ajStrMatchC(squal,miradirectories[i].def))
	    ajFmtPrintAppS(cl," -%S%s=%S",prefix,miradirectories[i].mname,
			   squal);
	ajStrDel(&squal);
	++i;
    }

    ajStrDel(&key);
    ajStrDel(&prefix);
    
    return;
}




/* @funcstatic emiraest_dolistsingles ****************************************
**
** Get ACD single value list entries.
** Only add to command line if value different from the default
**
** @param [w] cl [AjPStr*] command line
** @param [r] table [AjPTable] table relating qualifiers to prefixes
**
** @return [void]
** @@
******************************************************************************/

static void emiraest_dolistsingles(AjPStr *cl, AjPTable table)
{
    ajuint i;
    AjPStr squal = NULL;
    AjPStr prefix = NULL;
    AjPStr key    = NULL;
    AjPStr value  = NULL;
    
    prefix = ajStrNew();
    key    = ajStrNew();
    
    i = 0;

    while(miralistsingles[i].qname)
    {
	squal = ajAcdGetListSingle(miralistsingles[i].qname);
	ajStrAssignC(&key,miralistsingles[i].qname);
	ajStrAssignC(&prefix,"");

	value = ajTableFetch(table, key);
	if(value)
	    ajStrAssignS(&prefix,value);

	if(!ajStrMatchC(squal,miralistsingles[i].def))
	    ajFmtPrintAppS(cl," -%S%s=%S",prefix,miralistsingles[i].mname,
			   squal);
	ajStrDel(&squal);
	++i;
    }
    
    ajStrDel(&key);
    ajStrDel(&prefix);

    return;
}




/* @funcstatic emiraest_dointegers ********************************************
**
** Get ACD integers.
** Only add to command line if value different from the default
**
** @param [w] cl [AjPStr*] command line
** @param [r] table [AjPTable] table relating qualifiers to prefixes
**
** @return [void]
** @@
******************************************************************************/

static void emiraest_dointegers(AjPStr *cl, AjPTable table)
{
    ajuint i;
    ajint  iqual;
    int  dval = 0;
    AjPStr prefix = NULL;
    AjPStr key    = NULL;
    AjPStr value  = NULL;
    
    prefix = ajStrNew();
    key    = ajStrNew();
    
    i = 0;

    while(miraintegers[i].qname)
    {
	iqual = ajAcdGetInt(miraintegers[i].qname);
	ajStrAssignC(&key,miraintegers[i].qname);
	ajStrAssignC(&prefix,"");

	value = ajTableFetch(table, key);
	if(value)
	    ajStrAssignS(&prefix,value);

	if(sscanf(miraintegers[i].def,"%d",&dval) != 1)
	    ajFatal("Dval conversion error in emiraest_dointegers()");

	if(iqual != dval)
	    ajFmtPrintAppS(cl," -%S%s=%d",prefix,miraintegers[i].mname,iqual);

	++i;
    }

    ajStrDel(&key);
    ajStrDel(&prefix);
    
    return;
}




/* @funcstatic emiraest_makepreftab *******************************************
**
** Make table relating qualifiers to prefixes
** e.g. -project has the prefix -GE:
**
** @return [AjPTable] qualifier/prefix table
** @@
******************************************************************************/

static AjPTable emiraest_makepreftab(void)
{
    AjPTable ret = NULL;
    ajuint i;
    AjPStr key = NULL;
    AjPStr value = NULL;
    
    ret = ajTablestrNewLen(MIRATAB_GUESS);

    i = 0;
    while(miraprefix[i].qname)
    {
	key   = ajStrNewC(miraprefix[i].qname);
	value = ajStrNewC(miraprefix[i].prefix);

	ajTablePut(ret, (void *)key, (void *)value);
	++i;
    }

    return ret;
}
