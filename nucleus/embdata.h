#ifdef __cplusplus
extern "C"
{
#endif

#ifndef embdata_h
#define embdata_h



void     embDataListDel( AjPList data);
AjPTable embDataListGetTable( AjPList fullList, ajuint required);
void     embDataListGetTables( AjPList fullList, AjPList returnList,
			       ajuint required);
void     embDataListRead( AjPList data, AjPFile file);

#endif

#ifdef __cplusplus
}
#endif
