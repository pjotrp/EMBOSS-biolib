/* meme-cs.h 
 client/server stuff for meme
*/

#ifndef MEME_CS_H
# define MEME_CS_H

#include "macros.h"
#include "user.h"

#undef MIN
#undef MAX

#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
/*#include <stdio.h>*/
#include <errno.h>
#include <fcntl.h>
#include "readwrite.h"

#ifdef __cplusplus
  #define CONST
  extern "C" {
#else
  #define CONST const
#endif

char *getwd(char *path);

/* operating system dependent declarations */
#ifndef sunsparcsolaris

  #ifndef sgi4d
    int accept(int, struct sockaddr *, int *);
    int bind(int, CONST struct sockaddr *, int);
    int connect(int, CONST struct sockaddr *, int);
  #endif

  int listen(int, int);
  int socket(int, int, int);

  #ifdef intelparagon
    unsigned short htons(unsigned int);
  #else
    #if !defined(sunsparc) && !defined(crayc90) && !defined(crayt3e) \
  && !defined(sgi4d)
      unsigned short htons(unsigned short);
    #endif
  #endif

#endif

#ifdef __cplusplus
}
#endif

#endif
