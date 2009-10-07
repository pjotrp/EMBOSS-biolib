/******************************************************************************
** @source Ensembl Density functions.
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.4 $
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

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ensdensity.h"
#include <math.h>




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

/* @datastatic DensityPTypeRatio **********************************************
**
** Ensembl Density Type Ratio.
**
** Holds Ensembl Density Types and corresponding ratios.
**
** @alias DensitySTypeRatio
** @alias DensityOTypeRatio
**
** @attr DensityType [EnsPDensityType] Ensembl Density Type
** @attr Ratio [float] Ratio
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct DensitySTypeRatio
{
    EnsPDensityType DensityType;
    float Ratio;
    ajuint Padding;
} DensityOTypeRatio;

#define DensityPTypeRatio DensityOTypeRatio*




/* @datastatic DensityPLengthValue ********************************************
**
** Ensembl Density Length Value.
**
** Holds density values and corresponding lengths.
**
** @alias DensitySLengthValue
** @alias DensityOLengthValue
**
** @attr Length [ajuint] Length
** @attr Value [float] Value
** @@
******************************************************************************/

typedef struct DensitySLengthValue
{
    ajuint Length;
    float Value;
} DensityOLengthValue;

#define DensityPLengthValue DensityOLengthValue*




static const char *densityTypeValueType[] =
{
    NULL,
    "sum",
    "ratio",
    NULL
};




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPAnalysisadaptor
ensRegistryGetAnalysisadaptor(EnsPDatabaseadaptor dba);

extern EnsPAssemblyMapperadaptor
ensRegistryGetAssemblyMapperadaptor(EnsPDatabaseadaptor dba);

extern EnsPCoordSystemadaptor
ensRegistryGetCoordSystemadaptor(EnsPDatabaseadaptor dba);

extern EnsPDensityFeatureadaptor
ensRegistryGetDensityFeatureadaptor(EnsPDatabaseadaptor dba);

extern EnsPDensityTypeadaptor
ensRegistryGetDensityTypeadaptor(EnsPDatabaseadaptor dba);

extern EnsPSliceadaptor
ensRegistryGetSliceadaptor(EnsPDatabaseadaptor dba);

static AjBool densityTypeadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                              const AjPStr statement,
                                              EnsPAssemblyMapper am,
                                              EnsPSlice slice,
                                              AjPList dts);

static AjBool densityTypeadaptorCacheInsert(EnsPDensityTypeadaptor adaptor,
                                            EnsPDensityType *Pdt);

static AjBool densityTypeadaptorCacheInit(EnsPDensityTypeadaptor adaptor);

static void densityTypeadaptorCacheClearIdentifier(void **key, void **value,
                                                   void *cl);

static AjBool densityTypeadaptorCacheExit(EnsPDensityTypeadaptor adaptor);

static void densityTypeadaptorFetchAll(const void *key, void **value, void *cl);

static AjBool densityFeatureadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                 const AjPStr statement,
                                                 EnsPAssemblyMapper mapper,
                                                 EnsPSlice slice,
                                                 AjPList dfs);

static void *densityFeatureadaptorCacheReference(void *value);

static void densityFeatureadaptorCacheDelete(void **value);

static ajuint densityFeatureadaptorCacheSize(const void *value);

static EnsPFeature densityFeatureadaptorGetFeature(const void *value);

static int densityTypeRatioCompareRatio(const void* P1, const void* P2);

static int densityFeatureCompareStart(const void* P1, const void* P2);




/* @filesection ensdensity ****************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPDensityType] Density Type ********************************
**
** Functions for manipulating Ensembl Density Type objects
**
** @cc Bio::EnsEMBL::DensityType CVS Revision: 1.5
**
** @nam2rule DensityType
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Density Type by pointer.
** It is the responsibility of the user to first destroy any previous
** Density Type. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPDensityType]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPDensityType] Ensembl Density Type
** @argrule Ref object [EnsPDensityType] Ensembl Density Type
**
** @valrule * [EnsPDensityType] Ensembl Density Type
**
** @fcategory new
******************************************************************************/




/* @func ensDensityTypeNew ****************************************************
**
** Default Ensembl Density Type constructor.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::DensityType::new
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
** @param [r] type [AjEnum] Value type
** @param [r] size [ajuint] Block size
** @param [r] features [ajuint] Number of Features
**
** @return [EnsPDensityType] Ensembl Density Type or NULL
** @@
******************************************************************************/

EnsPDensityType ensDensityTypeNew(EnsPDensityTypeadaptor adaptor,
                                  ajuint identifier,
                                  EnsPAnalysis analysis,
                                  AjEnum type,
                                  ajuint size,
                                  ajuint features)
{
    EnsPDensityType dt = NULL;
    
    if(!analysis)
        return NULL;
    
    AJNEW0(dt);
    
    dt->Use            = 1;
    dt->Identifier     = identifier;
    dt->Adaptor        = adaptor;
    dt->Analysis       = ensAnalysisNewRef(analysis);
    dt->ValueType      = type;
    dt->BlockSize      = size;
    dt->RegionFeatures = features;
    
    return dt;
}




/* @func ensDensityTypeNewObj *************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPDensityType] Ensembl Density Type
**
** @return [EnsPDensityType] Ensembl Density Type or NULL
** @@
******************************************************************************/

EnsPDensityType ensDensityTypeNewObj(const EnsPDensityType object)
{
    EnsPDensityType dt = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(dt);
    
    dt->Use            = 1;
    dt->Identifier     = object->Identifier;
    dt->Adaptor        = object->Adaptor;
    dt->Analysis       = ensAnalysisNewRef(object->Analysis);
    dt->ValueType      = object->ValueType;
    dt->BlockSize      = object->BlockSize;
    dt->RegionFeatures = object->RegionFeatures;
    
    return dt;
}




/* @func ensDensityTypeNewRef *************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] dt [EnsPDensityType] Ensembl Density Type
**
** @return [EnsPDensityType] Ensembl Density Type or NULL
** @@
******************************************************************************/

EnsPDensityType ensDensityTypeNewRef(EnsPDensityType dt)
{
    if(!dt)
	return NULL;
    
    dt->Use++;
    
    return dt;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Density Type.
**
** @fdata [EnsPDensityType]
** @fnote None
**
** @nam3rule Del Destroy (free) a Density Type object
**
** @argrule * Pdt [EnsPDensityType*] Density Type object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensDensityTypeDel ****************************************************
**
** Default destructor for an Ensembl Density Type.
**
** @param [d] Pdt [EnsPDensityType*] Ensembl Density Type address
**
** @return [void]
** @@
******************************************************************************/

void ensDensityTypeDel(EnsPDensityType *Pdt)
{
    EnsPDensityType pthis = NULL;
    
    if(!Pdt)
        return;
    
    if(!*Pdt)
        return;

    pthis = *Pdt;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pdt = NULL;
	
	return;
    }
    
    ensAnalysisDel(&pthis->Analysis);
    
    AJFREE(pthis);

    *Pdt = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Density Type object.
**
** @fdata [EnsPDensityType]
** @fnote None
**
** @nam3rule Get Return Density Type attribute(s)
** @nam4rule Getadaptor Return the Ensembl Density Type Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetAnalysis Return the Ensembl Analysis
** @nam4rule GetValueType Return the value type
** @nam4rule GetBlockSize Return the block size
** @nam4rule GetRegionFeatures Return the region features
**
** @argrule * dt [const EnsPDensityType] Density Type
**
** @valrule Adaptor [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Analysis [EnsPAnalysis] Ensembl Analysis
** @valrule ValueType [AjEnum] Value type
** @valrule BlockSize [ajuint] Block size
** @valrule RegionFeatures [ajuint] Region features
**
** @fcategory use
******************************************************************************/




/* @func ensDensityTypeGetadaptor *********************************************
**
** Get the Ensembl Density Type Adaptor element of an Ensembl Density Type.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] dt [const EnsPDensityType] Ensembl Density Type
**
** @return [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
** @@
******************************************************************************/

EnsPDensityTypeadaptor ensDensityTypeGetadaptor(const EnsPDensityType dt)
{
    if(!dt)
        return NULL;
    
    return dt->Adaptor;
}




/* @func ensDensityTypeGetIdentifier ******************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Density Type.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] dt [const EnsPDensityType] Ensembl Density Type
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensDensityTypeGetIdentifier(const EnsPDensityType dt)
{
    if(!dt)
        return 0;
    
    return dt->Identifier;
}




/* @func ensDensityTypeGetAnalysis ********************************************
**
** Get the Ensembl Analysis element of an Ensembl Density Type.
**
** @cc Bio::EnsEMBL::DensityType::analysis
** @param [r] dt [const EnsPDensityType] Ensembl Density Type
**
** @return [EnsPAnalysis] Ensembl Analysis or NULL
** @@
******************************************************************************/

EnsPAnalysis ensDensityTypeGetAnalysis(const EnsPDensityType dt)
{
    if(!dt)
        return NULL;
    
    return dt->Analysis;
}




/* @func ensDensityTypeGetValueType *******************************************
**
** Get the value type element of an Ensembl Density Type.
**
** @cc Bio::EnsEMBL::DensityType::value_type
** @param [r] dt [const EnsPDensityType] Ensembl Density Type
**
** @return [AjEnum] Value type or ensEDensityTypeValueTypeNULL
** @@
******************************************************************************/

AjEnum ensDensityTypeGetValueType(const EnsPDensityType dt)
{
    if(!dt)
        return ensEDensityTypeValueTypeNULL;
    
    return dt->ValueType;
}




/* @func ensDensityTypeGetBlockSize *******************************************
**
** Get the block size element of an Ensembl Density Type.
**
** @cc Bio::EnsEMBL::DensityType::block_size
** @param [r] dt [const EnsPDensityType] Ensembl Density Type
**
** @return [ajuint] Block size or 0
** @@
******************************************************************************/

ajuint ensDensityTypeGetBlockSize(const EnsPDensityType dt)
{
    if(!dt)
        return 0;
    
    return dt->BlockSize;
}




/* @func ensDensityTypeGetRegionFeatures **************************************
**
** Get the region features element of an Ensembl Density Type.
**
** @cc Bio::EnsEMBL::DensityType::region_features
** @param [r] dt [const EnsPDensityType] Ensembl Density Type
**
** @return [ajuint] Region features or 0
** @@
******************************************************************************/

ajuint ensDensityTypeGetRegionFeatures(const EnsPDensityType dt)
{
    if(!dt)
        return 0;
    
    return dt->RegionFeatures;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Density Type object.
**
** @fdata [EnsPDensityType]
** @fnote None
**
** @nam3rule Set Set one element of a Density Type
** @nam4rule Setadaptor Set the Ensembl Density Type Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetAnalysis Set the Ensembl Analysis
** @nam4rule SetValueType Set the value type
** @nam4rule SetBlockSize Set the block size
** @nam4rule SetRegionFeatures Set the region features
**
** @argrule * dt [EnsPDensityType] Ensembl Density Feature object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensDensityTypeSetadaptor *********************************************
**
** Set the Ensembl Density Type Adaptor element of an Ensembl Density Type.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] dt [EnsPDensityType] Ensembl Density Type
** @param [r] adaptor [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityTypeSetadaptor(EnsPDensityType dt,
                                EnsPDensityTypeadaptor adaptor)
{
    if(!dt)
        return ajFalse;
    
    dt->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensDensityTypeSetIdentifier ******************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Density Type.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] dt [EnsPDensityType] Ensembl Density Type
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityTypeSetIdentifier(EnsPDensityType dt,
                                   ajuint identifier)
{
    if(!dt)
        return ajFalse;
    
    dt->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensDensityTypeSetAnalysis ********************************************
**
** Set the Ensembl Analysis element of an Ensembl Density Type.
**
** @cc Bio::EnsEMBL::DensityType::analysis
** @param [u] dt [EnsPDensityType] Ensembl Density Type
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityTypeSetAnalysis(EnsPDensityType dt, EnsPAnalysis analysis)
{
    if(!dt)
        return ajFalse;
    
    ensAnalysisDel(&dt->Analysis);
    
    dt->Analysis = ensAnalysisNewRef(analysis);
    
    return ajTrue;
}




/* @func ensDensityTypeSetValueType *******************************************
**
** Set the value type element of an Ensembl Density Type.
**
** @cc Bio::EnsEMBL::DensityType::value_type
** @param [u] dt [EnsPDensityType] Ensembl Density Type
** @param [r] type [AjEnum] Value type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityTypeSetValueType(EnsPDensityType dt, AjEnum type)
{
    if(!dt)
        return ajFalse;
    
    dt->ValueType = type;
    
    return ajTrue;
}




/* @func ensDensityTypeSetBlockSize *******************************************
**
** Set the block size element of an Ensembl Density Type.
**
** @cc Bio::EnsEMBL::DensityType::block_size
** @param [u] dt [EnsPDensityType] Ensembl Density Type
** @param [u] size [ajuint] Block size
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityTypeSetBlockSize(EnsPDensityType dt, ajuint size)
{
    if(!dt)
        return ajFalse;
    
    dt->BlockSize = size;
    
    return ajTrue;
}




/* @func ensDensityTypeSetRegionFeatures **************************************
**
** Set the region features element of an Ensembl Density Type.
**
** @cc Bio::EnsEMBL::DensityType::region_features
** @param [u] dt [EnsPDensityType] Ensembl Density Type
** @param [u] features [ajuint] Region features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityTypeSetRegionFeatures(EnsPDensityType dt, ajuint features)
{
    if(!dt)
        return ajFalse;
    
    dt->RegionFeatures = features;
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Density Type object.
**
** @fdata [EnsPDensityType]
** @nam3rule Trace Report Ensembl Density Type elements to debug file
**
** @argrule Trace dt [const EnsPDensityType] Ensembl Density Type
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensDensityTypeTrace **************************************************
**
** Trace an Ensembl Density Type.
**
** @param [r] dt [const EnsPDensityType] Ensembl Density Type
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityTypeTrace(const EnsPDensityType dt, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!dt)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensDensityTypeTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Analysis %p\n"
	    "%S  ValueType '%s'\n"
	    "%S  BlockSize %u\n"
	    "%S  RegionFeatures %u\n",
	    indent, dt,
	    indent, dt->Use,
	    indent, dt->Identifier,
	    indent, dt->Adaptor,
	    indent, dt->Analysis,
	    indent, ensDensityTypeValeTypeToChar(dt->ValueType),
	    indent, dt->BlockSize,
	    indent, dt->RegionFeatures);
    
    ensAnalysisTrace(dt->Analysis, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensDensityTypeGetMemSize *********************************************
**
** Get the memory size in bytes of an Ensembl Density Type.
**
** @param [r] dt [const EnsPDensityType] Ensembl Density Type
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensDensityTypeGetMemSize(const EnsPDensityType dt)
{
    ajuint size = 0;
    
    if(!dt)
	return 0;
    
    size += (ajuint) sizeof (EnsODensityType);
    
    size += ensAnalysisGetMemSize(dt->Analysis);
    
    return size;
}




/* @func ensDensityTypeValeTypeFromStr ****************************************
**
** Convert an AJAX String into an Ensembl Density Type value type element.
**
** @param [r] type [const AjPStr] Value type string
**
** @return [AjEnum] Ensembl Density Type value type element or
**                  ensEDensityTypeValueTypeNULL
** @@
******************************************************************************/

AjEnum ensDensityTypeValeTypeFromStr(const AjPStr type)
{
    register ajint i = 0;
    
    AjEnum etype = ensEDensityTypeValueTypeNULL;
    
    for(i = 1; densityTypeValueType[i]; i++)
	if(ajStrMatchC(type, densityTypeValueType[i]))
	    etype = i;
    
    if(!etype)
	ajDebug("ensDensityTypeValeTypeFromStr encountered "
		"unexpected string '%S'.\n", type);
    
    return etype;
}




/* @func ensDensityTypeValeTypeToChar *****************************************
**
** Convert an Ensembl Density Type value type element
** into a C-type (char*) string.
**
** @param [r] type [const AjEnum] Ensembl Density Type value type enumerator
**
** @return [const char*] Ensembl Density Type value type C-type (char*) string
** @@
******************************************************************************/

const char* ensDensityTypeValeTypeToChar(const AjEnum type)
{
    register ajint i = 0;
    
    if(!type)
	return NULL;
    
    for(i = 1; densityTypeValueType[i] && (i < type); i++);
    
    if (! densityTypeValueType[i])
	ajDebug("ensDensityTypeValeTypeToChar encountered an "
		"out of boundary error on gender %d.\n", type);
    
    return densityTypeValueType[i];
}




/* @datasection [EnsPDensityTypeadaptor] Density Type Adaptor *****************
**
** Functions for manipulating Ensembl Density Type Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::DensityTypeadaptor CVS Revision: 1.12
**
** @nam2rule DensityTypeadaptor
**
******************************************************************************/

static const char *densityTypeadaptorTables[] =
{
    "density_type",
    NULL
};

static const char *densityTypeadaptorColumns[] =
{
    "density_type.density_type_id",
    "density_type.analysis_id",
    "density_type.value_type",
    "density_type.block_size",
    "density_type.region_features",
    NULL
};

static EnsOBaseadaptorLeftJoin densityTypeadaptorLeftJoin[] =
{
    {NULL, NULL}
};

static const char *densityTypeadaptorDefaultCondition = NULL;

static const char *densityTypeadaptorFinalCondition = NULL;




/* @funcstatic densityTypeadaptorFetchAllBySQL ********************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl Density Type objects.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] dts [AjPList] AJAX List of Ensembl Density Types
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool densityTypeadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                              const AjPStr statement,
                                              EnsPAssemblyMapper am,
                                              EnsPSlice slice,
                                              AjPList dts)
{
    ajuint identifier = 0;
    ajuint analysisid = 0;
    ajuint size       = 0;
    ajuint features   = 0;
    
    AjEnum etype = ensEDensityTypeValueTypeNULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr type = NULL;
    
    EnsPAnalysis analysis  = NULL;
    EnsPAnalysisadaptor aa = NULL;
    
    EnsPDensityType dt         = NULL;
    EnsPDensityTypeadaptor dta = NULL;
    
    /*
     ajDebug("densityTypeadaptorFetchAllBySQL\n"
	     "  dba %p\n"
	     "  statement %p\n"
	     "  am %p\n"
	     "  slice %p\n"
	     "  dts %p\n",
	     dba,
	     statement,
	     am,
	     slice,
	     dts);
     */
    
    if(!dba)
        return ajFalse;
    
    if(!statement)
        return ajFalse;
    
    (void) am;
    
    (void) slice;
    
    if(!dts)
        return ajFalse;
    
    aa = ensRegistryGetAnalysisadaptor(dba);
    
    dta = ensRegistryGetDensityTypeadaptor(dba);
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier = 0;
	analysisid = 0;
	type = ajStrNew();
	size = 0;
	features = 0;
	etype = ensEDensityTypeValueTypeNULL;
	
        sqlr = ajSqlrowiterGet(sqli);
	
        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToUint(sqlr, &analysisid);
        ajSqlcolumnToStr(sqlr, &type);
        ajSqlcolumnToUint(sqlr, &size);
        ajSqlcolumnToUint(sqlr, &features);
	
	ensAnalysisadaptorFetchByIdentifier(aa, analysisid, &analysis);
	
	etype = ensDensityTypeValeTypeFromStr(type);
	
        dt = ensDensityTypeNew(dta,
			       identifier,
			       analysis,
			       etype,
			       size,
			       features);
	
        ajListPushAppend(dts, (void *) dt);
	
	ensAnalysisDel(&analysis);
	
	ajStrDel(&type);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    return ajTrue;
}




/* @funcstatic densityTypeadaptorCacheInsert **********************************
**
** Insert an Ensembl Density Type into the Density Type Adaptor-internal cache.
** If a Density Type with the same identifier element is already present in the
** adaptor cache, the Density Type is deleted and a pointer to the cached
** Density Type is returned.
**
** @param [u] adaptor [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
** @param [u] Pdt [EnsPDensityType*] Ensembl Density Type address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool densityTypeadaptorCacheInsert(EnsPDensityTypeadaptor adaptor,
                                            EnsPDensityType *Pdt)
{
    ajuint *Pidentifier = NULL;
    
    EnsPDensityType dt = NULL;
    
    if(!adaptor)
        return ajFalse;
    
    if(!adaptor->CacheByIdentifier)
        return ajFalse;
    
    if(!Pdt)
        return ajFalse;
    
    if(!*Pdt)
        return ajFalse;
    
    /* Search the identifer cache. */
    
    dt = (EnsPDensityType)
	ajTableFetch(adaptor->CacheByIdentifier,
		     (const void *) &((*Pdt)->Identifier));
    
    if(dt)
    {
        ajDebug("densityTypeadaptorCacheInsert replaced Density Type %p with "
		"one already cached %p.\n",
		*Pdt, dt);
	
	ensDensityTypeDel(Pdt);
	
	ensDensityTypeNewRef(dt);
	
	Pdt = &dt;
    }
    else
    {
	/* Insert into the identifier cache. */
	
	AJNEW0(Pidentifier);
	
	*Pidentifier = (*Pdt)->Identifier;
	
	ajTablePut(adaptor->CacheByIdentifier,
		   (void *) Pidentifier,
		   (void *) ensDensityTypeNewRef(*Pdt));
    }
    
    return ajTrue;
}




/* @funcstatic densityTypeadaptorCacheInit ************************************
**
** Initialise the internal Density Type cache of an
** Ensembl Density Type Adaptor.
**
** @param [u] adaptor [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool densityTypeadaptorCacheInit(EnsPDensityTypeadaptor adaptor)
{
    AjPList dts = NULL;
    
    EnsPDensityType dt = NULL;
    
    /*
     ajDebug("densityTypeadaptorCacheInit\n"
	     "  adaptor %p\n",
	     adaptor);
     */
    
    if(!adaptor)
        return ajFalse;
    
    if(adaptor->CacheByIdentifier)
        return ajFalse;
    else
        adaptor->CacheByIdentifier =
	    ajTableNewFunctionLen(0, ensTableCmpUInt, ensTableHashUInt);
    
    dts = ajListNew();
    
    ensBaseadaptorGenericFetch(adaptor->Adaptor,
			       (const AjPStr) NULL,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       dts);
    
    while(ajListPop(dts, (void **) &dt))
    {
        densityTypeadaptorCacheInsert(adaptor, &dt);
	
	ensDensityTypeDel(&dt);
    }
    
    ajListFree(&dts);
    
    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Density Type Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Density Type Adaptor. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPDensityTypeadaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
** @argrule Ref object [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
**
** @valrule * [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensDensityTypeadaptorNew *********************************************
**
** Default constructor for an Ensembl Density Type Adaptor.
**
** Ensembl Object Adaptors are singleton objects in the sense that a single
** instance of an Ensembl Object Adaptor connected to a particular database is
** sufficient to instantiate any number of Ensembl Objects from the database.
** Each Ensembl Object will have a weak reference to the Object Adaptor that
** instantiated it. Therefore, Ensembl Object Adaptors should not be
** instantiated directly, but rather obtained from the Ensembl Registry,
** which will in turn call this function if neccessary.
**
** @see ensRegistryGetDatabaseadaptor
** @see ensRegistryGetDensityTypeadaptor
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor or NULL
** @@
******************************************************************************/

EnsPDensityTypeadaptor ensDensityTypeadaptorNew(EnsPDatabaseadaptor dba)
{
    EnsPDensityTypeadaptor adaptor = NULL;
    
    if(!dba)
        return NULL;
    
    /*
     ajDebug("ensDensityTypeadaptorNew\n"
	     "  dba %p\n",
	     dba);
     */
    
    AJNEW0(adaptor);
    
    adaptor->Adaptor = ensBaseadaptorNew(dba,
					 densityTypeadaptorTables,
					 densityTypeadaptorColumns,
					 densityTypeadaptorLeftJoin,
					 densityTypeadaptorDefaultCondition,
					 densityTypeadaptorFinalCondition,
					 densityTypeadaptorFetchAllBySQL);
    
    /*
    ** NOTE: The cache cannot be initialised here because the
    ** densityTypeadaptorCacheInit function calls ensBaseadaptorGenericFetch,
    ** which calls densityTypeadaptorFetchAllBySQL, which calls
    ** ensRegistryGetDensityTypeadaptor. At that point, however, the
    ** Density Type Adaptor has not been stored in the Registry. Therefore,
    ** each ensDensityTypeadaptorFetch function has to test the presence of
    ** the adaptor-internal cache and eventually initialise before accessing
    ** it.
    ** 
    ** densityTypeadaptorCacheInit(adaptor);
    */
    
    return adaptor;
}




/* @funcstatic densityTypeadaptorCacheClearIdentifier *************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Density Type
** Adaptor-internal Density Type cache. This function deletes the
** unsigned integer identifier key and the Ensembl Density Type value data.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] Ensembl Density Type value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void densityTypeadaptorCacheClearIdentifier(void **key, void **value,
                                                   void *cl)
{
    if(!key)
	return;
    
    if(!*key)
	return;
    
    if(!value)
	return;
    
    if(!*value)
	return;
    
    (void) cl;
    
    AJFREE(*key);
    
    ensDensityTypeDel((EnsPDensityType *) value);

    *key   = NULL;
    *value = NULL;
    
    return;
}




/* @funcstatic densityTypeadaptorCacheExit ************************************
**
** Clears the internal Density Type cache of an Ensembl Density Type Adaptor.
**
** @param [u] adaptor [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool densityTypeadaptorCacheExit(EnsPDensityTypeadaptor adaptor)
{
    if(!adaptor)
        return ajFalse;
    
    /* Clear and delete the identifier cache. */
    
    ajTableMapDel(adaptor->CacheByIdentifier,
		  densityTypeadaptorCacheClearIdentifier,
		  NULL);
    
    ajTableFree(&(adaptor->CacheByIdentifier));
    
    return ajTrue;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Density Type Adaptor.
**
** @fdata [EnsPDensityTypeadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Density Type Adaptor object.
**
** @argrule * Padaptor [EnsPDensityTypeadaptor*] Ensembl Density Type Adaptor
**                                               object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensDensityTypeadaptorDel *********************************************
**
** Default destructor for an Ensembl Density Type Adaptor.
** This function also clears the internal caches.
**
** Ensembl Object Adaptors are singleton objects that are registered in the
** Ensembl Registry and weakly referenced by Ensembl Objects that have been
** instantiated by it. Therefore, Ensembl Object Adaptors should never be
** destroyed directly. Upon exit, the Ensembl Registry will call this function
** if required.
**
** @param [d] Padaptor [EnsPDensityTypeadaptor*] Ensembl Density Type Adaptor
**                                               address
**
** @return [void]
** @@
******************************************************************************/

void ensDensityTypeadaptorDel(EnsPDensityTypeadaptor* Padaptor)
{
    if(!Padaptor)
        return;
    
    if(!*Padaptor)
        return;
    
    /*
     ajDebug("ensDensityTypeadaptorDel\n"
	     "  Padaptor %p\n",
	     Padaptor);
     */
    
    densityTypeadaptorCacheExit(*Padaptor);
    
    ensBaseadaptorDel(&((*Padaptor)->Adaptor));
    
    AJFREE(*Padaptor);
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Density Type Adaptor object.
**
** @fdata [EnsPDensityTypeadaptor]
** @fnote None
**
** @nam3rule Get Return Ensembl Density Type Adaptor attribute(s)
** @nam4rule Getadaptor Return the Ensembl Base Adaptor
**
** @argrule * adaptor [const EnsPDensityTypeadaptor] Ensembl Density
**                                                   Type Adaptor
**
** @valrule Adaptor [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
**
** @fcategory use
******************************************************************************/




/* @func ensDensityTypeadaptorGetBaseadaptor **********************************
**
** Get the Ensembl Base Adaptor element of an Ensembl Density Type Adaptor.
**
** @param [r] adaptor [const EnsPDensityTypeadaptor] Ensembl Density
**                                                   Type Adaptor
**
** @return [EnsPBaseadaptor] Ensembl Base Adaptor
** @@
******************************************************************************/

EnsPBaseadaptor ensDensityTypeadaptorGetBaseadaptor(
    const EnsPDensityTypeadaptor adaptor)
{
    if(!adaptor)
        return NULL;
    
    return adaptor->Adaptor;
}




/* @func ensDensityTypeadaptorGetDatabaseadaptor ******************************
**
** Get the Ensembl Database Adaptor element of an Ensembl Density Type Adaptor.
**
** @param [r] adaptor [const EnsPDensityTypeadaptor] Ensembl Density
**                                                   Type Adaptor
**
** @return [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

EnsPDatabaseadaptor ensDensityTypeadaptorGetDatabaseadaptor(
    const EnsPDensityTypeadaptor adaptor)
{
    if(!adaptor)
        return NULL;
    
    return ensBaseadaptorGetDatabaseadaptor(adaptor->Adaptor);
}




/* @section object retrieval **************************************************
**
** Functions for retrieving Ensembl Density Type objects from an
** Ensembl Core database.
**
** @fdata [EnsPDensityTypeadaptor]
** @fnote None
**
** @nam3rule Fetch Retrieve Ensembl Density Type object(s)
** @nam4rule FetchAll Retrieve all Ensembl Density Type objects
** @nam5rule FetchAllBy Retrieve all Ensembl Density Type objects
**                      matching a criterion
** @nam4rule FetchBy Retrieve one Ensembl Density Type object
**                   matching a criterion
**
** @argrule * adaptor [const EnsPDensityTypeadaptor] Ensembl Density
**                                                   Type Adaptor
** @argrule FetchAll [AjPList] AJAX List of Ensembl Density Type objects
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory use
******************************************************************************/




/* @funcstatic densityTypeadaptorFetchAll *************************************
**
** An ajTableMap 'apply' function to return all Density Type objects from the
** Ensembl Density Type Adaptor-internal cache.
**
** @param [u] key [const void *] AJAX unsigned integer key data address
** @param [u] value [void**] Ensembl Density Type value data address
** @param [u] cl [void*] AJAX List of Ensembl Density Type objects,
**                       passed in via ajTableMap
** @see ajTableMap
**
** @return [void]
** @@
******************************************************************************/

static void densityTypeadaptorFetchAll(const void *key, void **value, void *cl)
{
    if(!key)
	return;
    
    if(!value)
	return;
    
    if(!*value)
	return;
    
    if(!cl)
	return;
    
    ajListPushAppend((AjPList) cl,
		     (void *)
		     ensDensityTypeNewRef(*((EnsPDensityType *) value)));
    
    return;
}




/* @func ensDensityTypeadaptorFetchAll ****************************************
**
** Fetch all Ensembl Density Types.
**
** The caller is responsible for deleting the Ensembl Density Types before
** deleting the AJAX List.
**
** @param [r] adaptor [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
** @param [u] dts [AjPList] AJAX List of Ensembl Density Types
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityTypeadaptorFetchAll(EnsPDensityTypeadaptor adaptor,
                                     AjPList dts)
{
    if(!adaptor)
        return ajFalse;
    
    if(!dts)
        return ajFalse;
    
    if(!adaptor->CacheByIdentifier)
        densityTypeadaptorCacheInit(adaptor);
    
    ajTableMap(adaptor->CacheByIdentifier,
	       densityTypeadaptorFetchAll,
	       (void *) dts);
    
    return ajTrue;
}




/* @func ensDensityTypeadaptorFetchByIdentifier *******************************
**
** Fetch an Ensembl Density Type by its SQL database-internal identifier.
** The caller is responsible for deleting the Ensembl Density Type.
**
** @param [r] adaptor [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Pdt [EnsPDensityType*] Ensembl Density Type address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityTypeadaptorFetchByIdentifier(EnsPDensityTypeadaptor adaptor,
                                              ajuint identifier,
                                              EnsPDensityType *Pdt)
{
    AjPList dts = NULL;
    
    AjPStr constraint = NULL;
    
    EnsPDensityType dt = NULL;
    
    if(!adaptor)
        return ajFalse;
    
    if(!identifier)
        return ajFalse;
    
    if(!Pdt)
	return ajFalse;
    
    /*
    ** Initally, search the identifier cache.
    ** For any object returned by the AJAX Table the reference counter needs
    ** to be incremented manually.
    */
    
    if(!adaptor->CacheByIdentifier)
        densityTypeadaptorCacheInit(adaptor);
    
    *Pdt = (EnsPDensityType)
	ajTableFetch(adaptor->CacheByIdentifier, (const void *) &identifier);
    
    if(*Pdt)
    {
	ensDensityTypeNewRef(*Pdt);
	
	return ajTrue;
    }
    
    /* For a cache miss re-query the database. */
    
    constraint = ajFmtStr("density_type.density_type_id = %u", identifier);
    
    dts = ajListNew();
    
    ensBaseadaptorGenericFetch(adaptor->Adaptor,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       dts);
    
    if(ajListGetLength(dts) > 1)
	ajWarn("ensDensityTypeadaptorFetchByIdentifier got more than one "
	       "Ensembl Density Type for (PRIMARY KEY) identifier %u.\n",
	       identifier);
    
    ajListPop(dts, (void **) Pdt);
    
    densityTypeadaptorCacheInsert(adaptor, Pdt);
    
    while(ajListPop(dts, (void **) &dt))
    {
	densityTypeadaptorCacheInsert(adaptor, &dt);
	
	ensDensityTypeDel(&dt);
    }
    
    ajListFree(&dts);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensDensityTypeadaptorFetchAllByAnalysisName **************************
**
** Fetch all Ensembl Density Types by an Ensembl Analysis name.
** The caller is responsible for deleting the Ensembl Density Types before
** deleting the AJAX List.
**
** @param [r] adaptor [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
** @param [r] name [const AjPStr] Ensembl Analysis name
** @param [u] dts [AjPList] AJAX List of Ensembl Density Types
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityTypeadaptorFetchAllByAnalysisName(
    EnsPDensityTypeadaptor adaptor,
    const AjPStr name,
    AjPList dts)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    EnsPAnalysis analysis = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!name)
	return ajFalse;
    
    if(!dts)
	return ajFalse;
    
    if(!adaptor->CacheByIdentifier)
        densityTypeadaptorCacheInit(adaptor);
    
    ajTableToarray(adaptor->CacheByIdentifier, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	analysis = ensDensityTypeGetAnalysis((EnsPDensityType) valarray[i]);
	
	if(ajStrMatchS(name, ensAnalysisGetName(analysis)))
	{
	    ajListPushAppend(dts, (void *)
			     ensDensityTypeNewRef((EnsPDensityType)
						  valarray[i]));
	}
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    return ajTrue;
}




/* @datasection [EnsPDensityFeature] Density Feature **************************
**
** Functions for manipulating Ensembl Density Feature objects
**
** @cc Bio::EnsEMBL::DensityFeature CVS Revision: 1.9
**
** @nam2rule DensityFeature
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Density Feature by pointer.
** It is the responsibility of the user to first destroy any previous
** Density Feature. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPDensityFeature]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPDensityFeature] Ensembl Density Feature
** @argrule Ref object [EnsPDensityFeature] Ensembl Density Feature
**
** @valrule * [EnsPDensityFeature] Ensembl Density Feature
**
** @fcategory new
******************************************************************************/




/* @func ensDensityFeatureNew **************************************************
**
** Default Ensembl Density Feature constructor.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPDensityFeatureadaptor] Ensembl Density
**                                                Feature Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Feature::new
** @param [u] feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::DensityFeature::new
** @param [u] dt [EnsPDensityType] Ensembl Density Type
** @param [r] value [float] Density value
**
** @return [EnsPDensityFeature] Ensembl Density Feature or NULL
** @@
******************************************************************************/

EnsPDensityFeature ensDensityFeatureNew(EnsPDensityFeatureadaptor adaptor,
                                        ajuint identifier,
                                        EnsPFeature feature,
                                        EnsPDensityType dt,
                                        float value)
{
    EnsPDensityFeature df = NULL;
    
    if (! feature)
        return NULL;
    
    AJNEW0(df);
    
    df->Use          = 1;
    df->Identifier   = identifier;
    df->Adaptor      = adaptor;
    df->Feature      = ensFeatureNewRef(feature);
    df->DensityType  = ensDensityTypeNewRef(dt);
    df->DensityValue = value;
    
    return df;
}




/* @func ensDensityFeatureNewObj **********************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPDensityFeature] Ensembl Density Feature
**
** @return [EnsPDensityFeature] Ensembl Density Feature or NULL
** @@
******************************************************************************/

EnsPDensityFeature ensDensityFeatureNewObj(const EnsPDensityFeature object)
{
    EnsPDensityFeature df = NULL;
    
    AJNEW0(df);
    
    df->Use          = 1;
    df->Identifier   = object->Identifier;
    df->Adaptor      = object->Adaptor;
    df->Feature      = ensFeatureNewRef(object->Feature);
    df->DensityType  = ensDensityTypeNewRef(object->DensityType);
    df->DensityValue = object->DensityValue;
    
    return df;
}




/* @func ensDensityFeatureNewRef **********************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] df [EnsPDensityFeature] Ensembl Density Feature
**
** @return [EnsPDensityFeature] Ensembl Density Feature or NULL
** @@
******************************************************************************/

EnsPDensityFeature ensDensityFeatureNewRef(EnsPDensityFeature df)
{
    if(!df)
	return NULL;
    
    df->Use++;
    
    return df;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Density Features.
**
** @fdata [EnsPDensityFeature]
** @fnote None
**
** @nam3rule Del Destroy (free) a Density Feature object
**
** @argrule * Pdf [EnsPDensityFeature*] Density Feature object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensDensityFeatureDel *************************************************
**
** Default destructor for an Ensembl Density Feature.
**
** @param [d] Pdf [EnsPDensityFeature*] Ensembl Density Feature address
**
** @return [void]
** @@
******************************************************************************/

void ensDensityFeatureDel(EnsPDensityFeature *Pdf)
{
    EnsPDensityFeature pthis = NULL;
    
    if(!Pdf)
        return;
    
    if(!*Pdf)
        return;

    pthis = *Pdf;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pdf = NULL;
	
	return;
    }
    
    ensFeatureDel(&pthis->Feature);
    
    ensDensityTypeDel(&pthis->DensityType);
    
    AJFREE(pthis);

    *Pdf = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Density Feature object.
**
** @fdata [EnsPDensityFeature]
** @fnote None
**
** @nam3rule Get Return Density Feature attribute(s)
** @nam4rule Getadaptor Return the Ensembl Density Feature Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetFeature Return the Ensembl Feature
** @nam4rule GetDensityType Return the Ensembl Density Type
** @nam4rule GetDensityValue Return the density value
**
** @argrule * df [const EnsPDensityFeature] Density Feature
**
** @valrule Adaptor [EnsPDensityFeatureadaptor] Ensembl Density Feature Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Feature [EnsPFeature] Ensembl Feature
** @valrule DensityType [EnsPDensityType] Ensembl Density Type
** @valrule DensityValue [float] Density value
**
** @fcategory use
******************************************************************************/




/* @func ensDensityFeatureGetadaptor ******************************************
**
** Get the Ensembl Density Feature Adaptor element of an
** Ensembl Density Feature.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] df [const EnsPDensityFeature] Ensembl Density Feature
**
** @return [EnsPDensityFeatureadaptor] Ensembl Density Feature Adaptor
** @@
******************************************************************************/

EnsPDensityFeatureadaptor ensDensityFeatureGetadaptor(
    const EnsPDensityFeature df)
{
    if(!df)
        return NULL;
    
    return df->Adaptor;
}




/* @func ensDensityFeatureGetIdentifier ***************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Density Feature.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] df [const EnsPDensityFeature] Ensembl Density Feature
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensDensityFeatureGetIdentifier(const EnsPDensityFeature df)
{
    if(!df)
        return 0;
    
    return df->Identifier;
}




/* @func ensDensityFeatureGetFeature ******************************************
**
** Get the Ensembl Feature element of an Ensembl Density Feature.
**
** @param [r] df [const EnsPDensityFeature] Ensembl Density Feature
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

EnsPFeature ensDensityFeatureGetFeature(const EnsPDensityFeature df)
{
    if(!df)
        return NULL;
    
    return df->Feature;
}




/* @func ensDensityFeatureGetDensityType **************************************
**
** Get the Ensembl Density Type element of an Ensembl Density Feature.
**
** @cc Bio::EnsEMBL::DensityFeature::density_type
** @param [r] df [const EnsPDensityFeature] Ensembl Density Feature
**
** @return [EnsPDensityType] Ensembl Density Type
** @@
******************************************************************************/

EnsPDensityType ensDensityFeatureGetDensityType(const EnsPDensityFeature df)
{
    if(!df)
        return NULL;
    
    return df->DensityType;
}




/* @func ensDensityFeatureGetDensityValue *************************************
**
** Get the density value element of an Ensembl Density Feature.
**
** @cc Bio::EnsEMBL::DensityFeature::density_value
** @param [r] df [const EnsPDensityFeature] Ensembl Density Feature
**
** @return [float] Density value
** @@
******************************************************************************/

float ensDensityFeatureGetDensityValue(const EnsPDensityFeature df)
{
    if(!df)
        return 0;
    
    return df->DensityValue;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Density Feature object.
**
** @fdata [EnsPDensityFeature]
** @fnote None
**
** @nam3rule Set Set one element of a Density Feature
** @nam4rule Setadaptor Set the Ensembl Density Feature Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetFeature Set the Ensembl Feature
** @nam4rule SetDensityType Set the Ensembl Density Type
** @nam4rule SetDensityValue Set the density value
**
** @argrule * df [EnsPDensityFeature] Ensembl Density Feature object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensDensityFeatureSetadaptor ******************************************
**
** Set the Ensembl Density Feature Adaptor element of an
** Ensembl Density Feature.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] df [EnsPDensityFeature] Ensembl Density Feature
** @param [r] adaptor [EnsPDensityFeatureadaptor] Ensembl Density
**                                                Feature Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityFeatureSetadaptor(EnsPDensityFeature df,
                                   EnsPDensityFeatureadaptor adaptor)
{
    if(!df)
        return ajFalse;
    
    df->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensDensityFeatureSetIdentifier ***************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Density Feature.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] df [EnsPDensityFeature] Ensembl Density Feature
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityFeatureSetIdentifier(EnsPDensityFeature df, ajuint identifier)
{
    if(!df)
        return ajFalse;
    
    df->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensDensityFeatureSetFeature *******************************************
**
** Set the Ensembl Feature element of an Ensembl Density Feature.
**
** @param [u] df [EnsPDensityFeature] Ensembl Density Feature
** @param [u] feature [EnsPFeature] Ensembl Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityFeatureSetFeature(EnsPDensityFeature df, EnsPFeature feature)
{
    if(!df)
        return ajFalse;
    
    ensFeatureDel(&(df->Feature));
    
    df->Feature = ensFeatureNewRef(feature);
    
    return ajTrue;
}




/* @func ensDensityFeatureSetDensityType **************************************
**
** Set the Ensembl Density Type element of an Ensembl Density Feature.
**
** @cc Bio::EnsEMBL::DensityFeature::density_type
** @param [u] df [EnsPDensityFeature] Ensembl Density Feature
** @param [u] dt [EnsPDensityType] Ensembl Density Type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityFeatureSetDensityType(EnsPDensityFeature df,
                                       EnsPDensityType dt)
{
    if(!df)
        return ajFalse;
    
    ensDensityTypeDel(&(df->DensityType));
    
    df->DensityType = ensDensityTypeNewRef(dt);
    
    return ajTrue;
}




/* @func ensDensityFeatureSetDensityValue *************************************
**
** Set the density value element of an Ensembl Density Feature.
**
** @cc Bio::EnsEMBL::DensityFeature::density_value
** @param [u] df [EnsPDensityFeature] Ensembl Density Feature
** @param [u] value [float] Density value
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityFeatureSetDensityValue(EnsPDensityFeature df, float value)
{
    if(!df)
        return ajFalse;
    
    df->DensityValue = value;
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Density Feature object.
**
** @fdata [EnsPDensityFeature]
** @nam3rule Trace Report Ensembl Density Feature elements to debug file
**
** @argrule Trace df [const EnsPDensityFeature] Ensembl Density Feature
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensDensityFeatureTrace ***********************************************
**
** Trace an Ensembl Density Feature.
**
** @param [r] df [const EnsPDensityFeature] Ensembl Density Feature
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityFeatureTrace(const EnsPDensityFeature df, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!df)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensDensityFeatureTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Feature %p\n"
	    "%S  DensityType %p\n"
	    "%S  DensityValue %f\n",
	    indent, df,
	    indent, df->Use,
	    indent, df->Identifier,
	    indent, df->Adaptor,
	    indent, df->Feature,
	    indent, df->DensityType,
	    indent, df->DensityValue);
    
    ensFeatureTrace(df->Feature, level + 1);
    
    ensDensityTypeTrace(df->DensityType, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensDensityFeatureGetMemSize ******************************************
**
** Get the memory size in bytes of an Ensembl Density Feature.
**
** @param [r] df [const EnsPDensityFeature] Ensembl Density Feature
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensDensityFeatureGetMemSize(const EnsPDensityFeature df)
{
    ajuint size = 0;
    
    if(!df)
	return 0;
    
    size += (ajuint) sizeof (EnsODensityFeature);
    
    size += ensFeatureGetMemSize(df->Feature);
    
    size += ensDensityTypeGetMemSize(df->DensityType);
    
    return size;
}




/* @datasection [EnsPDensityFeatureadaptor] Density Feature Adaptor ***********
**
** Functions for manipulating Ensembl Density Feature Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::DensityFeatureadaptor CVS Revision: 1.22
**
** @nam2rule DensityFeatureadaptor
**
******************************************************************************/

static const char *densityFeatureadaptorTables[] =
{
    "density_feature",
    NULL
};




static const char *densityFeatureadaptorColumns[] =
{
    "density_feature.density_feature_id",
    "density_feature.seq_region_id",
    "density_feature.seq_region_start",
    "density_feature.seq_region_end",
    "density_feature.density_type_id",
    "density_feature.density_value",
    NULL
};

static EnsOBaseadaptorLeftJoin densityFeatureadaptorLeftJoin[] =
{
    {NULL, NULL}
};

static const char *densityFeatureadaptorDefaultCondition = NULL;

static const char *densityFeatureadaptorFinalCondition = NULL;




/* @funcstatic densityFeatureadaptorFetchAllBySQL *****************************
**
** Fetch all Ensembl Density Feature objects via an SQL statement.
**
** @cc Bio::EnsEMBL::DBSQL::DensityFeatureadaptor::_objs_from_sth
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [r] mapper [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] dfs [AjPList] AJAX List of Ensembl Density Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool densityFeatureadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                 const AjPStr statement,
                                                 EnsPAssemblyMapper mapper,
                                                 EnsPSlice slice,
                                                 AjPList dfs)
{
    float value = 0;
    
    ajuint identifier = 0;
    ajuint dtid       = 0;
    
    ajuint srid    = 0;
    ajuint srstart = 0;
    ajuint srend   = 0;
    
    ajint slstart  = 0;
    ajint slend    = 0;
    ajint slstrand = 0;
    ajint sllength = 0;
    
    AjPList mrs = NULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    EnsPAssemblyMapper am         = NULL;
    EnsPAssemblyMapperadaptor ama = NULL;
    
    EnsPCoordSystemadaptor csa = NULL;
    
    EnsPFeature feature = NULL;
    
    EnsPDensityFeature df         = NULL;
    EnsPDensityFeatureadaptor dfa = NULL;
    
    EnsPDensityType dt         = NULL;
    EnsPDensityTypeadaptor dta = NULL;
    
    EnsPMapperResult mr = NULL;
    
    EnsPSlice srslice   = NULL;
    EnsPSliceadaptor sa = NULL;
    
    ajDebug("densityFeatureadaptorFetchAllBySQL\n"
	    "  dba %p\n"
	    "  statement %p\n"
	    "  mapper %p\n"
	    "  slice %p\n"
	    "  dfs %p\n",
	    dba,
	    statement,
	    mapper,
	    slice,
	    dfs);
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    if(!dfs)
	return ajFalse;
    
    csa = ensRegistryGetCoordSystemadaptor(dba);
    
    dfa = ensRegistryGetDensityFeatureadaptor(dba);
    
    dta = ensRegistryGetDensityTypeadaptor(dba);
    
    sa = ensRegistryGetSliceadaptor(dba);
    
    if(slice)
	ama = ensRegistryGetAssemblyMapperadaptor(dba);
    
    mrs = ajListNew();
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier = 0;
	srid       = 0;
	srstart    = 0;
	srend      = 0;
	dtid       = 0;
	value      = 0;
	
	sqlr = ajSqlrowiterGet(sqli);
	
	ajSqlcolumnToUint(sqlr, &identifier);
	ajSqlcolumnToUint(sqlr, &srid);
	ajSqlcolumnToUint(sqlr, &srstart);
	ajSqlcolumnToUint(sqlr, &srend);
	ajSqlcolumnToUint(sqlr, &dtid);
        ajSqlcolumnToFloat(sqlr, &value);
	
	/* Need to get the internal Ensembl Sequence Region identifier. */
	
	srid = ensCoordSystemadaptorGetInternalSeqRegionIdentifier(csa, srid);
	
	/*
	 ** Since the Ensembl SQL schema defines Sequence Region start and end
	 ** coordinates as unsigned integers for all Features, the range needs
	 ** checking.
	 */
	
	if(srstart <= INT_MAX)
	    slstart = (ajint) srstart;
	else
	    ajFatal("densityFeatureadaptorFetchAllBySQL got a "
		    "Sequence Region start coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srstart, INT_MAX);
	
	if(srend <= INT_MAX)
	    slend = (ajint) srend;
	else
	    ajFatal("densityFeatureadaptorFetchAllBySQL got a "
		    "Sequence Region end coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srend, INT_MAX);
	
	slstrand = 1;
	
	/* Fetch a Slice spanning the entire Sequence Region. */
	
	ensSliceadaptorFetchBySeqRegionIdentifier(sa, srid, 0, 0, 0, &srslice);
	
	/*
	** Obtain an Ensembl Assembly Mapper if none was defined, but a
	** destination Slice was.
	*/
	
	if(mapper)
	    am = ensAssemblyMapperNewRef(mapper);
	
	if((! mapper) &&
	   slice &&
	    (! ensCoordSystemMatch(ensSliceGetCoordSystem(slice),
				   ensSliceGetCoordSystem(srslice))))
	    am =
		ensAssemblyMapperadaptorFetchByCoordSystems(
                    ama,
		    ensSliceGetCoordSystem(slice),
		    ensSliceGetCoordSystem(srslice));
	
	/*
	** Remap the Feature coordinates to another Ensembl Coordinate System
	** if an Ensembl Assembly Mapper was provided.
	*/
	
	if(am)
	{
	    ensAssemblyMapperFastMap(am,
				     ensSliceGetSeqRegion(srslice),
				     slstart,
				     slend,
				     slstrand,
				     mrs);
	    
	    /*
	    ** The ensAssemblyMapperFastMap function returns at best one
	    ** Ensembl Mapper Result.
	    */
	    
	    ajListPop(mrs, (void **) &mr);
	    
	    /*
	    ** Skip Features that map to gaps or
	    ** Coordinate System boundaries.
	    */
	    
	    if (ensMapperResultGetType(mr) != ensEMapperResultCoordinate)
	    {
		/* Load the next Feature but destroy first! */
		
		ensSliceDel(&srslice);
		
		ensAssemblyMapperDel(&am);
		
		ensMapperResultDel(&mr);
		
		continue;
	    }
	    
	    srid = ensMapperResultGetObjectIdentifier(mr);
	    
	    slstart = ensMapperResultGetStart(mr);
	    
	    slend = ensMapperResultGetEnd(mr);
	    
	    slstrand = ensMapperResultGetStrand(mr);
	    
	    /*
	    ** Delete the Sequence Region Slice and fetch a Slice in the
	    ** Coordinate System we just mapped to.
	    */
	    
	    ensSliceDel(&srslice);
	    
	    ensSliceadaptorFetchBySeqRegionIdentifier(sa,
						      srid,
						      0,
						      0,
						      0,
						      &srslice);
	    
	    ensMapperResultDel(&mr);
	}
	
	/*
	** Convert Sequence Region Slice coordinates to destination Slice
	** coordinates, if a destination Slice has been provided.
	*/
	
	if(slice)
	{
	    /* Check that the length of the Slice is within range. */
	    
	    if(ensSliceGetLength(slice) <= INT_MAX)
		sllength = (ajint) ensSliceGetLength(slice);
	    else
		ajFatal("densityFeatureadaptorFetchAllBySQL got a Slice, "
			"which length (%u) exceeds the "
			"maximum integer limit (%d).",
			ensSliceGetLength(slice), INT_MAX);
	    
	    /*
	    ** Nothing needs to be done if the destination Slice starts at 1
	    ** and is on the forward strand.
	    */
	    
	    if((ensSliceGetStart(slice) != 1) ||
		(ensSliceGetStrand(slice) < 0))
	    {
		if(ensSliceGetStrand(slice) >= 0)
		{
		    slstart = slstart - ensSliceGetStart(slice) + 1;
		    
		    slend = slend - ensSliceGetStart(slice) + 1;
		}
		else
		{
		    slend = ensSliceGetEnd(slice) - slstart + 1;
		    
		    slstart = ensSliceGetEnd(slice) - slend + 1;
		    
		    slstrand *= -1;
		}
	    }
	    
	    /*
	     ** Throw away Features off the end of the requested Slice or on
	     ** any other than the requested Slice.
	     */
	    
	    if((slend < 1) ||
		(slstart > sllength) ||
		(srid != ensSliceGetSeqRegionIdentifier(slice)))
	    {
		/* Load the next Feature but destroy first! */
		
		ensSliceDel(&srslice);
		
		ensAssemblyMapperDel(&am);
		
		continue;
	    }
	    
	    /* Delete the Sequence Region Slice and set the requested Slice. */
	    
	    ensSliceDel(&srslice);
	    
	    srslice = ensSliceNewRef(slice);
	}
	
	ensDensityTypeadaptorFetchByIdentifier(dta, dtid, &dt);
	
	/* Finally, create a new Ensembl Density Feature. */
	
	feature = ensFeatureNewS((EnsPAnalysis) NULL,
				 srslice,
				 slstart,
				 slend,
				 1);
	
	df = ensDensityFeatureNew(dfa, identifier, feature, dt, value);
	
	ajListPushAppend(dfs, (void *) df);
	
	ensDensityTypeDel(&dt);
	
	ensFeatureDel(&feature);
	
	ensAssemblyMapperDel(&am);
	
	ensSliceDel(&srslice);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    ajListFree(&mrs);
    
    return ajTrue;
}




/* @funcstatic densityFeatureadaptorCacheReference ****************************
**
** Wrapper function to reference an Ensembl Density Feature
** from an Ensembl Cache.
**
** @param [r] value [void *] Ensembl Density Feature
**
** @return [void *] Ensembl Density Feature or NULL
** @@
******************************************************************************/

static void *densityFeatureadaptorCacheReference(void *value)
{
    if(!value)
	return NULL;
    
    return (void *) ensDensityFeatureNewRef((EnsPDensityFeature) value);
}




/* @funcstatic densityFeatureadaptorCacheDelete *******************************
**
** Wrapper function to delete an Ensembl Density Feature
** from an Ensembl Cache.
**
** @param [r] value [void**] Ensembl Density Feature address
**
** @return [void]
** @@
******************************************************************************/

static void densityFeatureadaptorCacheDelete(void **value)
{
    if(!value)
	return;
    
    ensDensityFeatureDel((EnsPDensityFeature *) value);
    
    return;
}




/* @funcstatic densityFeatureadaptorCacheSize *********************************
**
** Wrapper function to determine the memory size of an Ensembl Density Feature
** from an Ensembl Cache.
**
** @param [r] value [const void*] Ensembl Density Feature
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

static ajuint densityFeatureadaptorCacheSize(const void *value)
{
    if(!value)
	return 0;
    
    return ensDensityFeatureGetMemSize((const EnsPDensityFeature) value);
}




/* @funcstatic densityFeatureadaptorGetFeature ********************************
**
** Wrapper function to get the Ensembl Feature of an Ensembl Density Feature
** from an Ensembl Feature Adaptor.
**
** @param [r] value [const void*] Ensembl Density Feature
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

static EnsPFeature densityFeatureadaptorGetFeature(const void *value)
{
    if(!value)
	return NULL;
    
    return ensDensityFeatureGetFeature((const EnsPDensityFeature) value);
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Density Feature Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Density Feature Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPDensityFeatureadaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPDensityFeatureadaptor] Ensembl Density
**                                                 Feature Adaptor
** @argrule Ref object [EnsPDensityFeatureadaptor] Ensembl Density
**                                                 Feature Adaptor
**
** @valrule * [EnsPDensityFeatureadaptor] Ensembl Density Feature Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensDensityFeatureadaptorNew ******************************************
**
** Default Ensembl Density Feature Adaptor constructor.
**
** @cc Bio::EnsEMBL::DBSQL::DensityFeatureadaptor::new
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDensityFeatureadaptor] Ensembl Density Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPDensityFeatureadaptor ensDensityFeatureadaptorNew(EnsPDatabaseadaptor dba)
{
    EnsPDensityFeatureadaptor dfa = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(dfa);
    
    dfa->Adaptor =
	ensFeatureadaptorNew(dba,
			     densityFeatureadaptorTables,
			     densityFeatureadaptorColumns,
			     densityFeatureadaptorLeftJoin,
			     densityFeatureadaptorDefaultCondition,
			     densityFeatureadaptorFinalCondition,
			     densityFeatureadaptorFetchAllBySQL,
			     (void* (*)(const void* key)) NULL, /* Fread */
			     densityFeatureadaptorCacheReference,
			     (AjBool (*)(const void* value)) NULL, /* Fwrite */
			     densityFeatureadaptorCacheDelete,
			     densityFeatureadaptorCacheSize,
			     densityFeatureadaptorGetFeature,
			     "Density Feature");
    
    return dfa;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Density Feature Adaptor.
**
** @fdata [EnsPDensityFeatureadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Density Feature Adaptor object
**
** @argrule * Padaptor [EnsPDensityFeatureadaptor*] Ensembl Density Feature
**                                                  Adaptor object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensDensityFeatureadaptorDel ******************************************
**
** Default destructor for an Ensembl Density Feature Adaptor.
**
** @param [d] Padaptor [EnsPDensityFeatureadaptor*] Ensembl Density Feature
**                                                  Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensDensityFeatureadaptorDel(EnsPDensityFeatureadaptor *Padaptor)
{
    if(!Padaptor)
	return;
    
    if(!*Padaptor)
	return;
    
    ensFeatureadaptorDel(&((*Padaptor)->Adaptor));
    
    AJFREE(*Padaptor);
    
    return;
}




/* @funcstatic densityTypeRatioCompareRatio ***********************************
**
** Comparison function to sort Density Ratios by their ratio
** in ascending order.
**
** @param [r] P1 [const void*] Density Ratio address 1
** @param [r] P2 [const void*] Density Ratio address 2
**
** @return [int] The comparison function returns an integer less than,
**               equal to, or greater than zero if the first argument is
**               considered to be respectively less than, equal to, or
**               greater than the second.
** @@
******************************************************************************/

static int densityTypeRatioCompareRatio(const void* P1, const void* P2)
{
    int value = 0;
    
    const DensityPTypeRatio dtr1 = NULL;
    const DensityPTypeRatio dtr2 = NULL;
    
    dtr1 = *(DensityPTypeRatio const *) P1;
    
    dtr2 = *(DensityPTypeRatio const *) P2;
    
    if(!dtr1)
    {
	ajDebug("densityTypeRatioCompareRatio got empty dtr1.\n");
	
	return 0;
    }
    
    if(!dtr2)
    {
	ajDebug("densityTypeRatioCompareRatio got empty dtr2.\n");
	
	return 0;
    }
    
    /*
     ajDebug("densityTypeRatioCompareRatio\n"
	     "  dtr1 %p\n"
	     "  dtr2 %p\n",
	     dtr1,
	     dtr2);
     */
    
    if(dtr1->Ratio < dtr2->Ratio)
        value = -1;
    
    if(dtr1->Ratio == dtr2->Ratio)
        value = 0;
    
    if(dtr1->Ratio > dtr2->Ratio)
        value = +1;
    
    return value;
}




/* @funcstatic densityFeatureCompareStart *************************************
**
** Comparison function to sort Density Features by their Feature start
** in ascending order.
**
** @param [r] P1 [const void*] Density Feature address 1
** @param [r] P2 [const void*] Density Feature address 2
**
** @return [int] The comparison function returns an integer less than,
**               equal to, or greater than zero if the first argument is
**               considered to be respectively less than, equal to, or
**               greater than the second.
** @@
******************************************************************************/

static int densityFeatureCompareStart(const void* P1, const void* P2)
{
    int value = 0;
    
    const EnsPDensityFeature df1 = NULL;
    const EnsPDensityFeature df2 = NULL;
    
    EnsPFeature feature1 = NULL;
    EnsPFeature feature2 = NULL;
    
    df1 = *(EnsPDensityFeature const *) P1;
    
    df2 = *(EnsPDensityFeature const *) P2;
    
    if(!df1)
    {
	ajDebug("densityFeatureCompareStart got empty df1.\n");
	
	return 0;
    }
    
    if(!df2)
    {
	ajDebug("densityFeatureCompareStart got empty df2.\n");
	
	return 0;
    }
    
    /*
     ajDebug("densityFeatureCompareStart\n"
	     "  df1 %p\n"
	     "  df2 %p\n",
	     df1,
	     df2);
     */
    
    feature1 = ensDensityFeatureGetFeature(df1);
    
    feature2 = ensDensityFeatureGetFeature(df2);
    
    /*
    ** FIXME: The Perl API compares the start of Feature 1 with the
    ** end of Feature 2.
    */
    
    if(feature1->Start < feature2->End)
        value = -1;
    
    if(feature1->Start == feature2->End)
        value = 0;
    
    if(feature1->Start > feature2->End)
        value = +1;
    
    return value;
}




/* @func ensDensityFeatureadaptorFetchAllBySlice ******************************
**
** Fetch all Ensembl Density Features on an Ensembl Slice.
**
** @cc Bio::EnsEMBL::DBSQL::BaseFeatureadaptor::fetch_all_by_Slice
** @param [r] adaptor [EnsPDensityFeatureadaptor] Ensembl Density
**                                                Feature Adaptor
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [r] anname [AjPStr] Ensembl Analysis name
** @param [r] blocks [ajuint] The number of
features that are desired. The ratio between the size of these
features and the size of the features in the database will be
used to determine which database features will be used.
** @param [r] interpolate [AjBool] A flag indicating
whether the features in the database should be interpolated to
fit them to the requested number of features.  If true the
features will be interpolated to provide $num_blocks features.
This will not guarantee that exactly $num_blocks features are
returned due to rounding etc. but it will be close.
** @param [r] maxratio [float] The maximum ratio between the size of the
requested features (as determined by blocks) and the actual
size of the features in the database.  If this value is exceeded
then an empty list will be returned.  This can be used to
prevent excessive interpolation of the database values.
** @param [r] dfs [AjPList] AJAX List of Ensembl Density Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDensityFeatureadaptorFetchAllBySlice(
    EnsPDensityFeatureadaptor adaptor,
    EnsPSlice slice,
    const AjPStr anname,
    ajuint blocks,
    AjBool interpolate,
    float maxratio,
    AjPList dfs)
{
    double portion = 0.0;
    double ratio   = 0.0;
    float value    = 0.0;
    
    register ajuint i = 0;
    
    /* Block start and end */
    ajint bstart   = 0;
    ajint bend     = 0;
    ajuint blength = 0;
    
    /* Feature start and end */
    ajint fstart = 0;
    ajint fend   = 0;
    
    ajint bsize = 0;
    
    AjPList dts       = NULL;
    AjPList dlvs      = NULL;
    AjPList dtrs      = NULL;
    AjPList dfeatures = NULL;
    
    AjPStr constraint = NULL;
    AjPStr names      = NULL;
    
    EnsPAnalysis analysis = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    EnsPDensityFeature df    = NULL;
    EnsPDensityFeature newdf = NULL;
    
    EnsPDensityType dt         = NULL;
    EnsPDensityType newdt      = NULL;
    EnsPDensityTypeadaptor dta = NULL;
    
    EnsPFeature feature    = NULL;
    EnsPFeature newfeature = NULL;
    
    DensityPLengthValue dlv = NULL;
    
    DensityPTypeRatio dtr = NULL;
    
    ajDebug("ensDensityFeatureadaptorFetchAllBySlice\n"
	    "  adaptor %p\n"
	    "  slice %p\n"
	    "  anname '%S'\n"
	    "  blocks %u\n"
	    "  interpolate %B\n"
	    "  maxratio %f\n"
	    "  dfs %p\n",
	    adaptor,
	    slice,
	    anname,
	    blocks,
	    interpolate,
	    maxratio,
	    dfs);
    
    if(!adaptor)
	return ajFalse;
    
    if(!slice)
	return ajFalse;
    
    if(!dfs)
	return ajFalse;
    
    if(!blocks)
	blocks = 50;
    
    /*
     ** Get all of the Density Types and choose the one with the
     ** block size closest to our desired block size.
     */
    
    dba = ensFeatureadaptorGetDatabaseadaptor(adaptor->Adaptor);
    
    dta = ensRegistryGetDensityTypeadaptor(dba);
    
    dts = ajListNew();
    
    ensDensityTypeadaptorFetchAllByAnalysisName(dta, anname, dts);
    
    if(!ajListGetLength(dts))
    {
	ensDensityTypeadaptorFetchAll(dta, dts);
	
	names = ajStrNew();
	
	while(ajListPop(dts, (void **) &dt))
	{
	    analysis = ensDensityTypeGetAnalysis(dt);
	    
	    names = ajFmtPrintAppS(&names, "'%S' ",
				   ensAnalysisGetName(analysis));
	    
	    ensDensityTypeDel(&dt);
	}
	
	ajWarn("ensDensityFeatureadaptorFetchAllBySlice got an invalid "
	       "Ensembl Analysis name '%S' only the following are allowed %S.",
	       anname,
	       names);
	
	ajStrDel(&names);
	
	ajListFree(&dts);
	
	return ajFalse;
    }
    
    /*
     ** The window size is the length of the Ensembl Slice
     ** divided by the number of requested blocks.
     */
    
    bsize = (ceil( ensSliceGetLength(slice) / (double) blocks));
    
    dtrs = ajListNew();
    
    while(ajListPop(dts, (void *) &dt))
    {
	if(ensDensityTypeGetBlockSize(dt) > 0)
	    ratio = (double) bsize /
		(double) (ajint) ensDensityTypeGetBlockSize(dt);
	else
	{
	    /*
	    ** This is only valid if the requested Sequence Region is the one
	    ** the Features are stored on. Please use sensibly or find a
	    ** better implementation.
	    */

	    ratio = (double) bsize /
                (ensSliceGetSeqRegionLength(slice) /
                 (double) (ajint) ensDensityTypeGetRegionFeatures(dt));
	}
	
        /*
	** We prefer to use a block size that is smaller than the
	** required one, which gives better results on interpolation.
	** Give larger bits a disadvantage and make them comparable.
	*/
	
	if(ratio < 1)
	    ratio = 5 / ratio;
	
	AJNEW0(dtr);
	
	dtr->DensityType = dt;
	
	dtr->Ratio = (float) ratio;
	
	ajListPushAppend(dtrs, (void *) dtr);
    }
    
    ajListFree(&dts);
    
    /*
    ** Sort the AJAX List of Ensembl Density Types by their ratio
    ** and get the best one.
    */
    
    ajListSort(dtrs, densityTypeRatioCompareRatio);
    
    ajListPeekFirst(dtrs, (void **) &dtr);
    
    if(dtr)
	ajDebug("ensDensityFeatureadaptorFetchAllBySlice got ratio %f "
                "and maxratio %f.\n", dtr->Ratio, maxratio);
    else
	ajDebug("ensDensityFeatureadaptorFetchAllBySlice got no ratio.\n");
    
    /*
    ** The ratio was not good enough, or the Ensembl Analysis name was not
    ** in the 'density_type' table, return an empty list.
    */
    
    if((! dtr) || (dtr->Ratio > maxratio))
    {
	while(ajListPop(dtrs, (void **) &dtr))
	{
	    ensDensityTypeDel(&(dtr->DensityType));
	    
	    AJFREE(dtr);
	}
	
	ajListFree(&dtrs);
	
	return ajTrue;
    }
    
    constraint = ajFmtStr("density_feature.density_type_id = %u",
			  ensDensityTypeGetIdentifier(dtr->DensityType));
    
    ensFeatureadaptorFetchAllBySliceConstraint(adaptor->Adaptor,
					       slice,
					       constraint,
					       (const AjPStr) NULL,
					       dfs);
    
    ajStrDel(&constraint);
    
    if(!interpolate)
    {
	while(ajListPop(dtrs, (void **) &dtr))
	{
	    ensDensityTypeDel(&(dtr->DensityType));
	    
	    AJFREE(dtr);
	}
	
	ajListFree(&dtrs);
	
	return ajTrue;
    }
    
    /*
    ** Interpolate the Density Features into new Density Features of a
    ** different size, but move them to a new AJAX List first. Sort the
    ** Ensembl Density Features by ascening start position.
    */
    
    dfeatures = ajListNew();
    
    while(ajListPop(dfs, (void **) &df))
	ajListPushAppend(dfeatures, (void *) df);
    
    ajListSort(dfeatures, densityFeatureCompareStart);
    
    /* Resize the Features that were returned. */
    
    bstart = 1;
    
    bend = bstart + bsize - 1;
    
    /*
    ** Create a new Ensembl Density Type for the interpolated
    ** Ensembl Density Features that are not stored in the database.
    */
    
    newdt = ensDensityTypeNewObj(dtr->DensityType);
    
    ensDensityTypeSetIdentifier(newdt, 0);
    
    ensDensityTypeSetBlockSize(newdt, bsize);
    
    /* FIXME: For debugging only. */
    ensDensityTypeTrace(dtr->DensityType, 0);
    ensDensityTypeTrace(newdt, 0);
    
    dlvs = ajListNew();
    
    while(bstart < (ajint) ensSliceGetLength(slice))
    {
	value = 0.0;
	
	ajDebug("ensDensityFeatureadaptorFetchAllBySlice "
		"bstart %d bend %d value %f\n",
		bstart, bend, value);
	
	/*
	** Construct a new Ensembl Density Feature using all the old
	** Ensembl Density Features that overlapped.
	*/
	
	while(ajListPeekNumber(dfeatures, i, (void **) &df) &&
	       (feature = ensDensityFeatureGetFeature(df)) &&
	       (ensFeatureGetStart(feature) < bend))
	{
	    /* FIXME: For debugging only.
	    ensDensityFeatureTrace(df, 0);
	    */
	    
	    /*
	    ** What portion of this Feature is used
	    ** to construct the new block?
	    */
	    
	    /* Constrain the Feature start to no less than the block start. */
	    
	    fstart = (ensFeatureGetStart(feature) < bstart)
	    ? bstart : ensFeatureGetStart(feature);
	    
	    /* Constrain the Feature end to no more than the block end. */
	    
	    fend = (ensFeatureGetEnd(feature) > bend)
		? bend : ensFeatureGetEnd(feature);
	    
	    /* Constrain the Feature end to no more than the Slice length. */
	    
	    fend = (fend > (ajint) ensSliceGetLength(slice))
		? (ajint) ensSliceGetLength(slice) : fend;
	    
	    ajDebug("ensDensityFeatureadaptorFetchAllBySlice "
		    "bstart %d bend %d fstart %d fend %d id %u value %f\n",
		    bstart, bend, fstart, fend,
		    ensDensityFeatureGetIdentifier(df),
		    ensDensityFeatureGetDensityValue(df));
	    
	    switch(ensDensityTypeGetValueType(newdt))
	    {
		case ensEDensityTypeValueTypeSum:
		    
		    portion = (fend - fstart + 1) /
		    ensFeatureGetLength(feature);
		    
		    /*
		    ** Take a percentage of density value, depending on
		    ** how much of the Ensembl Density Feature was overlapped.
		    */
		    
		    value += (float) (portion *
                                      ensDensityFeatureGetDensityValue(df));
		    
		    break;
		    
		case ensEDensityTypeValueTypeRatio:
		    
		    /*
		    ** Maintain a running total of the length used from each
		    ** Ensembl Density Feature and its value.
		    */
		    
		    AJNEW0(dlv);
		    
		    dlv->Length = fend - fstart + 1;
		    
		    dlv->Value = ensDensityFeatureGetDensityValue(df);
		    
		    ajListPushAppend(dlvs, (void *) dlv);
		    
		    break;
		    
		default:
		    
		    ajWarn("ensDensityFeatureadaptorFetchAllBySlice got an "
			   "Ensembl Density Type with an unknown type (%d).",
			   ensDensityTypeGetValueType(newdt));
	    }
	    
	    /*
	    ** Do not look at the next Density Feature,
	    ** if only a part of this one has been used.
	    */
	    
	    if(fend < ensFeatureGetEnd(feature))
		break;
	    
	    i++;
	}
	
	if(ensDensityTypeGetValueType(newdt) == ensEDensityTypeValueTypeRatio)
	{
	    /*
	    ** Take a weighted average of all the density values
	    ** of the Features used to construct this one.
	    */
	    
	    blength = bend - bstart + 1;
	    
	    while(ajListPop(dlvs, (void **) &dlv))
	    {
		if(blength > 0)
		    value += dlv->Value * (float) dlv->Length /
			(float) blength;
		
		ajDebug("ensDensityFeatureadaptorFetchAllBySlice "
			"bstart %d bend %d blength %u "
			"DLV Length %u DLV Value %f "
			"value %f \n",
			bstart, bend, blength,
			dlv->Length, dlv->Value,
			value);
		
		AJFREE(dlv);
	    }
	}
	
	/*
	 ** Interpolated features are not stored in the database so they do not
	 ** need an identifier or adaptor.
	 */
	
	newfeature = ensFeatureNewObj(feature);
	
	ensFeatureMove(newfeature, bstart, bend, 0);
	
	newdf = ensDensityFeatureNew((EnsPDensityFeatureadaptor) NULL,
				     0,
				     newfeature,
				     newdt,
				     value);
	
	ajListPushAppend(dfs, (void *) newdf);
	
	ensFeatureDel(&newfeature);
	
	/* Clear the AJAX List of remaining density length values. */
	
	while(ajListPop(dlvs, (void **) &dlv))
	    AJFREE(dlv);
	
	/* Advance one block. */
	
	bstart = bend + 1;
	
	bend  += bsize;
    }
    
    ajListFree(&dlvs);
    
    ensDensityTypeDel(&newdt);
    
    /* Clear the AJAX List of intermediary Ensembl Density Features. */
    
    while(ajListPop(dfeatures, (void **) &df))
	ensDensityFeatureDel(&df);
    
    ajListFree(&dfeatures);
    
    /* Clear the AJAX List of density type ratios. */
    
    while(ajListPop(dtrs, (void **) &dtr))
    {
	ensDensityTypeDel(&(dtr->DensityType));
	
	AJFREE(dtr);
    }
    
    ajListFree(&dtrs);
    
    return ajTrue;
}
