/******************************************************************************
** @source AJAX 3D vector functions
**
** AjP3dVector objects are structures each of three floats specifying the
**  components of vectors in Cartesian three-space.
**
** The three float values stored in an AjP3dVector are, respectively,
**  the components of the vector it describes in the positive X, Y and
**  Z directions in a conventional right-handed Cartesian system
**
** Alternatively they can be thought of as the coefficients of the 
**  i, j, and k unit vectors in the x y and z directions respectively
**
** @author Copyright (C) 2003 Damian Counsell
** @version 1.0
** @modified $ Date: $
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

#include "ajax.h"
#include <math.h>


/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

/* ==================================================================== */
/* ========================= constructors ============================= */
/* ==================================================================== */

/* @section 3-D vector Constructors *******************************************
**
** All constructors return a new vector by pointer. It is the responsibility
** of the user to first destroy any previous vector. The target pointer
** does not need to be initialised to NULL, but it is good programming practice
** to do so anyway.
**
******************************************************************************/

/* @func aj3dVectorNew *******************************************************
**
** Default constructor for zeroed AJAX 3D vectors.
**
** @return [AjP3dVector] Pointer to a zeroed 3D vector
** @@
******************************************************************************/

AjP3dVector aj3dVectorNew(void)
{
    AjP3dVector ajp3dVectorReturnedVector = NULL;

    AJNEW0(ajp3dVectorReturnedVector);

    return ajp3dVectorReturnedVector;
}




/* @func aj3dVectorCreate ****************************************************
**
** Constructor for initialized AJAX 3D vectors.
** @param [r] fX [float] x component of 3D vector
** @param [r] fY [float] y component of 3D vector
** @param [r] fZ [float] z component of 3D vector
**
** @return [AjP3dVector] Pointer to an initialized 3D vector
** @@
******************************************************************************/

AjP3dVector aj3dVectorCreate(float fX, float fY, float fZ)
{
    AjP3dVector ajp3dVectorReturnedVector;

    AJNEW0(ajp3dVectorReturnedVector);
    ajp3dVectorReturnedVector->x = fX;
    ajp3dVectorReturnedVector->y = fY;
    ajp3dVectorReturnedVector->z = fZ;

    return ajp3dVectorReturnedVector;  
}




/* ==================================================================== */
/* =========================== destructor ============================= */
/* ==================================================================== */


/* @section 3D Vector Destructors ********************************************
**
** Destruction is achieved by deleting the pointer to the 3-D vector and
**  freeing the associated memory
**
******************************************************************************/

/* @func Aj3dVectorDel *******************************************************
**
** Default destructor for Ajax 3-D Vectors.
**
** If the given pointer is NULL, or a NULL pointer, simply returns.
**
** @param  [w] pthis [AjP3dVector*] Pointer to the 3-D vector to be deleted.
**         The pointer is always deleted.
** @return [void]
** @@
******************************************************************************/

void aj3dVectorDel(AjP3dVector* pthis)
{
    AjP3dVector thys = NULL;

    thys = pthis ? *pthis :0;

    if(!pthis)
	return;

    if(!*pthis)
	return;

    thys->x = 0.0;
    thys->y = 0.0;
    thys->z = 0.0;
    AJFREE(thys);
    *pthis = NULL;

    return;
}




/* @func Aj3dVectorCrossProduct **********************************************
**
** calculates the cross product of two 3D vectors, that is their "torque"
**
** @param [r] ajp3dVectorFirst [AjP3dVector] first 3D vector
** @param [r] ajp3dVectorSecond [AjP3dVector] second 3D vector
** @param [w] ajp3dVectorCrossProduct [AjP3dVector] 3D vector to contain
              cross product
** @@
******************************************************************************/

void aj3dVectorCrossProduct(AjP3dVector ajp3dVectorFirst,
			    AjP3dVector ajp3dVectorSecond,
			    AjP3dVector ajp3dVectorCrossProduct)
{
    float floatXOfCrossProduct;
    float floatYOfCrossProduct;
    float floatZOfCrossProduct;

    /* compute cross product */
    floatXOfCrossProduct  = ajp3dVectorFirst->y * ajp3dVectorSecond->z;
    floatXOfCrossProduct -= ajp3dVectorFirst->z * ajp3dVectorSecond->y;

    ajp3dVectorCrossProduct->x   = floatXOfCrossProduct;

    floatYOfCrossProduct  = ajp3dVectorFirst->z * ajp3dVectorSecond->x;
    floatYOfCrossProduct -= ajp3dVectorFirst->x * ajp3dVectorSecond->z;

    ajp3dVectorCrossProduct->y   = floatYOfCrossProduct;

    floatZOfCrossProduct  = ajp3dVectorFirst->x * ajp3dVectorSecond->y;
    floatZOfCrossProduct -= ajp3dVectorFirst->y * ajp3dVectorSecond->x;

    ajp3dVectorCrossProduct->z   = floatZOfCrossProduct;

    return;
}




/* @func aj3dVectorBetweenPoints *********************************************
**
** calculates the vector from one point in space (start) to another (end)
**
** @param [r] ajpFloatStartPoint [AjPFloat] co-ordinates of origin of vector
** @param [r] ajpFloatEndPoint co-ordinates [AjPFloat] of target of vector
** @param [r] ajpFloatVectorBetweenPoints [AjPFloat] vector from start to end
**
** @return [float] angle between vectors in degrees
** @@
******************************************************************************/

void aj3dVectorBetweenPoints(float startX, float startY, float startZ,
			     float endX, float endY, float endZ,
			     AjP3dVector ajp3dVectorBetweenPoints)
{
    /* compute vector between points */
    ajp3dVectorBetweenPoints->x = endX - startX;
    ajp3dVectorBetweenPoints->y = endY - startY;
    ajp3dVectorBetweenPoints->z = endZ - startZ;

    return;
}




/* @func aj3dVectorLength ****************************************************
**
** calculates the magnitude of a vector
**
** @param [r] ajp3dVectorToBeSized [AjP3dVector] vector to be sized
**
** @return floatVectorLength [float] length of vector to be sized
** @@
******************************************************************************/

float aj3dVectorLength(AjP3dVector ajp3dVectorToBeSized)
{
    float floatSquareOfVectorLength;
    float floatVectorLength;

    /* compute vector length */
    floatSquareOfVectorLength = ajp3dVectorToBeSized->x *
	ajp3dVectorToBeSized->x;

    floatSquareOfVectorLength += ajp3dVectorToBeSized->y *
	ajp3dVectorToBeSized->y;

    floatSquareOfVectorLength += ajp3dVectorToBeSized->z *
	ajp3dVectorToBeSized->z;

    floatVectorLength = (float)sqrt((double)floatSquareOfVectorLength);

    return floatVectorLength;
}




/* @func aj3dVectorAngle ***********************************************
**
** calculates the angle between two vectors
**
** @param [r] ajpFloatFirstVector [AjP3dVector] first vector
** @param [r] ajpFloatSecondVector [AjP3dVector] second vector
**
** @return [float] angle between vectors in degrees
** @@
******************************************************************************/

float aj3dVectorAngle(AjP3dVector ajp3dVectorFirst,
		      AjP3dVector ajp3dVectorSecond)
{
    float floatCosineOfVectorAngle;
    float floatVectorAngleInDegrees;
    float floatVectorAngle;

    /* XXXX STILL NEED TO INSERT A TRAP FOR VERY SMALL VALUES OF ANGLE */

    /* compute vector angle */
    floatCosineOfVectorAngle = aj3dVectorDotProduct(ajp3dVectorFirst,
						    ajp3dVectorSecond);
    floatCosineOfVectorAngle /= aj3dVectorLength(ajp3dVectorFirst);
    floatCosineOfVectorAngle /= aj3dVectorLength(ajp3dVectorSecond);

    floatVectorAngle = acos((double) floatCosineOfVectorAngle);

    floatVectorAngleInDegrees = ajRadToDeg(floatVectorAngle);

    return( floatVectorAngleInDegrees );
}




/* @func aj3dVectorDihedralAngle *********************************************
**
** calculates the angle from the plane perpendicular to A x B to the plane
**  perpendicular to B x C (where A, B and C are vectors)
**
** @param [r] ajp3dVectorA [AjP3dVector] 
** @param [r] ajp3dVectorB [AjP3dVector] 
** @param [r] ajp3dVectorC [AjP3dVector] 
**
** @return [float] dihedral angle
** @@
******************************************************************************/

float aj3dVectorDihedralAngle(AjP3dVector ajp3dVectorA,
			      AjP3dVector ajp3dVectorB,
			      AjP3dVector ajp3dVectorC)
{ 
    float floatDihedralAngle;

    AjP3dVector ajp3dVectorTorqueFirst  = NULL;
    AjP3dVector ajp3dVectorTorqueSecond = NULL;
    AjP3dVector ajp3dVectorTorqueThird  = NULL;

    ajp3dVectorTorqueFirst  = aj3dVectorNew();
    ajp3dVectorTorqueSecond = aj3dVectorNew();
    ajp3dVectorTorqueThird  = aj3dVectorNew();

    aj3dVectorCrossProduct(ajp3dVectorA, ajp3dVectorB,
			   ajp3dVectorTorqueFirst);
    aj3dVectorCrossProduct(ajp3dVectorB, ajp3dVectorC,
			   ajp3dVectorTorqueSecond);

    floatDihedralAngle = aj3dVectorAngle(ajp3dVectorTorqueFirst,
					 ajp3dVectorTorqueSecond);

    aj3dVectorCrossProduct(ajp3dVectorTorqueFirst, ajp3dVectorTorqueSecond,
			   ajp3dVectorTorqueThird);

    if(aj3dVectorDotProduct(ajp3dVectorB, ajp3dVectorTorqueThird) < 0.0)
	floatDihedralAngle = -1.0 * floatDihedralAngle;

    return floatDihedralAngle;
}




/* @func aj3dVectorDotProduct ***********************************************
**
** calculates the dot product of two 3D vectors, that is their summed common
**  scalar magnitude
**
** @param [r] ajp3dVectorFirst [AjP3dVector] first vector
** @param [r] ajp3dVectorSecond [AjP3dVector] second vector
**
** @return [float] dot product of first and second vectors
** @@
******************************************************************************/

float aj3dVectorDotProduct(AjP3dVector ajp3dVectorFirst,
			   AjP3dVector ajp3dVectorSecond)
{
    float floatDotProduct;

    /* compute dot product */
    floatDotProduct  = ajp3dVectorFirst->x * ajp3dVectorSecond->x;
    floatDotProduct += ajp3dVectorFirst->y * ajp3dVectorSecond->y;
    floatDotProduct += ajp3dVectorFirst->z * ajp3dVectorSecond->z;

    return(floatDotProduct);
}




/* @func aj3dVectorSum ********************************************************
**
** calculates the dot product of two 3D vectors, that is their summed common
**  scalar magnitude"
**
** @param [r] ajp3dVectorFirst [AjP3dVector] first vector
** @param [r] ajp3dVectorSecond [AjP3dVector] second vector
** @param [w] ajp3dVectorSum [AjP3dVector] sum of first and second vectors
**
** @@
******************************************************************************/

void aj3dVectorSum(AjP3dVector ajp3dVectorFirst,
		   AjP3dVector ajp3dVectorSecond,
		   AjP3dVector ajp3dVectorSum)
{
  /* compute sum of vectors by adding individual components */
  ajp3dVectorSum->x = ajp3dVectorFirst->x + ajp3dVectorSecond->x;
  ajp3dVectorSum->y = ajp3dVectorFirst->y + ajp3dVectorSecond->y;
  ajp3dVectorSum->z = ajp3dVectorFirst->z + ajp3dVectorSecond->z;

  return;
}
