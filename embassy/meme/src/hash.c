/**********************************************************************/
/*
	hashing functions
*/
/**********************************************************************/

#include "hash.h"

#define HASH(s, c, n)	(((c) + (s)) % (n))


/**********************************************************************/
/*
	hash
*/
/**********************************************************************/
int hash(
  char *seq_name, 
  int col, 
  int n
)
{
  int i, p;
  p = col;
  for (i=0; seq_name[i] != 0; i++) p += seq_name[i];
  p = p % n;
  return p;
}
  

/**********************************************************************/
/*
	hash_create
*/
/**********************************************************************/
HASH_TABLE hash_create(
  int n
)
{
  int i;
  HASH_TABLE ht = (HASH_TABLE) malloc(sizeof(struct hash_table));

  /* initialize hash table */
  ht->n = n;
  ht->table = (HASH_TABLE_ENTRY **) malloc(n * sizeof(HASH_TABLE_ENTRY *));
  for (i=0; i<n; i++) ht->table[i] = NULL;

  return ht;
}
/**********************************************************************/
/*
	hash_destroy
*/
/**********************************************************************/
void hash_destroy(
  HASH_TABLE ht 		/* hash table to destroy */
)
{
  int i;
  for (i=0; i < ht->n; i++) {
    HASH_TABLE_ENTRY *hte = ht->table[i];
    while (hte != NULL) {
      HASH_TABLE_ENTRY *next = hte->next;
      myfree(hte->seq_name);
      myfree(hte);
      hte = next;
    }
  }
  myfree(ht->table);
  myfree(ht);
}


/**********************************************************************/
/*
	hash_insert
*/
/**********************************************************************/
void hash_insert(
  char *seq_name,	/* sequence name */
  int col,		/* column number */
  HASH_TABLE ht		/* the hash table */
)
{
  /* compute the position in hash table of entry */
  int p = hash(seq_name,col,ht->n);

  /* make a hash-table entry */
  HASH_TABLE_ENTRY *hte = (HASH_TABLE_ENTRY *) malloc(sizeof(HASH_TABLE_ENTRY));
  hte->seq_name = (char *) malloc (strlen(seq_name) + 1);
  strcpy(hte->seq_name, seq_name);
  hte->col = col;

  /* insert the entry into the table */
  hte->next = ht->table[p];
  ht->table[p] = hte;
  /*printf("p = %d\n", p);*/
}

/**********************************************************************/
/*
	hash_lookup
*/
/**********************************************************************/
extern BOOLEAN hash_lookup(
  char *seq_name,	/* sequence name */
  int col,		/* column number */
  HASH_TABLE ht		/* the hash table */
)
{
  /* compute the position in hash table of entry */
  int p = hash(seq_name,col,ht->n);

  /* get collision list for this hash index */
  HASH_TABLE_ENTRY *hte = ht->table[p];
  /*printf("p = %d\n", p);*/
  while (hte != NULL) {
    if ((hte->col == col) && (!strcmp(hte->seq_name, seq_name))) return TRUE; 
    hte = hte->next;
  }
  return FALSE;
}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/hash.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
