/***************************************************************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *  @author: Copyright (C) Hugh Morgan
 *
 ***************************************************************/

/*
 * ChangeColourOccuranceFactory.java
 *
 * Created on June 24, 2002, 11:46 AM
 */

package org.emboss.grout;

import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;

import com.sun.j3d.utils.picking.*;
import javax.media.j3d.*;
import javax.vecmath.Point3d;
import javax.vecmath.Point2d;
import javax.vecmath.Color4f;
import javax.vecmath.Color3f;
import java.awt.Point;
import java.awt.Dimension;
import java.awt.Color;

/*
import ;
 */

/** Changes one line of a graphic to a specific colour */
public class ChangeColourOccuranceFactory implements PickOccuranceFactory,
MouseListener
{
    
    /** The GroupPanel that this came from */
    GroutPanel originator;
    /** The colour to change it to */
    Color3f colour;
    
    public ChangeColourOccuranceFactory(GroutPanel origin)
    {
        originator = origin;
        colour = new Color3f(Color.white);
    }
    
    public ChangeColourOccuranceFactory(GroutPanel origin, String col)
    {
        originator = origin;
        if(col.equals("White"))
        {
            colour = new Color3f(Color.white);
        }
        if(col.equals("Black"))
        {
            colour = new Color3f(Color.black);
        }
        if(col.equals("Blue"))
        {
            colour = new Color3f(Color.blue);
        }
        if(col.equals("Pink"))
        {
            colour = new Color3f(Color.pink);
        }
        if(colour == null)
        {
            System.err.println("Colour " + col + " not recognised, using white");
            colour = new Color3f(Color.white);
        }
    }
    
    public void PerformPick(double x1, double y1, double x2, double y2)
    {
        // System.out.println("ZoomToOccurance at (" + x1 + ", " + y1 +
        //													 ") to (" + x2 + ", " + y2  + ")");
    }
    
    public void PerformPick(double x, double y)
    {
        // System.out.println("ZoomToOccurance at (" + x + ", " + y + ")");
    }
    
    private ChangeColourOccurance changeColourOfLine(PickIntersection pickIntersection,
    ChangeColourOccurance undoableOccurance)
    {
        
        GeometryArray geometry = pickIntersection.getGeometryArray();
        int[] colourIndex = pickIntersection.getPrimitiveColorIndices();
        int[] arrayIndex = pickIntersection.getPrimitiveCoordinateIndices();
        
        Color4f[] colours = pickIntersection.getPrimitiveColors();
        
        //				System.out.println("pickIntersection = " + pickIntersection);
        // System.out.println("geometry = " + geometry);
        
        // System.out.println("colourIndex = " + colourIndex);
        // System.out.println("colours = " + colours);
        
        Point3d p3d1 = pickIntersection.getClosestVertexCoordinates();
        Point3d[] coords = pickIntersection.getPrimitiveCoordinates();
        int index = pickIntersection.getClosestVertexIndex();
        Color4f colour2 = pickIntersection.getPointColor();
        
        // System.out.println("p3d1 = " + p3d1);
        // System.out.println("coords = " + coords);
        // System.out.println("index = " + index);
        // System.out.println("arrayIndex = " + arrayIndex);
        // System.out.println("colour2 = " + colour2);
        
        // System.out.print("Match between");
        for(int i = 0; i < coords.length; ++i)
        {
            // System.out.print(" point " + i + " = " + coords[i]);
        }
        // System.out.println("");
        //				System.out.println(" = " + );
        
        if(undoableOccurance == null)
        {
            // System.out.println("colours = " + colours);
            // System.out.println("colours[0] = " + colours[0]);
            // System.out.println(" colours[0].get() = " + colours[0].get());
            undoableOccurance = new ChangeColourOccurance(this,
            new Color3f(colours[0].get()),
            colour,
            pickIntersection);
            originator.addUndoableOccurance(undoableOccurance);
        }
        else
        {
            undoableOccurance.addPickIntersection(pickIntersection);
        }
        //				for(int i = 0; i < colourIndex.length; ++i)
        for(int i = 0; i < arrayIndex.length; ++i)
        {
            geometry.setColor(arrayIndex[i], colour);
        }
        
        return undoableOccurance;
    }
    
    public void mouseClicked(MouseEvent me)
    {
        Canvas3D canvas = (Canvas3D) me.getSource();
        OneGraphic graphic1 = (OneGraphic) canvas.getParent().getParent();
        
        PickCanvas pickCanvas = new PickCanvas(canvas,
        graphic1.getRendWindContainer().getUniverse().getLocale());
        
        pickCanvas.setMode(PickTool.GEOMETRY_INTERSECT_INFO);
        pickCanvas.setTolerance(5.0f);
        pickCanvas.setShapeLocation(me);
        
        PickResult pickResult = pickCanvas.pickClosest();
        
        if (pickResult != null)
        {
            PickIntersection pickIntersection = 
								pickResult.getClosestIntersection(pickCanvas.getStartPosition());
            ChangeColourOccurance undoableOccurance = changeColourOfLine(pickIntersection, null);
            for(int j = 0; j < pickResult.numIntersections(); ++j)
            {
                PickIntersection testIntersection = pickResult.getIntersection(j);
                Point3d[] testCoords = testIntersection.getPrimitiveCoordinates();
                Point3d[] pickCoords = pickIntersection.getPrimitiveCoordinates();
                boolean match = true;
                if(testCoords.length == pickCoords.length)
                {
                    for(int i = 0; i < testCoords.length; ++i)
                    {
                        if(testCoords[i].x != pickCoords[i].x)
                        {
                            match = false;
                        }
                        if(testCoords[i].y != pickCoords[i].y)
                        {
                            match = false;
                        }
                    }
                    if(match)
                    {
                        changeColourOfLine(testIntersection, undoableOccurance);
                    }
                }
            }
        } else
        {
            System.err.println("Cannot find line, please try again.");
        }
    }
    
    public void mouseEntered(MouseEvent me)
    {
    }
    public void mouseExited(MouseEvent me)
    {
    }
    public void mousePressed(MouseEvent me)
    {
    }
    public void mouseReleased(MouseEvent me)
    {
    }
    
    
}
