/*
** Copyright (C) 1999-2000 Paul.Sokolovsky@technologist.com
** see COPYING.PW32 for details
*/

#include "win32.h"
#include "dirent_w32.h"
#include <malloc.h>

struct dirent _dirent_buf;
int __ino = 0;

#define isupperxdigit(c) (isdigit(c) || (c)>='A' && (c)<='F')




DIR *opendir(const char *dir)
{
    HANDLE h;
    int len;
    DIR *d;

    /* POSIX2WIN32(dir); */

    len = strlen(dir);
    d = (DIR*) malloc(sizeof(DIR)+len+3);
    if(d)
    {
	char *p=(char*)d+sizeof(DIR);

	memcpy(p,dir,len);
	if (p[len-1] != '\\')
	    p[len++] = '\\';
	p[len++] = '*';
	p[len] = 0;
	d->name = p;
	d->count = 0;
    }

    if (!d)
	return NULL;
  
    if ((h=FindFirstFile(d->name,(WIN32_FIND_DATA*)&_dirent_buf)) ==
	INVALID_HANDLE_VALUE)
    {
	free(d);
	return NULL;
    }
    d->handle=h;

    _dirent_buf.dwReserved0 = 1;

    return d;
}





struct dirent *readdir(DIR *dir)
{
    if(!_dirent_buf.dwReserved0)
    {
	if(!FindNextFile(dir->handle,(WIN32_FIND_DATA*)&_dirent_buf))
	    return NULL;
    }
    _dirent_buf.dwReserved0=0;
    _dirent_buf.d_ino=__ino++;

    /* urldecode name */
    {
	char *p=strchr(_dirent_buf.d_name,'%');

	if(p && p[1] && p[2])
	{
	    char *to = p; 
	    do
	    {
		if(*p=='%' && isupperxdigit(p[1]) && isupperxdigit(p[2]))
		{
		    int a;
		    int b;
		    a = *++p - '0';
		    if(a>9)
			a -= 'A' - ('9'+1);
		    b = *++p - '0';
		    if(b>9)
			b -= 'A'-('9'+1);
		    *to++ = (a<<4) | b;
		    ++p;
		}
		else
		  *to++ = *p++;
	    } while(*p);

	    *to = 0;
	}
    }
    _dirent_buf.d_namlen = strlen(_dirent_buf.d_name);

    ++dir->count;

    return (struct dirent*) &_dirent_buf;
}





int closedir(DIR *dir)
{
  HANDLE h=dir->handle;

  free(dir);
  if(FindClose(h))
      return 0;

  return 1;
}
