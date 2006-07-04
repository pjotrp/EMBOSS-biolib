#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajdom_h
#define ajdom_h


#define AJDOM_ELEMENT_NODE                1
#define AJDOM_ATTRIBUTE_NODE              2
#define AJDOM_TEXT_NODE                   3
#define AJDOM_CDATA_SECTION_NODE          4
#define AJDOM_ENTITY_REFERENCE_NODE       5
#define AJDOM_ENTITY_NODE                 6
#define AJDOM_PROCESSING_INSTRUCTION_NODE 7
#define AJDOM_COMMENT_NODE                8
#define AJDOM_DOCUMENT_NODE               9
#define AJDOM_DOCUMENT_TYPE_NODE          10
#define AJDOM_DOCUMENT_FRAGMENT_NODE      11
#define AJDOM_NOTATION_NODE               12




typedef struct AjSDomNode
{
    AjPStr name;
    AjPStr value;
    ajuint type;
    struct AjSDomNode     *parentnode;
    struct AjSDomNodeList *childnodes;
    struct AjSDomNode     *firstchild;
    struct AjSDomNode     *lastchild;
    struct AjSDomNode     *previoussibling;
    struct AjSDomNode     *nextsibling;

    struct AjSDomNodeList *attributes;

    struct AjSDomNode     *ownerdocument;

    ajuint subtreeModified;

    union 
    {
	struct
	{
	    struct AjSDomNode   *doctype;
	    struct AjSDomNode   *documentelement;
	    struct AjSDomNode   *document;
	    struct AjSDomNode   *defaultview;
	    struct AjSDomNode   *commonparent;
	    AjPStr version;
	    AjPStr encoding;
	    ajint standalone;
	} Document;

	struct
	{
	    AjPStr name;
	    struct AjSDomNodeList *entities;
	    struct AjSDomNodeList *notations;
	    AjPStr publicid;
	    AjPStr systemid;
	    AjPStr internalsubset;
	} DocumentType;

	struct 
	{
	    AjPStr tagname;
	} Element;

	struct
	{
	    AjPStr name;
	    ajint specified;
	    AjPStr value;
	    struct AjSDomNode *ownerelement;
	} Attr;

	struct
	{
	    AjPStr data;
	    ajint length;
	} CharacterData;

	struct
	{
	    AjPStr publicid;
	    AjPStr systemid;
	} Notation;

	struct
	{
	    AjPStr publicid;
	    AjPStr systemid;
	    AjPStr notationname;
	} Entity;

	struct
	{
	    AjPStr target;
	    AjPStr data;
	} ProcessingInstruction;
    } sub;
} AjODomNode;

#define AjPDomNode AjODomNode*




typedef struct AjSDomNodeEntry
{
    struct AjSDomNodeEntry *prev;
    struct AjSDomNodeEntry *next;
    AjPDomNode node;
} AjODomNodeEntry;

#define AjPDomNodeEntry AjODomNodeEntry*




typedef struct AjSDomNodeList 
{
    AjPDomNode ownerdocument;
    AjPDomNode ownerelement;
    ajint length;
    AjPDomNodeEntry first;
    AjPDomNodeEntry last;
    ajuint filter;
    struct AjSDomNodeList *list;
    AjPTable table;
} AjODomNodeList;

#define AjPDomNodeList AjODomNodeList*




#define AjPDomDocument AjPDomNode
#define AjPDomNodeMap AjPDomNodeList
#define AjPDomDocumentType AjPDomNode
#define AjPDomElement AjPDomNode
#define AjPDomText AjPDomNode
#define AjPDomCharacterData AjPDomNode
#define AjPDomDocumentFragment AjPDomNode
#define AjPDomCDATASection AjPDomNode
#define AjPDomComment AjPDomNode
#define AjPDomAttr AjPDomNode
#define AjPDomEntityReference AjPDomNode
#define AjPDomPi AjPDomNode




/*
** Prototype definitions
*/


AjPDomNode ajDomDocumentCreateNode(AjPDomDocument doc, ajuint nodetype);

AjPDomDocumentType ajDomImplementationCreateDocumentType(AjPStr qualname,
							 AjPStr publicid,
							 AjPStr systemid);
AjPDomDocumentType ajDomImplementationCreateDocumentTypeC(char *qualname,
							  char *publicid,
							  char *systemid);

AjPDomDocument  ajDomImplementationCreateDocument(AjPStr uri, AjPStr qualname,
						  AjPDomDocumentType doctype);
AjPDomDocument ajDomImplementationCreateDocumentC(char *uri, char *qualname,
						  AjPDomDocumentType doctype);

AjPDomNodeEntry ajDomNodeListAppend(AjPDomNodeList list, AjPDomNode child);

AjPStr          ajDomElementGetAttribute(const AjPDomElement element,
					 const AjPStr name);
AjPStr          ajDomElementGetAttributeC(const AjPDomElement element,
				          const char *name);

void            ajDomElementSetAttribute(const AjPDomElement element,
					 const AjPStr name,
					 const AjPStr value);
void            ajDomElementSetAttributeC(const AjPDomElement element,
					  const char *name,
			                  const char *value);
void            ajDomElementRemoveAttribute(AjPDomElement element,
					    const AjPStr name);
void            ajDomElementRemoveAttributeC(AjPDomElement element,
					     const char *name);

AjPDomNode     ajDomElementGetAttributeNode(const AjPDomElement element,
					    const AjPStr name);
AjPDomNode     ajDomElementGetAttributeNodeC(const AjPDomElement element,
					     const char *name);

AjPDomNode     ajDomElementSetAttributeNode(AjPDomElement element,
					    AjPDomNode newattr);
AjPDomNodeList ajDomElementGetElementsByTagName(AjPDomElement element,
						const AjPStr name);
AjPDomNodeList ajDomElementGetElementsByTagNameC(AjPDomElement element,
						 const char *name);

void           ajDomElementNormalise(AjPDomElement element);
AjPStr         ajDomCharacterDataSubstringData(AjPDomCharacterData data,
					       ajint offset, ajint count);
void           ajDomCharacterDataAppendData(AjPDomCharacterData data,
					    const AjPStr arg);
void           ajDomCharacterDataAppendDataC(AjPDomCharacterData data,
					     const char *arg);
void           ajDomCharacterDataInsertData(AjPDomCharacterData data,
					    ajint offset, const AjPStr arg);
void           ajDomCharacterDataInsertDataC(AjPDomCharacterData data,
					     ajint offset, const char *arg);

void           ajDomCharacterDataDeleteData(AjPDomCharacterData data,
					    ajint offset, ajint count);
void           ajDomCharacterDataReplaceData(AjPDomCharacterData data,
					     ajint offset, ajint count,
					     const AjPStr arg);
void           ajDomCharacterDataReplaceDataC(AjPDomCharacterData data,
					      ajint offset, ajint count,
					      const char *arg);

ajint          ajDomCharacterDataGetLength(AjPDomCharacterData data);
AjPDomText     ajDomTextSplitText(AjPDomText text, ajint offset);
AjPDomElement  ajDomDocumentCreateElement(AjPDomDocument doc,
					  const AjPStr tagname);
AjPDomElement ajDomDocumentCreateElementC(AjPDomDocument doc,
					  const char *tagname);

AjPDomDocumentFragment ajDomDocumentCreateDocumentFragment(AjPDomDocument doc);

AjPDomText ajDomDocumentCreateTextNode(AjPDomDocument doc, const AjPStr data);
AjPDomText ajDomDocumentCreateTextNodeC(AjPDomDocument doc, const char *data);

AjPDomComment ajDomDocumentCreateComment(AjPDomDocument doc,
					 const AjPStr data);
AjPDomComment ajDomDocumentCreateCommentC(AjPDomDocument doc,
					  const char *data);

AjPDomCDATASection ajDomDocumentCreateCDATASection(AjPDomDocument doc,
						   const AjPStr data);
AjPDomCDATASection ajDomDocumentCreateCDATASectionC(AjPDomDocument doc,
						    const char *data);

AjPDomAttr ajDomDocumentCreateAttribute(AjPDomDocument doc,
					const AjPStr name);
AjPDomAttr ajDomDocumentCreateAttributeC(AjPDomDocument doc,
					 const char *name);

AjPDomEntityReference ajDomDocumentCreateEntityReference(AjPDomDocument doc,
							 const AjPStr name);
AjPDomEntityReference ajDomDocumentCreateEntityReferenceC(AjPDomDocument doc,
							  const char *name);

AjPDomPi ajDomDocumentCreateProcessingInstruction(AjPDomDocument doc,
						  const AjPStr target,
						  const AjPStr data);
AjPDomPi ajDomDocumentCreateProcessingInstructionC(AjPDomDocument doc,
						   const char *target,
						   const char *data);



AjPDomNodeList ajDomDocumentGetElementsByTagName(AjPDomDocument doc,
						 const AjPStr name);
AjPDomNodeList ajDomDocumentGetElementsByTagNameC(AjPDomDocument doc,
						  const char *name);

AjPDomDocumentType ajDomDocumentGetDoctype(AjPDomDocument doc);
AjPDomElement      ajDomDocumentGetDocumentElement(AjPDomDocument doc);

AjPDomNode         ajDomNodeAppendChild(AjPDomNode node,
					AjPDomNode extrachild);
AjPDomNode         ajDomNodeInsertBefore(AjPDomNode node, AjPDomNode newchild,
			                 AjPDomNode refchild);
AjPDomNode         ajDomRemoveChild(AjPDomNode node, AjPDomNode child);
void               ajDomDocumentDestroyNode(AjPDomDocument doc,
					    AjPDomNode node);
void               ajDomDocumentDestroyNodeList(AjPDomDocument doc,
						AjPDomNodeList list,
						AjBool donodes);
AjPDomNode         ajDomNodeReplaceChild(AjPDomNode node, AjPDomNode newchild,
					 AjPDomNode oldchild);
AjPDomNode         ajDomNodeCloneNode(AjPDomNode node, AjBool deep);
AjPDomNode         ajDomNodeListItem(const AjPDomNodeList list, ajint index);
AjBool             ajDomNodeListExists(AjPDomNodeList list, AjPDomNode child);
AjPDomNodeEntry    ajDomNodeListRemove(AjPDomNodeList list, AjPDomNode child);
AjPDomNodeList     ajDomCreateNodeList(AjPDomDocument doc);

AjPDomNode         ajDomNodeMapGetItem(const AjPDomNodeMap map,
				       const AjPStr name);
AjPDomNode         ajDomNodeMapGetItemC(const AjPDomNodeMap map,
					const char *name);
AjPDomNode         ajDomNodeMapSetItem(AjPDomNodeMap map, AjPDomNode arg);

AjPDomNode         ajDomNodeMapRemoveItem(AjPDomNodeMap map,
			 		  const AjPStr name);
AjPDomNode         ajDomNodeMapRemoveItemC(AjPDomNodeMap map,
			 		   const char *name);
AjPDomNode         ajDomNodeMapItem(const AjPDomNodeMap map, ajint index);

AjPDomNodeEntry  ajDomNodeListInsert(AjPDomNodeList list, AjPDomNode newchild,
				     AjPDomNode refchild);
AjPDomNodeEntry  ajDomNodeListReplace(AjPDomNodeList list, AjPDomNode newchild,
				      AjPDomNode oldchild);

AjBool ajDomNodeHasChildNodes(const AjPDomNode node);


ajint ajDomWrite(AjPDomDocument node, AjPFile outf);




void ajDomPrintNode(AjPDomNode node, ajint indent);
void ajDomPrintNode2(AjPDomNode node);
void ajDomNodePrintNode(AjPDomNode node);


/*
** End of prototype definitions
*/



#endif


#ifdef __cplusplus
}
#endif
