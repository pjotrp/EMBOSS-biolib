/******************************************************************************
** @source AJAX memory functions
**
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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ajdefine.h"
#include "ajassert.h"
#include "ajexcept.h"
#include "ajmem.h"

extern void ajDebug(char *fmt, ...);

static const Except_T Mem_Failed =
{
    "Allocation failed, insufficient memory available"
};

static const Except_T Mem_Badcount =
{
    "Allocation bad byte count"
};

static ajlong memAlloc       = 0;
static ajlong memFree        = 0;
static ajlong memCount       = 0;
static ajlong memResize      = 0;
static ajlong memResizeCount = 0;
static ajlong memTotal       = 0;
static ajlong memZero        = 0;




/* @func ajMemAlloc ***********************************************************
**
** Allocates memory using malloc, and fails with an error message if
** unsuccessful.
**
** @param [r] nbytes [ajlong] Number of bytes required
** @param [r] file [const char*] Source file name, generated by a macro.
** @param [r] line [ajint] Source line number, generated by a macro.
** @param [r] nofail [AjBool] If true, return with a NULL pointer when
**                            unable to allocate.
** @return [void*] Successfully allocated memory, or NULL on failure.
**                            Normal behaviour is to
**                            raise an exception and fail, or if running
**                            with Java, to print to standard error
**                            and exit.
** @@
******************************************************************************/

void* ajMemAlloc(ajlong nbytes, const char* file, ajint line, AjBool nofail)
{
    void *ptr;

    if(nbytes <= 0)
    {
#ifdef HAVE_JAVA
	fprintf(stderr,"Attempt to allocate <=0 bytes");
	exit(-1);
#else
	ajExceptRaise(&Mem_Badcount, file, line);
#endif
    }

    ptr = malloc(nbytes);
    if(ptr == NULL)
    {
	if (nofail)
	    return NULL;
#ifdef HAVE_JAVA
	fprintf(stderr,"Memory allocation failed in ajMemAlloc");
	exit(-1);
#else
	if(file == NULL)
	    AJRAISE(Mem_Failed);
	else
	    ajExceptRaise(&Mem_Failed, file, line);
#endif
    }

    memAlloc += nbytes;
    memCount++;
    memTotal++;

    return ptr;
}




/* @func ajMemCalloc **********************************************************
**
** Allocates memory using calloc for an array of elements,
** and fails with an error message if unsuccessful.
**
** @param [r] count [ajlong] Number of elements required
** @param [r] nbytes [ajlong] Number of bytes required per element
** @param [r] file [const char*] Source file name, generated by a macro.
** @param [r] line [ajint] Source line number, generated by a macro.
** @param [r] nofail [AjBool] If true, return with a NULL pointer when
**                            unable to allocate.
** @return [void*] Successfully allocated memory, or NULL on failure.
**                            Normal behaviour is to
**                            raise an exception and fail, or if running
**                            with Java, to print to standard error
**                            and exit.
** @@
******************************************************************************/

void* ajMemCalloc(ajlong count, ajlong nbytes,
		  const char* file, ajint line, AjBool nofail)
{
    void *ptr;

    assert(count > 0);
    assert(nbytes > 0);

    ptr = calloc(count, nbytes);
    if(ptr == NULL)
    {
	if (nofail)
	    return NULL;
#ifdef HAVE_JAVA
	fprintf(stderr,"Memory allocation failed in ajMemCalloc");
	exit(-1);
#else
	if(file == NULL)
	    AJRAISE(Mem_Failed);
	else
	    ajExceptRaise(&Mem_Failed, file, line);
#endif
    }

    memAlloc += (count*nbytes);
    memCount++;
    memTotal++;

    return ptr;
}




/* @func ajMemCalloc0 *********************************************************
**
** Allocates memory using malloc for an array of elements,
** and fails with an error message if unsuccessful.
**
** The memory is initialised to zero.
**
** @param [r] count [ajlong] Number of elements required
** @param [r] nbytes [ajlong] Number of bytes required
** @param [r] file [const char*] Source file name, generated by a macro.
** @param [r] line [ajint] Source line number, generated by a macro.
** @param [r] nofail [AjBool] If true, return with a NULL pointer when
**                            unable to allocate.
** @return [void*] Successfully allocated memory, or NULL on failure.
**                            Normal behaviour is to
**                            raise an exception and fail, or if running
**                            with Java, to print to standard error
**                            and exit.
** @@
******************************************************************************/

void* ajMemCalloc0(ajlong count, ajlong nbytes,
		   const char* file, ajint line, AjBool nofail)
{
    void *ptr;

    ptr = ajMemCalloc(count, nbytes, file, line, nofail);
    if (ptr == NULL)
	return NULL;

    memset(ptr, 0, count*nbytes);

    memZero += (count*nbytes);

    return ptr;
}




/* @func ajMemFree ************************************************************
**
** Frees memory using 'free' and zeroes the pointer. Ignores NULL
** (uninitialized) pointers.
**
** @param [u] ptr [void*] Pointer to memory previously allocated with 'malloc'
** @param [r] file [const char*] Source file name, generated by a macro.
** @param [r] line [ajint] Source line number, generated by a macro.
** @return [void]
** @@
******************************************************************************/

void ajMemFree(void* ptr, const char* file, ajint line)
{
    if(ptr)
    {
	free(ptr);
	memCount--;
	memFree++;
	ptr = NULL;
    }

    return;
}




/* @func ajMemResize **********************************************************
**
** Resizes previously allocated memory, and ensures data is copied to
** the new location if it is moved.
**
** If the pointer is new then new memory is allocated automatically.
**
** @param [u] ptr [void*] Pointer to memory previously allocated with 'malloc'
** @param [r] nbytes [ajlong] Number of bytes required
** @param [r] file [const char*] Source file name, generated by a macro.
** @param [r] line [ajint] Source line number, generated by a macro.
** @param [r] nofail [AjBool] If true, return with a NULL pointer when
**                            unable to allocate.
** @return [void*] Successfully reallocated memory, or NULL on failure.
**                            Normal behaviour is to
**                            raise an exception and fail, or if running
**                            with Java, to print to standard error
**                            and exit.
** @@
******************************************************************************/

void* ajMemResize(void* ptr, ajlong nbytes,
		  const char* file, ajint line, AjBool nofail)
{
    assert(nbytes > 0);

    if(ptr == NULL)
    {
	ptr = ajMemAlloc(nbytes, file, line, nofail);
	return ptr;
    }

    ptr = realloc(ptr, nbytes);

    if(ptr == NULL)
    {
	if (nofail)
	    return NULL;
#ifdef HAVE_JAVA
	fprintf(stderr,"Memory allocation failed in ajMemResize");
	exit(-1);
#else
	if(file == NULL)
	    AJRAISE(Mem_Failed);
	else
	    ajExceptRaise(&Mem_Failed, file, line);
#endif
    }
  
    memResize += nbytes;
    memResizeCount++;
  
    return ptr;
}




/* @func ajMemArrB ************************************************************
**
** Creates an AjBool array.
** Use AJFREE to free the memory when no longer needed.
**
** @param [r] size [size_t] Number of array elements.
** @return [ajint*] Newly allocated array.
** @@
******************************************************************************/

ajint* ajMemArrB(size_t size)
{
    return AJCALLOC(size, sizeof(AjBool));
}




/* @func ajMemArrI ************************************************************
**
** Creates an integer array.
** Use AJFREE to free the memory when no longer needed.
**
** @param [r] size [size_t] Number of array elements.
** @return [ajint*] Newly allocated array.
** @@
******************************************************************************/

ajint* ajMemArrI(size_t size)
{
    return AJCALLOC(size, sizeof(ajint));
}




/* @func ajMemArrF ************************************************************
**
** Creates a float array.
** Use AJFREE to free the memory when no longer needed.
**
** @param [r] size [size_t] Number of array elements.
** @return [float*] Newly allocated array.
** @@
******************************************************************************/

float* ajMemArrF(size_t size)
{
    return AJCALLOC(size, sizeof(float));
}




/* @func ajMemStat ************************************************************
**
** Prints a summary of memory usage with debug calls
**
** @param [r] title [char*] Title for this summary
** @return [void]
** @@
******************************************************************************/

void ajMemStat(char* title)
{
    static ajlong statAlloc       = 0;
    static ajlong statCount       = 0;
    static ajlong statFree        = 0;
    static ajlong statResize      = 0;
    static ajlong statResizeCount = 0;
    static ajlong statTotal       = 0;
    static ajlong statZero        = 0;

    ajDebug("Memory usage since last call %s:\n", title);
    ajDebug("Memory usage (bytes): %ld allocated, %ld reallocated "
	    "%ld zeroed\n",
	    memAlloc - statAlloc, memResize - statResize, memZero - statZero);
    ajDebug("Memory usage (number): %ld allocates, "
	    "%ld frees, %ld resizes, %ld in use\n",
	    memTotal - statTotal, memFree - statFree,
	    memResizeCount - statResizeCount, memCount - statCount);

    statAlloc  = memAlloc;
    statCount  = memCount;
    statFree   = memFree;
    statResize = memResize;
    statTotal  = memTotal;
    statZero   = memZero;

    statResizeCount = memResizeCount;

    return;
}




/* @func ajMemExit ************************************************************
**
** Prints a summary of memory usage with debug calls
**
** @return [void]
** @@
******************************************************************************/

void ajMemExit(void)
{
    ajDebug("Memory usage (bytes): %ld allocated, %ld reallocated "
	    "%ld zeroed\n",
	    memAlloc, memResize, memZero);
    ajDebug("Memory usage (number): %ld allocates, "
	    "%ld frees, %ld resizes, %ld in use\n",
	    memTotal, memFree, memResizeCount, memCount);

    return;
}
