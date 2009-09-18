/******************************************************************************
** @source Ensembl Table functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.1 $
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

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "enstable.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */




/* @func ensTableCmpUInt ******************************************************
**
** AJAX Table function to compare AJAX unsignd integer (ajuint)
** hash key values.
**
** @param [r] x [const void*] AJAX unsigned integer value address
** @param [r] y [const void*] AJAX unsigned integer value address
**
** @return [ajint] 0 if the values are identical, 1 if they are not
** @@
******************************************************************************/

ajint ensTableCmpUInt(const void *x, const void *y)
{
    const ajuint *a = NULL;
    const ajuint *b = NULL;
    
    a = (const ajuint *) x;
    b = (const ajuint *) y;
    
    /*
     ajDebug("ensTableCmpUInt *a %u *b %u result %d\n", *a, *b, (*a != *b));
     */
    
    return (*a != *b);
}




/* @func ensTableHashUInt *****************************************************
**
** AJAX Table function to handle AJAX unsigned integer (ajuint)
** hash key values.
**
** @param [r] key [const void*] AJAX unsigned integer key value address
** @param [r] hashsize [ajuint] Hash size (maximum hash value)
**
** @return [ajuint] Hash value
** @@
******************************************************************************/

ajuint ensTableHashUInt(const void *key, ajuint hashsize)
{
    const ajuint *a = NULL;
    
    if(!key)
	return 0;
    
    if(!hashsize)
	return 0;
    
    a = (const ajuint *) key;
    
    /*
     ajDebug("ensTableHashUInt result %u\n", ((*a >> 2) % hashsize));
     */
    
    return ((*a >> 2) % hashsize);
}
