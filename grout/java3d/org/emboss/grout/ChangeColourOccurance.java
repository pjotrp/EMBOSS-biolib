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
 * ChangeColourOccurance.java
 *
 * Created on May 27th 2003
 */

package org.emboss.grout;

import com.sun.j3d.utils.picking.*;
import javax.vecmath.Color3f;
import javax.vecmath.Point3d;
import javax.media.j3d.*;
import java.util.Vector;

/** Changes one line of a graphic to a specific colour
 * @author hmorgan2
 */
public class ChangeColourOccurance implements UndoableOccurance
{
    
    private ChangeColourOccuranceFactory source;
    private Color3f originalColour;
    private Color3f newColour;
    //		private PickIntersection pickIntersection;
    private Vector pickIntersections = new Vector();;
    
    /** Creates a new instance of ChangeColourOccurance */
    public ChangeColourOccurance(ChangeColourOccuranceFactory sc, Color3f orig,
    Color3f newCol, PickIntersection pi)
    {
        source = sc;
        originalColour = orig;
        newColour = newCol;
        pickIntersections.addElement(pi);
    }
    
    public void addPickIntersection(PickIntersection pi)
    {
        pickIntersections.addElement(pi);
    }
    
    public void undo()
    {
        changeColourOfLine(originalColour);
    }
    
    public void redo()
    {
        changeColourOfLine(newColour);
    }
    
    private void changeColourOfLine(Color3f colour)
    {
        for(int j = 0; j < pickIntersections.size(); ++j)
        {
            PickIntersection pickIntersection =
            ((PickIntersection) pickIntersections.elementAt(j));
            GeometryArray geometry = pickIntersection.getGeometryArray();
            int[] colourIndex = pickIntersection.getPrimitiveColorIndices();
            
            for(int i = 0; i < colourIndex.length; ++i)
            {
                geometry.setColor(colourIndex[i], colour);
            }
        }
    }
}
