/********************************************************************
** @source AJAX structure functions
**
** @author Copyright (C) 2002 Hugh Morgan (hmorgan2@hgmp.mrc.ac.uk)
** @author Copyright (C) 2002 Alan Bleasby
** @version 1.0 
** @modified Feb 27 HLM First version
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
********************************************************************/

#ifdef GROUT

#include "ajax.h"
#include <math.h>

#define ONE_METER_FONT 1.784
#define RESIDUE_SQUARE_SIZE 1.7
#define QUOTE_CHARACTERS "\""


static AjPXmlNode	xml_MakeNewNode(AjPXmlFile file,AjPStr name,
					AjPXmlNode parent);
static AjPXmlNode 	xml_MakeNewShapeNode(AjPXmlFile file,
					     AjPXmlNode parentNode,
					     AjPStr nameReqd);
static AjPXmlNode 	xml_GetNodeTypeMakeIfNot(AjPXmlFile file,
						 AjPStr nameReqd);
static AjPXmlFile 	xml_CreateNewOutputFile();
static AjPXmlNode 	xml_GetCurrentGraphic(AjPXmlFile file);
static AjPXmlNode 	xml_GetCurrentScene(AjPXmlFile file);
static AjPXmlNode 	xml_SetNode(GdomeNode *node);
static GdomeNode* 	xml_GetNode(AjPXmlNode node);
static GdomeElement* 	xml_GetNodeElement(AjPXmlNode node);
static AjPXmlNode 	xml_NewNode(AjPXmlFile file, AjPStr name,
				    AjPXmlNode parent);
static AjPXmlNode 	xml_GetParent(AjPXmlNode node);

static void 	xml_AddGroutOption(AjPXmlFile file, AjPStr name, AjPStr value);
static void 	xml_AddCylinder(AjPXmlFile file, double xCentre,
				double yCentre, double angle, double height,
				double width);
static AjPStr 	xml_StrFromBool(AjBool boole);
static AjBool 	xml_StrFromDouble(AjPStr *result, double val);
static AjBool 	xml_AngleIsInSecondHalfOfCircle(double angle);
static AjBool 	xml_PressentGraphicTypeIs(AjPXmlFile file, AjPStr name);
static AjPStr 	xml_GetAttribute(AjPXmlNode node, AjPStr atName);
static void 	xml_SetAttribute(AjPXmlNode node, AjPStr atName,
				 AjPStr atValue);
static AjPStr 	xml_GetIndex(AjPXmlNode node);
static void 	xml_SetIndex(AjPXmlNode node, AjPStr index);
static AjPStr 	xml_GetPoints(AjPXmlNode node);
static AjBool 	xml_SetPoints(AjPXmlNode node, AjPStr points);

static AjPStr 	xml_PresentColourAsString(AjPXmlFile file);
static AjBool 	xml_FileNeedsProtoDeclare(AjPXmlFile file, AjPStr protoName);
static AjBool 	xml_IsShapeThisColour(AjPXmlFile file, AjPXmlNode shape);

static void 	xml_AddArc(AjPXmlFile file, double xCentre, double yCentre,
			   double startAngle, double endAngle, double radius);

static void 	xml_AddACoord(double x, double y, AjBool joined, AjPStr* coord,
			      AjPStr* index);
static int 	xml_GetLastInt(AjPStr str);
static double 	xml_GetetLastDouble(AjPStr str);
static double 	xml_GetDoubleNo(AjPStr str, int index);
static void 	xml_AddQuotes(AjPStr *title);

static void 	xml_AddGraphProto(AjPXmlFile file);
static void 	xml_AddDNAPlotProto(AjPXmlFile file);
static void 	xml_AddCommonBit(AjPXmlFile file);
static AjBool 	xml_WriteFile(AjPXmlFile file, AjPStr filename);
static AjBool 	xml_WriteStdout(AjPXmlFile file);

static void 	xml_SetCurrentGraphic(AjPXmlFile file, AjPXmlNode node);
static void 	xml_SetCurrentScene(AjPXmlFile file, AjPXmlNode node);
static void 	xml_ClearFile(AjPXmlFile file);
static void 	xml_UnrefNode(AjPXmlNode node);





/* @func ajXmlNodeNew  ***********************************************
**
** Create a new node and allocate the memory
**
** @return [thys] the new node
** @@
*********************************************************************/
AjPXmlNode ajXmlNodeNew()
{
    AjPXmlNode thys;
    AJNEW0(thys);
    
    return thys;
}




/* @func ajXmlFileNew  ***********************************************
**
** Create a new file and allocate the memory
**
** @return [thys] the new file
** @@
*********************************************************************/
AjPXmlFile ajXmlFileNew()
{
    AjPXmlFile thys;

    AJNEW0(thys);

    return thys;
}




/* @func ajXmlNodeDel  ***********************************************
**
** Delete the node
**
** @param [w] thys [AjPXmlFile] the node to delete
**
** @return [void] 
** @@
*********************************************************************/
void ajXmlNodeDel(AjPXmlNode *thys)
{
    if(!thys || !*thys)
	return;

    AJFREE(*thys);
    *thys = NULL;
    
    return;
}

/* @func ajXmlFileDel  ***********************************************
**
** Delete the file
**
** @param [w] thys [AjPXmlFile] the file to delete
**
** @return [void] 
** @@
*********************************************************************/
void ajXmlFileDel(AjPXmlFile *thys)
{
    if(!thys || !*thys)
	return;

    AJFREE(*thys);

    return;
}



/* @func ajXmlSetMaxMin  *********************************************
**
** sets the max min values of a graph
**
** @param [w] file [AjPXmlFile] the file to add the max min values to
** @param [r] xMin [double] the x min value
** @param [r] yMin [double] the y min value
** @param [r] xMax [double] the x max value
** @param [r] yMax [double] the y max value
**
** @return [AjBool] ajTrue if the current Graphics object is valid to 
** add MaxMin, eg. a graph
** @@
*********************************************************************/
AjBool ajXmlSetMaxMin (AjPXmlFile file, double xMin, double yMin, 
		       double xMax, double yMax)
{
    AjPXmlNode minMaxNode;
    AjPStr doub=NULL;
    AjPStr value=NULL;
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;
    
    value = ajStrNewC("[");
    
    nameString = ajStrNewC("Graph");
    if(!xml_PressentGraphicTypeIs(file, nameString))
	return ajFalse;
    
    ajStrDel(&nameString);
    nameString = ajStrNewC("fieldValue");
    minMaxNode = xml_MakeNewNode(file, nameString, 
				 xml_GetCurrentGraphic(file));
    xml_StrFromDouble(&doub, xMin);
    ajStrAppC(&value, ajStrStr(doub));  
    ajStrAppC(&value, ", ");  
    ajStrDel(&doub);
    xml_StrFromDouble(&doub, yMin);
    ajStrAppC(&value, ajStrStr(doub));  
    ajStrAppC(&value, ", ");  
    ajStrDel(&doub);
    xml_StrFromDouble(&doub, xMax);
    ajStrAppC(&value, ajStrStr(doub));  
    ajStrAppC(&value, ", ");  
    ajStrDel(&doub);
    xml_StrFromDouble(&doub, yMax);
    ajStrAppC(&value, ajStrStr(doub));  
    ajStrAppC(&value, "]");  
    ajStrDel(&nameString);
    nameString = ajStrNewC("fieldName");
    nameString2 = ajStrNewC("MaxMin");
    xml_SetAttribute(minMaxNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("value");
    xml_SetAttribute(minMaxNode, nameString, value);
    
    xml_UnrefNode(minMaxNode);
    
    ajStrDel(&doub);
    ajStrDel(&value);
    ajStrDel(&nameString);
    ajStrDel(&nameString2);
    
    return ajTrue;
}

/* @func ajXmlWriteFile **********************************************
**
** writes the file 
**
** @param [w] file [AjPXmlFile] the file to write
** @param [r] filename [AjPStr] the filename to write
**
** @return [AjBool] ajTrue if file correctly written
** @@
*********************************************************************/
AjBool ajXmlWriteFile(AjPXmlFile file, AjPStr filename)
{
    return xml_WriteFile(file, filename);
}




/* @func ajXmlWriteStdout ********************************************
**
** writes the file 
**
** @param [w] file [AjPXmlFile] the file to write
**
** @return [AjBool] ajTrue if file correctly written
** @@
*********************************************************************/
AjBool ajXmlWriteStdout(AjPXmlFile file)
{
    return xml_WriteStdout(file);
}




/* @func ajXmlClearFile **********************************************
**
** Clears the memory allocated to this file
**
** @param [w] file [AjPXmlFile] the file to clear
**
** @return [void]
** @@
*********************************************************************/
void ajXmlClearFile(AjPXmlFile file)
{
    xml_ClearFile(file);

    return;
}




/* @func ajXmlSetSource  *********************************************
**
** Adds the source to a graph object
**
** @param [w] file [AjPXmlFile] the file to add the title to
** @param [r] title [AjPStr] the title
**
** @return [AjBool] ajTrue if the current Graphics object is valid to 
** add Source, eg. a graph
** @@
*********************************************************************/
AjBool ajXmlSetSource(AjPXmlFile file, AjPStr title)
{
    AjPXmlNode titleNode;
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;

    nameString = ajStrNewC("Graph");
    if(!xml_PressentGraphicTypeIs(file, nameString))
	return ajFalse;

    ajStrDel(&nameString);
    nameString = ajStrNewC("fieldValue");
    titleNode = xml_MakeNewNode(file, nameString, 
				xml_GetCurrentGraphic(file));
    ajStrDel(&nameString);
    nameString = ajStrNewC("fieldName");
    nameString2 = ajStrNewC("Source");
    xml_SetAttribute(titleNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("value");
    xml_SetAttribute(titleNode, nameString, title);

    xml_UnrefNode(titleNode);
    ajStrDel(&nameString);
    ajStrDel(&nameString2);

    return ajTrue;
}




/* @func ajXmlAddMainTitle  ******************************************
**
** Adds a title to a graph object
**
** @param [w] file [AjPXmlFile] the file to add the title to
** @param [r] title [AjPStr] the title
**
** @return [AjBool] ajTrue if the current Graphics object is valid to 
** add Source, eg. a graph
** @@
*********************************************************************/
AjBool ajXmlAddMainTitle(AjPXmlFile file, AjPStr title)
{
    AjPXmlNode graphNode;
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;
    AjPStr titleAltered=NULL;

    /*
       nameString = ajStrNewC("Graph");
       if(!xml_PressentGraphicTypeIs(file, nameString))
       return ajFalse;
       
       ajStrDel(&nameString);
       */

    titleAltered = ajStrNewS(title);

    /* Not sure about this hugh */
    xml_AddQuotes(&titleAltered);

    printf("title w st = %p st s = %s\n", title, ajStrStr(title));
    fflush(stdout);


    nameString = ajStrNewC("fieldValue");
    graphNode = xml_MakeNewNode(file, nameString, 
				xml_GetCurrentGraphic(file));


    printf("title e st = %p st s = %s\n", title, ajStrStr(title));
    fflush(stdout);

    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    nameString2 = ajStrNewC("Graph.mainTitle");
    xml_SetAttribute(graphNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("value");
    xml_SetAttribute(graphNode, nameString, titleAltered);

    printf("title r st = %p st s = %s\n", title, ajStrStr(title));
    fflush(stdout);


    xml_UnrefNode(graphNode);
    ajStrDel(&nameString);
    ajStrDel(&nameString2);
    ajStrDel(&titleAltered);

    printf("title t st = %p st s = %s\n", title, ajStrStr(title));
    fflush(stdout);


    return ajTrue;
}




/* @func ajXmlAddXTitle  *********************************************
**
** Adds a title to a graph object
**
** @param [w] file [oXX3DFile] the file to add the title to
** @param [r] title [AjPStr] the title
**
** @return [AjBool] ajTrue if the current Graphics object is valid to 
** add XTitle, eg. a graph
** @@
*********************************************************************/
AjBool ajXmlAddXTitle (AjPXmlFile file, AjPStr title)
{
    AjPXmlNode titleNode;
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;
    AjPStr titleAltered=NULL;

    titleAltered = ajStrNewS(title);

    /* Not sure about this hugh */
    xml_AddQuotes(&titleAltered);

    nameString = ajStrNewC("Graph");
    if(!xml_PressentGraphicTypeIs(file, nameString))
	return ajFalse;

    ajStrDel(&nameString);
    nameString = ajStrNewC("fieldValue");
    titleNode = xml_MakeNewNode(file, nameString, 
				xml_GetCurrentGraphic(file));
    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    nameString2 = ajStrNewC("Graph.xTitle");
    xml_SetAttribute(titleNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("value");
    xml_SetAttribute(titleNode, nameString, titleAltered);

    xml_UnrefNode(titleNode);
    ajStrDel(&nameString);
    ajStrDel(&nameString2);
    ajStrDel(&titleAltered);

    return ajTrue;
}




/* @func ajXmlAddYTitle  *********************************************
**
** Adds a title to a graph object
**
** @param [w] file [oXX3DFile] the file to add the title to
** @param [r] title [AjPStr] the title
**
** @return [AjBool] ajTrue if the current Graphics object is valid to
** add YTitle, eg. a graph
** @@
*********************************************************************/
AjBool ajXmlAddYTitle (AjPXmlFile file, AjPStr title)
{
    AjPXmlNode titleNode;
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;
    AjPStr titleAltered=NULL;

    titleAltered = ajStrNewS(title);

    /* Not sure about this hugh */
    xml_AddQuotes(&titleAltered);

    nameString = ajStrNewC("Graph");
    if(!xml_PressentGraphicTypeIs(file, nameString))
	return ajFalse;

    ajStrDel(&nameString);
    nameString = ajStrNewC("fieldValue");
    titleNode = xml_MakeNewNode(file, nameString, 
				xml_GetCurrentGraphic(file));
    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    nameString2 = ajStrNewC("Graph.yTitle");
    xml_SetAttribute(titleNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("value");
    xml_SetAttribute(titleNode, nameString, titleAltered);

    xml_UnrefNode(titleNode);
    ajStrDel(&nameString);
    ajStrDel(&nameString2);
    ajStrDel(&titleAltered);

    return ajTrue;
}




/* @func ajXmlAddMainTitleC  *****************************************
**
** Adds a title to a graph object from *char
**
** @param [w] file [AjPXmlFile] the file to add the title to
** @param [r] title [*char] the title
**
** @return [AjBool] ajTrue if the current Graphics object is valid to 
** add Source, eg. a graph
** @@
*********************************************************************/
AjBool ajXmlAddMainTitleC(AjPXmlFile file, char *title)
{
    AjPStr nameString=NULL;
    AjBool returnValue;
  
    nameString = ajStrNewC(title);
    returnValue = ajXmlAddMainTitle(file, nameString);
    ajStrDel(&nameString);

    return returnValue;
}




/* @func ajXmlAddXTitleC  ********************************************
**
** Adds a title to a graph object from *char
**
** @param [w] file [oXX3DFile] the file to add the title to
** @param [r] title [*char] the title
**
** @return [AjBool] ajTrue if the current Graphics object is valid to 
** add XTitle, eg. a graph
** @@
*********************************************************************/
AjBool ajXmlAddXTitleC (AjPXmlFile file, char *title)
{
    AjPStr nameString=NULL;
    AjBool returnValue;
  
    nameString = ajStrNewC(title);
    returnValue = ajXmlAddXTitle(file, nameString);
    ajStrDel(&nameString);

    return returnValue;
}




/* @func ajXmlAddYTitleC  ********************************************
**
** Adds a title to a graph object from *char
**
** @param [w] file [oXX3DFile] the file to add the title to
** @param [r] title [*char] the title
**
** @return [AjBool] ajTrue if the current Graphics object is valid to
** add YTitle, eg. a graph
** @@
*********************************************************************/
AjBool ajXmlAddYTitleC (AjPXmlFile file, char *title)
{
    AjPStr nameString=NULL;
    AjBool returnValue;
  
    nameString = ajStrNewC(title);
    returnValue = ajXmlAddYTitle(file, nameString);
    ajStrDel(&nameString);

    return returnValue;
}




/* @func ajXmlAddTextCentred *****************************************
**
** Adds some text, centered at point given
**
** @param [w] file [oXX3DFile] the file to add the line to
** @param [r] x [double] the x coordinate
** @param [r] y [double] the y coordinate
** @param [r] size [double] font size
** @param [r] angle [double] font angle
** @param [r] fontFamily [AjPStr] the name of the font family
** @param [r] fontStyle [AjPStr] the style or the font
** @param [r] text [AjPStr] the text to be written
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddTextCentred(AjPXmlFile file, double x, double y, 
			 double size, double angle, AjPStr fontFamily,
			 AjPStr fontStyle, AjPStr text)
{
    float stringLength;

    stringLength = ajStrLen(text) * size / ONE_METER_FONT;
    ajXmlAddText(file, (x - ((stringLength / 2) * cos(angle))), y, 
		 size, angle, fontFamily, fontStyle, text);
    
    return;
}




/* @func ajXmlAddText ***********************************************
**
** Adds some text
**
** @param [w] file [oXX3DFile] the file to add the line to
** @param [r] x [double] the x coordinate
** @param [r] y [double] the y coordinate
** @param [r] size [double] font size
** @param [r] angle [double] font angle
** @param [r] fontFamily [AjPStr] the name of the font family
** @param [r] fontStyle [AjPStr] the style or the font
** @param [r] text [AjPStr] the text to be written
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddText(AjPXmlFile file, double x, double y, double size, 
		  double angle, AjPStr fontFamily, AjPStr fontStyle, 
		  AjPStr text)
{
    AjPXmlNode transformNode = NULL;
    AjPXmlNode elText = NULL;
    AjPXmlNode elFont = NULL;
    AjPXmlNode tranformParent = NULL;
    AjPXmlNode tempNode = NULL;
    AjPStr ajAttributeValue=NULL;
    AjPStr temp=NULL;
    AjPStr name=NULL;
    AjPStr nameString=NULL;
    AjPStr attributeName=NULL;
    AjPStr attributeValue=NULL;
    ajint i;

    GdomeNodeList *listAppearance = NULL;
    GdomeException exc;
    GdomeDOMString *nodeName = NULL;

    temp = ajStrNew();
    ajAttributeValue = ajStrNew();

    nameString = ajStrNewC("Graph");

    if(xml_PressentGraphicTypeIs(file, nameString))
    {

	printf("PressentGraphicTypeIs graph\n");
	fflush(stdout);
      

	nodeName = gdome_str_mkref("fieldValue");
	listAppearance = gdome_el_getElementsByTagName
	    (xml_GetNodeElement(xml_GetCurrentGraphic(file)), nodeName, 
	     &exc);
	for(i=0; (i<gdome_nl_length(listAppearance, &exc)
		  && tranformParent == NULL) ; ++i)
	{
	    tempNode = xml_SetNode(gdome_nl_item(listAppearance, i, 
						 &exc));
	    attributeName = ajStrNewC("name");
	    attributeValue = xml_GetAttribute(tempNode, attributeName);
	    if(ajStrMatchC(attributeValue, "children"))
	    {
		tranformParent = xml_SetNode(gdome_nl_item(listAppearance, 
							   i, &exc));
	    }
	    ajXmlNodeDel(&tempNode);
	    ajStrDel(&attributeName);
	    ajStrDel(&attributeValue);
	}	    
	if(tranformParent == NULL)
	{
	    nameString = ajStrNewC("fieldValue");
	    tranformParent = xml_MakeNewNode(file, nameString, 
					     xml_GetCurrentGraphic(file));
		
	    ajStrDel(&attributeName);
	    attributeName = ajStrNewC("name");
	    ajStrDel(&attributeValue);
	    attributeValue = ajStrNewC("children");
	    xml_SetAttribute(tranformParent, attributeName, 
			     attributeValue);
	}
    }
    else
    {

	printf("PressentGraphicTypeIs NULL\n");
	/*    ajXmlWriteStdout(file); */
	fflush(stdout);
      
	tranformParent = xml_GetCurrentGraphic(file);
    }

    nameString = ajStrNewC("Transform");
    transformNode = xml_MakeNewNode(file, nameString, tranformParent);

    xml_AddACoord(x, y, ajFalse, &ajAttributeValue, &temp);
    ajStrDel(&nameString);
    nameString = ajStrNewC("translation");
    xml_SetAttribute(transformNode, nameString, ajAttributeValue);
  
    ajStrDel(&ajAttributeValue);
    ajAttributeValue = ajStrNewC("0 0 1 ");
    ajStrDel(&temp);
    xml_StrFromDouble(&temp, angle);
    ajStrApp(&ajAttributeValue, temp);
  
    ajStrDel(&nameString);
    nameString = ajStrNewC("rotation");
    xml_SetAttribute(transformNode, nameString, ajAttributeValue);

    /* I create a new element, the Text node  */
    ajStrDel(&nameString);
    nameString = ajStrNewC("Text");
    elText = xml_MakeNewShapeNode(file, transformNode, nameString);

    ajStrDel(&nameString);
    nameString = ajStrNewC("string");
    xml_SetAttribute(elText, nameString, text);

    /* I have had to remove this for now, as the parser cannot handle 
       this type of node */
    /*
       if(fontFamily != NULL || fontStyle != NULL || size >= 0)
       {
       name =ajStrNewC("FontStyle");
       elFont = xml_MakeNewNode(file, name, elText);
       if(fontFamily != NULL)
       {
       ajStrDel(&nameString);
       nameString = ajStrNewC("family");
       xml_SetAttribute(elFont, nameString, fontFamily);
       }
       if(fontStyle != NULL)
       {
       ajStrDel(&nameString);
       nameString = ajStrNewC("style");
       xml_SetAttribute(elFont, nameString, fontStyle);
       }
       if(size >= 0)
       {
       ajStrFromDouble(&temp, size, 2);
       ajStrDel(&nameString);
       nameString = ajStrNewC("size");
       xml_SetAttribute(elFont, nameString, temp);
       }
       }
       */

    ajStrDel(&ajAttributeValue);
    ajStrDel(&temp);
    ajStrDel(&name);
    ajStrDel(&nameString);

    xml_UnrefNode(transformNode);
    xml_UnrefNode(elText);
    if(elFont != NULL)
	xml_UnrefNode(elFont);

    return;
}




/* @func ajXmlAddTextWithJustify *************************************
**
** Adds some text with the justification set
**
** @param [w] file [oXX3DFile] the file to add the line to
** @param [r] x [double] the x coordinate
** @param [r] y [double] the y coordinate
** @param [r] size [double] font size
** @param [r] angle [double] font angle
** @param [r] fontFamily [AjPStr] the name of the font family
** @param [r] fontStyle [AjPStr] the style or the font
** @param [r] text [AjPStr] the text to be written
** @param [r] horizontal [AjBool] whether the text advances 
** horizontally in its major direction (horizontal = ajTrue) or 
** vertically in its major direction (horizontal = ajFalse).
** @param [r] leftToRight [AjBool] direction of text advance in the 
** major or minor direction
** @param [r] topToBottom [AjBool] direction of text advance in the 
** major or minor direction
** @param [r] justifyMajor [AjPStr] alignment of the text in the 
** Major direction
** @param [r] justifyMinor [AjPStr] alignment of the text in the 
** Minor direction
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddTextWithJustify(AjPXmlFile file, double x, double y, 
			     double size, double angle, 
			     AjPStr fontFamily, AjPStr fontStyle, 
			     AjPStr text, AjBool horizontal, 
			     AjBool leftToRight, AjBool topToBottom, 
			     AjPStr justifyMajor, AjPStr justifyMinor)
{
    AjPXmlNode transformNode;
    AjPXmlNode elText;
    AjPXmlNode elFont;
    AjPStr ajAttributeValue=NULL;
    AjPStr temp=NULL;
    AjPStr name=NULL;
    AjPStr nameString=NULL;

    temp = ajStrNew();
    ajAttributeValue = ajStrNew();

    nameString = ajStrNewC("Transform");
    transformNode = xml_MakeNewNode(file, nameString, 
				    xml_GetCurrentGraphic(file));

    xml_AddACoord(x, y, ajFalse, &ajAttributeValue, &temp);
    ajStrDel(&nameString);
    nameString = ajStrNewC("translation");
    xml_SetAttribute(transformNode, nameString, ajAttributeValue);
  
    ajStrClear(&ajAttributeValue);
    xml_AddACoord(angle, 0, ajFalse, &ajAttributeValue, &temp);
    ajStrDel(&nameString);
    nameString = ajStrNewC("rotation");
    xml_SetAttribute(transformNode, nameString, ajAttributeValue);

    /* I create a new element, the Text node  */
    ajStrDel(&nameString);
    nameString = ajStrNewC("Text");
    elFont = elText = xml_MakeNewShapeNode(file, transformNode, 
					   nameString);

    ajStrDel(&nameString);
    nameString = ajStrNewC("string");
    xml_SetAttribute(elText, nameString, text);

    name =ajStrNewC("Font");
    elFont = xml_MakeNewNode(file, name, elText);
    if(fontFamily != NULL)
    {
	ajStrDel(&nameString);
	ajStrDel(&nameString);
	nameString = ajStrNewC("family");
	xml_SetAttribute(elFont, nameString, fontFamily);
    }
    if(fontStyle != NULL)
    {
	ajStrDel(&nameString);
	nameString = ajStrNewC("style");
	xml_SetAttribute(elFont, nameString, fontStyle);
    }
    if(size >= 0)
    {
	ajStrFromInt(&temp, size);
	ajStrDel(&nameString);
	nameString = ajStrNewC("size");
	xml_SetAttribute(elFont, nameString, temp);
    }

    ajStrDel(&nameString);
    nameString = ajStrNewC("horizontal");
    xml_SetAttribute(elFont, nameString, xml_StrFromBool(horizontal));
    ajStrDel(&nameString);
    nameString = ajStrNewC("leftToRight");
    xml_SetAttribute(elFont, nameString, xml_StrFromBool(leftToRight));
    ajStrDel(&nameString);
    nameString = ajStrNewC("topToBottom");
    xml_SetAttribute(elFont, nameString, xml_StrFromBool(topToBottom));
    ajAttributeValue = ajStrNewC("\"");
    ajStrAppC(&ajAttributeValue, ajStrStr(justifyMajor));
    ajStrAppC(&ajAttributeValue, "\",\"");
    ajStrAppC(&ajAttributeValue, ajStrStr(justifyMinor));
    ajStrAppC(&ajAttributeValue, "\"");

    ajStrDel(&nameString);
    nameString = ajStrNewC("justify");
    xml_SetAttribute(elFont, nameString, ajAttributeValue);

    ajStrDel(&ajAttributeValue);
    ajStrDel(&temp);
    ajStrDel(&name);
    ajStrDel(&nameString);

    xml_UnrefNode(transformNode);
    xml_UnrefNode(elText);
    xml_UnrefNode(elFont);

    return;
}




/* @func ajXmlAddTextOnArc  *****************************************
**
** adds text on an arc.  If the text is too long it is shrunk, if it 
** does 
** not fill the arc it is centred. A character of length 1m is size 
** 1.784
**
** @param [w] file [AjPXmlFile] the file to add the text to
** @param [r] xCentre [double] the x value of the centre of the 
** circle
** @param [r] yCentre [double] the y value of the centre of the 
** circle
** @param [r] startAngle [double] the angle of the start of the arc
** @param [r] endAngle [double] the angle of the end of the arc
** @param [r] radius [double] the radius of the circle
** @param [r] size [double] the font size
** @param [r] fontFamily [AjPStr] the font Family 
** @param [r] fontStyle [AjPStr] the  font Style
** @param [r] text [AjPStr] the text
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddTextOnArc(AjPXmlFile file, double xCentre, double yCentre,
		       double startAngle, double endAngle, 
		       double radius, double size, AjPStr fontFamily, 
		       AjPStr fontStyle, AjPStr text)
{
    double anglularExtentPerLetter;
    double letterAngle;
    double letterXPos;
    double letterYPos;
    double textLength;
    double arcLength;
    double lettersInFront;

    int i;

    AjPStr letter = NULL;

    letter = ajStrNew();

    textLength = ajStrLen(text) / ONE_METER_FONT;
    arcLength = (endAngle - startAngle) * radius;
    
    if(textLength > arcLength)
    {
	size = (arcLength * ONE_METER_FONT) / ajStrLen(text);
	lettersInFront = 0;
    }
    else if(textLength < arcLength)
	lettersInFront = ((arcLength - textLength) / 2) / 
	    (size / ONE_METER_FONT);
    else
	lettersInFront = 0;
    
    anglularExtentPerLetter = size / (radius * ONE_METER_FONT);
    
    /* why does this run one more time than it should? */
    /* 
       for(i=lettersInFront; i<(((double) ajStrLen(text)) + 
       lettersInFront); ++i) 
       */

    for(i=0; i<ajStrLen(text); ++i)
    {
	ajStrAssSubC(&letter, ajStrStr(text), i, i);
	if(!ajStrMatchC(letter, " "))
	{
	    letterAngle = ((((double)i) + lettersInFront) * 
			   anglularExtentPerLetter) + startAngle;
	    letterXPos = (cos(letterAngle + startAngle) * radius) +
		xCentre;
	    letterYPos = (sin(letterAngle + startAngle) * radius) +
		yCentre;
	
	    ajXmlAddText(file, letterXPos, letterYPos, size, 
			 letterAngle, fontFamily, fontStyle, 
			 letter);
	}
    }

    ajStrDel(&letter);

    return;
}




/* @func ajXmlAddJoinedLineSetEqualGapsF ****************************
**
** Adds a set of joined lines with the x values equaly spaced
**
** @param [w] file [AjPXmlFile] the file to add the line to
** @param [r] y1 [*float] pointer to 1st y coordinate
** @param [r] numberOfPoints [int] number Of Points
** @param [r] startX [float] x start value
** @param [r] increment [float] increment
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddJoinedLineSetEqualGapsF(AjPXmlFile file, float *y, 
				     int numberOfPoints, float startX,
				     float increment)
{
    double xn[numberOfPoints];
    double yn[numberOfPoints];
    int i;
    
    for(i=0.0; i<numberOfPoints; ++i)
    {
	xn[i] = (double) startX + (((double) i) * increment);
	yn[i] = (double) y[i];
    }

    ajXmlAddJoinedLineSet(file, xn, yn, numberOfPoints);
    
    return;
}




/* @func ajXmlAddJoinedLineSetF *************************************
**
** Adds a set of joined lines
**
** @param [w] file [AjPXmlFile] the file to add the line to
** @param [r] x1 [*float] pointer to 1st x coordinate
** @param [r] y1 [*float] pointer to 1st y coordinate
** @param [r] numberOfPoints [double] number Of Points
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddJoinedLineSetF(AjPXmlFile file, float *x, float *y, 
			    int numberOfPoints)
{
    /*
       double xn[numberOfPoints];
       double yn[numberOfPoints];
       */
    double *xn;
    double *yn;
    int i;
    
    printf("About to print table before\n");
    fflush(stdout);
    /*    ajStrTablePrint(file->nodeTypes); */
    fflush(stdout);
    printf("Printed table before\n");
    fflush(stdout);

    AJCNEW(xn, numberOfPoints);
    AJCNEW(yn, numberOfPoints);

    for(i=0; i<numberOfPoints; ++i)
    {
	xn[i] = (double) x[i];
	yn[i] = (double) y[i];
    }

    ajXmlAddJoinedLineSet(file, xn, yn, numberOfPoints);

    /* clear array? hugh */
    AJFREE(xn);
    AJFREE(yn);
    
    printf("About to print table after\n");
    fflush(stdout);
    /*    ajStrTablePrint(file->nodeTypes); */
    fflush(stdout);
    printf("Printed table after\n");
    fflush(stdout);

    
    return;
}




/* @func ajXmlAddJoinedLineSet ***************************************
**
** Adds a set of joined lines
**
** @param [w] file [AjPXmlFile] the file to add the line to
** @param [r] x1 [*double] pointer to 1st x coordinate
** @param [r] y1 [*double] pointer to 1st y coordinate
** @param [r] numberOfPoints [int] number Of Points
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddJoinedLineSet(AjPXmlFile file, double *x, double *y, 
			   int numberOfPoints)
{
    AjPXmlNode el;
    int count;
    AjPStr index=NULL;
    AjPStr coord=NULL;
    AjPStr nameString=NULL;

    
    printf("About to print table q\n");
    fflush(stdout);
    /*    ajStrTablePrint(file->nodeTypes); */
    fflush(stdout);
    printf("Printed table q\n");
    fflush(stdout);

    /* I try and find preexisting IndexedLineSet's  */
    nameString = ajStrNewC("IndexedLineSet");
    el = xml_GetNodeTypeMakeIfNot(file, nameString); 

    coord = xml_GetPoints(el);
    index = xml_GetIndex(el);

    for(count = 0; count < numberOfPoints; ++count)
	xml_AddACoord(x[count], y[count], (count!=0), &coord, &index);

    xml_SetPoints(el, coord);
    xml_SetIndex(el, index);

    /* tidy up */
    ajStrDel(&coord);
    ajStrDel(&index);
    ajStrDel(&nameString);

    if(el != xml_GetCurrentGraphic(file))
	xml_UnrefNode(el);

    return;
}




/* @func ajXmlAddLine  ***********************************************
**
** Adds a line
**
** @param [w] file [AjPXmlFile*] the file to add the line to
** @param [r] x1 [double] 1st x coordinate
** @param [r] y1 [double] 1st y coordinate
** @param [r] x2 [double] 2nd x coordinate
** @param [r] y2 [double] 2nd y coordinate
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddLine(AjPXmlFile file, double x1, double y1, double x2, 
		  double y2)
{
    AjPXmlNode el;
    AjPStr coord=NULL;
    AjPStr index=NULL;
    AjPStr nameString=NULL;

    nameString = ajStrNewC("IndexedLineSet");
    el = xml_GetNodeTypeMakeIfNot(file, nameString);

    /* setting up the coords to add */
    /* coord = xml_GetAttribute(el, nameString); old line */
    coord = xml_GetPoints(el);
    index = xml_GetIndex(el);

    xml_AddACoord(x1, y1, ajFalse, &coord, &index);
    xml_AddACoord(x2, y2, ajTrue, &coord, &index);

    /* xml_SetAttribute(el, nameString, coord); old line */
    xml_SetPoints(el, coord);
    xml_SetIndex(el, index);

    /* tidy up */
    ajStrDel(&coord);
    ajStrDel(&index);
    ajStrDel(&nameString);

    if(el != xml_GetCurrentGraphic(file))
	xml_UnrefNode(el);

    return;
}




/* @func ajXmlAddLineF  **********************************************
**
** Adds a line with floats
**
** @param [w] file [AjPXmlFile] the file to add the line to
** @param [r] x1 [float] 1st x coordinate
** @param [r] y1 [float] 1st y coordinate
** @param [r] x2 [float] 2nd x coordinate
** @param [r] y2 [float] 2nd y coordinate
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddLineF(AjPXmlFile file, float x1, float y1, float x2, 
		   float y2)
{
    ajXmlAddLine(file, (double) x1, (double) y1, (double) x2, 
		 (double) y2);

    return;
}




/* @func ajXmlAddPoint  **********************************************
**
** Adds a Point
**
** @param [w] file [AjPXmlFile] the file to add the point to
** @param [r] x [double] x coordinate
** @param [r] y [double] y coordinate
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddPoint(AjPXmlFile file, double x1, double y1)
{
    AjPXmlNode el;
    AjPStr coord=NULL;
    AjPStr index=NULL;
    AjPStr nameString=NULL;

    nameString = ajStrNewC("IndexedLineSet");
    el = xml_GetNodeTypeMakeIfNot(file, nameString);
  
    /* setting up the coords to add */
    coord = xml_GetPoints(el);
    index = xml_GetIndex(el);

    xml_AddACoord(x1, y1, ajFalse, &coord, &index);
    xml_AddACoord(x1, y1, ajTrue, &coord, &index);

    xml_SetPoints(el, coord);
    xml_SetIndex(el, index);

    /* tidy up */
    ajStrDel(&coord);
    ajStrDel(&index);
    ajStrDel(&nameString);

    if(el != xml_GetCurrentGraphic(file))
	xml_UnrefNode(el);

    return;
}




/* @func ajXmlAddHistogramEqualGapsF ********************************
**
** Adds a Histogram with each bar the same width, with y axis at 0.
**
** @param [w] file [AjPXmlFile] the file to add the line to
** @param [r] y [float*] the y coordinates, heights of bars
** @param [r] numPoints [int] number of bars
** @param [r] startX [double] start position of first bar
** @param [r] xGap [int] x size of each bin / bar
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddHistogramEqualGapsF(AjPXmlFile file, float *y, 
				 int numPoints, float startX, 
				 float xGap)
{
    int bar;
    
    for(bar = 0; bar < numPoints; ++bar)
	if(y[bar] != 0.0)
	    ajXmlAddRectangle(file, (startX + (bar * xGap)), 0, 
			      (startX + ((bar+ 1) * xGap)), y[bar], 
			      ajTrue);

    return;
}




/* @func ajXmlAddRectangleSet  ***************************************
**
** Adds a rectangle
**
** @param [w] file [AjPXmlFile] the file to add the line to
** @param [r] x1 [double] 1st x coordinate
** @param [r] y1 [double] 1st y coordinate
** @param [r] x2 [double] 2nd x coordinate
** @param [r] y2 [double] 2nd y coordinate
** @param [r] numPoints [int] number of points
** @param [r] fill [AjBool] Filled rectangle?
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddRectangleSet(AjPXmlFile file, double *x1, double *y1, 
			  double *x2, double *y2, int numPoints, 
			  AjBool fill)
{
    int point;
    
    for(point = 0; point < numPoints; ++point)
	ajXmlAddRectangle(file, x1[point], y1[point], x2[point], 
			  y2[point], fill);

    return;
}




/* @func ajXmlAddRectangle  ******************************************
**
** Adds a rectangle
**
** @param [w] file [AjPXmlFile] the file to add the line to
** @param [r] x1 [double] 1st x coordinate
** @param [r] y1 [double] 1st y coordinate
** @param [r] x2 [double] 2nd x coordinate
** @param [r] y2 [double] 2nd y coordinate
** @param [r] fill [AjBool] Filled rectangle?
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddRectangle(AjPXmlFile file, double x1, double y1, 
		       double x2, double y2, AjBool fill)
{
    AjPXmlNode el;
    AjPStr coord=NULL;
    AjPStr index=NULL;
    AjPStr nameString=NULL;

    if(fill)
    {  
	nameString = ajStrNewC("IndexedFaceSet");
	el = xml_GetNodeTypeMakeIfNot(file, nameString);

	/* setting up the coords to add */
	coord = xml_GetPoints(el);
	/*
	   ajStrDel(&nameString);
	   nameString = ajStrNewC("coordIndex");
	   index = xml_GetAttribute(el, nameString);
	   */
	index = xml_GetIndex(el);

	if(!(((x1 > x2) && (y1 > y2)) || ((x1 < x2) && (y1 < y2))) )
	{
	    xml_AddACoord(x1, y1, ajFalse, &coord, &index);
	    xml_AddACoord(x1, y2, ajTrue, &coord, &index);
	    xml_AddACoord(x2, y2, ajTrue, &coord, &index);
	    xml_AddACoord(x2, y1, ajTrue, &coord, &index);
	}
	else
	{
	    xml_AddACoord(x2, y1, ajFalse, &coord, &index);
	    xml_AddACoord(x2, y2, ajTrue, &coord, &index);
	    xml_AddACoord(x1, y2, ajTrue, &coord, &index);
	    xml_AddACoord(x1, y1, ajTrue, &coord, &index);
	}
	    
	/* xml_SetAttribute(el, nameString, coord); old line */
	xml_SetPoints(el, coord);
	xml_SetIndex(el, index);
	/*
	   ajStrDel(&nameString);
	   nameString = ajStrNewC("coordIndex");
	   xml_SetAttribute(el, nameString, index);
	   */
	/* tidy up */
	xml_UnrefNode(el);
	ajStrDel(&coord);
	ajStrDel(&index);
    }
    else
    {
	double x[5];
	double y[5];
      
	if((((x1 > x2) && (y1 > y2)) || ((x1 < x2) && (y1 < y2))) )
	{
	    x[0] = x1;
	    x[1] = x2;
	    x[2] = x2;
	    x[3] = x1;
	    x[4] = x1;
      
	    y[0] = y1;
	    y[1] = y1;
	    y[2] = y2;
	    y[3] = y2;
	    y[4] = y1;
	}
	else
	{
	    x[0] = x2;
	    x[1] = x1;
	    x[2] = x1;
	    x[3] = x2;
	    x[4] = x2;
      
	    y[0] = y1;
	    y[1] = y1;
	    y[2] = y2;
	    y[3] = y2;
	    y[4] = y1;
	}	  
      
	ajXmlAddJoinedLineSet(file, x, y, 5);      
    }
      
    ajStrDel(&nameString);

    return;
}




/* @func ajXmlAddCylinder  *******************************************
**
** Adds a Cylinder
**
** @param [w] file [AjPXmlFile] the file to add the Cylinder to
** @param [r] x1 [double] 1st x coordinate
** @param [r] y1 [double] 1st y coordinate
** @param [r] x2 [double] 2nd x coordinate
** @param [r] y2 [double] 2nd y coordinate
** @param [r] width [double] width 
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddCylinder(AjPXmlFile file, double x1, double y1, 
		      double x2, double y2, double width)
{
    double height;

    double xCentre;
    double yCentre;

    double angle;

    height  = pow((pow((x2-x1), 2) + (pow((y2-y1), 2))), 0.5);
    xCentre =  x1 + ((x2 - x1) / 2);
    yCentre = y1 + ((y2 - y1) / 2);
    angle   = atan2((y2 - y1), (x2 - x1));

    xml_AddCylinder(file, xCentre, yCentre, angle, height, width);

    return;
}




/* @func ajXmlAddPointLabelCircle ************************************
**
** adds a Point Label
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] angle [double] the angle of label to add
** @param [r] xCentre [double] the x value of the centre of the 
** circle
** @param [r] yCentre [double] the y value of the centre of the 
** circle
** @param [r] radius [double] the radius of the circle
** @param [r] length [double] the length of the line of the label
** @param [r] size [double] the font size
** @param [r] fontFamily [AjPStr] the font Family 
** @param [r] fontStyle [AjPStr] the  font Style
** @param [r] text [AjPStr] the text of label to add
**
** @return [AjBool] ajTrue if correctly added
** @@
*********************************************************************/
AjBool ajXmlAddPointLabelCircle(AjPXmlFile file, double angle, 
				double xCentre, double yCentre, 
				double radius, double length, 
				double size, AjPStr fontFamily, 
				AjPStr fontStyle, AjPStr text)
{
    double xStart;
    double yStart;
    double xEnd;
    double yEnd;
    double textXStart;
    double textYStart;

    AjPStr textPrinted = NULL;
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;
    
    nameString = ajStrNewC("DNAPlot");
    if(!xml_PressentGraphicTypeIs(file, nameString))
	return ajFalse;

    xStart = (sin(angle) * radius) + xCentre;
    yStart = (cos(angle) * radius) + yCentre;
    xEnd = (sin(angle) * (radius + length)) + xCentre;
    yEnd = (cos(angle) * (radius + length)) + yCentre;
    
    if(xml_AngleIsInSecondHalfOfCircle(angle))
    {
	textPrinted = ajStrNewS(text);
	ajStrRev(&textPrinted);
    }
    else
	textPrinted = ajStrNewS(text);

    textXStart = (cos(angle) * (radius + (length * 2))) + xCentre;
    textYStart = (sin(angle) * (radius + (length * 2))) + yCentre;

    ajXmlAddLine(file, xStart, yStart, xEnd, yEnd);
    ajStrDel(&nameString);
    nameString = ajStrNewC("BEGIN");
    nameString2 = ajStrNewC("MIDDLE");
    ajXmlAddTextWithJustify(file, textXStart, textYStart, size, 
			    (angle - acos(0)), fontFamily, fontStyle, 
			    textPrinted, ajTrue, ajTrue, ajTrue, nameString,
			    nameString2);

    ajStrDel(&textPrinted);
    ajStrDel(&nameString);
    ajStrDel(&nameString);
    ajStrDel(&nameString2);

    return ajTrue;
}




/* @func ajXmlAddSectionLabelCircle **********************************
**
** adds a Section Label
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] startAngle [double] the start angle of label to add
** @param [r] endAngle [double] the end angle of label to add
** @param [r] xCentre [double] the x value of the centre of the 
** circle
** @param [r] yCentre [double] the y value of the centre of the 
** circle
** @param [r] radius [double] the radius of the circle
** @param [r] width [double] the width of the label
** @param [r] labelArmAngle [double] the angle subtended by the label
** arms (ignored if labelStyle = Block
** @param [r] labelStyle [AjPStr] the label style
** @param [r] textPosition [double] the text Position relative to the
** circle
** @param [r] size [double] the font size
** @param [r] fontFamily [AjPStr] the font Family 
** @param [r] fontStyle [AjPStr] the  font Style
** @param [r] text [AjPStr] the text of label to add
**
** @return [AjBool] ajTrue if correctly added
** @@
*********************************************************************/
AjBool ajXmlAddSectionLabelCircle(AjPXmlFile file, double startAngle, 
				  double endAngle, double xCentre, 
				  double yCentre, double radius, 
				  double width, double labelArmAngle, 
				  AjPStr labelStyle, 
				  double textPosition, double size, 
				  AjPStr fontFamily, AjPStr fontStyle,
				  AjPStr text)
{
    double xStartInner;
    double yStartInner;
    double xEndInner;
    double yEndInner;
    double textXStart;
    double textYStart;

    double xStartOuter;
    double yStartOuter;
    double xEndOuter;
    double yEndOuter;

    double xStartInnerLabel;
    double yStartInnerLabel;
    double xEndInnerLabel;
    double yEndInnerLabel;

    double xStartOuterLabel;
    double yStartOuterLabel;
    double xEndOuterLabel;
    double yEndOuterLabel;

    AjPStr textPrinted = NULL;
    AjPStr nameString=NULL;
    
    nameString = ajStrNewC("DNAPlot");
    if(!xml_PressentGraphicTypeIs(file, nameString))
	return ajFalse;


    if(ajStrMatchC(labelStyle, "Block"))
    {
	xStartInner = (sin(startAngle) * (radius - (width / 2)) + 
		       xCentre);
	yStartInner = (cos(startAngle) * (radius - (width / 2)) + 
		       yCentre);
	xEndInner = (sin(endAngle) * (radius - (width / 2)) + 
		     xCentre);
	yEndInner = (cos(endAngle) * (radius - (width / 2)) + 
		     yCentre);
	xStartOuter = (sin(startAngle) * (radius + (width / 2)) + 
		       xCentre);
	yStartOuter = (cos(startAngle) * (radius + (width / 2)) + 
		       yCentre);
	xEndOuter = (sin(endAngle) * (radius + (width / 2)) + 
		     xCentre);
	yEndOuter = (cos(endAngle) * (radius + (width / 2)) + 
		     yCentre);

	textXStart = (sin(startAngle) * (radius + textPosition) + 
		      xCentre);
	textYStart = (cos(startAngle) * (radius + textPosition) + 
		      yCentre);

	/* should realy make this a long nurbs, perhaps later */

	ajXmlAddArc(file, xCentre, yCentre, startAngle, endAngle, 
		    (radius - (width / 2)));
	ajXmlAddArc(file, xCentre, yCentre, startAngle, endAngle, 
		    (radius + (width / 2)));
	ajXmlAddLine(file, xStartInner, yStartInner, xStartOuter, 
		     yStartOuter);
	ajXmlAddLine(file, xEndInner, yEndInner, xEndOuter, 
		     yEndOuter);
        ajXmlAddTextOnArc(file, xCentre, yCentre, startAngle, 
			  endAngle, (radius + textPosition), size, 
			  fontFamily, fontStyle, text);
    }
    else if(ajStrMatchC(labelStyle, "SquareBracketed") || 
	    ajStrMatchC(labelStyle, "Arrowed") )
    {
	ajXmlAddArc(file, xCentre, yCentre, startAngle, endAngle, 
		    radius);
        ajXmlAddTextOnArc(file, xCentre, yCentre, startAngle, 
			  endAngle, (radius + textPosition), size, 
			  fontFamily, fontStyle, text);

	if(ajStrMatchC(labelStyle, "SquareBracketed"))
	{
	    xStartInner = (sin(startAngle) * (radius - (width / 2)) + 
			   xCentre);
	    yStartInner = (cos(startAngle) * (radius - (width / 2)) + 
			   yCentre);
	    xEndInner = (sin(endAngle) * (radius - (width / 2)) + 
			 xCentre);
	    yEndInner = (cos(endAngle) * (radius - (width / 2)) + 
			 yCentre);
	    xStartOuter = (sin(startAngle) * (radius + (width / 2)) + 
			   xCentre);
	    yStartOuter = (cos(startAngle) * (radius + (width / 2)) + 
			   yCentre);
	    xEndOuter = (sin(endAngle) * (radius + (width / 2)) + 
			 xCentre);
	    yEndOuter = (cos(endAngle) * (radius + (width / 2)) + 
			 yCentre);
	    
	    xStartInnerLabel = (sin(startAngle - labelArmAngle) * 
				(radius - (width / 2)) + xCentre);
	    yStartInnerLabel = (cos(startAngle - labelArmAngle) * 
				(radius - (width / 2)) + yCentre);
	    xEndInnerLabel = (sin(endAngle + labelArmAngle) * 
			      (radius - (width / 2)) + xCentre);
	    yEndInnerLabel = (cos(endAngle + labelArmAngle) * 
			      (radius - (width / 2)) + yCentre);
	    xStartOuterLabel = (sin(startAngle - labelArmAngle) * 
				(radius + (width / 2)) + xCentre);
	    yStartOuterLabel = (cos(startAngle - labelArmAngle) * 
				(radius + (width / 2)) + yCentre);
	    xEndOuterLabel = (sin(endAngle + labelArmAngle) * 
			      (radius + (width / 2)) + xCentre);
	    yEndOuterLabel = (cos(endAngle + labelArmAngle) * 
			      (radius + (width / 2)) + yCentre);
	    
	    ajXmlAddLine(file, xStartInner, yStartInner, xStartOuter, 
			 yStartOuter);
	    ajXmlAddLine(file, xStartInner, yStartInner, 
			 xStartInnerLabel, yStartInnerLabel);
	    ajXmlAddLine(file, xStartOuter, xStartOuter, 
			 xStartOuterLabel,yStartOuterLabel );
	    ajXmlAddLine(file, xEndInner, yEndInner, xEndOuter, 
			 yEndOuter);
	    ajXmlAddLine(file, xEndInner, yEndInner, xEndInnerLabel, 
			 yEndInnerLabel);
	    ajXmlAddLine(file, xEndOuter, xEndOuter, xEndOuterLabel,
			 yEndOuterLabel );
	}
	if(ajStrMatchC(labelStyle, "Arrowed"))
	{
	    xStartInner = (sin(startAngle) * radius + xCentre);
	    yStartInner = (cos(startAngle) * radius + yCentre);
	    xEndInner = (sin(endAngle) * radius + xCentre);
	    yEndInner = (cos(endAngle) * radius + yCentre);
	    
	    xStartInnerLabel = (sin(startAngle + labelArmAngle) * 
				(radius - (width / 2)) + xCentre);
	    yStartInnerLabel = (cos(startAngle + labelArmAngle) * 
				(radius - (width / 2)) + yCentre);
	    xEndInnerLabel = (sin(endAngle - labelArmAngle) * 
			      (radius - (width / 2)) + xCentre);
	    yEndInnerLabel = (cos(endAngle - labelArmAngle) * 
			      (radius - (width / 2)) + yCentre);
	    xStartOuterLabel = (sin(startAngle + labelArmAngle) * 
				(radius + (width / 2)) + xCentre);
	    yStartOuterLabel = (cos(startAngle + labelArmAngle) * 
				(radius + (width / 2)) + yCentre);
	    xEndOuterLabel = (sin(endAngle - labelArmAngle) * 
			      (radius + (width / 2)) + xCentre);
	    yEndOuterLabel = (cos(endAngle - labelArmAngle) * 
			      (radius + (width / 2)) + yCentre);
	
	    ajXmlAddLine(file, xStartInner, yStartInner, 
			 xStartOuterLabel, yStartOuterLabel);
	    ajXmlAddLine(file, xStartInner, yStartInner, 
			 xStartInnerLabel, yStartInnerLabel);
	    ajXmlAddLine(file, xEndInner, yEndInner, xEndOuterLabel, 
			 yEndOuterLabel);
	    ajXmlAddLine(file, xEndInner, yEndInner, xEndInnerLabel, 
			 yEndInnerLabel);
	}	
    }

    ajStrDel(&textPrinted);
    ajStrDel(&nameString);

    return ajTrue;
}




/* @func ajXmlAddPointLabelLinear ************************************
**
** adds a Point Label on a linear DNA plot
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] angle [double] the angle of label to add
** @param [r] xPoint [double] the x value of the label
** @param [r] yPoint [double] the y value of the label
** @param [r] length [double] the length of the line of the label
** @param [r] textParallelToLine [AjBool] ajTrue if the text if 
** Parrallel to the line, ajFalse if the line is at right angles to the 
** line
** @param [r] size [double] the font size
** @param [r] fontFamily [AjPStr] the font Family 
** @param [r] fontStyle [AjPStr] the  font Style
** @param [r] text [AjPStr] the text of label to add
**
** @return [AjBool] ajTrue if correctly added
** @@
*********************************************************************/
AjBool ajXmlAddPointLabelLinear(AjPXmlFile file, double angle, 
				double xPoint, double yPoint, 
				double length, 
				AjBool textParallelToLine, 
				double size, AjPStr fontFamily, 
				AjPStr fontStyle, AjPStr text)
{
    double xEnd;
    double yEnd;
    double textXStart;
    double textYStart;
    double textAngle;

    AjPStr textPrinted = NULL;
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;
    
    nameString = ajStrNewC("DNAPlot");
    if(!xml_PressentGraphicTypeIs(file, nameString))
	return ajFalse;

    xEnd = (sin(angle) * length) + xPoint;
    yEnd = (cos(angle) * length) + yPoint;
    textXStart = (sin(angle) * length * 2) + xPoint;
    textYStart = (cos(angle) * length * 2) + yPoint;
    
    if(textParallelToLine)
	textAngle = angle;
    else
	textAngle = angle + (acos(0) / 2);

    if(xml_AngleIsInSecondHalfOfCircle(textAngle))
    {
	textPrinted = ajStrNewS(text);
	ajStrRev(&textPrinted);
    }
    else
	textPrinted = ajStrNewS(text);


    ajXmlAddLine(file, xPoint, yPoint, xEnd, yEnd);
    if(textParallelToLine)
    {
	nameString = ajStrNewC("BEGIN");
	nameString2 = ajStrNewC("MIDDLE");
	ajXmlAddTextWithJustify(file, textXStart, textYStart, size, 
				(textAngle - acos(0)), fontFamily, 
				fontStyle, textPrinted, ajTrue, ajTrue, 
				ajTrue, nameString, nameString2);
    }
    else
    {
	nameString = ajStrNewC("MIDDLE");
	ajXmlAddTextWithJustify(file, textXStart, textYStart, size, 
				(textAngle - acos(0)), fontFamily, 
				fontStyle, textPrinted, ajTrue, ajTrue, 
				ajTrue, nameString, nameString);
    }
    
    ajStrDel(&textPrinted);
    ajStrDel(&nameString);
    ajStrDel(&nameString2);

    return ajTrue;
}




/* @func ajXmlAddSectionLabelLinear **********************************
**
** adds a Section Label to Linear DNA Plot
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] xStart [double] the start x value of label to add
** @param [r] yStart [double] the start y value of label to add
** @param [r] xEnd [double] the end x value of label to add
** @param [r] yEnd [double] the end y value of label to add
** @param [r] width [double] the width of the label
** @param [r] labelArmLength [double] the distance the arms go back 
** (ignored if labelStyle = Block
** @param [r] labelStyle [AjPStr] the label style
** @param [r] textPosition [double] the text Position relative to the
** circle
** @param [r] size [double] the font size
** @param [r] fontFamily [AjPStr] the font Family 
** @param [r] fontStyle [AjPStr] the  font Style
** @param [r] text [AjPStr] the text of label to add
**
** @return [AjBool] ajTrue if correctly added
** @@
*********************************************************************/
AjBool ajXmlAddSectionLabelLinear(AjPXmlFile file, double xStart, 
				  double yStart, double xEnd, 
				  double yEnd, double width, 
				  double labelArmLength, 
				  AjPStr labelStyle, 
				  double textPosition, double size, 
				  AjPStr fontFamily, AjPStr fontStyle,
				  AjPStr text)
{
    double xStartInner;
    double yStartInner;
    double xEndInner;
    double yEndInner;
    double textXStart;
    double textYStart;

    double xStartOuter;
    double yStartOuter;
    double xEndOuter;
    double yEndOuter;

    double xChange;
    double yChange;
    double angle;
    
    double xStartLabel;
    double xEndLabel;
    double yOuter;
    double yInner;

    AjPStr textPrinted = NULL;
    /*    
       if(!xml_PressentGraphicTypeIs(file, ajStrNewC("DNAPlot")))
       {
       printf"Not DNA plot");
       return ajFalse;
       } 
       */
    xChange = xEnd - xStart;
    yChange = yEnd - yStart;
    angle = atan(width / labelArmLength);

    if(yChange < 0)
	angle += (acos(0) * 2);

    textXStart = xStart + (sin(angle) * textPosition);
    textYStart = yStart + (sin(angle) * textPosition);

    xStartInner = xStart - (sin(angle) * (width / 2));
    yStartInner = yStart - (cos(angle) *(width / 2));
    xEndInner = xEnd - (sin(angle) *(width / 2));
    yEndInner = yEnd - (cos(angle) *(width / 2));
    xStartOuter = xStart + (sin(angle) *(width / 2));
    yStartOuter = yStart + (cos(angle) *(width / 2));
    xEndOuter = xEnd + (sin(angle) *(width / 2));
    yEndOuter = yEnd + (cos(angle) *(width / 2));

    if(text != NULL)
    {
	ajXmlAddTextCentred(file, textXStart, textYStart, size, angle,
			    fontFamily, fontStyle, text);
    }
    
    

    if(ajStrMatchC(labelStyle, "Cylinder"))
	ajXmlAddCylinder(file, xStart, yStart, xEnd, yEnd, width);
    else if(ajStrMatchC(labelStyle, "Block"))
    {
	ajXmlAddLine(file, xStartInner, yStartInner, xStartOuter, 
		     yStartOuter);
	ajXmlAddLine(file, xStartOuter, yStartOuter, xEndOuter, 
		     yEndOuter);
	ajXmlAddLine(file, xEndInner, yEndInner, xEndOuter, 
		     yEndOuter);
	ajXmlAddLine(file, xStartInner, yStartInner, xEndInner, 
		     yEndInner);
    }
    else if(ajStrMatchC(labelStyle, "SquareBracketed") || 
	    ajStrMatchC(labelStyle, "Arrowed") || 
	    ajStrMatchC(labelStyle, "Lined") )
    {
	ajXmlAddLine(file, xStart, yStart, xEnd, yEnd);

	xStartLabel = xStart + 2 * (sin(angle) * 
				    (labelArmLength / 2));
	xEndLabel = xEnd - 2 * (sin(angle) * (labelArmLength / 2));
	yOuter = yStart + 2 * (cos(angle) * (labelArmLength / 2));
	yInner = yStart - 2 * (cos(angle) * (labelArmLength / 2));

	if(ajStrMatchC(labelStyle, "SquareBracketed"))
	{
	    ajXmlAddLine(file, xStart, yInner, xStart, yOuter);
	    ajXmlAddLine(file, xStart, yInner, xStartLabel, yInner);
	    ajXmlAddLine(file, xStart, yOuter, xStartLabel,yOuter);
	    ajXmlAddLine(file, xEnd, yInner, xEnd, yOuter);
	    ajXmlAddLine(file, xEnd, yInner, xEndLabel, yInner);
	    ajXmlAddLine(file, xEnd, yOuter, xEndLabel,yOuter);
	}
	if(ajStrMatchC(labelStyle, "Arrowed"))
	{
	    ajXmlAddLine(file, xStart, yStart, xStartLabel, yOuter);
	    ajXmlAddLine(file, xStart, yStart, xStartLabel, yInner);
	    ajXmlAddLine(file, xEnd, yEnd, xEndLabel, yOuter);
	    ajXmlAddLine(file, xEnd, yEnd, xEndLabel, yInner);
	}
	if(ajStrMatchC(labelStyle, "Lined"))
	{
	    ajXmlAddLine(file, xStart, yOuter, xStart, yInner);
	}
    }
    else if(ajStrMatchC(labelStyle, "ReverseSquareBracketed") ||
	    ajStrMatchC(labelStyle, "ReverseArrowed") )
    {
	ajXmlAddLine(file, xStart, yStart, xEnd, yEnd);

	xStartLabel = xStart - 2 * (sin(angle) * 
				    (labelArmLength / 2));
	xEndLabel = xEnd + 2 * (sin(angle) * (labelArmLength / 2));
	yOuter = yStart + 2 * (cos(angle) * (labelArmLength / 2));
	yInner = yStart - 2 * (cos(angle) * (labelArmLength / 2));

	if(ajStrMatchC(labelStyle, "ReverseSquareBracketed"))
	{
	    ajXmlAddLine(file, xStart, yInner, xStart, yOuter);
	    ajXmlAddLine(file, xStart, yInner, xStartLabel, yInner);
	    ajXmlAddLine(file, xStart, yOuter, xStartLabel,yOuter);
	    ajXmlAddLine(file, xEnd, yInner, xEnd, yOuter);
	    ajXmlAddLine(file, xEnd, yInner, xEndLabel, yInner);
	    ajXmlAddLine(file, xEnd, yOuter, xEndLabel,yOuter);
	}
	if(ajStrMatchC(labelStyle, "ReverseArrowed"))
	{
	    ajXmlAddLine(file, xStart, yStart, xStartLabel, yOuter);
	    ajXmlAddLine(file, xStart, yStart, xStartLabel, yInner);
	    ajXmlAddLine(file, xEnd, yEnd, xEndLabel, yOuter);
	    ajXmlAddLine(file, xEnd, yEnd, xEndLabel, yInner);
	}
    }

    ajStrDel(&textPrinted);

    return ajTrue;
}




/* @func ajXmlAddSquareResidue ***************************************
**
** adds a residue letter in a square
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] xStart [double] the start x value of label to add
** @param [r] yStart [double] the start y value of label to add
**
** @return [AjBool] ajTrue if correctly added
** @@
*********************************************************************/
AjBool ajXmlAddSquareResidueLinear(AjPXmlFile file, char residue, 
				   float x, float y)
{ 
    float *xCoords;
    float *yCoords;
     
    AJCNEW(xCoords, 5);
    AJCNEW(yCoords, 5);
    
    xCoords[0] = x - RESIDUE_SQUARE_SIZE;
    xCoords[1] = x + RESIDUE_SQUARE_SIZE;
    xCoords[2] = x + RESIDUE_SQUARE_SIZE;
    xCoords[3] = x - RESIDUE_SQUARE_SIZE;
    xCoords[4] = x - RESIDUE_SQUARE_SIZE;
    
    yCoords[0] = y - RESIDUE_SQUARE_SIZE;
    yCoords[1] = y - RESIDUE_SQUARE_SIZE;
    yCoords[2] = y + RESIDUE_SQUARE_SIZE;
    yCoords[3] = y + RESIDUE_SQUARE_SIZE;
    yCoords[4] = y - RESIDUE_SQUARE_SIZE;
    
    ajXmlAddJoinedLineSetF(file, xCoords, yCoords, 5);

    ajXmlAddNakedResidueLinear(file, residue, x, y);
    
    AJFREE(xCoords);
    AJFREE(yCoords);

    return ajTrue;
}




/* @func ajXmlAddOctagonalResidue ************************************
**
** adds a residue letter in a square
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] xStart [double] the start x value of label to add
** @param [r] yStart [double] the start y value of label to add
**
** @return [AjBool] ajTrue if correctly added
** @@
*********************************************************************/
AjBool ajXmlAddOctagonalResidueLinear(AjPXmlFile file, char residue, 
				      float x, float y)
{
    float *xCoords;
    float *yCoords;

    AJCNEW(xCoords, 9);
    AJCNEW(yCoords, 9);
    
    xCoords[0] = x - (0.5 * RESIDUE_SQUARE_SIZE);
    xCoords[1] = x + (0.5 * RESIDUE_SQUARE_SIZE);
    xCoords[2] = x + RESIDUE_SQUARE_SIZE;
    xCoords[3] = x + RESIDUE_SQUARE_SIZE;
    xCoords[4] = x + (0.5 * RESIDUE_SQUARE_SIZE);
    xCoords[5] = x - (0.5 * RESIDUE_SQUARE_SIZE);
    xCoords[6] = x - RESIDUE_SQUARE_SIZE;
    xCoords[7] = x - RESIDUE_SQUARE_SIZE;
    xCoords[8] = x - (0.5 * RESIDUE_SQUARE_SIZE);
    
    yCoords[0] = y + RESIDUE_SQUARE_SIZE;
    yCoords[1] = y + RESIDUE_SQUARE_SIZE;
    yCoords[2] = y + (0.5 * RESIDUE_SQUARE_SIZE);
    yCoords[3] = y - (0.5 * RESIDUE_SQUARE_SIZE);
    yCoords[4] = y - RESIDUE_SQUARE_SIZE;
    yCoords[5] = y - RESIDUE_SQUARE_SIZE;
    yCoords[6] = y - (0.5 * RESIDUE_SQUARE_SIZE);
    yCoords[7] = y + (0.5 * RESIDUE_SQUARE_SIZE);
    yCoords[8] = y + RESIDUE_SQUARE_SIZE;
    
    ajXmlAddJoinedLineSetF(file, xCoords, yCoords, 9);

    ajXmlAddNakedResidueLinear(file, residue, x, y);
    
    AJFREE(xCoords);
    AJFREE(yCoords);

    return ajTrue;
}




/* @func ajXmlAddDiamondResidue **************************************
**
** adds a residue letter in a square
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] xStart [double] the start x value of label to add
** @param [r] yStart [double] the start y value of label to add
**
** @return [AjBool] ajTrue if correctly added
** @@
*********************************************************************/
AjBool ajXmlAddDiamondResidueLinear(AjPXmlFile file, char residue, 
				    float x, float y)
{    
    float *xCoords;
    float *yCoords;

    AJCNEW(xCoords, 5);
    AJCNEW(yCoords, 5);
    
    xCoords[0] = x;
    xCoords[1] = x + (1.41 * RESIDUE_SQUARE_SIZE);
    xCoords[2] = x;
    xCoords[3] = x - (1.41 * RESIDUE_SQUARE_SIZE);
    xCoords[4] = x;
    
    yCoords[0] = y + (1.41 * RESIDUE_SQUARE_SIZE);
    yCoords[1] = y;
    yCoords[2] = y - (1.41 * RESIDUE_SQUARE_SIZE);
    yCoords[3] = y;
    yCoords[4] = y + (1.41 * RESIDUE_SQUARE_SIZE);
    
    ajXmlAddJoinedLineSetF(file, xCoords, yCoords, 5);

    ajXmlAddNakedResidueLinear(file, residue, x, y);
    
    AJFREE(xCoords);
    AJFREE(yCoords);

    return ajTrue;
}




/* @func ajXmlAddNakedResidueLinear **********************************
**
** adds a residue letter in a square
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] xStart [double] the start x value of label to add
** @param [r] yStart [double] the start y value of label to add
**
** @return [AjBool] ajTrue if correctly added
** @@
*********************************************************************/
AjBool ajXmlAddNakedResidueLinear(AjPXmlFile file, char residue, 
				  float x, float y)
{    
    static char residueString[2];
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;

    residueString[1]='\0';
    *residueString=residue;

    nameString = ajStrNewC("TYPEWRITER");
    nameString2 = ajStrNewC(residueString);
    ajXmlAddText(file, (x - 1.2), (y - 1.2), 4.0, 0, nameString,
		 NULL, nameString2);
    
    ajStrDel(&nameString);
    ajStrDel(&nameString2);

    return ajTrue;
}




/* @func ajXmlAddSquareResidue ***************************************
**
** adds a residue letter in a square
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] xStart [double] the start x value of label to add
** @param [r] yStart [double] the start y value of label to add
**
** @return [AjBool] ajTrue if correctly added
** @@
*********************************************************************/
AjBool ajXmlAddSquareResidue(AjPXmlFile file, char residue, 
			     double radius, double angle)
{ 
    static float squareSize;
    
    float  x;
    float  y;
 
    float *xCoords;
    float *yCoords;

    squareSize = 0.03;
     
    ajPolToRec(radius, angle, &x, &y);

    AJCNEW(xCoords, 5);
    AJCNEW(yCoords, 5);
    
    xCoords[0] = x - squareSize;
    xCoords[1] = x + squareSize;
    xCoords[2] = x + squareSize;
    xCoords[3] = x - squareSize;
    xCoords[4] = x - squareSize;
    
    yCoords[0] = y - squareSize;
    yCoords[1] = y - squareSize;
    yCoords[2] = y + squareSize;
    yCoords[3] = y + squareSize;
    yCoords[4] = y - squareSize;
    
    /*    ajXmlAddJoinedLineSetF(file, xCoords, yCoords, 5); */

    ajXmlAddNakedResidue(file, residue, radius, angle);
    
    AJFREE(xCoords);
    AJFREE(yCoords);

    return ajTrue;
}




/* @func ajXmlAddOctagonalResidue ************************************
**
** adds a residue letter in a square
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] xStart [double] the start x value of label to add
** @param [r] yStart [double] the start y value of label to add
**
** @return [AjBool] ajTrue if correctly added
** @@
*********************************************************************/
AjBool ajXmlAddOctagonalResidue(AjPXmlFile file, char residue, 
				double radius, double angle)
{
    float squareSize;
    
    float  x;
    float  y;
 
    float *xCoords;
    float *yCoords;

    squareSize = 0.03;
    
    ajPolToRec(radius, angle, &x, &y);

    AJCNEW(xCoords, 9);
    AJCNEW(yCoords, 9);
    
    xCoords[0] = x - (0.5 * squareSize);
    xCoords[1] = x + (0.5 * squareSize);
    xCoords[2] = x + squareSize;
    xCoords[3] = x + squareSize;
    xCoords[4] = x + (0.5 * squareSize);
    xCoords[5] = x - (0.5 * squareSize);
    xCoords[6] = x - squareSize;
    xCoords[7] = x - squareSize;
    xCoords[8] = x - (0.5 * squareSize);
    
    yCoords[0] = y + squareSize;
    yCoords[1] = y + squareSize;
    yCoords[2] = y + (0.5 * squareSize);
    yCoords[3] = y - (0.5 * squareSize);
    yCoords[4] = y - squareSize;
    yCoords[5] = y - squareSize;
    yCoords[6] = y - (0.5 * squareSize);
    yCoords[7] = y + (0.5 * squareSize);
    yCoords[8] = y + squareSize;
    
    ajXmlAddJoinedLineSetF(file, xCoords, yCoords, 9);

    ajXmlAddNakedResidue(file, residue, radius, angle);
    
    AJFREE(xCoords);
    AJFREE(yCoords);

    return ajTrue;
}




/* @func ajXmlAddDiamondResidue **************************************
**
** adds a residue letter in a square
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] xStart [double] the start x value of label to add
** @param [r] yStart [double] the start y value of label to add
**
** @return [AjBool] ajTrue if correctly added
** @@
*********************************************************************/
AjBool ajXmlAddDiamondResidue(AjPXmlFile file, char residue, 
			      double radius, double angle)
{    
    float squareSize;
    
    float  x;
    float  y;
 
    float *xCoords;
    float *yCoords;

    squareSize = 0.03;

    ajPolToRec(radius, angle, &x, &y);

    AJCNEW(xCoords, 5);
    AJCNEW(yCoords, 5);
    
    xCoords[0] = x;
    xCoords[1] = x + (1.41 * squareSize);
    xCoords[2] = x;
    xCoords[3] = x - (1.41 * squareSize);
    xCoords[4] = x;
    
    yCoords[0] = y + (1.41 * squareSize);
    yCoords[1] = y;
    yCoords[2] = y - (1.41 * squareSize);
    yCoords[3] = y;
    yCoords[4] = y + (1.41 * squareSize);
    
    ajXmlAddJoinedLineSetF(file, xCoords, yCoords, 5);

    ajXmlAddNakedResidue(file, residue, radius, angle);
    
    AJFREE(xCoords);
    AJFREE(yCoords);

    return ajTrue;
}




/* @func ajXmlAddNakedResidue ****************************************
**
** adds a residue letter in a square
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] xStart [double] the start x value of label to add
** @param [r] yStart [double] the start y value of label to add
**
** @return [AjBool] ajTrue if correctly added
** @@
*********************************************************************/
AjBool ajXmlAddNakedResidue(AjPXmlFile file, char residue, 
			    double radius, double angle)
{    
    float  x;
    float  y;
    static char residueString[2];
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;

    residueString[1]='\0';
    *residueString=residue;

    ajPolToRec(radius, angle, &x, &y);

    nameString = ajStrNewC("TYPEWRITER");
    nameString2 = ajStrNewC(residueString);
    ajXmlAddText(file, (x - 0.018), (y - 0.018), 0.07, 0, nameString,
		 NULL, nameString2);
    
    ajStrDel(&nameString);
    ajStrDel(&nameString2);

    return ajTrue;
}




/* @func ajXmlFitTextOnLine ******************************************
**
** Computes the character size (in vrml terms) needed to write a text 
** string with specified length (in world meters).  The length 
** of the string is the distance between (x1,y1) and (x2,y2)
**
** @param [r] x1 [PLFLT] x1 coor.
** @param [r] y1 [float] y1 coor.
** @param [r] x2 [float] x2 coor.
** @param [r] y2 [float] y2 coor.
** @param [r] text [char*] The text to be displayed.
** @return [float] The character size (in mm) that fits the specified
**                 height and length.
** @@
*********************************************************************/
float ajXmlFitTextOnLine(float x1, float y1, float x2, float y2, 
			 AjPStr text)
{
    float lineLength;
    ajint textLength;
    
    textLength = ajStrLen(text);

    lineLength = pow((pow((x2 - x1), 2) + pow((y2 - y1), 2)), 0.5);
    
    return (lineLength / textLength) * ONE_METER_FONT;
}




/* @func ajXmlGetColour **********************************************
**
** gets Colour
**
** @param [r] file [AjPXmlFile] the file get colour of
** @param [r] r [double] red component
** @param [r] g [double] green component
** @param [r] b [double] blue component
**
** @return [void]
** @@
*********************************************************************/
void ajXmlGetColour(AjPXmlFile file, double r, double g, double b)
{
    r = file->colour[0];
    g = file->colour[1];
    b = file->colour[2];

    return;
}




/* @func ajXmlSetColour **********************************************
**
** sets Colour
**
** @param [w] file [AjPXmlFile] the file to alter
** @param [r] r [double] red component
** @param [r] g [double] green component
** @param [r] b [double] blue component
**
** @return [void]
** @@
*********************************************************************/
void ajXmlSetColour(AjPXmlFile file, double r, double g, double b)
{
    /* should I make a private function do this? Hardly worth it,*/
    file->colour[0] = r;
    file->colour[1] = g;
    file->colour[2] = b;

    return;
}




/* @func ajXmlSetColourFromCode *************************************
**
** sets Colour from the code.  Codes are -
**
** 0 black (default background)
** 1 red
** 2 yellow
** 3 green
** 4 aquamarine
** 5 pink
** 6 wheat
** 7 grey
** 8 brown
** 9 blue
** 10 BlueViolet
** 11 cyan
** 12 turquoise
** 13 magenta
** 14 salmon
** 15 white (default foreground)
**
** @param [w] file [AjPXmlFile] the file to alter
** @param [r] colour [ajint] colour
**
** @return [void]
** @@
*********************************************************************/
void ajXmlSetColourFromCode(AjPXmlFile file, ajint colour)
{
    switch (colour)
    {
    case 0:
	ajXmlSetColour(file, 0.0, 0.0, 0.0);
	break;
    case 1:
	ajXmlSetColour(file, 1.0, 0.0, 0.0);
 	break;
    case 2:
	ajXmlSetColour(file, 1.0, 1.0, 0.0);
	break;
    case 3:
	ajXmlSetColour(file, 0.0, 1.0, 0.0);
	break;
    case 4:
	ajXmlSetColour(file, 0.0, 1.0, 1.0);
	break;
    case 5:
	ajXmlSetColour(file, 0.8, 0.2, 0.2);
	break;
    case 6:
	ajXmlSetColour(file, 0.5, 0.5, 0.1);
	break;
    case 7:
	ajXmlSetColour(file, 0.5, 0.5, 0.5);
	break;
    case 8:
	ajXmlSetColour(file, 0.4, 0.4, 0.4);
	break;
    case 9:
	ajXmlSetColour(file, 0.0, 0.0, 1.0);
	break;
    case 10:
	ajXmlSetColour(file, 1, 0, 1);
	break;
    case 11:
	ajXmlSetColour(file, 0.2, 0.2, 0.8);
	break;
    case 12:
	ajXmlSetColour(file, 0.2, 0.8, 0.2);
	break;
    case 13:
	ajXmlSetColour(file, 0.6, 0.4, 0.2);
	break;
    case 14:
	ajXmlSetColour(file, 0.5, 0.2, 0.2);
	break;
    case 15:
	ajXmlSetColour(file, 1, 1, 1);
	break;
    default:
	break;
    }    

    return;
}




/* @func ajXmlCreateNewOutputFile ************************************
**
** Sets up the output file 
**
** @return [AjPXmlFile] a colection of pointers to the file created
** @@
*********************************************************************/
AjPXmlFile ajXmlCreateNewOutputFile()
{
    return xml_CreateNewOutputFile();
}




/* @func ajXmlAddGraphicC ********************************************
**
** adds a general graphics group from char*
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] type [char*] the type of graphic to add
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddGraphicC(AjPXmlFile file, char *type)
{
    AjPStr nameString=NULL;
  
    nameString = ajStrNewC(type);
    ajXmlAddGraphic(file, nameString);
    ajStrDel(&nameString);

    return;
}




/* @func ajXmlAddGraphic *********************************************
**
** adds a general graphics group
**
** @param [w] file [AjPXmlFile] the file to add the object to
** @param [r] type [ajPStr] the type of graphic to add
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddGraphic(AjPXmlFile file, AjPStr type)
{
    AjPXmlNode el = NULL;
    /*  AjPXmlNode elChildren = NULL; */
    /*  AjPStr name = NULL; */
    AjPStr nameString=NULL;

    if(type == NULL)
    {
	/* I create a new element, the group */
	nameString = ajStrNewC("Group");
	el = xml_MakeNewNode(file, nameString, xml_GetCurrentScene(file));

	/* I create a new element, the children */
	/* No I do not.  This is no longer nessasery */
	/*
	   ajStrDel(&nameString);
	   nameString =ajStrNewC("children");
	   elChildren = xml_MakeNewNode(file, nameString, el);
	   */

	/* I move file->currentGraphic to the children */
	/* note - cos you do this you cannot unref elChildren */
	xml_SetCurrentGraphic(file, el);
	/*
	   xml_UnrefNode(el);
	   */
    } 
    else 
    {
	if(ajStrMatchC(type, "Graph")) 
	{
	    nameString = ajStrNewC("Graph");
	    if(xml_FileNeedsProtoDeclare(file, nameString))
		xml_AddGraphProto(file);
	    ajStrDel(&nameString);
	    nameString = ajStrNewC("ProtoInstance");
	    el = xml_MakeNewNode(file, nameString, file->currentScene);
	    ajStrDel(&nameString);
	    nameString = ajStrNewC("name");
	    xml_SetAttribute(el, nameString, type);
	    xml_SetCurrentGraphic(file, el);   
	}
	else 
	    if(ajStrMatchC(type, "Histogram")) 
	    {
		nameString = ajStrNewC("Graph");
		if(xml_FileNeedsProtoDeclare(file, nameString))
		    xml_AddGraphProto(file);
		el = xml_MakeNewNode(file, type, file->currentScene);
		xml_SetCurrentGraphic(file, el);   
	    }
	    else 
	    {
		if(ajStrMatchC(type, "DNAPlot")) 
		{
		    nameString = ajStrNewC("DNAPlot");
		    if(xml_FileNeedsProtoDeclare(file, nameString))
			xml_AddDNAPlotProto(file);
		    el = xml_MakeNewNode(file, type, file->currentScene);
		    xml_SetCurrentGraphic(file, el);   
		}
	    }
    }

    ajTableFree(&file->nodeTypes);
    file->nodeTypes = ajStrTableNew(1);

    /*  el unrefed above if type == NULL */
    ajStrDel(&nameString);
    /*  gdome_n_unref(elChildren, &exc); */

    return;
}




/* @func ajXmlAddArc  ***********************************************
**
** adds an arc
**
** @param [w] file [AjPXmlFile] the file to add the max min values to
** @param [r] xCentre [double] the x value of the centre of the 
** circle
** @param [r] yCentre [double] the y value of the centre of the 
** circle
** @param [r] startAngle [double] the startAngle of the arc
** @param [r] endAngle [double] the endAngle of the arc
** @param [r] radius [double] the radius of the circle
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddArc(AjPXmlFile file, double xCentre, double yCentre, 
		 double startAngle, double endAngle, double radius)
{
    double angularExtent;
    double modAngularExtent;
    double arcCentreAngle;
    
    angularExtent = endAngle - startAngle;
    modAngularExtent = pow(pow(angularExtent, 2), 0.5);
    
    if(xml_AngleIsInSecondHalfOfCircle(modAngularExtent))
    {
	arcCentreAngle = startAngle + (angularExtent / 2);
	xml_AddArc(file, xCentre, yCentre, startAngle, arcCentreAngle,
		   radius);
	xml_AddArc(file, xCentre, yCentre, arcCentreAngle, endAngle,
		   radius);
    }
    else
	xml_AddArc(file, xCentre, yCentre, startAngle, endAngle,
		   radius);

    return;
}




/* @func ajXmlAddCircle  *********************************************
**
** adds a circle
**
** @param [w] file [AjPXmlFile] the file to add the max min values to
** @param [r] xCentre [double] the x value of the centre of the 
** circle
** @param [r] yCentre [double] the y value of the centre of the 
** circle
** @param [r] radius [double] the radius of the circle
**
** @return [void]
** @@
******************************************************************************/
void ajXmlAddCircleF(AjPXmlFile file, float xCentre, float yCentre, 
		     float radius)
{
    ajXmlAddCircle(file, (double) xCentre, (double) yCentre, 
		   (double) radius);

    return;
}




/* @func ajXmlAddCircle  *********************************************
**
** adds a circle
**
** @param [w] file [AjPXmlFile] the file to add the max min values to
** @param [r] xCentre [double] the x value of the centre of the 
** circle
** @param [r] yCentre [double] the y value of the centre of the 
** circle
** @param [r] radius [double] the radius of the circle
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddCircle(AjPXmlFile file, double xCentre, double yCentre, 
		    double radius)
{
    AjPStr temp = NULL;
    AjPStr controlPoints = NULL;
    AjPStr weights = NULL;
    AjPXmlNode circleNode;

    double controlPointsDbs[14] = 
    {
	0, 1, 
	pow(3, 0.5), 1, 
	pow(0.75, 0.5), -0.5, 
	0, 2,
	-pow(0.75, 0.5), -0.5,
	-pow(3, 0.5), 1, 
	0, 1, 
    };
    int i;
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;

    temp = ajStrNew();
    controlPoints = ajStrNew();
    weights = ajStrNew();

    for(i=0; i<14; ++i)
	controlPointsDbs[i] *= radius;

    for(i=0; i<14; i+=2)
    {
	controlPointsDbs[i] += xCentre;
	controlPointsDbs[i+1] += yCentre;
    }
    for(i=0; i<14; i+=2)
	xml_AddACoord(controlPointsDbs[i], controlPointsDbs[i+1], 
		      ajFalse, &controlPoints, &temp);
    
    nameString = ajStrNewC("NurbsCurve2D");
    circleNode = xml_MakeNewShapeNode(file, 
				      xml_GetCurrentGraphic(file), 
				      nameString);
    nameString = nameString;
    nameString2 = ajStrNewC("0 0 0 1 1 2 2 3 3 3");
    xml_SetAttribute(circleNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("order");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("3");
    xml_SetAttribute(circleNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("controlPoint");
    xml_SetAttribute(circleNode, nameString, controlPoints);
    ajStrDel(&nameString);
    nameString = ajStrNewC("weight");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("1 0.5 1 0.5 1 0.5 1");
    xml_SetAttribute(circleNode, nameString, nameString2);

    ajStrDel(&nameString);
    ajStrDel(&nameString2);

    return;
}




/* @func ajXmlAddGroutOption  ****************************************
**
** adds an option value pair for the Grout display
**
** @param [w] file [AjPXmlFile] the file to add the max min values to
** @param [r] name [AjPStr] name of option
** @param [r] value [AjPStr] value of option
**
** @return [void]
** @@
*********************************************************************/
void ajXmlAddGroutOption(AjPXmlFile file, AjPStr name, AjPStr value)
{
    xml_AddGroutOption(file, name, value);
    return;
}




/* @func xml_AddGroutOption  ****************************************
**
** adds an option value pair for the Grout display
**
** @param [w] file [AjPXmlFile] the file to add the max min values to
** @param [r] name [AjPStr] name of option
** @param [r] value [AjPStr] value of option
**
** @return [void]
** @@
*********************************************************************/
void xml_AddGroutOption(AjPXmlFile file, AjPStr name, AjPStr value)
{
    AjPStr temp = NULL;
    AjPXmlNode headNode = NULL;
    AjPXmlNode otherNode = NULL;
    GdomeException exc;
    int i;
    GdomeNodeList *listShapes = NULL;
    GdomeDOMString *nodeName;

    otherNode = xml_SetNode((GdomeNode *) gdome_doc_documentElement 
			    (file->doc, &exc));
    nodeName = gdome_str_mkref("head");
    listShapes = gdome_el_getElementsByTagName
	((xml_GetNodeElement(otherNode)), 
	 nodeName, &exc);
    for(i=0; (i<gdome_nl_length(listShapes, &exc) 
	      && headNode == NULL) ; ++i)
	headNode = xml_SetNode(gdome_nl_item(listShapes, i, &exc));


    if(headNode == NULL)
    {
	temp = ajStrNewC("head");
	headNode  = xml_MakeNewNode(file, temp, otherNode);
	ajStrDel(&temp);
    }
    
    temp = ajStrNewC("meta");
    xml_UnrefNode(otherNode);
    otherNode = xml_MakeNewNode(file, temp, headNode);
    ajStrDel(&temp);
    temp = ajStrNewC("name");
    xml_SetAttribute(otherNode, temp, name);
    ajStrDel(&temp);
    temp = ajStrNewC("content");
    xml_SetAttribute(otherNode, temp, value);

    ajStrDel(&temp);
    xml_UnrefNode(otherNode);
    xml_UnrefNode(headNode);
    gdome_str_unref(nodeName);
    gdome_nl_unref(listShapes, &exc);

    return;
}




/* @funcstatic xml_AddCylinder  ********************************************
**
** Adds a Cylinder
**
** @param [w] file [AjPXmlFile] the file to add the Cylinder to
** @param [r] xCentre [double] x coordinate of centre
** @param [r] yCentre [double] y coordinate of centre
** @param [r] angle [double] angle relative to vertical
** @param [r] height [double] height
** @param [r] width [double] width 
**
** @return [void]
** @@
*********************************************************************/
static void xml_AddCylinder(AjPXmlFile file, double xCentre, double yCentre, 
			    double angle, double height, double width)
{
    AjPStr translation;
    AjPStr rotation;
    AjPStr temp;
    AjPXmlNode transformNode;
    AjPXmlNode elCylinder;
    AjPStr nameString=NULL;
    
    xml_StrFromDouble(&translation, xCentre);
    ajStrAppC(&translation, " ");
    xml_StrFromDouble(&temp, yCentre);
    ajStrApp(&translation, temp);
    ajStrAppC(&translation, " 0");

    rotation = ajStrNewC("0 0 1 ");
    ajStrDel(&temp);
    xml_StrFromDouble(&temp, angle);
    ajStrApp(&rotation, temp);
    
    nameString = ajStrNewC("Transform");
    transformNode = xml_MakeNewNode(file,nameString, 
				    xml_GetCurrentGraphic(file));

    ajStrDel(&nameString);
    nameString = ajStrNewC("translation");
    xml_SetAttribute(transformNode, nameString, translation);
    ajStrDel(&nameString);
    nameString = ajStrNewC("rotation");
    xml_SetAttribute(transformNode, nameString, rotation);
  
    ajStrDel(&nameString);
    nameString = ajStrNewC("Cylinder");
    elCylinder = xml_MakeNewShapeNode(file, transformNode, 
				      nameString);

    ajStrDel(&temp);
    xml_StrFromDouble(&temp, height);
    ajStrDel(&nameString);
    nameString = ajStrNewC("height");
    xml_SetAttribute(elCylinder, nameString, temp);
    ajStrDel(&temp);
    xml_StrFromDouble(&temp, width);
    ajStrDel(&nameString);
    nameString = ajStrNewC("width");
    xml_SetAttribute(elCylinder, nameString, temp);

    ajStrDel(&temp);
    ajStrDel(&rotation);
    ajStrDel(&translation);

    xml_UnrefNode(elCylinder);
    xml_UnrefNode(transformNode);
    ajStrDel(&nameString);

    return;
}




/* @funcstatic xml_StrFromBool *********************************************
**
** converts an AjBool into a string
**
** @param [r] boole [AjBool] the AjBool
**
** @return [AjPStr] "ajTrue" or "FALSE" depending on the AjBool
** @@
*********************************************************************/
static AjPStr xml_StrFromBool(AjBool boole)
{
    AjPStr text = NULL;
    
    if(boole)
	text = ajStrNewC("TRUE");
    else
	text = ajStrNewC("FALSE");

    return text;
}




/* @funcstatic xml_StrFromDouble  ************************************
**
** Local Method
** retuns a string with the correct number of decimal places 
**
** @param [r] val [double] the double to change to string
** @param [w] result [AjPStr*] the string to change to double
**
** @return [AjBool] ajTrue if the string was correctly assigned
** @@
*********************************************************************/
static AjBool xml_StrFromDouble(AjPStr *result, double val)
{
    AjPStr whole=NULL;
    AjPStr signStr=NULL;
    AjPStr mantisa=NULL;
    AjPStr exponent=NULL;
    int sign;
    int i;
    int exponentInt;
    int decimalPlaces;
    AjBool iret;
    
    whole = ajStrNew();
    signStr = ajStrNew();
    mantisa = ajStrNew();
    exponent = ajStrNew();

    ajStrFromDoubleE(&whole, val, 14);

    ajStrAssSubC(&signStr, ajStrStr(whole), 0, 0);

    if(ajStrMatchC(signStr, "-"))
	sign = 1;
    else
	sign = 0;

    ajStrAssSubC(&mantisa, ajStrStr(whole), (0+sign), (15+sign));
    ajStrAssSubC(&exponent, ajStrStr(whole), (17+sign), (20+sign));
    ajStrToInt(exponent, &exponentInt);

    for(i=14; ajStrSuffixC(mantisa, "0"); --i)
	iret = ajStrChop(&mantisa);
    decimalPlaces = i-exponentInt;
    if(decimalPlaces<0)
	decimalPlaces = 0;

    ajStrDel(&whole);
    ajStrDel(&signStr);
    ajStrDel(&mantisa);
    ajStrDel(&exponent);

    return ajStrFromDouble(result, val, decimalPlaces); 
}




/* @funcstatic xml_AngleIsInSecondHalfOfCircle *****************************
**
** Calculates if this angle is in the second half of the circle
**
** @param [r] angle [double] the angle
**
** @return [AjBool] ajTrue if the angle is in the second half of the 
** circle
** @@
*********************************************************************/
static AjBool xml_AngleIsInSecondHalfOfCircle(double angle)
{
    int halfCount;
    int intHalfHalfCount;
    double halfHalfCount;
    double doubleIntHalfHalfCount;
    
    /* there must be a better way to do it */
    halfCount = (int) angle / (acos(0) * 2);
    halfHalfCount = ((double) halfCount) / 2.0;
    intHalfHalfCount = ((int) halfHalfCount);  
    doubleIntHalfHalfCount = ((double) intHalfHalfCount);
    
    if(halfHalfCount == doubleIntHalfHalfCount)
	return ajFalse;
    else
	return ajTrue;
}




/* @funcstatic xml_PressentGraphicTypeIs  ****************************
**
** Local Method
** 
**
** @param [w] file [AjPXmlFile] the file to add the node to
** @param [r] name [AjPStr] name graphic should be
**
** @return [AjBool] True if graphic type is name
** @@
*********************************************************************/
static AjBool xml_PressentGraphicTypeIs(AjPXmlFile file, AjPStr name)
{
    AjBool returnValue;
    AjPStr attributeName = NULL;
    AjPStr attributeValue = NULL;

    attributeName = ajStrNewC("name");
    attributeValue = xml_GetAttribute(xml_GetCurrentGraphic(file), 
				      attributeName);

    returnValue = ajStrMatch(attributeValue, name);
  
    if(ajStrMatchC(attributeValue, "") && (name == NULL))
	returnValue = ajTrue;

    printf("pointers attributeValue = %p name = %p\n",
	   (attributeValue), (name));
    fflush(stdout);
    printf("attributeValue = %s name = %s returnValue = %d\n",
	   ajStrStr(attributeValue), ajStrStr(name), returnValue);
    fflush(stdout);

    ajStrDel(&attributeValue);
    ajStrDel(&attributeName);

  

    return returnValue;
}




/* @funcstatic xml_GetAttribute  *************************************
**
** Local Method
** Sets an attribute
**
** @param [w] node [GdomeNode*] node to set attribute of
** @param [r] atName [AjPStr] name of attribute
**
** @return [AjPStr] value of attribute
** @@
*********************************************************************/
static AjPStr xml_GetAttribute(AjPXmlNode node, AjPStr atName)
{
    GdomeDOMString *attributeValue;
    GdomeException exc;
    AjPStr temp = NULL;
    GdomeDOMString *nodeName = NULL;
    
    nodeName = gdome_str_mkref_dup(ajStrStr(atName));
    attributeValue = gdome_el_getAttribute (xml_GetNodeElement(node),
					    nodeName, &exc);
    
    /* Do I need to unref the attributeValue? I guess yes so I do not 
       use the line below */
    /*
       return ajStrNewC(attributeValue->str); 
       */
    temp = ajStrNewC(attributeValue->str);
    gdome_str_unref(attributeValue);
    gdome_str_unref (nodeName);
    
    return temp;
}




/* @funcstatic xml_SetAttribute  ************************************
**
** Local Method
** Sets an attribute
**
** @param [w] node [GdomeNode*] node to set attribute of
** @param [r] atName [AjPStr] name of attribute
** @param [r] atValue [AjPStr] value of attribute
**
** @@
*********************************************************************/
static void xml_SetAttribute(AjPXmlNode node, AjPStr atName, AjPStr atValue)
{
    GdomeException exc;
    GdomeDOMString *nodeName = NULL;
    GdomeDOMString *nodeName2 = NULL;
    
    nodeName = gdome_str_mkref_dup(ajStrStr(atName));
    nodeName2 = gdome_str_mkref_dup(ajStrStr(atValue));
    gdome_el_setAttribute (xml_GetNodeElement(node), nodeName, 
			   nodeName2, &exc);
    gdome_str_unref(nodeName);
    gdome_str_unref(nodeName2);

    return;
}




/* @funcstatic xml_GetIndex  ****************************************
**
** Local Method
** Gets the list of coord index's for an Indexed Line Set
**
** @param [r] node [GdomeNode*] node to get the index's of
**
** @return [AjPStr] value of index's
** @@
*********************************************************************/
static AjPStr xml_GetIndex(AjPXmlNode passedNode)
{
    GdomeException exc;
    GdomeDOMString *nodeName = NULL;
    AjBool proto;
    AjPStr attributeName = NULL;
    AjPStr ajAttributeValue = NULL;
    AjPXmlNode tempNode = NULL;
    AjPXmlNode returnNode = NULL;
    AjPXmlNode node = NULL;
    AjPStr temp = NULL;
    AjPStr nameString = NULL;
    GdomeNodeList *listShapes = NULL;
    int i;


    proto = ajFalse;
    
    nodeName = gdome_el_tagName(xml_GetNodeElement(passedNode), &exc);
    printf("gdome_el_tagName xml_GetIndex = %s\n", 
	   gdome_el_tagName(xml_GetNodeElement(passedNode), &exc)->str);
    fflush(stdout);
    
    if(ajStrMatchCC("Shape", nodeName->str))
    {
	printf("ajStrMatchCC Shape xml_GetIndex node = %p\n", node);
	fflush(stdout);
	gdome_str_unref(nodeName);
	nodeName = gdome_str_mkref("IndexedLineSet");
	listShapes = gdome_el_getElementsByTagName
	    (xml_GetNodeElement(passedNode), nodeName, &exc);
	for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		  && node == NULL) ; ++i)
	{
	    node = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	}
	gdome_nl_unref(listShapes, &exc);
	printf("listShapes Line xml_GetIndex node = %p\n", node);
	if(node == NULL)
	{
	    printf("ajStrMatchCC IndexedFaceSet xml_GetIndex\n");
	    fflush(stdout);
	    gdome_str_unref(nodeName);
	    nodeName = gdome_str_mkref("IndexedFaceSet");
	    listShapes = gdome_el_getElementsByTagName
		(xml_GetNodeElement(passedNode), nodeName, &exc);
	    for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		      && node == NULL) ; ++i)
	    {
		node = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	    }
	    gdome_nl_unref(listShapes, &exc);
	    printf("listShapes Face xml_GetIndex node = %p\n", node);
	}
    }
    else
    {
	printf("no match\n");
	node = passedNode;
    }

    nameString = ajStrNewC("ProtoInstance");
    gdome_str_unref(nodeName);
    nodeName = gdome_el_tagName(xml_GetNodeElement(passedNode), &exc);

    if(ajStrMatchC(nameString, nodeName->str))
    {
	attributeName = ajStrNewC("name");
	ajAttributeValue = xml_GetAttribute(node, attributeName);
	if(ajStrMatchC(ajAttributeValue, "Graph"))
	{
	    proto = ajTrue;
	}
	ajStrDel(&attributeName);
	ajStrDel(&ajAttributeValue);
    }

    ajStrDel(&nameString);
    gdome_str_unref(nodeName);

    if(proto)
    {
	gdome_str_unref(nodeName);
	nodeName = gdome_el_tagName(xml_GetNodeElement(node), &exc);
	printf("gdome_el_tagName if(proto) = %s\n", nodeName->str);
	fflush(stdout);

	gdome_str_unref(nodeName);
	nodeName = gdome_str_mkref("fieldValue");
	listShapes = gdome_el_getElementsByTagName
	    (xml_GetNodeElement(node), nodeName, &exc);
	for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		  && returnNode == NULL) ; ++i)
	{
	    tempNode = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	    attributeName = ajStrNewC("name");
	    ajAttributeValue = xml_GetAttribute(tempNode, 
						attributeName);

	    if(ajStrMatchC(ajAttributeValue, "Graph.index"))
	    {
		ajStrDel(&attributeName);
		attributeName = ajStrNewC("value");
		temp = xml_GetAttribute(tempNode, attributeName);
	    }

	    ajStrDel(&attributeName);
	    ajStrDel(&ajAttributeValue);
	    xml_UnrefNode(tempNode);
	}
	gdome_str_unref(nodeName);
	gdome_nl_unref(listShapes, &exc);
    }
    else
    {
	nodeName = gdome_el_tagName(xml_GetNodeElement(node), &exc);
	printf("gdome_el_tagName else = %s\n", nodeName->str);
	fflush(stdout);
	
	/*
	   if(ajStrMatchCC("Shape", nodeName->str))
	   {
	   gdome_str_unref(nodeName);
	   nodeName = gdome_str_mkref("IndexedFaceSet");
	   listShapes = gdome_el_getElementsByTagName
	   (xml_GetNodeElement(passedNode), nodeName, &exc);
	   for(i=0; (i<gdome_nl_length(listShapes, &exc) 
	   && node == NULL) ; ++i)
	   {
	   tempNode = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	   }
	   gdome_nl_unref(listShapes, &exc);
	   nameString = ajStrNewC("coordIndex");
	   temp = xml_GetAttribute(tempNode, nameString);
	   ajStrDel(&nameString);
	   xml_UnrefNode(tempNode);	    
	   }
	   else
	   */
	{
	    nameString = ajStrNewC("coordIndex");
	    temp = xml_GetAttribute(node, nameString);
	    ajStrDel(&nameString);
	}
	
    }
    
    return temp;
}




/* @funcstatic xml_SetIndex  ****************************************
**
** Local Method
** Sets the list of coord index's for an Indexed Line Set
**
** @param [r] node [GdomeNode*] node to get the index's of
** @param [r] index [AjPStr] value of index's
**
** @return [AjBool] ajTrue if the index set correctly
** @@
*********************************************************************/
static void xml_SetIndex(AjPXmlNode passedNode, AjPStr index)
{
    GdomeException exc;
    GdomeDOMString *nodeName = NULL;
    AjBool proto;
    AjPStr attributeName = NULL;
    AjPStr ajAttributeValue = NULL;
    AjPXmlNode tempNode = NULL;
    AjPXmlNode returnNode = NULL;
    AjPXmlNode node = NULL;
    AjPStr nameString = NULL;
    GdomeNodeList *listShapes = NULL;
    int i;

    printf("gdome_el_tagName xml_SetIndex = %s\n", 
	   gdome_el_tagName(xml_GetNodeElement(passedNode), &exc)->str);
    fflush(stdout);

    proto = ajFalse;
    
    nodeName = gdome_el_tagName(xml_GetNodeElement(passedNode), &exc);

    if(ajStrMatchCC("Shape", nodeName->str))
    {
	printf("ajStrMatchCC Shape\n");
	nodeName = gdome_str_mkref("IndexedLineSet");
	gdome_str_unref(nodeName);
	listShapes = gdome_el_getElementsByTagName
	    (xml_GetNodeElement(passedNode), nodeName, &exc);
	for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		  && node == NULL) ; ++i)
	{
	    node = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	}
	gdome_nl_unref(listShapes, &exc);
	if(node == NULL)
	{
	    printf("ajStrMatchCC IndexedFaceSet xml_SetIndex\n");
	    fflush(stdout);
	    gdome_str_unref(nodeName);
	    nodeName = gdome_str_mkref("IndexedFaceSet");
	    listShapes = gdome_el_getElementsByTagName
		(xml_GetNodeElement(passedNode), nodeName, &exc);
	    for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		      && node == NULL) ; ++i)
	    {
		node = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	    }
	    gdome_nl_unref(listShapes, &exc);
	}
    }
    else
    {
	printf("no match\n");
	node = passedNode;
    }

    nameString = ajStrNewC("ProtoInstance");
    gdome_str_unref(nodeName);
    nodeName = gdome_el_tagName(xml_GetNodeElement(passedNode), &exc);

    if(ajStrMatchC(nameString, nodeName->str))
    {
	attributeName = ajStrNewC("name");
	ajAttributeValue = xml_GetAttribute(node, attributeName);
	if(ajStrMatchC(ajAttributeValue, "Graph"))
	{
	    proto = ajTrue;
	}
	ajStrDel(&attributeName);
	ajStrDel(&ajAttributeValue);
    }

    ajStrDel(&nameString);
    gdome_str_unref(nodeName);
    
    if(proto)
    {
	nodeName = gdome_str_mkref("fieldValue");
	listShapes = gdome_el_getElementsByTagName
	    (xml_GetNodeElement(node), nodeName, &exc);
	for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		  && returnNode == NULL) ; ++i)
	{
	    tempNode = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	    attributeName = ajStrNewC("name");
	    ajAttributeValue = xml_GetAttribute(tempNode, 
						attributeName);

	    if(ajStrMatchC(ajAttributeValue, "Graph.index"))
	    {
		ajStrDel(&attributeName);
		attributeName = ajStrNewC("value");
		xml_SetAttribute(tempNode, attributeName, index);
	    }

	    ajStrDel(&attributeName);
	    ajStrDel(&ajAttributeValue);
	    xml_UnrefNode(tempNode);
	}
	gdome_str_unref(nodeName);
	gdome_nl_unref(listShapes, &exc);
    }
    else
    {
	if(ajStrMatchCC("Shape", nodeName->str))
	{
	    gdome_str_unref(nodeName);
	    nodeName = gdome_str_mkref("IndexedFaceSet");
	    listShapes = gdome_el_getElementsByTagName
		(xml_GetNodeElement(passedNode), nodeName, &exc);
	    for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		      && node == NULL) ; ++i)
	    {
		tempNode = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	    }
	    gdome_nl_unref(listShapes, &exc);
	    nameString = ajStrNewC("coordIndex");
	    xml_SetAttribute(tempNode, nameString, index);
	    ajStrDel(&nameString);
	    xml_UnrefNode(tempNode);	    
	}
	else
	{
	    nameString = ajStrNewC("coordIndex");
	    xml_SetAttribute(node, nameString, index);
	    ajStrDel(&nameString);
	}
    }
    
    return;
}




/* @funcstatic xml_GetPoints  ****************************************
**
** Local Method
** Gets the list of points for an Indexed Line Set
**
** @param [r] node [GdomeNode*] node to get the points of
**
** @return [AjPStr] value of points
** @@
*********************************************************************/
static AjPStr xml_GetPoints(AjPXmlNode passedNode)
{
    GdomeDOMString *attributeValue;
    GdomeException exc;
    AjPStr temp = NULL;
    AjPStr nameString = NULL;
    GdomeNodeList *listCoordinate = NULL;
    GdomeNodeList *listShapes = NULL;
    AjPXmlNode coordinateNode = NULL;
    int i;
    GdomeDOMString *nodeName = NULL;
    AjBool proto;
    AjPStr attributeName = NULL;
    AjPStr ajAttributeValue = NULL;
    AjPXmlNode tempNode = NULL;
    AjPXmlNode node = NULL;

    proto = ajFalse;

    printf("gdome_el_tagName xml_GetPoints = %s\n", 
	   gdome_el_tagName(xml_GetNodeElement(passedNode), &exc)->str);
    fflush(stdout);

    nameString = ajStrNewC("ProtoInstance");
    nodeName = gdome_el_tagName(xml_GetNodeElement(passedNode), &exc);

    printf("xml_GetPoints passedNode = %p name = %s\n", passedNode, nodeName->str);
    fflush(stdout);

    if(ajStrMatchCC("Shape", nodeName->str))
    {
	printf("ajStrMatchCC Shape\n");
	fflush(stdout);
	gdome_str_unref(nodeName);
	nodeName = gdome_str_mkref("IndexedLineSet");
	listShapes = gdome_el_getElementsByTagName
	    (xml_GetNodeElement(passedNode), nodeName, &exc);
	for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		  && node == NULL) ; ++i)
	{
	    node = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	}
	gdome_nl_unref(listShapes, &exc);
	if(node == NULL)
	{
	    printf("ajStrMatchCC IndexedFaceSet xml_GetPoints\n");
	    fflush(stdout);
	    gdome_str_unref(nodeName);
	    nodeName = gdome_str_mkref("IndexedFaceSet");
	    listShapes = gdome_el_getElementsByTagName
		(xml_GetNodeElement(passedNode), nodeName, &exc);
	    for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		      && node == NULL) ; ++i)
	    {
		node = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	    }
	    gdome_nl_unref(listShapes, &exc);
	}
    }
    else
    {
	printf("no match\n");
	node = passedNode;
    }
    
    printf("here getP node = %p\n", node);
    fflush(stdout);
    printf("here getP proto = %d\n", proto);
    fflush(stdout);
    printf("here getP node node = %p\n", xml_GetNodeElement(node));
    fflush(stdout);
    printf("here getP node name p = %p\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)); 
    fflush(stdout);
    printf("here getP1 node name s = %s\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)->str); 
    fflush(stdout);
    
    gdome_str_unref(nodeName);
    nodeName = gdome_el_tagName(xml_GetNodeElement(passedNode), &exc);
    
    if(ajStrMatchC(nameString, nodeName->str))
    {
	attributeName = ajStrNewC("name");
	ajAttributeValue = xml_GetAttribute(node, attributeName);
	if(ajStrMatchC(ajAttributeValue, "Graph"))
	{
	    proto = ajTrue;
	}
	ajStrDel(&attributeName);
	ajStrDel(&ajAttributeValue);
    }

    gdome_str_unref(nodeName);
    printf("here getP2 node = %p\n", node);
    fflush(stdout);
    printf("here getP node el = %p\n", xml_GetNodeElement(node));
    fflush(stdout);
    printf("here getP node name p = %p\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)); 
    fflush(stdout);
    printf("here getP node name s = %s\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)->str); 
    fflush(stdout);

    printf("here getP2 proto = %d\n", proto);
    fflush(stdout);

    if(proto)
    {
	nodeName = gdome_str_mkref("fieldValue");
	listShapes = gdome_el_getElementsByTagName
	    (xml_GetNodeElement(node), nodeName, &exc);
	for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		  && temp == NULL) ; ++i)
	{
	    tempNode = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	    attributeName = ajStrNewC("name");
	    ajAttributeValue = xml_GetAttribute(tempNode, 
						attributeName);

	    if(ajStrMatchC(ajAttributeValue, "Graph.points"))
	    {
		ajStrDel(&attributeName);
		attributeName = ajStrNewC("value");
		temp = xml_GetAttribute(tempNode, attributeName);
	    }

	    ajStrDel(&attributeName);
	    ajStrDel(&ajAttributeValue);
	    xml_UnrefNode(tempNode);
	}
	gdome_str_unref(nodeName);
	gdome_nl_unref(listShapes, &exc);
	printf("here getP3 node = %p\n", node);
	fflush(stdout);
	printf("here getP node el = %p\n", xml_GetNodeElement(node));
	fflush(stdout);
	printf("here getP node name p = %p\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)); 
	fflush(stdout);
	printf("here getP node name s = %s\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)->str); 
	fflush(stdout);
    }
    else
    {

	nodeName = gdome_n_nodeName(xml_GetNode(node), &exc);
	printf("here getP4 node = %p\n", node);
	fflush(stdout);
	printf("here getP node el = %p\n", xml_GetNodeElement(node));
	fflush(stdout);
	printf("here getP node name p = %p\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)); 
	fflush(stdout);
	printf("here getP node name s = %s\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)->str); 
	fflush(stdout);
	printf("here getP nodeName = %s\n", nodeName->str); 
	fflush(stdout);
    
	if(!(ajStrMatchCC(nodeName->str, "IndexedLineSet")
	     || ajStrMatchCC(nodeName->str, "IndexedFaceSet")
	     || ajStrMatchCC(nodeName->str, "IndexedPointSet")))
	{
	    printf("Here is an exception, cannot get points from a node that is not an IndexedSet, this is a %s", nodeName->str);
	    fflush(stdout);
	    return ajFalse;
	}
	printf("here getP0 nodeName = %s\n", nodeName->str); 
	fflush(stdout);
 	gdome_str_unref(nodeName);
	nodeName = gdome_str_mkref("Coordinate");
	printf("here getPe node name p = %p\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)); 
	fflush(stdout);
	listCoordinate = gdome_el_getElementsByTagName
	    (xml_GetNodeElement(node), nodeName, &exc);
	printf("here getPr node name p = %p\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)); 
	fflush(stdout);
	if(gdome_nl_length(listCoordinate, &exc)!=0)
	{
	    coordinateNode = xml_SetNode( gdome_nl_item 
					 (listCoordinate, 0, &exc));
	}
	printf("here getP5 node = %p\n", node);
	fflush(stdout);
	printf("here getP node el = %p\n", xml_GetNodeElement(node));
	fflush(stdout);
	printf("here getP node name p = %p\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)); 
	fflush(stdout);
	printf("here getP node name s = %s\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)->str); 
	fflush(stdout);
	if(xml_GetNode(coordinateNode) == NULL)
	{
	    printf("Here is an exception, IndexedLineSet does not have points");
	    printf("here getP6 node = %p\n", node);
	    fflush(stdout);
	    printf("here getP node el = %p\n", xml_GetNodeElement(node));
	    fflush(stdout);
	    printf("here getP node name p = %p\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)); 
	    fflush(stdout);
	    printf("here getP node name s = %s\n", gdome_el_tagName(xml_GetNodeElement(node), &exc)->str); 
	    fflush(stdout);
	    return ajStrNewC("");
	}	
    
	gdome_str_unref(nodeName);
	nodeName = gdome_str_mkref("point");
	attributeValue = gdome_el_getAttribute 
	    (xml_GetNodeElement(coordinateNode), nodeName, &exc);

	xml_UnrefNode(coordinateNode);
	if(listCoordinate != NULL)
	    gdome_nl_unref(listCoordinate, &exc);
    
	/* Do I need to unref the attributeValue? I guess yes so I do not 
	   use the line below */
	/*
	   return ajStrNewC(attributeValue->str); 
	   */
	temp = ajStrNewC(attributeValue->str);
	gdome_str_unref(nodeName);
	gdome_str_unref(attributeValue);
    }
    printf("here getP7 node = %p\n", node);
    fflush(stdout);
    printf("here getP node el = %p\n", xml_GetNodeElement(node));
    fflush(stdout);
    printf("here getP node name p = %p\n",
	   gdome_el_tagName(xml_GetNodeElement(node), &exc)); 
    fflush(stdout);
    printf("here getP node name s = %s\n",
	   gdome_el_tagName(xml_GetNodeElement(node), &exc)->str); 
    fflush(stdout);
    
    ajStrDel(&nameString);
    
    return temp;
}




/* @funcstatic xml_SetPoints  ****************************************
**
** Local Method
** Sets the list of points for an Indexed Line Set
**
** @param [w] node [GdomeNode*] node to set the points of
** @param [r] points [AjPStr] value of points
**
** @return [AjBool] ajTrue if points set correctly
** @@
*********************************************************************/
static AjBool xml_SetPoints(AjPXmlNode passedNode, AjPStr points)
{
    GdomeException exc;
    GdomeNodeList *listCoordinate = NULL;
    AjPXmlNode coordinateNode = NULL;
    int i;
    GdomeDOMString *nodeName2 = NULL;
    GdomeDOMString *nodeName = NULL;
    
    AjPStr nameString = NULL;
    GdomeNodeList *listShapes = NULL;
    AjBool proto;
    AjPStr attributeName = NULL;
    AjPStr ajAttributeValue = NULL;
    AjPXmlNode tempNode = NULL;
    AjPXmlNode returnNode = NULL;
    AjPXmlNode node = NULL;

    proto = ajFalse;
    
    printf("gdome_el_tagName xml_SetPoints = %s\n", 
	   gdome_el_tagName(xml_GetNodeElement(passedNode), &exc)->str);
    fflush(stdout);

    nameString = ajStrNewC("ProtoInstance");
    nodeName = gdome_el_tagName(xml_GetNodeElement(passedNode), &exc);

    if(ajStrMatchCC("Shape", nodeName->str))
    {
	printf("ajStrMatchCC Shape\n");
	nodeName = gdome_str_mkref("IndexedLineSet");
	listShapes = gdome_el_getElementsByTagName
	    (xml_GetNodeElement(passedNode), nodeName, &exc);
	for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		  && node == NULL) ; ++i)
	{
	    node = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	}
	gdome_nl_unref(listShapes, &exc);
	if(node == NULL)
	{
	    printf("ajStrMatchCC IndexedFaceSet xml_SetPoints\n");
	    fflush(stdout);
	    gdome_str_unref(nodeName);
	    nodeName = gdome_str_mkref("IndexedFaceSet");
	    listShapes = gdome_el_getElementsByTagName
		(xml_GetNodeElement(passedNode), nodeName, &exc);
	    for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		      && node == NULL) ; ++i)
	    {
		node = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	    }
	    gdome_nl_unref(listShapes, &exc);
	}
    }
    else
    {
	printf("no match\n");
	node = passedNode;
    }
    
    gdome_str_unref(nodeName);
    nodeName = gdome_el_tagName(xml_GetNodeElement(passedNode), &exc);

    if(ajStrMatchC(nameString, nodeName->str))
    {
	attributeName = ajStrNewC("name");
	ajAttributeValue = xml_GetAttribute(node, attributeName);
	if(ajStrMatchC(ajAttributeValue, "Graph"))
	{
	    proto = ajTrue;
	}
	ajStrDel(&attributeName);
	ajStrDel(&ajAttributeValue);
    }
        
    gdome_str_unref(nodeName);

    if(proto)
    {
	nodeName = gdome_str_mkref("fieldValue");
	listShapes = gdome_el_getElementsByTagName
	    (xml_GetNodeElement(node), nodeName, &exc);
	for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		  && returnNode == NULL) ; ++i)
	{
	    tempNode = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	    attributeName = ajStrNewC("name");
	    ajAttributeValue = xml_GetAttribute(tempNode, 
						attributeName);

	    if(ajStrMatchC(ajAttributeValue, "Graph.points"))
	    {
		ajStrDel(&attributeName);
		attributeName = ajStrNewC("value");
		xml_SetAttribute(tempNode, attributeName, points);
	    }

	    ajStrDel(&attributeName);
	    ajStrDel(&ajAttributeValue);
	    xml_UnrefNode(tempNode);
	}
	gdome_nl_unref(listShapes, &exc);
	gdome_str_unref(nodeName);
    }
    else
    {
	nodeName = gdome_n_nodeName(xml_GetNode(node), &exc);
    
	if(!(ajStrMatchCC(nodeName->str, "IndexedLineSet")
	     || ajStrMatchCC(nodeName->str, "IndexedFaceSet")
	     || ajStrMatchCC(nodeName->str, "IndexedPointSet")))
	{
	    printf("Here is an exception, cannot set points from a node "
		   "that is not an IndexedSet, this is a %s", nodeName->str);
	    return ajFalse;
	}
	gdome_str_unref(nodeName);
	nodeName = gdome_str_mkref("Coordinate");
	listCoordinate = gdome_el_getElementsByTagName
	    ((xml_GetNodeElement(node)), nodeName, &exc);
	if(gdome_nl_length(listCoordinate, &exc)!=0)
	{
	    coordinateNode = xml_SetNode( gdome_nl_item 
					 (listCoordinate, 0, &exc));
	}
	gdome_nl_unref(listCoordinate, &exc);
	if(xml_GetNode(coordinateNode) == NULL)
	{
	    printf("Here is an exception, "
		   "IndexedLineSet does not have points");
	    return ajFalse;
	}	
    
	gdome_str_unref(nodeName);
	nodeName = gdome_str_mkref_dup("point");
	nodeName2 = gdome_str_mkref_dup(ajStrStr(points));
	gdome_el_setAttribute(xml_GetNodeElement(coordinateNode), 
			      nodeName, nodeName2, &exc);

	xml_UnrefNode(coordinateNode);
	gdome_str_unref(nodeName);
	gdome_str_unref(nodeName2);

    }
    
    ajStrDel(&nameString);
    
    return ajTrue;
}




/* @funcstatic xml_MakeNewNode  **************************************
**
** Local Method
** Makes a new DOM node and appends it to the parent node
**
** @param [w] file [AjPXmlFile] the file to add the node to
** @param [r] name [AjPStr] name of node
** @param [r] parentNode [AjPXmlNode] parent node to add to
**
** @return [AjPXmlNode] new node
** @@
*********************************************************************/
static AjPXmlNode xml_MakeNewNode(AjPXmlFile file, AjPStr name, 
				  AjPXmlNode parent)
{
    GdomeException exc;
    GdomeNode *parentNode ; 
    GdomeNode *elNode ; 
    GdomeNode *anotherElNode ; 
    AjPXmlNode el;

    GdomeDOMString *nodeName = NULL;

    nodeName = gdome_str_mkref_dup(ajStrStr(name));
    elNode = (GdomeNode *) gdome_doc_createElement (file->doc, nodeName,
						    &exc);
    el = xml_SetNode(elNode);

    parentNode = xml_GetNode(parent);
    anotherElNode = gdome_n_appendChild (parentNode, elNode, &exc);
    /*  gdome_n_appendChild (parentNode, elNode, &exc); hugh */

    gdome_str_unref (nodeName);
    gdome_n_unref(anotherElNode, &exc);

    return el;
}




/* @funcstatic xml_PresentColourAsString  **********************************
**
** Local Method
** returns present Colour As String
** @param [r] file [AjPXmlFile] the file with the pressent colour
**
** @return [AjPStr] the colour
** @@
*********************************************************************/
static  AjPStr xml_PresentColourAsString(AjPXmlFile file)
{
    AjPStr colour=NULL;
    AjPStr temp=NULL;
    int i;

    colour = ajStrNew();

    for(i=0; i<3; ++i)
    {
	xml_StrFromDouble(&temp, file->colour[i]);
	ajStrApp(&colour, temp);
	ajStrDel(&temp);
	if(i<2)
	    ajStrAppC(&colour, " ");
    }

    return colour;
}




/* @funcstatic xml_FileNeedsProtoDeclare  **********************************
**
** Local Method
** returns whether this file needs this type of proto Declaration
**
** @param [r] file [AjPXmlFile] the file to check
** @param [r] protoName [AjPStr] the proto name
**
** @return [AjBool] true if this file needs this type of proto Declaration
** @@
*********************************************************************/
static AjBool xml_FileNeedsProtoDeclare(AjPXmlFile file, AjPStr protoName)
{
    GdomeException exc;
    AjPXmlNode pressentNode;
    GdomeNodeList* listProtos;
    int i;
    AjPStr pressentProtoName = NULL; 
    AjPStr nameString = NULL;
    GdomeDOMString *nodeName = NULL;

    pressentNode = xml_GetCurrentGraphic(file);

    nodeName = gdome_str_mkref("ProtoDeclare");
    listProtos = gdome_el_getElementsByTagName
	((xml_GetNodeElement(xml_GetCurrentScene(file))), nodeName, 
	 &exc);
    for(i=0; i<gdome_nl_length(listProtos, &exc); ++i)
    {
	pressentNode = xml_SetNode( gdome_nl_item (listProtos, i, 
						   &exc));
	nameString = ajStrNewC("name");
	pressentProtoName = xml_GetAttribute(pressentNode, 
					     nameString);
	if(ajStrMatch(pressentProtoName, protoName))
	{
	    gdome_nl_unref(listProtos, &exc);
	    ajStrDel(&nameString);
	    gdome_str_unref(nodeName);
	    ajStrDel(&pressentProtoName);
	    xml_UnrefNode(pressentNode);
	    return ajFalse;
	}
	xml_UnrefNode(pressentNode);
	ajStrDel(&pressentProtoName);
    }

    /*  Both of these cause a crash. hugh */
    /*
       xml_UnrefNode(pressentNode);
       gdome_n_unref(pressentNode, &exc); 
       */
    gdome_nl_unref(listProtos, &exc);
    ajStrDel(&nameString);
    gdome_str_unref(nodeName);
    /* pressentNode and pressentProtoName unrefed above  */

    return ajTrue;
}




/* @funcstatic xml_IsShapeThisColour  *************************************
**
** Local Method
** returns whether this shape is the pressent colour
**
** @param [r] file [AjPXmlFile] the file to check
** @param [r] shape [AjPXmlNode] the shape to check
**
** @return [AjBool] true if this shape is current colour
** @@
*********************************************************************/
static AjBool xml_IsShapeThisColour(AjPXmlFile file, AjPXmlNode shape)
{
    GdomeException exc;
    GdomeNodeList *listAppearance = NULL;
    GdomeNodeList *listMaterial = NULL;
    AjPXmlNode elMaterial = NULL;
    AjPXmlNode tempNode = NULL;
    AjPStr colour = NULL;
    AjPStr nameString = NULL;
    AjPStr presentColour = NULL;
    AjPStr attributeName = NULL;
    AjPStr attributeValue = NULL;
    AjBool returnValue;
    GdomeDOMString *nodeName = NULL;
    ajint i;

    nameString = ajStrNewC("Graph");

    printf("IsShapeThisColour xml_PressentGraphicTypeIs(file, nameString) = %d shape == xml_GetCurrentGraphic(file) = %d\n", xml_PressentGraphicTypeIs(file, nameString), (xml_GetNode(shape) == xml_GetNode(xml_GetCurrentGraphic(file))));
    fflush(stdout);
    printf("IsShapeThisColour here getP node = %p\n", shape);
    fflush(stdout);
    printf("IsShapeThisColour here getPGetCurrentGraphic  = %p\n", xml_GetCurrentGraphic(file));
    fflush(stdout);
    printf("IsShapeThisColour here getP node node = %p\n", xml_GetNode(shape));
    fflush(stdout);
    printf("IsShapeThisColour here getP node name p = %p\n", gdome_el_tagName(xml_GetNodeElement(shape), &exc)); 
    fflush(stdout);
    printf("IsShapeThisColour here getP node name s = %s\n", gdome_el_tagName(xml_GetNodeElement(shape), &exc)->str); 
    fflush(stdout);
    printf("IsShapeThisColour here getP GetCurrentGraphic node = %p\n", xml_GetNode(xml_GetCurrentGraphic(file)));
    fflush(stdout);
    printf("IsShapeThisColour here getP GetCurrentGraphic name p = %p\n", gdome_el_tagName(xml_GetNodeElement(xml_GetCurrentGraphic(file)), &exc)); 
    fflush(stdout);
    printf("IsShapeThisColour here getP GetCurrentGraphic name s = %s\n", gdome_el_tagName(xml_GetNodeElement(xml_GetCurrentGraphic(file)), &exc)->str); 
    fflush(stdout);

    printf("xml_PressentGraphicTypeIs(file, nameString) = %dxml_GetNode(shape) == xml_GetNode(xml_GetCurrentGraphic(file))  = %d ajTrue = %d\n", xml_PressentGraphicTypeIs(file, nameString), xml_GetNode(shape) == 
	   xml_GetNode(xml_GetCurrentGraphic(file)), ajTrue); 
    fflush(stdout);
  
    if(xml_PressentGraphicTypeIs(file, nameString) 
       && xml_GetNode(shape) == 
       xml_GetNode(xml_GetCurrentGraphic(file)))
    {

	printf("IsShapeThisColour Graph");
	fflush(stdout);

	nodeName = gdome_str_mkref("fieldValue");
	listAppearance = gdome_el_getElementsByTagName
	    ((xml_GetNodeElement(shape)), 
	     nodeName, &exc);
	for(i=0; (i<gdome_nl_length(listAppearance, &exc) 
		  && presentColour == NULL) ; ++i)
	{
	    tempNode = xml_SetNode(gdome_nl_item(listAppearance, i, 
						 &exc));
	    attributeName = ajStrNewC("name");
	    attributeValue = xml_GetAttribute(tempNode, attributeName);
	    if(ajStrMatchC(attributeValue, "Graph.colour"))
	    {
		ajStrDel(&attributeName);
		attributeName = ajStrNewC("value");
		colour = xml_GetAttribute(tempNode, 
					  attributeName);
	    }	    
	    ajXmlNodeDel(&tempNode);
	    ajStrDel(&attributeName);
	    ajStrDel(&attributeValue);
	}
	gdome_nl_unref(listAppearance, &exc);
	gdome_str_unref(nodeName);
	ajStrDel(&nameString);
	if(colour == NULL)
	{
	    nameString = ajStrNewC("fieldValue");
	    tempNode = xml_MakeNewNode(file, nameString, shape);
		
	    ajStrDel(&attributeName);
	    attributeName = ajStrNewC("name");
	    ajStrDel(&attributeValue);
	    attributeValue = ajStrNewC("Graph.colour");
	    xml_SetAttribute(tempNode, attributeName, 
			     attributeValue);

	    ajStrDel(&attributeName);
	    attributeName = ajStrNewC("value");
	    ajStrDel(&attributeValue);
	    attributeValue = xml_PresentColourAsString(file);
	    xml_SetAttribute(tempNode, attributeName, 
			     attributeValue);

	    ajStrDel(&nameString);
	    ajStrDel(&attributeName);
	    ajStrDel(&attributeValue);
	    ajXmlNodeDel(&tempNode);

	    printf("IsShapeThisColour made new returnValue = %d\n", returnValue);
	    fflush(stdout);

	    return ajTrue;
	}
	presentColour = xml_PresentColourAsString(file);
    }
    else
    {

	printf("IsShapeThisColour not Graph");
	fflush(stdout);
	/*    ajXmlWriteStdout(file);  */
	fflush(stdout);

	/* needs error checking */
	nodeName = gdome_str_mkref("Appearance");
	/*	listAppearance = gdome_el_getElementsByTagName
		(((GdomeElement *) gdome_nl_item(listAppearance, 0, &exc)), 
		nodeName, &exc);*/
	printf("IsShapeThisColour here getP node name s = %s\n", gdome_el_tagName(xml_GetNodeElement(shape), &exc)->str); 
	listAppearance = gdome_el_getElementsByTagName
	    (xml_GetNodeElement(shape), nodeName, &exc);
	gdome_str_unref(nodeName);

	printf("IsShapeThisColour listAppearance 0 = %p\n", listAppearance);
	fflush(stdout);
	nodeName = gdome_str_mkref("Material");
	listMaterial = gdome_el_getElementsByTagName
	    (((GdomeElement *) gdome_nl_item(listAppearance, 0, &exc)), 
	     nodeName, &exc);
	printf("IsShapeThisColour listMaterial 0 = %p\n", listMaterial);
	fflush(stdout);

	printf("IsShapeThisColour gdome_nl_item(listMaterial 0 = %p\n", gdome_nl_item(listMaterial, 0, &exc));
	fflush(stdout);

	printf("IsShapeThisColour listMaterial\n");
	fflush(stdout);


	elMaterial = xml_SetNode( gdome_nl_item(listMaterial, 0, &exc));
	nameString = ajStrNewC("diffuseColor");

	printf("IsShapeThisColour elMaterial = %p\n", elMaterial);
	fflush(stdout);
	printf("IsElMaterialThisColour here getP node = %p\n", elMaterial);
       	fflush(stdout);
	printf("IsElMaterialThisColour here getP node node = %p\n", xml_GetNode(elMaterial));
	fflush(stdout);
	printf("IsElMaterialThisColour here getP node name p = %p\n", gdome_el_tagName(xml_GetNodeElement(elMaterial), &exc)); 
	fflush(stdout);
	printf("IsElMaterialThisColour here getP node name s = %s\n", gdome_el_tagName(xml_GetNodeElement(elMaterial), &exc)->str); 
	fflush(stdout);

	colour = xml_GetAttribute(elMaterial, nameString);

	printf("IsShapeThisColour colour\n");
	fflush(stdout);


	presentColour = xml_PresentColourAsString(file);

	gdome_nl_unref(listAppearance, &exc);
	gdome_nl_unref(listMaterial, &exc);

	xml_UnrefNode(elMaterial);

	ajStrDel(&colour);
	ajStrDel(&presentColour);

	gdome_str_unref(nodeName);
    }

    printf("IsShapeThisColour colour = %s presentColour  = %s \n", ajStrStr(colour), ajStrStr(presentColour));
    fflush(stdout);

    
    returnValue = ajStrMatch(colour, presentColour);

    ajStrDel(&nameString);

    printf("IsShapeThisColour returnValue = %d\n", returnValue);
    fflush(stdout);


    return returnValue;
}




/* @funcstatic xml_MakeNewShapeNode  *********************************
**
** Local Method
** returns makes a New Shape Node
**
** @param [r] file [AjPXmlFile] the file to find the node in
** @param [r] parentNode [AjPXmlNode] parent node to add to
** @param [r] nameReqd [AjPStr] name of node
**
** @return [AjPXmlNode] new node
** @@
*********************************************************************/
static AjPXmlNode xml_MakeNewShapeNode(AjPXmlFile file, 
				AjPXmlNode parentNode, 
				AjPStr nameReqd)
{
    AjPXmlNode returnNode;
    AjPXmlNode shape;
    AjPXmlNode Appearance;
    AjPXmlNode Material;
    AjPStr nameString=NULL;
    AjPStr colour=NULL;

    nameString = ajStrNewC("Shape");
    shape = xml_MakeNewNode(file, nameString, parentNode); 

    returnNode = xml_MakeNewNode(file, nameReqd, shape);

    printf("xml_MakeNewNode name = %s\n", ajStrStr(nameReqd));
    fflush(stdout);
  

    ajStrDel(&nameString);
    nameString = ajStrNewC("Appearance");
    Appearance = xml_MakeNewNode(file,nameString, shape);
    ajStrDel(&nameString);
    nameString = ajStrNewC("Material");
    Material = xml_MakeNewNode(file,nameString, Appearance);

    ajStrDel(&nameString);
    nameString = ajStrNewC("diffuseColor");
    colour = xml_PresentColourAsString(file);
    xml_SetAttribute(Material, nameString, colour);

    xml_UnrefNode(shape);
    xml_UnrefNode(Appearance);
    xml_UnrefNode(Material);
    ajStrDel(&nameString);
    ajStrDel(&colour);
        
    return returnNode;
}




/* @funcstatic xml_AddArc  *************************************************
**
** adds an arc.  Assumes the arc is 0 <= extent < pi
**
** @param [w] file [AjPXmlFile] the file to add the max min values to
** @param [r] xCentre [double] the x value of the centre of the circle
** @param [r] yCentre [double] the y value of the centre of the circle
** @param [r] startAngle [double] the startAngle of the arc
** @param [r] endAngle [double] the endAngle of the arc
** @param [r] radius [double] the radius of the circle
**
** @return [void]
** @@
*********************************************************************/
static void xml_AddArc(AjPXmlFile file, double xCentre, double yCentre, 
		       double startAngle, double endAngle, double radius)
{
    double xStart;
    double yStart;
    double xMiddle;
    double yMiddle;
    double xEnd;
    double yEnd;
    double middleWeight;

    double anglularExtent;
    double xChordCentre;
    double yChordCentre;
    double chordLength;

    double vectorAngle;
    double vectorLength;
    double centreToChordCentre;
    double chordCentreToMiddle;

    AjPStr temp = NULL;
    AjPStr controlPoints = NULL;
    AjPStr weights = NULL;
    AjPXmlNode nurbsNode;
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;

    temp = ajStrNew();
    controlPoints = ajStrNew();
    weights = ajStrNew();

    xStart = sin(startAngle) * radius + xCentre;
    yStart = cos(startAngle) * radius + yCentre;
    xEnd = sin(endAngle) * radius + xCentre;
    yEnd = cos(endAngle) * radius + yCentre;
    
    anglularExtent = endAngle - startAngle;
    chordLength = pow(pow((xEnd - xStart), 2) + pow((yEnd - yStart), 
						    2), 0.5);
    xChordCentre = (xEnd - xStart) / 2 + xStart;
    yChordCentre = (yEnd - yStart) / 2 + yStart;
    
    vectorAngle = startAngle + (anglularExtent / 2);
    centreToChordCentre = pow((pow(radius, 2) - pow((chordLength / 2),
						    2)), 0.5);
    chordCentreToMiddle = (chordLength / 2) * tan(anglularExtent / 2);
    vectorLength = centreToChordCentre + chordCentreToMiddle;

    xMiddle = xCentre + (vectorLength * sin(vectorAngle));
    yMiddle = yCentre + (vectorLength * cos(vectorAngle));
    middleWeight = cos(anglularExtent / 2);

    xml_AddACoord(xStart, yStart, ajFalse, &controlPoints, &temp);
    xml_AddACoord(xMiddle, yMiddle, ajFalse, &controlPoints, &temp);
    xml_AddACoord(xEnd, yEnd, ajFalse, &controlPoints, &temp);

    ajStrAppC(&weights, "1 ");
    ajStrDel(&temp);
    xml_StrFromDouble(&temp, middleWeight);
    ajStrAppC(&weights, ajStrStr(temp));
    ajStrAppC(&weights, " 1");
    
    ajStrDel(&nameString);
    nameString = ajStrNewC("NurbsCurve");
    nurbsNode = xml_MakeNewShapeNode(file, 
				     xml_GetCurrentGraphic(file), 
				     nameString);
    ajStrDel(&nameString);
    nameString = ajStrNewC("knot");
    nameString2 = ajStrNewC("0,0,0,1,1,1");
    xml_SetAttribute(nurbsNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("order");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("3");
    xml_SetAttribute(nurbsNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("controlPoint");
    xml_SetAttribute(nurbsNode, nameString, controlPoints);
    ajStrDel(&nameString);
    nameString = ajStrNewC("weight");
    xml_SetAttribute(nurbsNode, nameString, weights);
    
    ajStrDel(&nameString);
    ajStrDel(&nameString2);
    ajStrDel(&temp);
    ajStrDel(&controlPoints);
    ajStrDel(&weights);

    return;
}




/* @funcstatic xml_GetNodeTypeMakeIfNot  *****************************
**
** Local Method
** returns first? node of this type, Makes a new one If Not
**
** @param [r] file [AjPXmlFile] the file to find the node in
** @param [r] nameReqd [AjPStr] name of node
**
** @return [AjPXmlNode] found or new node
** @@
*********************************************************************/
static AjPXmlNode xml_GetNodeTypeMakeIfNot(AjPXmlFile file, AjPStr nameReqd)
{
    GdomeException exc;
    AjPXmlNode returnNode = NULL;
    AjPXmlNode returnNode2 = NULL;
    AjPXmlNode coordinateNode;
    AjPXmlNode tempNode;
    AjPXmlNode shapeNodeParent = NULL;
    GdomeNodeList *listShapes = NULL;
    GdomeNodeList *listGeometrys = NULL;
    GdomeNodeList *listIndexLineSets = NULL;
    int i;
    int j;
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;
    AjPStr attributeName = NULL;
    AjPStr attributeValue = NULL;
    GdomeDOMString *nodeName = NULL;
    AjBool hasCoord;
    AjBool hasCoordIndex;
    AjPTable colourTable;

    printf("here file->nodeTypes = %p name = %s colour = %s\n", file->nodeTypes, ajStrStr(nameReqd), ajStrStr(xml_PresentColourAsString(file)));
    fflush(stdout);
    printf("here ajTableToarray %p\n", ajTableToarray(file->nodeTypes, NULL));
    fflush(stdout);
    printf("here ajTableLength %d\n", ajTableLength(file->nodeTypes));
    fflush(stdout);
    printf("here ajTableGet %p\n", ajTableGet(file->nodeTypes, nameReqd));
    fflush(stdout);
    /*    ajStrTablePrint(file->nodeTypes); */
    
    fflush(stdout);

    printf("Printed file->nodeTypes = %p nameReqd = %s colour = %s\n", file->nodeTypes, ajStrStr(nameReqd), ajStrStr(xml_PresentColourAsString(file)));
    fflush(stdout);
    /*    returnNode = (AjPXmlNode) ajTableGet(file->nodeTypes, nameReqd); */
    colourTable = (AjPTable) ajTableGet(file->nodeTypes, nameReqd);
    printf("done ajTableGet 1 colourTable = %p\n", colourTable);
    fflush(stdout);
    if(colourTable != NULL)
    {
	/*	ajStrTablePrint(colourTable); */
	printf("Printed colourTable\n");
	fflush(stdout);
	returnNode = (AjPXmlNode) ajTableGet(colourTable, 
					     xml_PresentColourAsString(file));
    }
    
    printf("here nd returnNode = %p\n", returnNode);
    fflush(stdout);

    /*   if(returnNode != NULL && xml_IsShapeThisColour(file, returnNode)) */
    if(returnNode != NULL)
    {
	printf("here vb returnNode node = %p\n", returnNode);
	fflush(stdout);
	printf("here vb returnNode node el = %p\n", xml_GetNodeElement(returnNode));
	fflush(stdout);
	GdomeException exc;
	printf("here vb returnNode node name p = %p\n", gdome_el_tagName(xml_GetNodeElement(returnNode), &exc)); 
	fflush(stdout);
	printf("here vb returnNode node name s = %s\n", gdome_el_tagName(xml_GetNodeElement(returnNode), &exc)->str); 
	fflush(stdout);

	/*	if(xml_IsShapeThisColour(file, returnNode)) */
	{
	    printf("returning from list\n");
	    fflush(stdout);
	    returnNode2 = xml_SetNode(xml_GetNode(returnNode));
	    gdome_n_ref(xml_GetNode(returnNode), &exc);

	    /*	    colourTable = (AjPTable) ajTableGet(file->nodeTypes, nameReqd);
		    if(colourTable == NULL)
		    {
		    colourTable = ajStrTableNew(1);
		    }
		    ajTablePut(colourTable, 
		    (const void *) xml_PresentColourAsString(file),
		    (void *)returnNode2); */
	    /*	ajXmlWriteStdout(file); */
	    fflush(stdout);
	    return returnNode2;
	}
    }
  
    hasCoord = ajFalse;
    hasCoordIndex = ajFalse;

    nameString = ajStrNewC("Graph");
  

    printf("here nothing = p\n");
    fflush(stdout);

    if(xml_PressentGraphicTypeIs(file, nameString) 
       && ajStrMatchC(nameReqd, "IndexedLineSet")
       && xml_IsShapeThisColour(file, xml_GetCurrentGraphic(file)))
    {

	printf("Doing the graph bit\n");
	fflush(stdout);

	nodeName = gdome_str_mkref("fieldValue");
	listShapes = gdome_el_getElementsByTagName
	    ((xml_GetNodeElement(xml_GetCurrentGraphic(file))), 
	     nodeName, &exc);
	for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		  && returnNode == NULL) ; ++i)
	{
	    tempNode = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	  
	    attributeName = ajStrNewC("fieldName");
	    attributeValue = xml_GetAttribute(tempNode, attributeName);

	    if(ajStrMatchC(attributeValue, "Graph.points"))
		hasCoord = ajTrue;
	    if(ajStrMatchC(attributeValue, "Graph.index"))
		hasCoordIndex = ajTrue;

	    ajStrDel(&attributeName);
	    ajStrDel(&attributeValue);
	    xml_UnrefNode(tempNode);
	}

	if(!hasCoord)
	{
	    ajStrDel(&nameString);
	    nameString = ajStrNewC("fieldValue");
	    tempNode = xml_MakeNewNode(file, nameString, 
				       xml_GetCurrentGraphic(file));
	    ajStrDel(&nameString);
	    nameString = ajStrNewC("name");
	    nameString2 = ajStrNewC("Graph.points");
	    xml_SetAttribute(tempNode, nameString, nameString2);
	    ajStrDel(&nameString);
	    ajStrDel(&nameString2);
	    nameString = ajStrNewC("value");
	    nameString2 = ajStrNewC("");
	    xml_SetAttribute(tempNode, nameString, nameString2);

	    xml_UnrefNode(tempNode);
	    ajStrDel(&nameString2);
	}
	if(!hasCoordIndex)
	{
	    ajStrDel(&nameString);
	    nameString = ajStrNewC("fieldValue");
	    tempNode = xml_MakeNewNode(file, nameString, 
				       xml_GetCurrentGraphic(file));
	    ajStrDel(&nameString);
	    nameString = ajStrNewC("name");
	    nameString2 = ajStrNewC("Graph.index");
	    xml_SetAttribute(tempNode, nameString, nameString2);
	    ajStrDel(&nameString);
	    ajStrDel(&nameString2);
	    nameString = ajStrNewC("value");
	    nameString2 = ajStrNewC("");
	    xml_SetAttribute(tempNode, nameString, nameString2);

	    xml_UnrefNode(tempNode);
	    ajStrDel(&nameString2);
	}

	gdome_str_unref(nodeName);
	gdome_nl_unref(listShapes, &exc);
	ajStrDel(&nameString);

	returnNode2 = xml_SetNode
	    (xml_GetNode(xml_GetCurrentGraphic(file)));
	gdome_n_ref(xml_GetNode(xml_GetCurrentGraphic(file)), &exc);

	colourTable = (AjPTable) ajTableGet(file->nodeTypes, nameReqd);
	if(colourTable == NULL)
	{
	    colourTable = ajStrTableNew(1);
    
	    printf("About to print table e\n");
	    fflush(stdout);
	    /*	    ajStrTablePrint(file->nodeTypes); */
	    fflush(stdout);
	    printf("Printed table e file->nodeTypes = %p nameReqd = %p colourTable = %p nameReqd s = %s\n", file->nodeTypes, nameReqd, colourTable, ajStrStr(nameReqd));
	    fflush(stdout);

	    /*
	       ajTablePut(file->nodeTypes, (const void *) nameReqd,
	       (void *)colourTable); 
	       */
	    ajTablePut(file->nodeTypes, (const void *) ajStrNewS(nameReqd),
		       (void *)colourTable);
    
	    printf("About to print table r\n");
	    fflush(stdout);
	    /*	    ajStrTablePrint(file->nodeTypes); */
	    fflush(stdout);
	    printf("Printed table r\n");
	    fflush(stdout);

	}
	ajTablePut(colourTable, 
		   (const void *) xml_PresentColourAsString(file),
		   (void *)returnNode2);
	/*	ajXmlWriteStdout(file); */
	fflush(stdout);
	return(xml_GetCurrentGraphic(file));
      
    } else 
    {

	printf("Doing the Shape bit\n");
	fflush(stdout);

	returnNode = NULL;

	nameString = ajStrNewC("Graph");
  
	if(xml_PressentGraphicTypeIs(file, nameString))
	{
	    printf("doing Shape, PressentGraphicTypeIs Graph\n");
	    fflush(stdout);
	    shapeNodeParent = NULL;
	    nodeName = gdome_str_mkref("fieldValue");
	    listShapes = gdome_el_getElementsByTagName
		((xml_GetNodeElement(xml_GetCurrentGraphic(file))), 
		 nodeName, &exc);
	    for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		      && returnNode == NULL) ; ++i)      

	    {
		nodeName = gdome_str_mkref("name");
		attributeValue = ajStrNewC(gdome_el_getAttribute
					   (((GdomeElement*)gdome_nl_item(listShapes, i, &exc)),
					    nodeName, 
					    &exc)->str);

		if(ajStrMatchC(attributeValue, "children"))
		{
		    shapeNodeParent = xml_SetNode(gdome_nl_item
						  (listShapes, i, &exc));
		}
	    }
	    printf("doing Shape, shapeNodeParent == %p\n", shapeNodeParent);
	    fflush(stdout);
	    if(shapeNodeParent == NULL)
	    {
		nameString = ajStrNewC("fieldValue");
		shapeNodeParent = xml_MakeNewNode(file, nameString, 
						  xml_GetCurrentGraphic(file));
		attributeName = ajStrNewC("name");
		attributeValue = ajStrNewC("children");
		xml_SetAttribute(shapeNodeParent, attributeName, 
				 attributeValue);
		 
	    }
	}
	else
	{
	    shapeNodeParent = xml_GetCurrentGraphic(file);
	}

	printf("Doing the adding return node = %p\n\n", returnNode);
	fflush(stdout);

     
	nodeName = gdome_str_mkref("Shape");
	listShapes = gdome_el_getElementsByTagName
	    ((xml_GetNodeElement(shapeNodeParent)), nodeName, &exc);
	for(i=0; (i<gdome_nl_length(listShapes, &exc) 
		  && returnNode == NULL) ; ++i)
	{
	    tempNode = xml_SetNode(gdome_nl_item(listShapes, i, &exc));
	    if(xml_IsShapeThisColour(file, tempNode))
	    {
		gdome_str_unref(nodeName);
		nodeName = gdome_str_mkref("geometry");
		listGeometrys = gdome_el_getElementsByTagName
		    ((GdomeElement *)gdome_nl_item(listShapes, i, &exc), 
		     nodeName, &exc);
		for(j=0; (j<gdome_nl_length(listGeometrys, &exc) 
			  && returnNode == NULL); ++j)
		{
		    gdome_str_unref(nodeName);
		    nodeName = gdome_str_mkref(ajStrStr(nameReqd));
		    listIndexLineSets  = gdome_el_getElementsByTagName
			((GdomeElement *)gdome_nl_item(listGeometrys, j, 
						       &exc), nodeName, &exc);
		    if(gdome_nl_length(listIndexLineSets, &exc)!=0)
		    {
			returnNode = xml_SetNode( gdome_nl_item 
						 (listIndexLineSets, 0, 
						  &exc));
		    }
		    gdome_nl_unref(listIndexLineSets, &exc);
		}
		gdome_nl_unref(listGeometrys, &exc);
	    }
	    xml_UnrefNode(tempNode);
	}

	if(returnNode == NULL)
	{
	    returnNode = xml_MakeNewShapeNode(file, shapeNodeParent, 
					      nameReqd);
	    if(ajStrMatchC(nameReqd, "IndexedLineSet")
	       || ajStrMatchC(nameReqd, "IndexedFaceSet")
	       || ajStrMatchC(nameReqd, "IndexedPointSet"))
	    {
		ajStrDel(&nameString);
		nameString = ajStrNewC("Coordinate");
		coordinateNode = xml_MakeNewNode(file, nameString, 
						 returnNode);

		xml_UnrefNode(coordinateNode);
		ajStrDel(&nameString);
	    }
	}
	 
	gdome_nl_unref(listShapes, &exc);
	gdome_str_unref(nodeName);
	/*	if(shapeNodeParent != xml_GetCurrentGraphic(file))
		{
		xml_UnrefNode(shapeNodeParent);
		} */

	/* really not sure about this hugh should be returning a shape not an indexed line set */
	/*	ajXmlNodeDel(&returnNode);
		returnNode = shapeNodeParent; */
	
	
	returnNode2 = xml_GetParent(returnNode);
	gdome_n_ref(xml_GetNode(returnNode2), &exc);

	colourTable = (AjPTable) ajTableGet(file->nodeTypes, nameReqd);
	if(colourTable == NULL)
	{
	    colourTable = ajStrTableNew(1);
	    ajTablePut(file->nodeTypes, (const void *) nameReqd,
		       (void *)colourTable);
	}
	ajTablePut(colourTable, 
		   (const void *) xml_PresentColourAsString(file),
		   (void *)returnNode2);

	printf("here vbd returnNode node = %p\n", returnNode2);
	fflush(stdout);
	printf("here vbd returnNode node el = %p\n", xml_GetNodeElement(returnNode2));
	fflush(stdout);
	printf("here vbd returnNode node name p = %p\n", gdome_el_tagName(xml_GetNodeElement(returnNode2), &exc)); 
	fflush(stdout);
	printf("here vbd returnNode node name s = %s\n", gdome_el_tagName(xml_GetNodeElement(returnNode2), &exc)->str); 
	fflush(stdout);

	/* listShapes and nodeName unrefed just above */
	/* tempNode, nameString and nameString2 unrefed as required */
	ajStrDel(&nameString);

	/*	ajXmlWriteStdout(file); */
	fflush(stdout);
	return returnNode;

    }
      
      
    printf("here vbc returnNode node = %p\n", returnNode);
    fflush(stdout);
    printf("here vbc returnNode node el = %p\n", xml_GetNodeElement(returnNode));
    fflush(stdout);
    printf("here vbc returnNode node name p = %p\n", gdome_el_tagName(xml_GetNodeElement(returnNode), &exc)); 
    fflush(stdout);
    printf("here vbc returnNode node name s = %s\n", gdome_el_tagName(xml_GetNodeElement(returnNode), &exc)->str); 
    fflush(stdout);
      
      
  
    returnNode2 = xml_SetNode(xml_GetNode(returnNode));
    gdome_n_ref(xml_GetNode(returnNode), &exc);

    colourTable = (AjPTable) ajTableGet(file->nodeTypes, nameReqd);
    if(colourTable == NULL)
    {
	colourTable = ajStrTableNew(1);
	ajTablePut(file->nodeTypes, 
		   (const void *) nameReqd,
		   (void *)colourTable);
    }
    ajTablePut(colourTable, 
	       (const void *) xml_PresentColourAsString(file),
	       (void *)returnNode2);
    /*    ajTablePut(file->nodeTypes, (const void *)nameReqd, 
	  (void *) returnNode2); */

    /* listShapes and nodeName unrefed just above */
    /* tempNode, nameString and nameString2 unrefed as required */
    ajStrDel(&nameString);

    /*	ajXmlWriteStdout(file); */
    fflush(stdout);
    return returnNode;
}




/* @funcstatic xml_AddACoord  ***************************************
**
** Local Method
** adds a coord to string, incremets the index in another
**
** @param [r] x [double]  x coordinate
** @param [r] y [double]  y coordinate
** @param [r] joined [AjBool] conect this to last line?
** @param [r] coord [AjPStr*] string to add coord to
** @param [r] index [AjPStr*] string to add index to
**
*********************************************************************/
static void xml_AddACoord(double x, double y, AjBool joined, AjPStr* coord, 
			  AjPStr* index)
{
    AjPStr temp = NULL;
    int lastIndex;

    if(ajStrCmpC((*coord), "") != 0)
    {
	ajStrAppC(coord,  ", ");
    }
    xml_StrFromDouble(&temp, x);
    ajStrAppC(coord, ajStrStr(temp));
    ajStrAppC(coord, " ");
    ajStrDel(&temp);
    xml_StrFromDouble(&temp, y);
    ajStrAppC(coord, ajStrStr(temp));
    ajStrAppC(coord, " ");
    ajStrDel(&temp);
    ajStrFromInt(&temp, 0);
    ajStrAppC(coord, ajStrStr(temp));
    
    if(ajStrCmpC((*index), "") != 0)
    {
	lastIndex = xml_GetLastInt(*index);
	ajStrAppC(index,  " ");
	if(!joined)
	{
	    ajStrAppC(index, "-1 ");
	}
    } else
    {
	lastIndex = -1;
    }
    ajStrDel(&temp);
    ajStrFromInt(&temp, (lastIndex+1));
    ajStrAppC(index, ajStrStr(temp));
      
    ajStrDel(&temp);

    return;
}




/* @funcstatic xml_GetLastInt  ***************************************
**
** gets Last Int of a string
**
** @param [r] str [AjPStr] string to find int in
**
** @return [int] the last interger of a string, 0 and an ecxeption if 
** not an int
** @@
*********************************************************************/
static int xml_GetLastInt(AjPStr str)
{
    int count;
    int i;
    int value;
    AjPStr token=NULL;

    count = ajStrTokenCount(&str, " ");
    token = ajStrTok(str);

    for(i = count - 1; i >= 1; --i)
    {
	token = ajStrTok(NULL);
    }

    if(token != NULL)
    {    
	if(ajStrIsInt (token))
	{
	    ajStrToInt(token, &value);
	} else
	{
	    printf("Here is an exception.\n Last token not Int\n");
	}
    }
    else
    {
	printf("Here is an exception.\n string passed empty\n");
    }

    /*
       ajStrDel(&token);
       */

    return value;
}




/* @funcstatic xml_GetLastDouble  ************************************
**
** gets Last Double of a string
**
** @param [r] str [AjPStr] string to find double in
**
** @return [int] the last double of a string, 0 and an exception if 
** not a double
** @@
*********************************************************************/
static double xml_GetetLastDouble(AjPStr str)
{
    int count;
    int i;
    double value;
    AjPStr token=NULL;

    count = ajStrTokenCount(&str, " ");
    token = ajStrTok(str);

    for(i = count - 1; i >= 1; --i)
    {
	token = ajStrTok(NULL);
    }
    if(token != NULL)
    {    
	if(ajStrIsDouble (token))
	{
	    ajStrToDouble(token, &value);
	}
	else
	{
	    printf("Here is an exception.\n Last token not Double\n");
	}
    }
    else
    {
	printf("Here is an exception.\n string passed empty\n");
    }

    /*  
       ajStrDel(&token);
       */

    return value;
}




/* @funcstatic xml_GetDoubleNo  **************************************
**
** returns the double at position index
**
** @param [r] str [AjPStr] string to find double in
** @param [r] index [int] position of required double
**
** @return [double] the index number double of a string, 
** @@
*********************************************************************/
static double xml_GetDoubleNo(AjPStr str, int index)
{
    int count;
    int i;
    double value;
    AjPStr token=NULL;

    count = ajStrTokenCount(&str, " ");
    token = ajStrTok(str);

    if(index>count)
    {
	printf("Here is an exception.\n index higher than no. of tokens\n");
    }

    for(i = 1; i <= index; ++i)
    {
	token = ajStrTok(NULL);
    }
    if(token != NULL)
    {    
	if(ajStrIsDouble (token))
	{
	    ajStrToDouble(token, &value);
	}
	else
	{
	    printf("Here is an exception.\n %d token not Double\n", 
		   index);
	}
    }
    else
    {
	printf("Here is an exception.\n string passed empty\n");
    }

    /*  
       ajStrDel(&token);
       */

    return value;
}




/* @funcstatic xml_AddQuotes  **************************************
**
** Adds some dodgy quotes to a string
**
** @param [r] str [AjPStr] String to add quotes to
**
** @@
*********************************************************************/
static void xml_AddQuotes(AjPStr *title)
{
    ajStrInsertC (title, 0, QUOTE_CHARACTERS);
    ajStrAppC(title, QUOTE_CHARACTERS);
}




/* @funcstatic xml_CreateNewOutputFile ************************************
**
** Sets up the output file 
**
** @return [AjPXmlFile] a colection of pointers to the file created
** @@
*********************************************************************/
static AjPXmlFile xml_CreateNewOutputFile()
{
    GdomeException exc;
    GdomeDOMString *name;
    GdomeDOMString *publicId;
    GdomeDOMString *systemId;
    AjPXmlFile file = NULL;
    AjPXmlNode rootElement;
    AjPStr nameString = NULL;
    AjPStr nameString2 = NULL;  
    GdomeDocumentType *docType;
  
    file = ajXmlFileNew();

    file->domimpl = gdome_di_mkref ();
    name = gdome_str_mkref_dup ("X3D");
    publicId = gdome_str_mkref_dup 
	("ISO//Web3D//DTD X3D 3.0 Interactive//EN");
    systemId = gdome_str_mkref_dup
	("http://www.web3d.org/specifications/x3d-3.0.dtd");
    docType = gdome_di_createDocumentType(file->domimpl, name, 
					  publicId, systemId, &exc);
  
    /* do I not need to allocate the space? I am sure not */
    ajXmlSetColour(file, 1.0, 1.0, 1.0);

    file->doc = gdome_di_createDocument(file->domimpl, NULL, name, 
					docType, &exc);

    rootElement = xml_SetNode((GdomeNode *) gdome_doc_documentElement 
			      (file->doc, &exc));

    nameString = ajStrNewC("profile");
    nameString2 = ajStrNewC("Immersive");
    xml_SetAttribute(rootElement, nameString, nameString2);

    xml_SetCurrentScene(file, rootElement);   
    xml_SetCurrentGraphic(file, rootElement);
  
    if (xml_GetNode(file->currentGraphic) == NULL) 
    {
	printf ("Document.documentElement: NULL\n\tException #%d\n", exc);
	/*
	   return NULL;
	   */
    }

    xml_AddCommonBit(file);

    file->nodeTypes = ajStrTableNew(1);

    gdome_str_unref (name);
    gdome_str_unref (publicId);
    gdome_str_unref (systemId);
    gdome_dt_unref (docType, &exc);
    ajStrDel(&nameString);
    ajStrDel(&nameString2);

    return file;
}




/* @funcstatic xml_AddGraphProto *************************************
**
** Adds the nodes that are set up the Graph Proto
**
** Could realy be all part of addGraphic, but it is easyer to read
** Private Function
**
** @param [w] file [AjPXmlFile] file to add bit to
**
** @return [void]
** @@
*********************************************************************/
static void xml_AddGraphProto(AjPXmlFile file)
{
    AjPXmlNode protoNode;
    AjPXmlNode fieldNode;
    AjPXmlNode groupNode;
    AjPXmlNode parentNode;
    AjPXmlNode grandParentNode;
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;

    /*    
       printf("here a\n");
       fflush(stdout);
       
       grandParentNode = xml_GetParent(xml_GetCurrentGraphic(file));
       
       printf("here s\n");
       fflush(stdout);
       
       parentNode = xml_GetParent(grandParentNode);
       protoNode = xml_MakeNewNode(file, nameString, parentNode);
       */

    nameString = ajStrNewC("ProtoDeclare");
    protoNode = xml_MakeNewNode(file, nameString, xml_GetCurrentScene(file));
    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    nameString2 = ajStrNewC("Graph");
    xml_SetAttribute(protoNode, nameString, nameString2);
    /*  ajStrDel(&nameString);
	nameString = ajStrNewC("EXPORT");
	nameString2 = ajStrNewC("Graphic_Children");
	xml_SetAttribute(protoNode, nameString, nameString2); */

    ajStrDel(&nameString);
    nameString = ajStrNewC("ProtoInterface");
    parentNode = xml_MakeNewNode(file, nameString, protoNode);
    
    ajStrDel(&nameString);
    nameString = ajStrNewC("field");
    fieldNode = xml_MakeNewNode(file,nameString, parentNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Graph.mainTitle");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("type");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("SFString");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("accessType");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("initializeOnly");
    xml_SetAttribute(fieldNode, nameString, nameString2);

    ajStrDel(&nameString);
    nameString = ajStrNewC("field");
    xml_UnrefNode(fieldNode);
    fieldNode = xml_MakeNewNode(file,nameString, parentNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Graph.xTitle");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("type");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("SFString");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("accessType");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("initializeOnly");
    xml_SetAttribute(fieldNode, nameString, nameString2);

    ajStrDel(&nameString);
    nameString = ajStrNewC("field");
    xml_UnrefNode(fieldNode);
    fieldNode = xml_MakeNewNode(file,nameString, parentNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Graph.yTitle");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("type");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("SFString");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("accessType");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("initializeOnly");
    xml_SetAttribute(fieldNode, nameString, nameString2);

    ajStrDel(&nameString);
    nameString = ajStrNewC("field");
    xml_UnrefNode(fieldNode);
    fieldNode = xml_MakeNewNode(file,nameString, parentNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Graph.colour");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("type");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("SFColor");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("accessType");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("initializeOnly");
    xml_SetAttribute(fieldNode, nameString, nameString2);

    ajStrDel(&nameString);
    nameString = ajStrNewC("field");
    xml_UnrefNode(fieldNode);
    fieldNode = xml_MakeNewNode(file,nameString, parentNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Graph.index");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("type");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("MFInt32");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("accessType");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("initializeOnly");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("value");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("0 1");
    xml_SetAttribute(fieldNode, nameString, nameString2);

    ajStrDel(&nameString);
    nameString = ajStrNewC("field");
    xml_UnrefNode(fieldNode);
    fieldNode = xml_MakeNewNode(file,nameString, parentNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Graph.points");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("type");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("MFVec3f");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("accessType");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("initializeOnly");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("value");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("0 0 0, 0 0 0");
    xml_SetAttribute(fieldNode, nameString, nameString2);

    ajStrDel(&nameString);
    nameString = ajStrNewC("field");
    xml_UnrefNode(fieldNode);
    fieldNode = xml_MakeNewNode(file,nameString, parentNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("children");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("type");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("MFNode");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("accessType");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("initializeOnly");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);

    nameString = ajStrNewC("ProtoBody");
    xml_UnrefNode(parentNode);
    parentNode = xml_MakeNewNode(file, nameString, protoNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("Group");
    groupNode = xml_MakeNewNode(file, nameString, parentNode);
    xml_UnrefNode(protoNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("Shape");
    protoNode = xml_MakeNewNode(file, nameString, groupNode);
    xml_UnrefNode(parentNode);
    ajStrDel(&nameString);
  
    /* This is the bit 'cos the appearance node is FKD */
    /*
       nameString = ajStrNewC("Appearance");
       parentNode = xml_MakeNewNode(file, nameString,  protoNode);
       ajStrDel(&nameString);
       nameString = ajStrNewC("Material");
       grandParentNode = xml_MakeNewNode(file, nameString, parentNode);
       xml_UnrefNode(parentNode);
       ajStrDel(&nameString);
       nameString = ajStrNewC("IS");
       parentNode = xml_MakeNewNode(file, nameString, grandParentNode);
       xml_UnrefNode(grandParentNode);
       ajStrDel(&nameString);
       nameString = ajStrNewC("connect");
       grandParentNode = xml_MakeNewNode(file, nameString, parentNode);
       
       
       ajStrDel(&nameString);
       nameString = ajStrNewC("protoField");
       ajStrDel(&nameString2);
       nameString2 = ajStrNewC("Graph.colour");
       xml_SetAttribute(grandParentNode, nameString, nameString2);
       ajStrDel(&nameString);
       nameString = ajStrNewC("nodeField");
       ajStrDel(&nameString2);
       nameString2 = ajStrNewC("emissiveColor");
       xml_SetAttribute(grandParentNode, nameString, nameString2);
       */

    xml_UnrefNode(fieldNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("IndexedLineSet");
    fieldNode = xml_MakeNewNode(file, nameString, protoNode);
    /* put this line back when you sort out the above hugh */
    /*
       xml_UnrefNode(parentNode);
       */
    ajStrDel(&nameString);
    nameString = ajStrNewC("IS");
    parentNode = xml_MakeNewNode(file, nameString, fieldNode);
    /* put this line back when you sort out the above hugh */
    /*
       xml_UnrefNode(grandParentNode);
       */
    ajStrDel(&nameString);
    nameString = ajStrNewC("connect");
    grandParentNode = xml_MakeNewNode(file, nameString, parentNode);

    ajStrDel(&nameString);
    nameString = ajStrNewC("protoField");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Graph.index");
    xml_SetAttribute(grandParentNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("nodeField");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("coordIndex");
    xml_SetAttribute(grandParentNode, nameString, nameString2);

    xml_UnrefNode(parentNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("Coordinate");
    parentNode = xml_MakeNewNode(file, nameString, fieldNode);
    xml_UnrefNode(grandParentNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("IS");
    grandParentNode = xml_MakeNewNode(file, nameString, parentNode);
    xml_UnrefNode(parentNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("connect");
    parentNode = xml_MakeNewNode(file, nameString, grandParentNode);

    ajStrDel(&nameString);
    nameString = ajStrNewC("protoField");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Graph.points");
    xml_SetAttribute(parentNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("nodeField");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("point");
    xml_SetAttribute(parentNode, nameString, nameString2);

    /* This is the replacement temp hugh see above as well */
    nameString = ajStrNewC("Color");
    xml_UnrefNode(parentNode);
    parentNode = xml_MakeNewNode(file, nameString, fieldNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("color");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("0 0 0");
    xml_SetAttribute(parentNode, nameString, nameString2);
  
    xml_UnrefNode(protoNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("Group");
    protoNode = xml_MakeNewNode(file, nameString, groupNode);
    xml_UnrefNode(grandParentNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("IS");
    grandParentNode = xml_MakeNewNode(file, nameString, protoNode);
    xml_UnrefNode(parentNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("connect");
    parentNode = xml_MakeNewNode(file, nameString, grandParentNode);

    ajStrDel(&nameString);
    nameString = ajStrNewC("protoField");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("children");
    xml_SetAttribute(parentNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("nodeField");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("children");
    xml_SetAttribute(parentNode, nameString, nameString2);

    xml_UnrefNode(protoNode);
    xml_UnrefNode(fieldNode);
    xml_UnrefNode(groupNode);
    xml_UnrefNode(grandParentNode);
    xml_UnrefNode(parentNode);
    ajStrDel(&nameString);
    ajStrDel(&nameString2);

    return;
}




/* @funcstatic xml_AddDNAPlotProto ***********************************
**
** Adds the nodes that set up the DNA Plot Proto
**
** @param [w] file [AjPXmlFile] file to add bit to
**
** @return [void]
** @@
*********************************************************************/
static void xml_AddDNAPlotProto(AjPXmlFile file)
{
    AjPXmlNode protoNode;
    AjPXmlNode fieldNode;
    AjPXmlNode parentNode;
    AjPXmlNode grandParentNode;
    AjPStr nameString=NULL;
    AjPStr nameString2=NULL;

    nameString = ajStrNewC("ProtoDeclare");
    parentNode = xml_GetParent(xml_GetCurrentScene(file));
    grandParentNode = xml_GetParent(parentNode);
    protoNode = xml_MakeNewNode(file, nameString, grandParentNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    nameString2 = ajStrNewC("DNAPlot");
    xml_SetAttribute(protoNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("EXPORT");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Graphic_Children");
    xml_SetAttribute(protoNode, nameString, nameString2);

    ajStrDel(&nameString);
    nameString = ajStrNewC("field");
    fieldNode = xml_MakeNewNode(file,nameString, protoNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Source");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("type");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("SFString");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("value");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("");
    xml_SetAttribute(fieldNode, nameString, nameString2);

    ajStrDel(&nameString);
    nameString = ajStrNewC("field");
    xml_UnrefNode(fieldNode);
    fieldNode = xml_MakeNewNode(file,nameString, protoNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("IS");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Graphic_Children.Shape");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("name");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Shape");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("type");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Node");
    xml_SetAttribute(fieldNode, nameString, nameString2);
    ajStrDel(&nameString);
    nameString = ajStrNewC("value");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("NULL");
    xml_SetAttribute(fieldNode, nameString, nameString2);

    ajStrDel(&nameString);
    nameString = ajStrNewC("children");
    xml_UnrefNode(fieldNode);
    fieldNode = xml_MakeNewNode(file,nameString, protoNode);
    ajStrDel(&nameString);
    nameString = ajStrNewC("DEF");
    ajStrDel(&nameString2);
    nameString2 = ajStrNewC("Graphic_Children");
    xml_SetAttribute(fieldNode, nameString, nameString2);

    xml_UnrefNode(protoNode);
    xml_UnrefNode(fieldNode);
    xml_UnrefNode(grandParentNode);
    xml_UnrefNode(parentNode);
    ajStrDel(&nameString);
    ajStrDel(&nameString2);

    return;
}




/* @funcstatic xml_AddCommonBit *************************************
**
** Adds the nodes that are common for all files
**
** Could realy be all part of setUpOutputFile, but it is easier to 
** read
** Private Function
**
** @param [w] file [AjPXmlFile] file to add common bit to
**
** @return [void]
** @@
*********************************************************************/
static void xml_AddCommonBit(AjPXmlFile file)
{
    /* remove this */
    /*
       AjPXmlNode el2;
       AjPXmlNode el3;
       AjPXmlNode elChildren;
       */
    AjPXmlNode el;
    AjPStr nameString=NULL;

    /* Xj3D does not like this at the moment, sort it out */
    /*
       nameString = ajStrNewC("head");
       el = xml_MakeNewNode(file, nameString, xml_GetCurrentGraphic(file));
       ajStrDel(&nameString);
       nameString = ajStrNewC("meta");
       el2 = xml_MakeNewNode(file, nameString, el);
       ajStrDel(&nameString);
       */
    nameString = ajStrNewC("Scene");
    /*
       xml_UnrefNode(el);
       */
    el = xml_MakeNewNode(file, nameString, xml_GetCurrentGraphic(file));
    /*
       ajStrDel(&nameString);
       nameString = ajStrNewC("Group");
       el3 = xml_MakeNewNode(file, nameString, el);
       ajStrDel(&nameString);
       nameString = ajStrNewC("children");
       elChildren = xml_MakeNewNode(file, nameString, el3); 
       */

    /* I move file->currentScene and file->currentGraphic to the Scene*/
    /*
       xml_SetCurrentScene(file, elChildren);
       xml_SetCurrentGraphic(file, elChildren);
       */
    xml_SetCurrentScene(file, el);
    xml_SetCurrentGraphic(file, el);

    /*
       xml_UnrefNode(el); 
       xml_UnrefNode(el2);
       */
    /*
       xml_UnrefNode(el);
       */
    /* can you have this line? */
    /*
       xml_UnrefNode(el3);
       */
    ajStrDel(&nameString);

    return;
}




/* @funcstatic xml_WriteFile ***********************************************
**
** writes the file 
**
** @param [r] file [oXX3DFile] the file to write
**
** @return [AjBool] ajTrue if file written correctly
** @@
*********************************************************************/
static AjBool xml_WriteFile(AjPXmlFile file, AjPStr filename)
{
    GdomeException exc;

    if (!gdome_di_saveDocToFile (file->domimpl, file->doc, 
				 ajStrStr(filename),
				 GDOME_SAVE_LIBXML_INDENT, &exc)) 
    {
	printf ("DOMImplementation.saveDocToFile: failed\n\tException #%d\n",
		exc);
	return ajFalse;
    }

    return ajTrue;
}




/* @funcstatic xml_WriteStdout *********************************************
**
** writes the file 
**
** @param [r] file [oXX3DFile] the file to write
**
** @return [AjBool] ajTrue if file writen correctly
** @@
*********************************************************************/
static AjBool xml_WriteStdout(AjPXmlFile file)
{
    GdomeException exc;
    char *output;
    ajint outputLength, i;
  
    if (!gdome_di_saveDocToMemoryEnc (file->domimpl, file->doc, &output,
				      "UTF-8", GDOME_SAVE_LIBXML_INDENT,
				      &exc)) 
    {
	printf ("DOMImplementation.saveDocToMemory: failed\n\tException #%d\n",
		exc);
	return ajFalse;
    }
    outputLength = strlen(output);
    for(i = 0; i < outputLength; ++i)
    {
	printf("%c", output[i]);
    }
    free(output);
  
    return ajTrue;
}




/* @funcstatic xml_GetCurrentGraphic  ********************************
**
** Local Method
** gets the current Graphic
**
** @param [r] file [AjPXmlFile] the file
**
** @return [AjPXmlNode] current Graphic
** @@
*********************************************************************/
static AjPXmlNode xml_GetCurrentGraphic(AjPXmlFile file)
{
    return file->currentGraphic;
}




/* @funcstatic xml_GetCurrentScene  **********************************
**
** Local Method
** gets the current scene
**
** @param [r] file [AjPXmlFile] the file
**
** @return [AjPXmlNode] current scene
** @@
*********************************************************************/
static AjPXmlNode xml_GetCurrentScene(AjPXmlFile file)
{
    return file->currentScene;
}




/* @funcstatic xml_SetCurrentGraphic  ********************************
**
** Local Method
** Sets the current graphic to be node
**
** @param [w] file [AjPXmlFile] file to write to
** @param [r] node [AjPXmlNode] the node
**
** @return void
** @@
*********************************************************************/
static void xml_SetCurrentGraphic(AjPXmlFile file, AjPXmlNode node)
{
    if(file->currentGraphic && 
       !(file->currentScene == file->currentGraphic))
    {
	if(xml_GetNode(file->currentGraphic) != 0)
	{
	    xml_UnrefNode(file->currentGraphic);
	} 
    } 
    
    file->currentGraphic = node;

    return;
}




/* @funcstatic xml_SetCurrentScene  **********************************
**
** Local Method
** Sets the current Scene to be node
**
** @param [w] file [AjPXmlFile] file to write to
** @param [r] node [AjPXmlNode] the node
**
** @return void
** @@
*********************************************************************/
static void xml_SetCurrentScene(AjPXmlFile file, AjPXmlNode node)
{
    /* why does this cause proplems, but above does not? */
    /* but above does cause problems, I am not sure why */
    /* Sorted now I think, but a bit messy */
    /* Is it messy?  I kind of like the way it does it */
    
    if(file->currentScene &&
       !(file->currentScene == file->currentGraphic))
    {
	if(xml_GetNode(file->currentScene) != 0)
	{
	    xml_UnrefNode(file->currentScene);
	} 
    } 


    file->currentScene = node;

    return;
}




/* @funcstatic xml_SetNode  *****************************************
**
** Local Method
** Makes an AjPXmlNode
**
** @param [r] node [GdomeNode*] node to set
**
** @return [AjPXmlNode] new node
** @@
*********************************************************************/
static AjPXmlNode xml_SetNode(GdomeNode *node)
{
    AjPXmlNode retNode = ajXmlNodeNew();
    
    retNode->theNode = node;

    return retNode;
}




/* @funcstatic xml_GetNode  *****************************************
**
** Local Method
** gets the node of a AjPXmlNode
**
** @param [w] node [GdomeNode*] node to set
**
** @return [GdomeNode*] new node
** @@
********************************************************************/
static GdomeNode* xml_GetNode(AjPXmlNode node)
{
    return node->theNode;
}




/* @funcstatic xml_GetNodeElement  **********************************
**
** Local Method
** gets the node of a AjPXmlNode as a GdomeElement
**
** @param [r] node [AjPXmlNode] node to get
**
** @return [GdomeElement*] the node
** @@
*********************************************************************/
static GdomeElement* xml_GetNodeElement(AjPXmlNode node)
{
    return (GdomeElement *) node->theNode;
}




/* @funcstatic xml_GetParent  **************************************
**
** Local Method
** gets the node of a AjPXmlNode as a GdomeElement
**
** @param [r] node [AjPXmlNode] node to get
**
** @return [GdomeElement*] the node
** @@
*********************************************************************/
static AjPXmlNode xml_GetParent(AjPXmlNode node)
{
    GdomeException exc; 

    /* this really should not be running xml_SetNode, but I think it 
    ** has to.
    ** OK only as long as you unref the node each time */

    return xml_SetNode(gdome_n_parentNode(xml_GetNode(node), &exc));
}




/* @funcstatic xml_ClearFile ***********************************************
**
** Clears the memory allocated to this file
**
** @param [r] file [AjPXmlFile] the file to clear
**
** @return [void]
** @@
*********************************************************************/
static void xml_ClearFile(AjPXmlFile file)
{
    GdomeException exc;

    ajXmlNodeDel(&file->currentGraphic);
    ajXmlNodeDel(&file->currentScene);
    /*  gdome_di_freeDoc (file->domimpl, file->doc, &exc);*/
    gdome_doc_unref (file->doc, &exc); 
    gdome_di_unref (file->domimpl, &exc);
    ajXmlFileDel(&file);

    return;
}




/* @funcstatic  xml_UnrefNode ***********************************************
**
** Clears the memory allocated to this node
**
** @param [r] node [AjPXmlNode] the node to clear
**
** @return [void]
** @@
*********************************************************************/
static void xml_UnrefNode(AjPXmlNode node)
{
    GdomeException exc;

    gdome_n_unref (xml_GetNode(node), &exc);
    ajXmlNodeDel(&node);

    return;
}

#endif
