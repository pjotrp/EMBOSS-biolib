
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef emboss_transeq_h
#define emboss_transeq_h


/*
** Prototype definitions
*/

int transeq_acd(int argc, char **argv);
unsigned char **emboss_transeq(int table, const unsigned char *sequence);

/*
** End of prototype definitions
*/

#endif

#ifdef __cplusplus
}
#endif
