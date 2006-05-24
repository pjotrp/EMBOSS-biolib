/* Copyright (C) 1998 DJ Delorie, see COPYING.DJ for details */
#ifndef DIRENT_W32
#define DIRENT_W32

#ifdef __cplusplus
extern "C" {
#endif

#define _HANDLE void*

#define dirent _PW_WIN32_FIND_DATA
#define d_name cFileName
#define d_namlen dwReserved1
/* dwReserved0 is internal flag */
#define d_ino nFileSizeLow
#define _HANDLE void*

typedef struct
{
    char *name;
     _HANDLE handle;
    unsigned count;
} DIR;

int		closedir(DIR *dirp);
DIR *		opendir(const char *_dirname);
struct dirent  *readdir(DIR *_dirp);

#ifdef __cplusplus
}
#endif

#endif	/* DIRENT_W32 */
