/**********************************************************************/
/*
	hash table data structures
*/
/**********************************************************************/

#include <macros.h>

/* hash table entry */
typedef struct hash_table_entry HASH_TABLE_ENTRY; 
struct hash_table_entry {
  char *seq_name;		/* sequence name */ 
  int col;			/* column number */
  HASH_TABLE_ENTRY *next;	/* link to collision list */
};

/* hash table */
typedef struct hash_table *HASH_TABLE;
struct hash_table {
  int n;			/* number of bins in hash table */
  HASH_TABLE_ENTRY **table;	/* array of entry pointers */
};

HASH_TABLE hash_create(
  int n
);

void hash_destroy(
  HASH_TABLE ht			/* hash table to destroy */
);

void hash_insert(
  char *seq_name,    	/* sequence name */
  int col,              /* column number */
  HASH_TABLE ht         /* the hash table */
);

extern BOOLEAN hash_lookup(
  char *seq_name,	/* sequence name */
  int col,              /* column number */
  HASH_TABLE ht         /* the hash table */
);
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/hash.h,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
