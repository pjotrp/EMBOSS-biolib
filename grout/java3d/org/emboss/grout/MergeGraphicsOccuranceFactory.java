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

/** Do not think this is used any more */
public class MergeGraphicsOccuranceFactory implements PickOccuranceFactory,
MouseListener
{
    
    GroutPanel originator;
    
    Point first;
    
    public MergeGraphicsOccuranceFactory(GroutPanel origin)
    {
        originator = origin;
    }
    
    public void PerformPick(double x1, double y1, double x2, double y2)
    {
        // System.out.println("MergeGraphics at (" + x1 + ", " + y1 +
        //													 ") to (" + x2 + ", " + y2  + ")");
    }
    
    public void PerformPick(double x, double y)
    {
        // System.out.println("MergeGraphics at (" + x + ", " + y + ")");
    }
    
    private void showRightClickMenu(final MouseEvent me)
    {
        // System.out.println("showRightClickMenu");
        
        JPopupMenu popup = new JPopupMenu();
        
        JMenuItem	menuItem = new JMenuItem("Delete Graphic");
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
        
        
        // menuItem = new JMenuItem("Do something else");
        // menuItem.addActionListener(new rightClickMenuListener());
        // popup.add(menuItem);
        
        popup.show(me.getComponent(), me.getX(), me.getY());
        
    }
    
    public void mouseClicked(MouseEvent me)
    {
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
    }
    
    public void mouseEntered(MouseEvent me)
    {
    }
    public void mouseExited(MouseEvent me)
    {
    }
    public void mousePressed(MouseEvent me)
    {
        first = me.getPoint();
    }
    public void mouseReleased(MouseEvent me)
    {
        // if(me.getButton() == MouseEvent.BUTTON3)
        if(javax.swing.SwingUtilities.isRightMouseButton(me))
        {
            showRightClickMenu(me);
            return;
        }
        
        
        OneGraphic graphic1 = (OneGraphic) ((Component) me.getSource())
        .getParent().getParent();
        int yPossInGroutPanel = me.getPoint().y + graphic1.getY();
        int xPossInGroutPanel = me.getPoint().x + 
						((Component) me.getSource()).getParent().getY();
        // System.out.println("yPossInGroutPanel = " + yPossInGroutPanel);
        Component dropedToGraphic = (Component) originator.getComponentAt
						(xPossInGroutPanel, yPossInGroutPanel);
        // System.out.println("dropedToGraphic = " + dropedToGraphic);
        if(dropedToGraphic != null)
        {
            if(OneGraphic.class.isAssignableFrom(dropedToGraphic.getClass()))
            {
                int dropThisGraphicNumber = 
										originator.getGraphicsVector().indexOf(graphic1);
                int dropedToGraphicNumber = 
										originator.getGraphicsVector().indexOf(dropedToGraphic);
                originator.mergeGraphics(dropedToGraphicNumber, 
																				 dropThisGraphicNumber);
            }
            else
            {
                System.err.println("Cannot add graphic to that!");
            }
        }
        else
        {
            System.err.println("Outside window, pos = " + xPossInGroutPanel + ", " + yPossInGroutPanel);
        }
    }
    
    
}
