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
 * ZoomToOccurance.java
 *
 * Created on June 24, 2002, 11:46 AM
 */

package org.emboss.grout;

/** Depricated */
public class ZoomToOccurance implements PickOccurance
{
    
    private double x1;
    private double y1;
    private double x2;
    private double y2;
    
    public ZoomToOccurance(double x, double y)
    {
        x1 = x;
        y1 = y;
        x2 = y2 = Integer.MIN_VALUE;
    }
    
    public ZoomToOccurance(double x1, double y1, double x2,
    double y2)
    {
        this.x1 = x1;
        this.y1 = y1;
        this.x2 = x2;
        this.y2 = y2;
    }
    
    public void PerformPick()
    {
        // System.out.println("ZoomToOccurance at (" + x1 + ", " + y1 +
        //													 ") to (" + x2 + ", " + y2  + ")");
    }
    
}
