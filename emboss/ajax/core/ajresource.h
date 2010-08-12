#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajresource_h
#define ajresource_h




/* @data AjPResource **********************************************************
**
** Ajax data resource object
**
** Describes a standard public data resource
**
** AjPResource is implemented as a pointer to a C data structure.
**
** @alias AjSResource
** @alias AjOResource
**
** @attr Id [AjPStr]       Standard identifier of resource
** @attr Idalt [AjPList]   Alternative identifiers for this resource
** @attr Acc [AjPStr]      Accession number for this resource
** @attr Name [AjPStr]     Name, usually the ID in full
** @attr Desc [AjPStr]     Description
** @attr Url [AjPStr]      Resource web server
** @attr Urllink [AjPStr]  URL on linking instructions.
** @attr Urlrest [AjPStr]  URL of documentation on REST interface
** @attr Urlsoap [AjPStr]  URL of documentation on SOAP interface
** @attr Cat [AjPStr]      Database category (from dbxref.txt)
** @attr Edamres [AjPList] EDAM ontology references for the data resource
** @attr Edamdat [AjPList] EDAM ontology references (retrievable data)
** @attr Edamid [AjPList]  EDAM ontology references (query fields)
** @attr Edamfmt [AjPList] EDAM ontology references (format of retrievable data)
** @attr Xref [AjPList]    Database cross-reference link details
** @attr Query [AjPList]   Formatted query definitions
** @attr Example [AjPList] Formatted query examples
** @attr Contact [AjPStr]  Resource contact name
** @attr Email [AjPStr]    Resource contact email address
** @attr CCxref [AjPStr]   Notes on "Xref" field.
** @attr CCmisc [AjPStr]   Miscellaneous comments.
** @attr CCrest [AjPStr]   Comment on RESTful interface.
** @attr CCsoap [AjPStr]   Comments on SOAP-based interface.
** @attr Status [AjPStr]   Status of resource (controlled vocabulary)
** @@
******************************************************************************/
    
typedef struct AjSResource
{
    AjPStr  Id;
    AjPList Idalt;
    AjPStr  Acc;
    AjPStr  Name;
    AjPStr  Desc;
    AjPStr  Url;
    AjPStr  Urllink;
    AjPStr  Urlrest;
    AjPStr  Urlsoap;
    AjPStr  Cat;
    AjPList Edamres;
    AjPList Edamdat;
    AjPList Edamid;
    AjPList Edamfmt;
    AjPList Xref;
    AjPList Query;
    AjPList Example;
    AjPStr  Contact;
    AjPStr  Email;
    AjPStr  CCxref;
    AjPStr  CCmisc;
    AjPStr  CCrest;
    AjPStr  CCsoap;
    AjPStr  Status;
} AjOResource;

#define AjPResource AjOResource*



/*
** Prototype definitions
*/

    void         ajResourceParse(AjPFile resfile, const char* validations);
    AjPResource  ajResourceNew(void);
    void         ajResourceDel(AjPResource *resource);
    void         ajResourceWriteAll(AjPResource resource, AjPFile resfile);
    void         ajResourceWriteBasic(AjPResource resource, AjPFile resfile);
    void         ajResourceWriteBasicws(AjPResource resource, AjPFile resfile);
    /*
** End of prototype definitions
*/


#endif

#ifdef __cplusplus
}
#endif
