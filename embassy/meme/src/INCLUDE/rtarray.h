/******************************************************************************
** 	rtarray.h:
**
**	Header for macros dimensioning arrays at run time.
**		DIM2(prow,row,col,type) - row x col array of type, prow is type **
**		FREE2(prow)             - release storage allocated by DIM2
**
**	Anderson, P. and Anderson, G., "Advanced C tips and techniques", Hayden Books, 
**		Indianapolis, pg 187, (1988)
******************************************************************************/
#ifndef RTARRAY_H
#define RTARRAY_H

/*#include <stdio.h>*/
#include <stdlib.h>

#define DIM2( prow, row, col, type ) { \
	int irows; \
	type *pdata; \
	pdata = (type *) calloc( (size_t)((row) * (col)), sizeof(type) ); \
	if ( pdata == (type *) NULL ) { \
		fprintf(stderr, "DIM2 - failure to calloc pdata - insufficient space in heap\n" ); \
		exit(1); \
	} \
	prow = (type **) calloc( (size_t)(row), sizeof(type *) ); \
	if ( prow == (type **) NULL ) { \
		fprintf(stderr, "DIM2 - failure to calloc" #prow "- insufficient space in heap\n" ); \
		exit(1); \
	} \
	for ( irows=0; irows<row; irows++ ) { \
		prow[irows] = pdata; \
		pdata += col; \
	} \
} 

#define FREE2(prow) { \
	free( *prow ); \
	free( prow ) ; \
} 

#endif

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/rtarray.h,v 1.1 2000/11/05 21:47:56 ajb Exp $ */
