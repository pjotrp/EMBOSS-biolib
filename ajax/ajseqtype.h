#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajseqtype_h
#define ajseqtype_h

void         ajSeqGap (AjPSeq thys, char gapc, char padc);
void         ajSeqGapLen (AjPSeq thys, char gapc, char padc, ajint ilen);
void         ajSeqGapS (AjPStr* seq, char gapc);
void         ajSeqPrintType (AjPFile outf, AjBool full);
void         ajSeqSetNuc (AjPSeq thys);
void         ajSeqSetProt (AjPSeq thys);
char         ajSeqTypeAnyS (AjPStr* pthys);
char         ajSeqTypeDnaS (AjPStr* pthys);
char         ajSeqTypeGapanyS (AjPStr* pthys);
char         ajSeqTypeGapdnaS (AjPStr* pthys);
char         ajSeqTypeGapnucS (AjPStr* pthys);
char         ajSeqTypeAnyprotS (AjPStr* pthys);
char         ajSeqTypeGapprotS (AjPStr* pthys);
char         ajSeqTypeGaprnaS (AjPStr* pthys);
AjBool       ajSeqTypeIsAny (AjPStr type_name);
AjBool       ajSeqTypeIsNuc (AjPStr type_name);
AjBool       ajSeqTypeIsProt (AjPStr type_name);
char         ajSeqTypeNucS (AjPStr* pthys);
char         ajSeqTypeProtS (AjPStr* pthys);
char         ajSeqTypePurednaS (AjPStr* pthys);
char         ajSeqTypePurenucS (AjPStr* pthys);
char         ajSeqTypePureprotS (AjPStr* pthys);
char         ajSeqTypePurernaS (AjPStr* pthys);
char         ajSeqTypeRnaS (AjPStr* pthys);
char         ajSeqTypeStopprotS (AjPStr* pthys);
void         ajSeqType (AjPSeq thys);
AjBool       ajSeqTypeCheckIn (AjPSeq thys, AjPSeqin seqin);
AjBool       ajSeqTypeCheckS (AjPStr* pthys, AjPStr type_name);

#endif

#ifdef __cplusplus
}
#endif
