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
 * ZoomToOccuranceFactory.java
 *
 * Created on June 24, 2002, 11:46 AM
 *
 */

package org.emboss.grout;

import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import java.awt.Component;
import javax.swing.JPopupMenu;
import javax.swing.JMenuItem;

import com.sun.j3d.utils.picking.*;
import javax.media.j3d.*;
import javax.vecmath.Point3d;
import javax.vecmath.Point2d;
import java.awt.Point;
import java.awt.Dimension;

/*
import ;
 */

/** Controls a Zoom to type pick event */
public class ZoomToOccuranceFactory implements PickOccuranceFactory,
MouseListener
{
    
    /** The Grout Panel that this is looking at */    
    GroutPanel originator;
    
    /** This remebers the start point of a drag event */    
    Point first;
    
    /**
     * Creates a new ZoomToOccuranceFactory 
     *
     * @param origin The GroutPanel that this is looking at
     */    
    public ZoomToOccuranceFactory(GroutPanel origin)
    {
        originator = origin;
    }
    
    /**
     * depricated
     *
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     */    
    public void PerformPick(double x1, double y1, double x2, double y2)
    {
        // System.out.println("ZoomToOccurance at (" + x1 + ", " + y1 +
        //													 ") to (" + x2 + ", " + y2  + ")");
    }
    
    /**
     * depricated
     *
     * @param x
     * @param y
     */    
    public void PerformPick(double x, double y)
    {
        // System.out.println("ZoomToOccurance at (" + x + ", " + y + ")");
    }
    
    /** This brings up the right click menu
     * @param me The mouse event
     */    
    private void showRightClickMenu(final MouseEvent me)
    {
        // System.out.println("showRightClickMenu");
        
        JPopupMenu popup = new JPopupMenu();
        
        JMenuItem menuItem = new JMenuItem("Delete Graphic");
        menuItem.addActionListener(new java.awt.event.ActionListener()
        {
            public void actionPerformed(java.awt.event.ActionEvent e)
            {
                originator.removeGraphic
                    ((((Component) me.getSource()).getParent()).getParent());
                originator.validate();
            }
        }
        );
        popup.add(menuItem);
        menuItem = new JMenuItem("Do something else");
        // menuItem.addActionListener(new rightClickMenuListener());
        popup.add(menuItem);
        
        popup.show(me.getComponent(), me.getX(), me.getY());
        
    }
    
    /**
     * Handles the mouse click
     *
     * @param me The mouse event
     */    
    public void mouseClicked(MouseEvent me)
    {
        // System.out.println("me.getClickCount() = " + me.getClickCount());
        // if(me.getButton() == MouseEvent.BUTTON3)
        if(javax.swing.SwingUtilities.isRightMouseButton(me))
        {
            showRightClickMenu(me);
            return;
        }
        // System.out.println("mouseClicked = " + me);
        Canvas3D canvas = (Canvas3D) me.getSource();
        OneGraphic graphic1 = (OneGraphic) canvas.getParent().getParent();
        ViewPositionModel model = graphic1.getViewPositionModel();
        PickCanvas pickCanvas = new PickCanvas(canvas,
            graphic1.getRendWindContainer().getUniverse().getLocale());

        pickCanvas.setMode(PickTool.GEOMETRY_INTERSECT_INFO);
        pickCanvas.setTolerance(5.0f);
        pickCanvas.setShapeLocation(me);
        
        PickResult pickResult = pickCanvas.pickClosest();
        
        if (pickResult != null)
        {
            PickIntersection pickIntersection = 
                pickResult.getClosestIntersection
                    (pickCanvas.getStartPosition());
            
            if (pickIntersection != null)
            {
                Point3d p3d1 = pickIntersection.getClosestVertexCoordinates();
                Point3d[] coords = pickIntersection.getPrimitiveCoordinates();
                int index = pickIntersection.getClosestVertexIndex();
                int[] arrayIndex = 
                    pickIntersection.getPrimitiveCoordinateIndices();
                GeometryArray geometry = pickIntersection.getGeometryArray();
                
                Point3d p3d2 = new Point3d();
                geometry.getCoordinate(index, p3d2);
                
                Point3d top = new Point3d(Double.NEGATIVE_INFINITY,
                Double.NEGATIVE_INFINITY, 0);
                Point3d bottom = new Point3d(Double.POSITIVE_INFINITY,
                Double.POSITIVE_INFINITY, 0);
                
                for(int i = 0; i < coords.length; ++i)
                {
                    if(coords[i].x > top.x)
                        top.x = coords[i].x;
                    if(coords[i].y > top.y)
                        top.y = coords[i].y;
                    if(coords[i].x < bottom.x)
                        bottom.x = coords[i].x;
                    if(coords[i].y < bottom.y)
                        bottom.y = coords[i].y;
                }
                
                model.setViewPosition(bottom, top);
            }
        } else
            System.err.println("Cannot find line, please try again.");
    }
    
    /**
     * For compliamce with MouseListener
     *
     * @param me The mouse event
     */    
    public void mouseEntered(MouseEvent me)
    {
    }
    /**
     * For compliamce with MouseListener
     *
     * @param me The mouse event
     */    
    public void mouseExited(MouseEvent me)
    {
    }
    /**
     * For mouse Drag events
     *
     * @param me The mouse event
     */    
    public void mousePressed(MouseEvent me)
    {
        first = me.getPoint();
    }
    /**
     * For mouse Drag events
     *
     * @param me The mouse event
     */    
    public void mouseReleased(MouseEvent me)
    {
        if(first.x != (me.getPoint()).x || first.y != (me.getPoint()).y)
        {
            Canvas3D canvas = (Canvas3D) me.getSource();
            
            Point bottom = new Point();
            Point top = new Point();
            
            if(first.x > me.getPoint().x)
            {
                bottom.x = me.getPoint().x;
                top.x = first.x;
            }
            else
            {
                top.x = me.getPoint().x;
                bottom.x = first.x;
            }
            if(first.y < me.getPoint().y)
            {
                bottom.y = me.getPoint().y;
                top.y = first.y;
            }
            else
            {
                top.y = me.getPoint().y;
                bottom.y = first.y;
            }
            
            // System.out.println("top = " + top + " bottom = " + bottom);
            
            Dimension screenSize = canvas.getScreen3D().getSize();
            Dimension canvasSize = canvas.getSize();
            
            // System.out.println("screenSize " + screenSize );
            // System.out.println("canvasSize " +  canvasSize);
            
            
            double changeBottomXBy = 
                (double) bottom.x / (double) canvasSize.width ;
            double changeTopXBy = (double) top.x / (double) canvasSize.width;
            double changeBottomYBy = 
                1 - (double) bottom.y / (double) canvasSize.height;
            double changeTopYBy = 
                1 - (double) top.y / (double) canvasSize.height;
            
            
            OneGraphic graphic1 = (OneGraphic) canvas.getParent().getParent();
            ViewPositionModel model = graphic1.getViewPositionModel();
            
            model.setFractionalWindow(changeBottomXBy, changeBottomYBy,
            changeTopXBy, changeTopYBy);
            
        }
    }
    
    
}
