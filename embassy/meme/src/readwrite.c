/* 
  Three utilities from _Unix_Network_Programming_ by W. R. Stevens.
*/
#include <stdio.h>
#include "readwrite.h"

/*******************************************************************
 *
 * int readn 
 *
 * Read "n" bytes from a descriptor.
 * Use in place of read() when fd is a stream socket
 *
 * Stevens, p. 279
 *
 *******************************************************************/

extern int readn(register int fd,
	  register char *ptr,
	  register int nbytes)
{
  int nleft, nread;
  nleft = nbytes;
  while (nleft > 0) {
    nread = read(fd, ptr, nleft);
    if (nread < 0)
      return(nread);		/* error, return < 0 */
    else if (nread == 0)
      break;			/* EOF */
    nleft -= nread;
    ptr += nread;
  }
  return(nbytes - nleft);	/* return >= 0 */
}


/*******************************************************************
 * int writen
 *
 * Write 'n' bytes to a descriptor.
 * Use in place of write() when fd is a stream socket.
 *
 * Stevens, p. 279
 *
 *******************************************************************/

extern int writen(register int fd,
	   register char *ptr,
	   register int nbytes)
{

  int nleft, nwritten;
  nleft = nbytes;
  while (nleft > 0) {
    nwritten = write(fd, ptr, nleft);
    if (nwritten <= 0)
      return(nwritten);		/* error */
    nleft -= nwritten;
    ptr += nwritten;
  }
  return(nbytes - nleft);
}

/***********************************************************************
 *
 * void readline
 *
 * Read a line from a descriptor. Read the line one byte at a time,
 * looking for the newline. We throw out the newline and end the 
 * buffer with a null.
 * We return the number of characters up to, but not including
 * the null (the same as strlen(3)).
 *
 * Stevens, p. 280.
 * 
 ***********************************************************************/
extern int readline(register int fd,
	     register char *ptr,
	     register int maxlen)
{
  int n, rc;
  char c;

  for (n = 1; n < maxlen; n++) {
    if ((rc = read(fd, &c, 1)) == 1) {
      if (c == '\n')
	break;
      *ptr++ = c;
    } else if (rc == 0) {
      if (n == 1) {
	return(0); /* EOF, no data read. */
      } else
	break; /* EOF, some data was read. */
    } else {
      fprintf(stderr, "Error in readn.\n");
      return(-1); /* error */
    }
  }

  *ptr = 0;
  /*printf("n=%d\n", n);*/
  return(n);
}

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/readwrite.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
