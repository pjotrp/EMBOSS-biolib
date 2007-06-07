/******************************************************************************
** @source AJAX system functions
**
** Copyright (c) Alan Bleasby 1999
** @version 1.0
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
******************************************************************************/

#include "ajax.h"

#ifndef WIN32
#ifndef __VMS
#include <termios.h>
#endif
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#else
#include "win32.h"
#endif


static AjPStr sysTokRets = NULL;
static AjPStr sysTokSou  = NULL;
static const char *sysTokp = NULL;


/* @filesection ajutil ********************************************************
**
** @nam1rule aj   Function belongs to the AJAX library.
**
*/

/* @datasection [none] System utility functions *******************************
**
** @nam2rule Sys  Function belongs to the AJAX ajsys library.
**
*/

/* @section Argument list manipulation functions ******************************
**
** Function for manipulating argument list.
**
** @fdata [none]
**
** @nam3rule  Arglist      Function for manipulating argument list.
** @nam4rule  ArglistBuild Generates a program name and argument list from a 
**                         command line string.
** @nam4rule  ArglistFree   Free memory in an argument list allocated by 
**                         ajSysArglistBuild.
** 
** @argrule Build   cmdline   [const AjPStr] Original command line
** @argrule Build   Pname     [char**] Returned program name
** @argrule Arglist PParglist [char***] Returns argument array
**
** @valrule Build [AjBool] True on success
** @valrule Free [void]
**
** @fcategory misc
**
******************************************************************************/

/* @func ajSysArglistBuild ****************************************************
**
** Generates a program name and argument list from a command line string.
**
** @param [r] cmdline [const AjPStr] Command line.
** @param [w] Pname [char**] Program name.
** @param [w] PParglist [char***] Argument list.
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

AjBool ajSysArglistBuild(const AjPStr cmdline, char** Pname, char*** PParglist)
{    
    static AjPRegexp argexp = NULL;
    AjPStr tmpline          = NULL;
    const char* cp;
    ajint ipos = 0;
    ajint iarg = 0;
    ajint ilen = 0;
    ajint i;
    char** al;
    AjPStr argstr = NULL;
    
    if(!argexp)
	argexp = ajRegCompC("^[ \t]*(\"([^\"]*)\"|'([^']*)'|([^ \t]+))");
    
    ajDebug("ajSysArglistBuild '%S'\n", cmdline);
    
    ajStrAssignS(&tmpline, cmdline);
    
    cp   = ajStrGetPtr(cmdline);
    ipos = 0;
    while(ajRegExecC(argexp, &cp[ipos]))
    {
	ipos += ajRegLenI(argexp, 0);
	iarg++;
    }
    
    AJCNEW(*PParglist, iarg+1);
    al   = *PParglist;
    ipos = 0;
    iarg = 0;
    while(ajRegExecC(argexp, &cp[ipos]))
    {
	ilen = ajRegLenI(argexp, 0);
	ajStrDelStatic(&argstr);
	for(i=2;i<5;i++)
	{
	    if(ajRegLenI(argexp, i))
	    {
		ajRegSubI(argexp, i, &argstr);
		/*ajDebug("parsed [%d] '%S'\n", i, argstr);*/
		break;
	    }
	}
	ipos += ilen;

	if(!iarg)
	    *Pname = ajCharNewS(argstr);

	al[iarg] = ajCharNewS(argstr);
	iarg++;
    }

    al[iarg] = NULL;
    
    ajRegFree(&argexp);
    argexp = NULL;
    ajStrDel(&tmpline);
    ajStrDel(&argstr);
    
    ajDebug("ajSysArglistBuild %d args for '%s'\n", iarg, *Pname);

    return ajTrue;
}


/* @obsolete ajSysArglist
** @rename ajSysArglistBuild
*/

__deprecated AjBool ajSysArglist(const AjPStr cmdline,
				 char** Pname, char*** PParglist)
{    
    return ajSysArglistBuild(cmdline, Pname, PParglist);
}


/* @func ajSysArglistFree *****************************************************
**
** Free memory in an argument list allocated by ajSysArgList
**
** @param [w] PParglist [char***] Argument list.
** @return [void]
** @@
******************************************************************************/

void ajSysArglistFree(char*** PParglist)
{
    char** ca;
    ajint i;

    ca = *PParglist;

    i = 0;
    while(ca[i])
    {
	AJFREE(ca[i]);
	++i;
    }

    AJFREE(*PParglist);

    return;
}


/* @obsolete ajSysArglistFree
** @rename ajSysArglistFree
*/

__deprecated void ajSysArgListFree(char*** PParglist)
{
    ajSysArglistFree(PParglist);
    return;
}

/* @section System cast functions *********************************************
**
** Function for casting one datatype to another.
**
**
** @nam3rule  Cast         Function for casting one datatype to another.
** @nam4rule  CastItoc     Convert Int to Char (for fussy compilers)
** @nam4rule  CastItouc    Convert Int to Unsigned Char  (for fussy compilers). 
** 
** @fcategory misc
**
******************************************************************************/

/* @func ajSysItoC ************************************************************
**
** Convert Int to Char
** Needed for very fussy compilers i.e. Digital C
**
** @param [r] v [ajint] integer
** @return [char] Character cast
** @@
******************************************************************************/

char ajSysItoC(ajint v)
{
    char c;

    c = (char) v;
    return c;
}




/* @func ajSysItoUC ***********************************************************
**
** Convert Int to Unsigned Char
** Needed for very fussy compilers i.e. Digital C
**
** @param [r] v [ajint] integer
** @return [unsigned char] Unisigned character cast
** @@
******************************************************************************/

unsigned char ajSysItoUC(ajint v)
{
    char c;

    c = (unsigned char) v;
    return c;
}


/* @section System functions for files ****************************************
**
** System functions for files.
**
**
** @nam3rule  File           System functions for files.
** @nam4rule  FileWhich      Searches $PATH sequentially for a user-EXECUTABLE 
**                           file.
** @nam5rule  FileWhichEnv   Uses environment to extract the PATH list.
** @nam4rule  FileUnlink     Deletes a file or link
** 
** @valrule   *  [AjBool]  True if operation is successful.
**
** @fcategory misc
**
******************************************************************************/


/* @func ajSysUnlink **********************************************************
**
** Deletes a file or link
**
** @param [r] s [const AjPStr] Filename in AjStr.
** @return [AjBool] true if deleted false otherwise
** @@
******************************************************************************/

AjBool ajSysUnlink(const AjPStr s)
{
    ajDebug("ajSysUnlink '%S'\n", s);

#ifndef WIN32
    if(!unlink(ajStrGetPtr(s)))
	return ajTrue;
#else
    if(DeleteFile(ajStrGetPtr(s)))
	return ajTrue;
#endif
    ajDebug("ajSysUnlink failed to delete '%S'\n", s);
    return ajFalse;
}










/* @func ajSysWhich ***********************************************************
**
** Gets the Basename of a file then searches $PATH sequentially until it
** finds a user-EXECUTABLE file of the same name.
**
** @param [u] s [AjPStr*] Filename in AjStr, replaced by full pathname
** @return [AjBool] True if executable found, false otherwise
** @@
******************************************************************************/

AjBool ajSysWhich(AjPStr *s)
{
    char *p;
    static AjPStr tname = NULL;
    static AjPStr fname = NULL;

    p = getenv("PATH");
    if(!p)
	return ajFalse;

    ajStrAssignS(&tname, *s);

    if(!fname)
	fname = ajStrNew();

    ajFileNameTrim(&tname);

    p=ajSysStrtok(p,PATH_SEPARATOR);

    if(p==NULL)
    {
	ajStrDelStatic(&fname);
	ajStrDelStatic(&tname);
	return ajFalse;
    }


    while(1)
    {
	ajFmtPrintS(&fname,"%s%s%S",p,SLASH_STRING,tname);

	if(ajFileStat(fname, AJ_FILE_X))
	{
	    ajStrSetClear(s);
	    ajStrAssignEmptyS(s,fname);
	    break;
	}

	if((p = ajSysStrtok(NULL,PATH_SEPARATOR))==NULL)
        {
	    ajStrDelStatic(&fname);
	    ajStrDelStatic(&tname);
	    return ajFalse;
        }
    }

    ajStrDelStatic(&fname);
    ajStrDelStatic(&tname);

    return ajTrue;
}




/* @func ajSysWhichEnv ********************************************************
**
** Gets the Basename of a file then searches $PATH sequentially until it
** finds a user-EXECUTABLE file of the same name. Reentrant.
**
** @param [u] s [AjPStr*] Filename in AjStr, replaced by full pathname
** @param [r] env [char* const[]] Environment
** @return [AjBool] True if executable found, false otherwise
** @@
******************************************************************************/

AjBool ajSysWhichEnv(AjPStr *s, char * const env[])
{
    ajint count;
    char *p = NULL;
    const char *cp;
    AjPStr tname = NULL;
    AjPStr fname = NULL;
    AjPStr path  = NULL;
    char   *save = NULL;
    AjPStr buf   = NULL;
    AjPStr tmp   = NULL;
    
    
    buf   = ajStrNew();
    tname = ajStrNew();
    tmp   = ajStrNew();
    ajStrAssignS(&tname,*s);
    
    fname = ajStrNew();
    path  = ajStrNew();
    
    ajFileNameTrim(&tname);

#ifdef WIN32
    ajStrAppendC(&tname,".exe");
#endif

    ajDebug("ajSysWhichEnv '%S' => %S\n", *s, tname);

    count = 0;
    while(env[count]!=NULL)
    {
	if(!(*env[count]))
	    break;

	/*ajDebug("  env[%d] '%s'\n", count, env[count]);*/

#ifndef WIN32
	if(!strncmp("PATH=",env[count],5))
#else
	if(!strnicmp("PATH=",env[count],5))
#endif
	    break;

	++count;
    }
    
   /* ajDebug("PATH  env[%d] '%s'\n", count, env[count]);*/

    if(env[count]==NULL || !(*env[count]))
    {
	ajStrDel(&fname);
	ajStrDel(&tname);
	ajStrDel(&path);
	ajStrDel(&buf);
	ajStrDel(&tmp);
	return ajFalse;
    }
    
    ajStrAssignC(&path, env[count]);
    cp = ajStrGetPtr(path);
    cp += 5;
    ajStrAssignC(&tmp,cp);

    /*ajDebug("tmp '%S' save '%S' buf '%S'\n", tmp, save, buf);*/
 
    p = ajSysStrtokR(ajStrGetuniquePtr(&tmp),PATH_SEPARATOR,&save,&buf);

    if(p==NULL)
    {
	ajStrDel(&fname);
	ajStrDel(&tname);
	ajStrDel(&path);
	ajStrDel(&buf);
	ajStrDel(&tmp);
	return ajFalse;
    }
    

    ajFmtPrintS(&fname,"%s%s%S",p,SLASH_STRING,tname);

    while(!ajFileStat(fname, AJ_FILE_X))
    {
	if((p = ajSysStrtokR(NULL,PATH_SEPARATOR,&save,&buf))==NULL)
	{
	    ajStrDel(&fname);
	    ajStrDel(&tname);
	    ajStrDel(&path);
	    ajStrDel(&buf);
	    ajStrDel(&tmp);
	    return ajFalse;
	}

	ajFmtPrintS(&fname,"%s%s%S",p,SLASH_STRING,tname);
    }
    
    
    ajStrAssignS(s,fname);
    ajDebug("ajSysWhichEnv returns '%S'\n", *s);

    ajStrDel(&fname);
    ajStrDel(&tname);
    ajStrDel(&path);
    ajStrDel(&buf);
    ajStrDel(&tmp);
    
    return ajTrue;
}




/* @section Wrappers to C functions *******************************************
**
** Functions for calling or substituting C-functions.
**
**
** @nam3rule  Func         Replacement for C-function.
** @nam4rule  FuncStrtok   strtok that doesn't corrupt the source string
** @nam5rule  FuncStrtokR  Reentrant version.
** @nam4rule  FuncFgets    An fgets replacement that will cope with Mac OSX
**                          files
** @nam4rule  FuncFopen    An fopen replacement to cope with cygwin and windows
** @nam4rule  FuncFdopen   Calls non-ANSI fdopen.
** @nam4rule  FuncStrdup   Duplicate BSD strdup function for very strict ANSI 
** @nam3rule  System       Execute a command line as if from the C shell
** @nam3rule  SystemEnv    Execute command line and pass the environment 
**                         received from main to extract the PATH list
** 
** @fcategory misc
**
******************************************************************************/


/* @func ajSysFdopen **********************************************************
**
** Place non-ANSI fdopen here
**
** @param [r] filedes [ajint] file descriptor
** @param [r] mode [const char *] file mode
** @return [FILE*] file pointer
** @@
******************************************************************************/

FILE* ajSysFdopen(ajint filedes, const char *mode)
{
    return fdopen(filedes,mode);
}




/* @func ajSysFgets ***********************************************************
**
** An fgets replacement that will cope with Mac OSX <CR> files
**
** @param [w] buf [char *] buffer
** @param [r] size [int] maximum length to read
** @param [u] fp [FILE *] stream
**
** @return [char*] buf or NULL
** @@
******************************************************************************/

char* ajSysFgets(char *buf, int size, FILE *fp)
{
#ifdef __ppc__
    int c = 0;
    char *p;
    int cnt;

    p = buf;
    if(!size || size<0)
        return NULL;

    cnt = 0;

    while(cnt!=size-1)
    {
	c = getc(fp);
	if(c==EOF || c=='\r' || c=='\n')
	    break;
        *(p++) = c;
        ++cnt;
    }


    *p ='\0';

    if(c==EOF && !cnt)
	return NULL;

    if(cnt == size-1)
        return buf;

    if(c=='\r' || c=='\n')
    {
	if(c=='\r' && cnt<size-2)
	{
	    if((c=getc(fp)) == '\n')
		*(p++) = '\r';
	    else
		ungetc(c,fp);
	}
	*(p++) = '\n';
    }

    *p = '\0';

    return buf;
#else
    return fgets(buf,size,fp);
#endif
}




/* @func ajSysFopen ***********************************************************
**
** An fopen replacement to cope with cygwin and windows
**
** @param [r] name [const char *] file to open
** @param [r] flags [const char*] r/w/a flags
**
** @return [FILE*] file or NULL
** @@
******************************************************************************/

FILE* ajSysFopen(const char *name, const char *flags)
{
    FILE   *ret  = NULL;
#ifdef __CYGWIN__
    AjPStr fname = NULL;
#endif
    
#ifdef __CYGWIN__
    if(*(name+1) == ':')
    {
	fname = ajStrNew();
	ajFmtPrintS(&fname,"/cygdrive/%c/%s",*name,name+2);
	ret = fopen(ajStrGetPtr(fname),flags);
	ajStrDel(&fname);
    }
    else
      ret = fopen(name,flags);
#else
	ret = fopen(name,flags);
#endif

	return ret;
}




/* @func ajSysStrdup **********************************************************
**
** Duplicate BSD strdup function for very strict ANSI compilers
**
** @param [r] s [const char *] string to duplicate
** @return [char*] Text string as for strdup
** @@
******************************************************************************/

char* ajSysStrdup(const char *s)
{
    static char *p;

    AJCNEW(p, strlen(s)+1);
    strcpy(p,s);

    return p;
}



/* @func ajSysStrtok **********************************************************
**
** strtok that doesn't corrupt the source string
**
** @param [r] s [const char *] source string
** @param [r] t [const char *] delimiter string
**
** @return [char*] pointer or NULL when nothing is found
** @@
******************************************************************************/

char* ajSysStrtok(const char *s, const char *t)
{
    ajint len;

    if(s)
    {
	if(!sysTokRets)
	{
	    sysTokSou  = ajStrNew();
	    sysTokRets = ajStrNew();
	}
	ajStrAssignC(&sysTokSou,s);
	sysTokp = ajStrGetPtr(sysTokSou);
    }

    if(!*sysTokp)
	return NULL;

    len = strspn(sysTokp,t);		/* skip over delimiters */
    sysTokp += len;
    if(!*sysTokp)
	return NULL;

    len = strcspn(sysTokp,t);		/* count non-delimiters */
    ajStrAssignSubC(&sysTokRets,sysTokp,0,len-1);
    sysTokp += len;		  /* skip over first delimiter only */

    return ajStrGetuniquePtr(&sysTokRets);
}



/* @func ajSysStrtokR *********************************************************
**
** Reentrant strtok that doesn't corrupt the source string.
** This function uses a string buffer provided by the caller.
**
** @param [u] s  [char *] source string
** @param [r] t [const char *] delimiter string
** @param [u] ptrptr [char **] ptr save
** @param [w] buf [AjPStr *] result buffer
**
** @return [char*] pointer or NULL
** @@
******************************************************************************/

char* ajSysStrtokR(char *s, const char *t,  char **ptrptr,
			 AjPStr *buf)
{
    char *p;
    ajint len;

    if(!*buf)
	*buf = ajStrNew();

    if(s!=NULL)
	p = s;
    else
	p = *ptrptr;

    if(!*p)
	return NULL;

    len = strspn(p,t);			/* skip over delimiters */
    p += len;
    if(!*p)
	return NULL;

    len = strcspn(p,t);			/* count non-delimiters */
    ajStrAssignSubC(buf,p,0,len-1);
    p += len;			       /* skip to first delimiter */

    *ptrptr = p;

    return ajStrGetuniquePtr(buf);
}




/* @func ajSystem *************************************************************
**
** Exec a command line as if from the C shell
**
** The exec'd program is passed a new argv array in argptr
**
** @param [r] cl [const AjPStr] The command line
** @return [void]
** @@
******************************************************************************/

void ajSystem(const AjPStr cl)
{
#ifndef WIN32
    pid_t pid;
    pid_t retval;
    ajint status;
    char *pgm;
    char **argptr;
    ajint i;

    AjPStr pname = NULL;

    if(!ajSysArglistBuild(cl, &pgm, &argptr))
	return;

    pname = ajStrNew();

    ajStrAssignC(&pname, pgm);

    if(!ajSysWhich(&pname))
	ajFatal("cannot find program '%S'", pname);

    pid=fork();

    if(pid==-1)
	ajFatal("System fork failed");

    if(pid)
    {
	while((retval=waitpid(pid,&status,0))!=pid)
	{
	    if(retval == -1)
		if(errno != EINTR)
		    break;
	}
    }
    else
    {
	execv(ajStrGetPtr(pname), argptr);
	ajExitAbort();			/* just in case */
    }

    ajStrDel(&pname);

    i = 0;
    while(argptr[i])
    {
	AJFREE(argptr[i]);
	++i;
    }
    AJFREE(argptr);

    AJFREE(pgm);

#endif
    return;
}




/* @func ajSystemOut **********************************************************
**
** Exec a command line as if from the C shell
**
** The exec'd program is passed a new argv array in argptr
**
** @param [r] cl [const AjPStr] The command line
** @param [r] outfname [const AjPStr] The output file name
** @return [void]
** @@
******************************************************************************/

void ajSystemOut(const AjPStr cl, const AjPStr outfname)
{
#ifndef WIN32
    pid_t pid;
    pid_t retval;
    ajint status;
    char *pgm;
    char **argptr;
    ajint i;

    AjPStr pname = NULL;

    if(!ajSysArglistBuild(cl, &pgm, &argptr))
	return;

    pname = ajStrNew();

    ajStrAssignC(&pname, pgm);

    if(!ajSysWhich(&pname))
	ajFatal("cannot find program '%S'", pname);

    fflush(stdout);

    pid=fork();

    if(pid==-1)
	ajFatal("System fork failed");

    if(pid)
    {
	while((retval=waitpid(pid,&status,0))!=pid)
	{
	    if(retval == -1)
		if(errno != EINTR)
		    break;
	}
    }
    else
    {
	/* this is the child process */

	if(!freopen(MAJSTRGETPTR(outfname), "wb", stdout))
	    ajErr("Failed to redirect standard output to '%S'", outfname);
	execv(ajStrGetPtr(pname), argptr);
	ajExitAbort();			/* just in case */
    }

    ajStrDel(&pname);

    i = 0;
    while(argptr[i])
    {
	AJFREE(argptr[i]);
	++i;
    }
    AJFREE(argptr);

    AJFREE(pgm);

#endif
    return;
}

/* @func ajSystemEnv **********************************************************
**
** Exec a command line as if from the C shell
**
** This routine must be passed the environment received from
** main(ajint argc, char **argv, char **env)
** The environment is used to extract the PATH list (see ajWhich)
**
** Note that the environment is passed through unaltered. The exec'd
** program is passed a new argv array
**
** @param [r] cl [const AjPStr] The command line
** @param [r] env [char* const[]] The environment
** @return [void]
** @@
******************************************************************************/

void ajSystemEnv(const AjPStr cl, char * const env[])
{
#ifndef WIN32
    pid_t pid;
    pid_t retval;
    ajint status;
    char *pgm = NULL;
    char **argptr = NULL;
    ajint i;

    AjPStr pname = NULL;

    if(!ajSysArglistBuild(cl, &pgm, &argptr))
	return;

    pname = ajStrNew();

    ajDebug("ajSystemEnv '%s' %S \n", pgm, cl);
    ajStrAssignC(&pname, pgm);
    if(!ajSysWhichEnv(&pname, env))
	ajFatal("cannot find program '%S'", pname);

    ajDebug("ajSystemEnv %S = %S\n", pname, cl);
    for (i=0;argptr[i]; i++)
    {
	ajDebug("%4d '%s'\n", i, argptr[i]);
    }

    pid = fork();
    if(pid==-1)
	ajFatal("System fork failed");

    if(pid)
    {
	while((retval=waitpid(pid,&status,0))!=pid)
	{
	    if(retval == -1)
		if(errno != EINTR)
		    break;
	}
    }
    else
    {
	execve(ajStrGetPtr(pname), argptr, env);
	ajExitAbort();			/* just in case */
    }

    ajStrDel(&pname);

    i = 0;
    while(argptr[i])
    {
	AJFREE(argptr[i]);
	++i;
    }
    AJFREE(argptr);

    AJFREE(pgm);

#endif

    return;
}





/* @section Miscellaneous system functions ************************************
**
** Miscellaneous system functions
**
** @fdata [none]
** @fcategory misc
**
** @nam3rule  Canon   Sets or unsets TTY canonical mode
** @nam3rule  Exit    Cleans up system internals memory
** 
** @valrule   *  [void]
**
******************************************************************************/

/* @func ajSysCanon  **********************************************************
**
** Sets or unsets TTY canonical mode
**
** @param [r] state [AjBool] state=true sets canon state=false sets noncanon
** @return [void]
** @@
******************************************************************************/

void ajSysCanon(AjBool state)
{
#ifndef WIN32
#ifndef __VMS
    static struct termios tty;


    tcgetattr(1, &tty);
    tty.c_cc[VMIN]  = '\1';
    tty.c_cc[VTIME] = '\0';
    tcsetattr(1,TCSANOW,&tty);

    if(state)
	tty.c_lflag |= ICANON;
    else
	tty.c_lflag &= ~(ICANON);

    tcsetattr(1, TCSANOW, &tty);
#endif
#endif
    return;
}

/* @func ajSysExit ************************************************************
**
** Cleans up system internals memory
**
** @return [void]
** @@
******************************************************************************/

void ajSysExit(void)
{
    ajStrDel(&sysTokSou);
    ajStrDel(&sysTokRets);

    return;
}





/* @obsolete ajSysBasename
** @rename ajFileNameTrim
*/

__deprecated void ajSysBasename(AjPStr *s)
{
    ajFileNameTrim(s);

    return;
}




/* @obsolete ajSysIsDirectory
** remove use ajFileDir instead
*/

__deprecated AjBool ajSysIsDirectory(const char *s)
{
    AjBool ret;
    AjPStr tmpstr = NULL;
    tmpstr = ajStrNewC(s);

    ret = ajFileDir(&tmpstr);
    ajStrDel(&tmpstr);

    return ret;
}




/* @obsolete ajSysIsRegular
** @remove use ajFileNameValid instead
*/

__deprecated AjBool ajSysIsRegular(const char *s)
{
    AjBool ret;
    AjPStr tmpstr;

    tmpstr = ajStrNewC(s);

    ret = ajFileNameValid(tmpstr);
    ajStrDel(&tmpstr);

    return ret;
}




