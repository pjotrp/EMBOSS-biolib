#ifndef WIN32_H
#define WIN32_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <limits.h>

#define DWORD unsigned int

#define unlink _unlink

typedef struct
{ 
    DWORD dwLowDateTime; 
    DWORD dwHighDateTime; 
} PW_FILETIME;


typedef struct _PW_WIN32_FIND_DATA
{ 
    DWORD dwFileAttributes; 
    PW_FILETIME ftCreationTime; 
    PW_FILETIME ftLastAccessTime; 
    PW_FILETIME ftLastWriteTime; 
    DWORD    nFileSizeHigh; 
    DWORD    nFileSizeLow; 
    DWORD    dwReserved0; 
    DWORD    dwReserved1; 
    char     cFileName[MAX_PATH]; 
    char     cAlternateFileName[14]; 
} PW_WIN32_FIND_DATA;

#endif	/* WIN32_H */
