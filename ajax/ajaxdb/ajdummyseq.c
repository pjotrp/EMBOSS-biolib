/******************************************************************************
** @source AJAX sequence split functions
**
** @author Copyright (C) Same as originals
** @version 1.0
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

#include "ajax.h"

/* @filesection ajaxseq ****************************************************
**
** @nam1rule aj Function belongs to the AJAXSEQ library.
**
*/

/* @datasection [none] dummy function ****************************************
**
** @nam2rule Dummy Function
**
*/


/* @func ajAjaxseqDummy ******************************************************
**
** Calls 'exit' with a successful code (zero), but first calls ajReset to
** call memory clean up and debug reporting functions.
**
** No cleanup or reporting routines are called. Simply crashes.
**
** @return [void]
** @@
******************************************************************************/

void ajAjaxseqDummy(void)
{
    return;
}
