#ifndef READWRITE_H
 #define READWRITE_H

#include <unistd.h>

extern int readline(register int fd,
		    register char *ptr,
		    register int maxlen);

extern int readn (register int fd,
		  register char *ptr,
		  register int nbytes);

extern int writen(register int fd,
		  register char *ptr,
		  register int nbytes);

#endif

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/readwrite.h,v 1.1 2000/11/05 21:47:56 ajb Exp $ */
