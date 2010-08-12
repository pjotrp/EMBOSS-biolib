/* Modified by AJB for EMBOSS */

/*
** EMBOSS defines LENDIAN & BENDIAN so use LENDIAN to
** define expat BYTEORDER and WORDS_BIGENDIAN
*/

#ifndef WIN32
#include "embossconf.h"
#endif

/* 1234 = LIL_ENDIAN, 4321 = BIGENDIAN */
#ifndef WORDS_BIGENDIAN
#define BYTEORDER 1234
#else
#define BYTEORDER 4321
#endif


/*
** AJB: EMBOSS assumes that bcopy is always there if memmove isn't
** so always leave this set - requird by expat if MEMMOVE doesn't
** exist
*/
#define HAVE_BCOPY 1

/*
** AJB: This isn't used by the expat library. EMBOSS config already
** tests for this
*/
/* Define to 1 if you have the <dlfcn.h> header file. */
/* #define HAVE_DLFCN_H 1 */

/* AJB: This isn't used by the expat library */
/* Define to 1 if you have the <fcntl.h> header file. */
/* #define HAVE_FCNTL_H 1 */

/* AJB: This isn't used by the expat library */
/* Define to 1 if you have the `getpagesize' function. */
/* #define HAVE_GETPAGESIZE 1 */

/* AJB: The EMBOSS configure already tests for this */
/* Define to 1 if you have the <inttypes.h> header file. */
/* #define HAVE_INTTYPES_H 1 */

/* AJB: The EMBOSS configure already tests for this */
/* Define to 1 if you have the `memmove' function. */
/* #define HAVE_MEMMOVE 1 */

/* AJB: The EMBOSS configure already tests for this */
/* Define to 1 if you have the <memory.h> header file. */
/* #define HAVE_MEMORY_H 1 */

/* AJB: This isn't used by the expat library */
/* Define to 1 if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* AJB: The EMBOSS configure already tests for this */
/* Define to 1 if you have the <stdint.h> header file. */
/* #define HAVE_STDINT_H 1 */

/* AJB: The EMBOSS configure already tests for this */
/* Define to 1 if you have the <stdlib.h> header file. */
/* #define HAVE_STDLIB_H 1 */

/* AJB: The EMBOSS configure already tests for this */
/* Define to 1 if you have the <strings.h> header file. */
/* #define HAVE_STRINGS_H 1 */

/* AJB: The EMBOSS configure already tests for this */
/* Define to 1 if you have the <string.h> header file. */
/* #define HAVE_STRING_H 1 */

/* AJB: The EMBOSS configure already tests for this */
/* Define to 1 if you have the <sys/stat.h> header file. */
/* #define HAVE_SYS_STAT_H 1 */

/* AJB: The EMBOSS configure already tests for this */
/* Define to 1 if you have the <sys/types.h> header file. */
/* #define HAVE_SYS_TYPES_H 1 */

/* AJB: The EMBOSS configure already tests for this */
/* Define to 1 if you have the <unistd.h> header file. */
/* #define HAVE_UNISTD_H 1 */

/* AJB: The EMBOSS configure already defines this */
/* Define to the address where bug reports for this package should be sent. */
/* #define PACKAGE_BUGREPORT "expat-bugs@libexpat.org" */

/* AJB: The EMBOSS configure already defines this */
/* Define to the full name of this package. */
/* #define PACKAGE_NAME "expat" */

/* AJB: The EMBOSS configure already defines this */
/* Define to the full name and version of this package. */
/* #define PACKAGE_STRING "expat 2.0.1" */

/* AJB: The EMBOSS configure already defines this */
/* Define to the one symbol short name of this package. */
/* #define PACKAGE_TARNAME "expat" */

/* AJB: The EMBOSS configure already defines this */
/* Define to the version of this package. */
/* #define PACKAGE_VERSION "2.0.1" */

/* AJB: The EMBOSS configure already tests for this */
/* Define to 1 if you have the ANSI C header files. */
 /* #define STDC_HEADERS 1 */

 /* AJB: This is dealt with above */
/* whether byteorder is bigendian */
/* #undef WORDS_BIGENDIAN */

/* Define to specify how much context to retain around the current parse
   point. */
#define XML_CONTEXT_BYTES 1024

/* Define to make parameter entity parsing functionality available. */
#define XML_DTD 1

/* Define to make XML Namespaces functionality available. */
#define XML_NS 1

/* Define to __FUNCTION__ or "" if `__func__' does not conform to ANSI C. */
/* #undef __func__ */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `long' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */