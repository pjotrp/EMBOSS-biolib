/*****************************************************************
 * HMMER - Biological sequence analysis with profile HMMs
 * Copyright (C) 1992-1998 Washington University School of Medicine
 * All Rights Reserved
 * 
 *     This source code is distributed under the terms of the
 *     GNU General Public License. See the files COPYING and LICENSE
 *     for details.
 *****************************************************************/

/* gsi.c
 * Interfaces for GSI "generic sequence index" files.
 * broken away from sqio.c and extended: SRE, Wed Aug  5 10:32:53 1998
 * 
 * 
 * GSI definition: 
 *    1 + <nfiles> + <nkeys> total records.
 *    Each record = 38 bytes.
 *
 *  one header record     :  <"GSI"    (32)> <nfiles (2)> <nkeys (4)> 
 *  <nfiles> file records :  <filename (32)> <fileno (2)> <fmt   (4)> 
 *  <nkeys>  key records  :  <key      (32)> <fileno (2)> <offset(4)> 
 *
 * Matches up with my Perl scripts that create GSI files.
 * 
 * RCS $Id: gsi.c,v 1.1 2001/07/29 14:13:49 ajb Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef SEEK_SET
#include <unistd.h>	/* needed for poor crippled SunOS */
#endif

#include "squid.h"
#ifdef MEMDEBUG
#include "dbmalloc.h"
#endif


/* Function: GSIOpen()
 * 
 * Purpose:  Open a GSI file. Returns the number of records in
 *           the file and a file pointer. Returns NULL on failure.
 *           The file pointer should be fclose()'d normally.
 */
GSIFILE *
GSIOpen(char *gsifile)
{
  GSIFILE    *gsi;
  char        magic[GSI_KEYSIZE];

  gsi = (GSIFILE *) MallocOrDie (sizeof(GSIFILE));
  if ((gsi->gsifp = fopen(gsifile, "r")) == NULL)
    { free(gsi); squid_errno = SQERR_NOFILE; return NULL; }

  if (! fread(magic, sizeof(char), GSI_KEYSIZE, gsi->gsifp))
    { free(gsi); squid_errno = SQERR_NODATA; return NULL; }
  if (strcmp(magic, "GSI") != 0) 
    { free(gsi); squid_errno = SQERR_FORMAT; return NULL; }

  if (! fread(&(gsi->nfiles), sizeof(sqd_uint16), 1, gsi->gsifp))
    { free(gsi); squid_errno = SQERR_NODATA; return NULL; }
  if (! fread(&(gsi->recnum), sizeof(sqd_uint32), 1, gsi->gsifp))
    { free(gsi); squid_errno = SQERR_NODATA; return NULL; }

  gsi->nfiles = sre_ntohs(gsi->nfiles); /* convert from network short */
  gsi->recnum = sre_ntohl(gsi->recnum); /* convert from network long  */

  return gsi;
}

/* Function: GSIGetRecord()
 * 
 * Purpose:  Each non-header record of a GSI index files consists
 *           of 38 bytes: 32 bytes of character string, a 2 byte
 *           short, and a 4 byte long. This function returns the
 *           three values.
 *           
 * Args:     gsi  - open GSI index file, correctly positioned at a record
 *           f1   - char[32], allocated by caller (or NULL if unwanted)
 *           f2   - pointer to short (or NULL if unwanted)
 *           f3   - pointer to long  (or NULL if unwanted)
 *                  
 * Return:   0 on failure and sets squid_errno.                  
 */
int
GSIGetRecord(GSIFILE *gsi, char *f1, sqd_uint16 *f2, sqd_uint32 *f3)
{
  if (f1 == NULL) fseek(gsi->gsifp, GSI_KEYSIZE, SEEK_CUR);
  else if (! fread(f1, GSI_KEYSIZE, 1, gsi->gsifp))
    { squid_errno = SQERR_NODATA; return 0; }

  if (f2 == NULL) fseek(gsi->gsifp, sizeof(sqd_uint16), SEEK_CUR);
  else if (! fread(f2, sizeof(sqd_uint16), 1, gsi->gsifp))
    { squid_errno = SQERR_NODATA; return 0; }

  if (f3 == NULL) fseek(gsi->gsifp, sizeof(sqd_uint32), SEEK_CUR);
  else if (! fread(f3, sizeof(sqd_uint32), 1, gsi->gsifp))
    { squid_errno = SQERR_NODATA; return 0; }

  if (f2 != NULL) *f2 = sre_ntohs(*f2);
  if (f3 != NULL) *f3 = sre_ntohl(*f3);

  return 1;
}


/* Function: GSIGetOffset()
 * 
 * Purpose:  From a key (sequence name), find a disk offset
 *           in an open general sequence index file by binary
 *           search. Presumably GSI indexing could be even faster
 *           if we used hashing.
 *   
 * Args:     gsi         - GSI index file, opened by GSIOpen()
 *           key         - name of key to retrieve indices for
 *           ret_seqfile - pre-alloced char[32] array for seqfile name
 *           ret_fmt     - format of seqfile
 *           ret_offset  - return: disk offset in seqfile.         
 */
int
GSIGetOffset(GSIFILE *gsi, char *key, char *ret_seqfile, 
	     int *ret_format, long *ret_offset)
{
  sqd_uint32  left, right, mid;
  int         cmp;
  char        name[GSI_KEYSIZE + 1];
  sqd_uint32  offset;
  sqd_uint16  filenum;
  sqd_uint32  fmt;

  name[GSI_KEYSIZE] = '\0';

  left  = gsi->nfiles + 1;
  right = gsi->nfiles + gsi->recnum;
  mid   = (left + right) / 2;
  fseek(gsi->gsifp, mid * GSI_RECSIZE, SEEK_SET);

  while (GSIGetRecord(gsi, name, &filenum, &offset))
    {
      cmp = strcmp(name, key);
      if      (cmp == 0)      break;	       /* found it!              */
      else if (left >= right) return 0;        /* oops, missed it; fail. */
      else if (cmp < 0)       left = mid + 1;  /* it's right of mid      */
      else if (cmp > 0)	      right = mid - 1; /* it's left of mid       */ 
      mid = (left + right) / 2;
      fseek(gsi->gsifp, mid * GSI_RECSIZE, SEEK_SET);
    }

  /* Using file number, look up the sequence file and format.
   */
  fseek(gsi->gsifp, filenum * GSI_RECSIZE, SEEK_SET);
  GSIGetRecord(gsi, ret_seqfile, NULL, &fmt);
  *ret_format =  (int) fmt;
  *ret_offset = (long) offset;
  
  return 1;
}
    
/* Function: GSIClose()
 * 
 * Purpose:  Close an open GSI sequence index file.
 */
void
GSIClose(GSIFILE *gsi)
{
  fclose(gsi->gsifp);
  free(gsi);
}


/* Function: GSIWriteHeader()
 * Date:     SRE, Wed Aug  5 10:36:02 1998 [St. Louis]
 *
 * Purpose:  Write the first record to an open GSI file:
 *           "GSI" <nfiles> <nkeys>
 *
 * Args:     fp      - open file to write to.
 *           nfiles  - number of files indexed
 *           nkeys   - number of keys indexed          
 *
 * Returns:  void
 */
void
GSIWriteHeader(FILE *fp, int nfiles, long nkeys)
{
  char       key[GSI_KEYSIZE];
  sqd_uint16 f1;
  sqd_uint32 f2;

  /* beware potential range errors here! should check.
   */
  f1 = (sqd_uint16) nfiles;
  f2 = (sqd_uint32) nkeys;
  f1 = sre_htons(f1);
  f2 = sre_htonl(f2);
  strcpy(key, "GSI");

  if (fwrite(key,   1, GSI_KEYSIZE, fp) < GSI_KEYSIZE) PANIC;
  if (fwrite(&f1,   2,  1, fp) < 1)  PANIC;
  if (fwrite(&f2,   4,  1, fp) < 1)  PANIC;
}


/* Function: GSIWriteFileRecord()
 * Date:     SRE, Wed Aug  5 10:45:51 1998 [St. Louis]
 *
 * Purpose:  Write a file record to an open GSI file.
 *
 * Args:     fp    - open GSI file
 *           fname - file name (max 31 characters)
 *           idx   - file number
 *           fmt   - file format (e.g. kPearson, etc.)
 *
 * Returns:  0 on failure. 1 on success.
 */
int
GSIWriteFileRecord(FILE *fp, char *fname, int idx, int fmt)
{
  sqd_uint16 f1;
  sqd_uint32 f2;

  if (strlen(fname) >= GSI_KEYSIZE) return 0;

  f1 = (sqd_uint16) idx;
  f2 = (sqd_uint32) fmt;
  f1 = sre_htons(f1);
  f2 = sre_htonl(f2);

  if (fwrite(fname, 1, GSI_KEYSIZE, fp) < GSI_KEYSIZE) PANIC;
  if (fwrite(&f1, 2, 1, fp) < 1)    PANIC;
  if (fwrite(&f2, 4, 1, fp) < 1)    PANIC;
  return 1;
}


/* Function: GSIWriteKeyRecord()
 * Date:     SRE, Wed Aug  5 10:52:30 1998 [St. Louis]
 *
 * Purpose:  Write a key record to a GSI file.
 *
 * Args:     fp      - open GSI file for writing
 *           key     - key (max 31 char + \0)
 *           fileidx - which file number to find this key in
 *           offset  - offset for this key       
 * 
 * Returns:  1 on success, else 0.
 *           will fail if key >= 32 chars, for instance.
 */
int
GSIWriteKeyRecord(FILE *fp, char *key, int fileidx, long offset)
{
  sqd_uint16 f1;
  sqd_uint32 f2;

  if (strlen(key) >= GSI_KEYSIZE) return 0;

  f1 = (sqd_uint16) fileidx;
  f2 = (sqd_uint32) offset;
  f1 = sre_htons(f1);
  f2 = sre_htonl(f2);
  
  if (fwrite(key, 1, GSI_KEYSIZE, fp) < GSI_KEYSIZE) PANIC;
  if (fwrite(&f1, 2,  1, fp) < 1) PANIC;
  if (fwrite(&f2, 4,  1, fp) < 1) PANIC;
  return 1;
}
