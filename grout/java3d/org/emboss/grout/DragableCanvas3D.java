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
 * DragableCanvas3D.java
 *
 * Created on October 4th, 2002
 */

package org.emboss.grout;

import javax.media.j3d.Canvas3D;
import java.awt.datatransfer.*;
import java.awt.dnd.*;

import com.sun.j3d.utils.picking.*;
import java.io.IOException;
//import java.beans.PropertyChangeEvent;
import java.awt.event.InputEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseEvent;
import javax.swing.JPopupMenu;
import javax.swing.JMenuItem;
import java.awt.Component;
import javax.media.j3d.BranchGroup;
import java.io.Serializable;
import java.util.Vector;
import java.awt.*;
import java.awt.event.*;

import org.emboss.jemboss.editor.*;

/** This ads drag and drop functionality to a Canvas3D
 * @author hmorgan2
 */
public class DragableCanvas3D extends Canvas3D implements
DragSourceListener, DragGestureListener, DropTargetListener,
MouseListener, MouseMotionListener, Serializable, ComponentListener
{
    
    private DragSource dragSource;
    private DropTarget dropTarget;
    
    private Vector targets = new Vector();
    
    private ViewPositionModel model;
    
    // static MouseListener listener;
    private MouseListener listener;
    
    /** Creates a new instance of DragableCanvas3D */
    public DragableCanvas3D(java.awt.GraphicsConfiguration graphicsConfiguration,
    boolean offScreen)
    {
        super(graphicsConfiguration, offScreen);
        dropTarget = new DropTarget(this, this);
        setupDragAndDrop();
        // addComponentListener(this);
        // setToolTipText("Try me!");
    }
    
    /** Does the setup for DnD */    
    public void setupDragAndDrop()
    {
        dragSource = new DragSource();
        dragSource.createDefaultDragGestureRecognizer(this,
        DnDConstants.ACTION_MOVE,
        this);
        // if(listener = null)
        //{
        listener = new DefaultBehaviourOccuranceFactory();
        //}
        addMouseListener(listener);
        addMouseListener(this);
        addMouseMotionListener(this);
    }
    
    /** This just adds a listener */    
    public void setLocationListener(DragableCanvas3DListener lis)
    {
        addDragableCanvas3DListener(lis);
    }
    
    public void dragGestureRecognized(DragGestureEvent e)
    {
        
        // System.out.println("getParent() = " + getParent());
        
        if(!e.getTriggerEvent().isShiftDown())
        {
            TransferableGraphic graphic =
            ((RendWindContainer) getParent()).pickGraphic(this,
            e.getDragOrigin());
            
            if(graphic != null)
            {
                dragSource.startDrag(e, DragSource.DefaultMoveDrop, graphic, this);
            }
        }
    }
    
    public void dragDropEnd(DragSourceDropEvent e)
    {
        OneGraphic originalGraphic = ((TransferableGraphic)
        e.getDragSourceContext().
        getTransferable())
        .getOriginalGraphic();
        RendWindContainer rendWindContainer = ((RendWindContainer) getParent());
        // if(originalGraphic.getScenes().size() == 0)
        if(rendWindContainer.getScenes().size() == 0)
        {
            // This bit is to replace an empty OneGraphic with a white box
            
            originalGraphic.makeAWhiteBox();
        }
        rendWindContainer.setBackgroundColour("White");
    }
    public void dragEnter(DragSourceDragEvent e)
    {
        // System.out.println("dragging dragEnter at drag");
        // System.out.println("dragEnter");
    }
    public void dragExit(DragSourceEvent e)
    {
        // System.out.println("dragging dragExit");
        // // System.out.println("dragExit");
    }
    public void dragOver(DragSourceDragEvent e)
    {
        // System.out.println("dragging dragOver");
        // System.out.println("dragOver");
    }
    public void dropActionChanged(DragSourceDragEvent e)
    {
        // System.out.println("dragging dropActionChanged");
        // System.out.println("dropActionChanged");
    }
    
    public void dragEnter(DropTargetDragEvent e)
    {
        DataFlavor[] flavours = e.getCurrentDataFlavors();
        for(int i = 0; i < flavours.length; ++i)
        {
            
        }
        // if(e.isDataFlavorSupported(new DataFlavor(TransferableGraphic.class,
        // 																					 "Grout Graphic")))
        if(true)
        {
            // System.out.println("dragging acceptDrag dragEnter");
            e.acceptDrag(DnDConstants.ACTION_MOVE);
            ((RendWindContainer) ((DragableCanvas3D) ((DropTarget) e.getSource()).getComponent()).getParent()).setBackgroundColour("LightGray");
        }
    }
    
    public void dragExit(DropTargetEvent e)
    {
        ((RendWindContainer) ((DragableCanvas3D) ((DropTarget) e.getSource()).getComponent()).getParent()).setBackgroundColour("White");
    }
    
    public void dragOver(DropTargetDragEvent e)
    {
        // System.out.println("dragging dragOver");
    }
    
    
    public void drop(DropTargetDropEvent e)
    {
        // System.out.println("df dragging drop this = " + this);
        // (new Exception()).printStackTrace();
        DataFlavor[] flavours = e.getCurrentDataFlavors();
        
        flavours = e.getTransferable().getTransferDataFlavors();
        
        try
        {
            Transferable tr = e.getTransferable();
            if(tr.isDataFlavorSupported
							 (new DataFlavor(TransferableGraphic.class,
															 "Grout Graphic")))
                // if(true)
            {
                e.acceptDrop(DnDConstants.ACTION_MOVE);
                RendWindContainer parent = (RendWindContainer) getParent();
                OneGraphic grandParent = ((OneGraphic) parent.getParent());
                TransferableGraphic transferableGraphic =
                (TransferableGraphic) tr.getTransferData
                (new DataFlavor(TransferableGraphic.class, "Grout Graphic"));
                
                GroutPanel toGreatgrandParent =
                ((GroutPanel) grandParent.getParent());
                GroutPanel fromGreatgrandParent =
                ((GroutPanel) transferableGraphic.getOriginalGraphic()
                .getParent());
                
                // I think this is the way to go
                transferableGraphic.getPresentGraphic().removeThisGraphic
										(transferableGraphic);
                
                // This does not work, both cos of the coppy of BranchGroup
                // and fromGreatgrandParent of previouly moved graphic
                // if(toGreatgrandParent != fromGreatgrandParent)
                if(false)
                {
                    BranchGroup newSceneGroup = (BranchGroup)
                    transferableGraphic.getSceneRoot().cloneTree(true);
                    Parser.setAllCapabilitys(transferableGraphic.getSceneRoot(), 0);
                    Parser.setAllCapabilitys(newSceneGroup, 0);
                    // this is better
                    transferableGraphic.getOriginalGraphic().addThisGraphic
												(transferableGraphic);
                    transferableGraphic.setSceneRoot(newSceneGroup);
                    
                }
                
                grandParent.addThisGraphic(transferableGraphic);
                // transferableGraphic.setPresentGraphic(grandParent);
                parent.setBackgroundColour("White");
                e.getDropTargetContext().dropComplete(true);
            } else
            {
                System.err.println("Rejected");
                e.rejectDrop();
            }
        } catch (IOException io)
        {
            io.printStackTrace();
            e.rejectDrop();
        } catch (UnsupportedFlavorException ufe)
        {
            ufe.printStackTrace();
            e.rejectDrop();
        }
    }
    
    public void drop(Transferable tr)
    {
        // System.out.println("df dragging drop this = " + this);
        try
        {
            if(tr.isDataFlavorSupported(new DataFlavor(tr.getClass(), "Grout Graphic")))
                // if(true)
            {
                RendWindContainer parent = (RendWindContainer) getParent();
                OneGraphic grandParent = ((OneGraphic) parent.getParent());
                TransferableGraphic transferableGraphic =
                (TransferableGraphic) tr.getTransferData
                (new DataFlavor(TransferableGraphic.class, "Grout Graphic"));
                GroutPanel toGreatgrandParent =
                ((GroutPanel) grandParent.getParent());
                GroutPanel fromGreatgrandParent =
                ((GroutPanel) transferableGraphic.getOriginalGraphic()
                .getParent());
                transferableGraphic.getPresentGraphic().removeThisGraphic(transferableGraphic);
                
                // This does not work, both cos of the coppy of BranchGroup
                // and fromGreatgrandParent of previouly moved graphic
                // if(toGreatgrandParent != fromGreatgrandParent)
                if(false)
                {
                    BranchGroup newSceneGroup = (BranchGroup)
                    transferableGraphic.getSceneRoot().cloneTree(true);
                    Parser.setAllCapabilitys(transferableGraphic.getSceneRoot(), 0);
                    Parser.setAllCapabilitys(newSceneGroup, 0);
                    transferableGraphic.getOriginalGraphic().addThisGraphic
												(transferableGraphic);
                    transferableGraphic.setSceneRoot(newSceneGroup);
                    
                }
                
                grandParent.addThisGraphic(transferableGraphic);
                parent.setBackgroundColour("White");
            } else
            {
                System.err.println("Rejected");
                // e.rejectDrop();
            }
        } catch (IOException io)
        {
            io.printStackTrace();
            // e.rejectDrop();
        } catch (UnsupportedFlavorException ufe)
        {
            ufe.printStackTrace();
        }
    }
    
    public void dropActionChanged
    (DropTargetDragEvent e)
    {
    }
    
    /** This shows the menu that pops up when you right click */    
    private void showRightClickMenu(final MouseEvent me,
    final TransferableGraphic tr)
    {
        // System.out.println("showRightClickMenu");
        
        JPopupMenu popup = new JPopupMenu();
        
        JMenuItem	menuItem = new JMenuItem("Delete 1 Graphic");
        menuItem.addActionListener(new java.awt.event.ActionListener()
        {
            public void actionPerformed(java.awt.event.ActionEvent e)
            {
                tr.getPresentGraphic().removeThisGraphic(tr);
                // originator.validate();
                RendWindContainer rendWindContainer = 
										((RendWindContainer) getParent());
                
                if(rendWindContainer.getScenes().size() == 0)
                {
                    java.awt.Container parent = 
												tr.getPresentGraphic().getParent();
                    parent.remove(tr.getOriginalGraphic());
                    parent.validate();
                }
            }
        }
        );
        popup.add(menuItem);
        // This adds the 'Seperate' item.  Does not work
        menuItem = new JMenuItem("Add to own Display");
        menuItem.addActionListener(new java.awt.event.ActionListener()
        {
            public void actionPerformed(java.awt.event.ActionEvent e)
            {
                RendWindContainer parent = (RendWindContainer) getParent();
                OneGraphic grandParent = ((OneGraphic) parent.getParent());
                GroutPanel greatGrandParent =
                ((GroutPanel) grandParent.getParent());
                greatGrandParent.newGroutWindow
                (grandParent.getGraphicNumber());
            }
        }
        );
        popup.add(menuItem);
        
        popup.show(me.getComponent(), me.getX(), me.getY());
        
    }
    
    public ViewPositionModel getModel()
    {
        RendWindContainer parent = ((RendWindContainer) getParent());
        OneGraphic grandParent = ((OneGraphic) parent.getParent());
        ViewPositionModel model = ((ViewPositionModel) grandParent
        .getViewPositionModel());
        return model;
    }
    
    public synchronized void addDragableCanvas3DListener(DragableCanvas3DListener listener)
    {
        targets.addElement(listener);
        // model = getModel();
    }
    
    public synchronized void removeDragableCanvas3DListener(DragableCanvas3DListener listener)
    {
        targets.removeElement(listener);
    }
    
    public void mouseClicked(MouseEvent e)
    {
    }
    public void mousePressed(MouseEvent e)
    {
        // System.out.println("mousePressed");
    }
    public void mouseReleased(MouseEvent e)
    {
        // System.out.println("mouseReleased");
    }
    public void mouseEntered(MouseEvent e)
    {
        // System.out.println("mouseEntered");
    }
    public void mouseExited(MouseEvent e)
    {
        // System.err.println("mouseExited");
        notifyTargets("mouseExited", null, null);
    }
    
    public void mouseDragged(MouseEvent e)
    {
    }
    
    public void mouseMoved(MouseEvent e)
    {
        if(model == null)
        {
            model = getModel();
        }
        int screenPositionX = e.getX();
        int screenPositionY = e.getY();
        Point screenPosition = e.getPoint();
        Dimension screenSize = getSize();
        
        Point.Double modelPosition = new Point.Double
        (model.transformScreenToModelX(screenPosition, screenSize),
        model.transformScreenToModelY(screenPosition, screenSize));
        
        notifyTargets("mouseMoved", null, modelPosition);
    }
    
    /** I put these in to make it serialisable, but it does not really work (Canvas3D is
     * inherantly non-serialisable)
     */    
    private void writeObject(java.io.ObjectOutputStream out)
    throws IOException
    {
        // // (new Exception()).printStackTrace();
        // originalGraphic.writeObject(out);
        // presentGraphic.writeObject(out);
        out.defaultWriteObject();
    }
    
    /** I put these in to make it serialisable, but it does not really work (Canvas3D is
     * inherantly non-serialisable)
     */    
    private void readObject(java.io.ObjectInputStream in)
    throws IOException, ClassNotFoundException
    {
        // // (new Exception()).printStackTrace();
        // originalGraphic.readObject(in);
        // presentGraphic.readObject(in);
        in.defaultReadObject();
    }
    
    protected void notifyTargets(String propertyName, Object oldValue,
    Object newValue)
    {
        Vector l;
        DragableCanvas3DEvent s = new DragableCanvas3DEvent((Object) this,
        propertyName,
        oldValue, newValue);
        synchronized(this)
        {
            l = (Vector) targets.clone();
        }
        for (int i = 0; i < l.size(); i++)
        {
            DragableCanvas3DListener sl = (DragableCanvas3DListener) 
								l.elementAt(i);
            sl.dragableCanvas3DChanged(s);
        }
    }
    
    public void componentMoved(ComponentEvent e)
    {
    }
    
    public void componentShown(ComponentEvent e)
    {
    }
    
    public void componentHidden(ComponentEvent e)
    {
    }
    
    public void componentResized(ComponentEvent e)
    {
        // (new Exception()).printStackTrace();
    }
    
}
