/********************************************************************
** @source AJAX Jave Native Interface (JNI) functions
**
** @author Copyright (C) 2001 Alan Bleasby
** @version 1.0 
** @modified Jul 07 2001 ajb First version
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

#ifdef HAVE_JAVA

#include <jni.h>
#include "ajax.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>


#include <pwd.h>
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#include <crypt.h>
#include <errno.h>

#ifdef SHADOW
#include <shadow.h>
#endif

#ifdef PAM
#include <security/pam_appl.h>
#endif

#ifdef AIX_SHADOW
#include <userpw.h>
#endif

#ifdef HPUX_SHADOW
#include <hpsecurity.h>
#include <prot.h>
#endif


#define AJ_OUTBUF 10000
#define AJNOTFOUND -999


static void empty_core_dump(void);
#ifndef NO_AUTH
static AjBool check_pass(AjPStr username, AjPStr password, ajint *uid,
			 ajint *gid, AjPStr *home);
#endif
static char **make_array(AjPStr str);

#ifdef PAM
static int PAM_conv(int num_msg, struct pam_message **msg,
		    struct pam_response **resp, void *appdata_ptr);
#endif



static AjBool ajJavaGetSeqFromUsa (AjPStr thys, AjPSeq *seq);
static AjBool ajJavaGetSeqsetFromUsa (AjPStr thys, AjPSeqset *seq);




/* @func Ajax.seqType **************************************************
**
** Return length, weight and type information for a sequence
**
** @param [rw] env [JNIEnv*] java environment
** @param [r] obj [jobject] java object
** @param [r] usa [jstring] usa
**
** @return [jboolean] true if the sequence exists
** @@
******************************************************************************/

JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_seqType
           (JNIEnv *env, jobject obj, jstring usa)
{

    AjPStr name=NULL;
    char *javaname=NULL;
    AjBool ok=ajFalse;
    AjPSeq seq=NULL;
    AjBool nuc=ajFalse;
    ajint  len=0;
    float  weight=0.;
    jclass jvc = (*env)->GetObjectClass(env,obj);
    jfieldID field;
    
    name = ajStrNew();
    seq  = ajSeqNew();
    
    javaname = (char *) (*env)->GetStringUTFChars(env,usa,0);
    ajStrAssC(&name,javaname);
    (*env)->ReleaseStringUTFChars(env,usa,javaname);

    ok = ajJavaGetSeqFromUsa(name,&seq);
    if(ok)
    {
	len = ajSeqLen(seq);
	nuc = ajSeqIsNuc(seq);
	weight = seq->Weight;
	
	field = (*env)->GetStaticFieldID(env,jvc,"length","I");
	(*env)->SetStaticIntField(env,jvc,field,len);

	field = (*env)->GetStaticFieldID(env,jvc,"protein","Z");
	(*env)->SetStaticBooleanField(env,jvc,field,(unsigned char)!nuc);

	field = (*env)->GetStaticFieldID(env,jvc,"weight","F");
	(*env)->SetStaticFloatField(env,jvc,field,weight);
    }
    
    ajStrDel(&name);
    ajSeqDel(&seq);

    return (unsigned char) ok;
}

/* @func Ajax.seqsetType **************************************************
**
** Return length, weight and type information for a seqset
**
** @param [rw] env [JNIEnv*] java environment
** @param [r] obj [jobject] java object
** @param [r] usa [jstring] usa
**
** @return [jboolean] true if the sequences exist
** @@
******************************************************************************/

JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_seqsetType
           (JNIEnv *env, jobject obj, jstring usa)
{
    AjPStr name=NULL;
    char *javaname=NULL;
    AjBool ok=ajFalse;
    AjPSeqset seq=NULL;
    AjBool nuc=ajFalse;
    ajint  len=0;
    float  weight=0.;
    jclass jvc = (*env)->GetObjectClass(env,obj);
    jfieldID field;
    
    name = ajStrNew();
    seq  = ajSeqsetNew();
    
    javaname = (char *) (*env)->GetStringUTFChars(env,usa,0);
    ajStrAssC(&name,javaname);
    (*env)->ReleaseStringUTFChars(env,usa,javaname);

    ok = ajJavaGetSeqsetFromUsa(name,&seq);
    if(ok)
    {
	len = ajSeqsetLen(seq);
	nuc = ajSeqsetIsNuc(seq);
	weight = ajSeqsetTotweight(seq);
	
	field = (*env)->GetStaticFieldID(env,jvc,"length","I");
	(*env)->SetStaticIntField(env,jvc,field,len);

	field = (*env)->GetStaticFieldID(env,jvc,"protein","Z");
	(*env)->SetStaticBooleanField(env,jvc,field,(unsigned char)!nuc);

	field = (*env)->GetStaticFieldID(env,jvc,"weight","F");
	(*env)->SetStaticFloatField(env,jvc,field,weight);
    }
    
    ajStrDel(&name);
    ajSeqsetDel(&seq);
    
    return (unsigned char) ok;
}



/* @funcstatic ajJavaGetSeqFromUsa ********************************************
**
** Return a sequence given a USA
**
** @param [r] thys [AjPStr] usa
** @param [w] seq [AjPSeq*] sequence
** @return [AjBool] ajTrue on success
******************************************************************************/

static AjBool ajJavaGetSeqFromUsa(AjPStr thys, AjPSeq *seq)
{
    AjPSeqin seqin;
    AjBool ok;
  
    ajNamInit("emboss");
  
    seqin = ajSeqinNew();
    seqin->multi = ajFalse;
    seqin->Text  = ajFalse;
  
    ajSeqinUsa (&seqin, thys);
    ok = ajSeqRead(*seq, seqin);
    ajSeqinDel (&seqin);

    if(!ok)
	return ajFalse;

    return ajTrue;
}



/* @funcstatic ajJavaGetSeqsetFromUsa *****************************************
**
** Return a seqset given a usa
**
** @param [r] thys [AjPStr] usa
** @param [w] seq [AjPSeqset*] seqset
** @return [AjBool] ajTrue on success
******************************************************************************/

static AjBool ajJavaGetSeqsetFromUsa(AjPStr thys, AjPSeqset *seq)
{
    AjPSeqin seqin;
    AjBool ok;
  
    ajNamInit("emboss");
  
    seqin = ajSeqinNew();
    seqin->multi = ajTrue;
    seqin->Text  = ajFalse;
  
    ajSeqinUsa (&seqin, thys);
    ok = ajSeqsetRead(*seq, seqin);
    ajSeqinDel (&seqin);


    if(!ok)
	return ajFalse;

    return ajTrue;
}


/* @func Ajax.userInfo **************************************************
**
** Return the uid, gid and home directory of a user
**
** @param [rw] env [JNIEnv*] java environment
** @param [r] obj [jobject] java object
** @param [r] door [jstring] username
** @param [r] key [jstring] password
**
** @return [jboolean] true if the username/password are valid
** @@
******************************************************************************/


JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_userInfo
(JNIEnv *env, jobject obj, jstring door, jstring key)
{
    AjPStr username=NULL;
    AjPStr password=NULL;
    AjPStr home=NULL;
    char *juser=NULL;
    char *jpass=NULL;
    AjBool ok=ajFalse;

    jclass jvc = (*env)->GetObjectClass(env,obj);
    jfieldID field;
    jstring  jhpstr;

    ajint uid=0;
    ajint gid=0;
    char  *hp=NULL;
    
    empty_core_dump();
    
    username = ajStrNew();
    password = ajStrNew();
    home     = ajStrNew();
    
    juser = (char *) (*env)->GetStringUTFChars(env,door,0);
    ajStrAssC(&username,juser);
    (*env)->ReleaseStringUTFChars(env,door,juser);

    jpass = (char *) (*env)->GetStringUTFChars(env,key,0);
    ajStrAssC(&password,jpass);
    (*env)->ReleaseStringUTFChars(env,key,jpass);


#ifndef NO_AUTH
    ok = check_pass(username,password,&uid,&gid,&home);
#endif

    field = (*env)->GetStaticFieldID(env,jvc,"uid","I");
    (*env)->SetStaticIntField(env,jvc,field,uid);

    field = (*env)->GetStaticFieldID(env,jvc,"gid","I");
    (*env)->SetStaticIntField(env,jvc,field,gid);


    hp = ajStrStr(home);
    field = (*env)->GetFieldID(env,jvc,"home","Ljava/lang/String;");    

    jhpstr = (*env)->NewStringUTF(env,hp);
    (*env)->SetObjectField(env,obj,field,jhpstr);
    

    bzero((void*)ajStrStr(username),ajStrLen(username));
    bzero((void*)ajStrStr(password),ajStrLen(password));
    bzero((void*)ajStrStr(home),ajStrLen(home));

    ajStrDel(&username);
    ajStrDel(&password);
    ajStrDel(&home);

    if(!ok)
	return ajFalse;
    
    return ajTrue;
}




/* @funcstatic empty_core_dump ***********************************************
**
** Set process coredump size to be zero
**
** @return [void]
** @@
******************************************************************************/

static void empty_core_dump()
{
    struct rlimit limit;

    limit.rlim_cur = 0;
    limit.rlim_max = 0;

    setrlimit(RLIMIT_CORE,&limit);

    return;
}




#ifdef SHADOW
static AjBool check_pass(AjPStr username, AjPStr password, ajint *uid,
			 ajint *gid, AjPStr *home)
{
    struct spwd *shadow = NULL;
    struct passwd *pwd  = NULL;
    char *p = NULL;
    

    shadow = getspnam(ajStrStr(username));
    
    if(!shadow)                 /* No such username */
        return ajFalse;
    

    pwd = getpwnam(ajStrStr(username));
    
    if(!pwd)
        return ajTrue;
    
    *uid = pwd->pw_uid;
    *gid = pwd->pw_gid;

    ajStrAssC(home,pwd->pw_dir);
    
    p = crypt(ajStrStr(password),shadow->sp_pwdp);

    if(!strcmp(p,shadow->sp_pwdp))
        return ajTrue;

    return ajFalse;
}
#endif



#ifdef AIX_SHADOW
static AjBool check_pass(AjPStr username, AjPStr password, ajint *uid,
			 ajint *gid, AjPStr *home)
{
    struct userpw *shadow = NULL;
    struct passwd *pwd  = NULL;
    char *p = NULL;

    shadow = getuserpw(ajStrStr(username));
    if(!shadow)	
	return ajFalse;

    pwd = getpwnam(ajStrStr(username));
    if(!pwd)
	return ajFalse;

    *uid = pwd->pw_uid;
    *gid = pwd->pw_gid;

    ajStrAssC(home,pwd->pw_dir);

    p = crypt(ajStrStr(password),shadow->upw_passwd);

    if(!strcmp(p,shadow->upw_passwd))
	return ajTrue;

    return ajFalse;
}
#endif



#ifdef HPUX_SHADOW
static AjBool check_pass(AjPStr username, AjPStr password, ajint *uid,
		      ajint *gid, AjPStr *home)
{
    struct pr_passwd *shadow = NULL;
    struct passwd pwd;
    
    char *p = NULL;

    shadow = getprpwnam(ajStrStr(username));
    if(!shadow)	
	return ajFalse;

    *uid = shadow->ufld.fd_uid;

    
    pwd = getpwnam(ajStrStr(username));
    if(!pwd)
	return ajFalse;
    
    *gid = pwd->pw_gid;

    ajStrAssC(home,pwd->pw_dir);

    p = crypt(ajStrStr(password),shadow->ufld.fd_encrypt);

    if(!strcmp(p,shadow->ufld.fd_encrypt))
	return ajTrue;

    return ajFalse;
}
#endif


#ifdef NO_SHADOW
static AjBool check_pass(AjPStr username, AjPStr password, ajint *uid,
		      ajint *gid, AjPStr *home)
{
    struct passwd *pwd  = NULL;
    char *p = NULL;
    
    pwd = getpwnam(ajStrStr(username));
    if(!pwd)		 /* No such username */
	return ajFalse;

    *uid = pwd->pw_uid;
    *gid = pwd->pw_gid;

    ajStrAssC(home,pwd->pw_dir);

    p = crypt(ajStrStr(password),pwd->pw_passwd);

    if(!strcmp(p,pwd->pw_passwd))
	return ajTrue;

    return ajFalse;
}
#endif




#ifdef PAM

struct ad_user
{
    char *username;
    char *password;
};


static int PAM_conv(int num_msg, struct pam_message **msg,
		    struct pam_response **resp, void *appdata_ptr)
{
    struct ad_user *user=(struct ad_user *)appdata_ptr;
    struct pam_response *response;
    int i;

    if (msg == NULL || resp == NULL || user == NULL)
    	return PAM_CONV_ERR;
    
    response= (struct pam_response *)
    	malloc(num_msg * sizeof(struct pam_response));
    
    for(i=0;i<num_msg;++i)
    {
	response[i].resp_retcode = 0;
	response[i].resp = NULL;

	switch(msg[i]->msg_style)
	{
	case PAM_PROMPT_ECHO_ON:
	    /* Store the login as the response */
	    response[i].resp = appdata_ptr ?
		(char *)strdup(user->username) : NULL;
	    break;

	case PAM_PROMPT_ECHO_OFF:
	    /* Store the password as the response */
	    response[i].resp = appdata_ptr ?
		(char *)strdup(user->password) : NULL;
	    break;

	case PAM_TEXT_INFO:
	case PAM_ERROR_MSG:
	    break;

	default:
	    if(response)
		free(response);
	    return PAM_CONV_ERR;
	}
    }

    /* On success, return the response structure */
    *resp= response;
    return PAM_SUCCESS;
}


static AjBool check_pass(AjPStr username,AjPStr password,ajint *uid,
		      ajint *gid,AjPStr *home)
{
    struct ad_user user_info;

    struct pam_cv
    {
	int (*cv)(int,struct pam_message **,struct pam_response **,void *);
	void *userinfo;
    };

    struct pam_cv conv;
    pam_handle_t *pamh = NULL;
    int retval;

    struct passwd *pwd = NULL;

    user_info.username = ajStrStr(username);
    user_info.password = ajStrStr(password);

    conv.cv = PAM_conv;
    conv.userinfo = (void *)&user_info;
    
    pwd = getpwnam(ajStrStr(username));
    if(!pwd)		 /* No such username */
	return ajFalse;

    *uid = pwd->pw_uid;
    *gid = pwd->pw_gid;

    ajStrAssC(home,pwd->pw_dir);

    retval = pam_start("emboss_auth",ajStrStr(username),
		       (struct pam_conv*)&conv,&pamh);
    
    if (retval == PAM_SUCCESS)
	retval= pam_authenticate(pamh,PAM_SILENT);
    
    if(retval==PAM_SUCCESS)
	retval = pam_acct_mgmt(pamh,0);
    
    if(pam_end(pamh,retval)!=PAM_SUCCESS)
    {
	pamh = NULL;
	return ajFalse;
    }

    if(retval==PAM_SUCCESS)
	return ajTrue;

    return ajFalse;
}
#endif



/* @func Ajax.setuid **************************************************
**
** Set the uid of the current process
**
** @param [rw] env [JNIEnv*] java environment
** @param [r] j [jclass] java class
** @param [r] uid [jint] uid
**
** @return [jint] 0 on success, -1 if failure
** @@
******************************************************************************/

JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_setuid
(JNIEnv *env, jclass j, jint uid)
{
    return((jint)setuid((uid_t)uid));
}


/* @func Ajax.seteuid **************************************************
**
** Set the effective uid of the current process
**
** @param [rw] env [JNIEnv*] java environment
** @param [r] j [jclass] java class
** @param [r] uid [jint] uid
**
** @return [jint] 0 on success, -1 if failure
** @@
******************************************************************************/

JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_seteuid
(JNIEnv *env, jclass j, jint uid)
{
    return((jint)seteuid((uid_t)uid));
}


/* @func Ajax.setgid **************************************************
**
** Set the gid of the current process
**
** @param [rw] env [JNIEnv*] java environment
** @param [r] j [jclass] java class
** @param [r] gid [jint] gid
**
** @return [jint] 0 on success, -1 if failure
** @@
******************************************************************************/

JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_setgid
(JNIEnv *env, jclass j, jint gid)
{
    return((jint)setgid((gid_t)gid));
}


/* @func Ajax.setegid **************************************************
**
** Set the effective gid of the current process
**
** @param [rw] env [JNIEnv*] java environment
** @param [r] j [jclass] java class
** @param [r] gid [jint] gid
**
** @return [jint] 0 on success, -1 if failure
** @@
******************************************************************************/

JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_setegid
(JNIEnv *env, jclass j, jint gid)
{
    return((jint)setegid((gid_t)gid));
}


/* @func Ajax.getuid **************************************************
**
** Return the uid of the current process
**
** @param [rw] env [JNIEnv*] java environment
** @param [r] j [jclass] java class
**
** @return [jint] uid
** @@
******************************************************************************/

JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_getuid
(JNIEnv *env, jclass j)
{
    return((jint)getuid());
}


/* @func Ajax.getgid **************************************************
**
** Return the gid of the current process
**
** @param [rw] env [JNIEnv*] java environment
** @param [r] j [jclass] java class
**
** @return [jint] gid
** @@
******************************************************************************/

JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_getgid
(JNIEnv *env, jclass j)
{
    return((jint)getgid());
}


/* @func Ajax.geteuid **************************************************
**
** Return the effective uid of the current process
**
** @param [rw] env [JNIEnv*] java environment
** @param [r] j [jclass] java class
**
** @return [jint] uid
** @@
******************************************************************************/

JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_geteuid
(JNIEnv *env, jclass j)
{
    return((jint)geteuid());
}



/* @func Ajax.getegid **************************************************
**
** Return the effective gid of the current process
**
** @param [rw] env [JNIEnv*] java environment
** @param [r] j [jclass] java class
**
** @return [jint] gid
** @@
******************************************************************************/

JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_getegid
(JNIEnv *env, jclass j)
{
    return((jint)getegid());
}



/* @func Ajax.fork **************************************************
**
** Fork off new process with given uid & gid, chdir and execute command
** return stdout and stderr of the child process
**
** @param [rw] env [JNIEnv*] java environment
** @param [r] obj [jobject] java object
** @param [r] commandline [jstring] command to execute
** @param [r] directory [jstring] chdir here before execution
** @param [r] uid [jint] uid for setuid
** @param [r] gid [jint] gid for setgid
**
** @return [jboolean] true if success
** @@
******************************************************************************/

JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_fork
(JNIEnv *env, jobject obj, jstring commandline,
 jstring environment, jstring directory, jint uid, jint gid)
{
    jclass jvc = (*env)->GetObjectClass(env,obj);
    jfieldID field;
    jstring  ostr;
    jstring  estr;
    
    char  *sptr;

    AjPStr prog=NULL;
    AjPStr cl=NULL;
    AjPStr envi=NULL;
    AjPStr dir=NULL;
    AjPStrTok handle=NULL;
    

    char **argp=NULL;
    char **envp=NULL;
    int  pid;
    int  status = 0;
    int  i=0;

    int  outpipe[2];
    int  errpipe[2];
    
    fd_set rec;
    struct timeval t;
    int nread=0;
    char *buf;
    AjPStr outstd=NULL;
    AjPStr errstd=NULL;
    int retval=0;
    

    AJCNEW0(buf,AJ_OUTBUF+1);
    

    prog  = ajStrNew();
    cl    = ajStrNew();
    envi  = ajStrNew();
    dir   = ajStrNew();

    outstd = ajStrNew();
    errstd = ajStrNew();

    
    sptr = (char *) (*env)->GetStringUTFChars(env,commandline,0);
    ajStrAssC(&cl,sptr);
    (*env)->ReleaseStringUTFChars(env,commandline,sptr);


    handle = ajStrTokenInit(cl," \t\n");
    ajStrToken(&prog,&handle,NULL);
    ajStrTokenClear(&handle);
    

    sptr = (char *) (*env)->GetStringUTFChars(env,environment,0);
    ajStrAssC(&envi,sptr);
    (*env)->ReleaseStringUTFChars(env,environment,sptr);

    sptr = (char *) (*env)->GetStringUTFChars(env,directory,0);
    ajStrAssC(&dir,sptr);
    (*env)->ReleaseStringUTFChars(env,directory,sptr);


    argp = make_array(cl);
    envp = make_array(envi);

    if(!ajSysWhichEnv(&prog,envp))
	return (unsigned char)ajFalse;

    pipe(outpipe);
    pipe(errpipe);

    pid = fork();
    if(pid == -1)
	return (unsigned char)ajFalse;
    
    if(!pid)			/* Child */
    {
	chdir(ajStrStr(dir));
	dup2(outpipe[1],1);
	dup2(errpipe[1],2);
	setgid(gid);
	setuid(uid);
	if(execve(ajStrStr(prog),argp,envp) == -1)
	    return (unsigned char)ajFalse;
    }



    while((retval=waitpid(pid,&status,WNOHANG))!=pid && !retval)
    {
	FD_ZERO(&rec);
	FD_SET(outpipe[0],&rec);
	t.tv_sec = 0;
	t.tv_usec = 0;
	select(outpipe[0]+1,&rec,NULL,NULL,&t);
	if(FD_ISSET(outpipe[0],&rec))
	{
	    nread = read(outpipe[0],(void *)buf,AJ_OUTBUF);
	    buf[nread]='\0';
	    ajStrAppC(&outstd,buf);
	}

	FD_ZERO(&rec);
	FD_SET(errpipe[0],&rec);
	t.tv_sec = 0;
	t.tv_usec = 0;
	select(errpipe[0]+1,&rec,NULL,NULL,&t);
	if(FD_ISSET(errpipe[0],&rec))
	{
	    nread = read(errpipe[0],(void *)buf,AJ_OUTBUF);
	    buf[nread]='\0';
	    ajStrAppC(&errstd,buf);
	}


    }


    FD_ZERO(&rec);
    FD_SET(outpipe[0],&rec);
    t.tv_sec = 0;
    t.tv_usec = 0;
    select(outpipe[0]+1,&rec,NULL,NULL,&t);
    if(FD_ISSET(outpipe[0],&rec))
    {
	nread = read(outpipe[0],(void *)buf,AJ_OUTBUF);
	buf[nread]='\0';
	ajStrAppC(&outstd,buf);
    }


    FD_ZERO(&rec);
    FD_SET(errpipe[0],&rec);
    t.tv_sec = 0;
    t.tv_usec = 0;
    select(errpipe[0]+1,&rec,NULL,NULL,&t);
    if(FD_ISSET(errpipe[0],&rec))
    {
	nread = read(errpipe[0],(void *)buf,AJ_OUTBUF);
	buf[nread]='\0';
	ajStrAppC(&errstd,buf);
    }


    field = (*env)->GetFieldID(env,jvc,"outStd","Ljava/lang/String;");    
    ostr = (*env)->NewStringUTF(env,ajStrStr(outstd));
    (*env)->SetObjectField(env,obj,field,ostr);

    field = (*env)->GetFieldID(env,jvc,"errStd","Ljava/lang/String;");
    estr = (*env)->NewStringUTF(env,ajStrStr(errstd));
    (*env)->SetObjectField(env,obj,field,estr);


    close(errpipe[0]);
    close(errpipe[1]);
    close(outpipe[0]);
    close(outpipe[1]);

    i = 0;
    while(argp[i])
	AJFREE(argp[i]);
    AJFREE(argp);

    i = 0;
    while(envp[i])
	AJFREE(envp[i]);
    AJFREE(envp);


    AJFREE(buf);

    ajStrDel(&prog);
    ajStrDel(&cl);
    ajStrDel(&envi);
    ajStrDel(&dir);
    ajStrDel(&outstd);
    ajStrDel(&errstd);
    
    return (unsigned char)ajTrue;
}


/* @funcstatic make_array ********************************************
**
** Construct argv and env arrays for Ajax.fork
**
** @param [r] str [AjPStr] space separated tokens
**
** @return [char**] env or argv array
******************************************************************************/

static char **make_array(AjPStr str)
{
    int n;
    char **ptr=NULL;
    AjPStrTok handle;
    AjPStr token;
    
    token = ajStrNew();
    
    n = ajStrTokenCount(&str," \t\n");

    AJCNEW0(ptr,n+1);

    ptr[n] = NULL;

    n = 0;
    
    handle = ajStrTokenInit(str," \t\n");
    while(ajStrToken(&token,&handle,NULL))
	ptr[n++] = ajCharNew(token);

    ajStrTokenClear(&handle);
    ajStrDel(&token);
    
    return ptr;
}


#endif
