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
 * TransferableGraphic.java
 *
 * Created on June 24, 2002, 11:46 AM
 */

package org.emboss.grout;

import java.util.Vector;
import javax.media.j3d.BranchGroup;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;

import java.io.IOException;
import java.io.Serializable;
import java.awt.datatransfer.UnsupportedFlavorException;

import javax.vecmath.Point3d;
import javax.media.j3d.BoundingBox;

/** This wraps the info requred when transfering a graphic (with drag and drop) */
public class TransferableGraphic implements Transferable, Serializable
{
    // these all need to be static as they cannot be Serializable and
    // must be transferable
    /** this is the scene root of the last graphic created */    
    static BranchGroup sceneRoot;
    /** this is the original graphic of the last graphic created */    
    static OneGraphic originalGraphic;
    /** this is the pressent graphic of the last graphic created */    
    static OneGraphic presentGraphic;
    /** this is the symbol branch group of the last graphic created */    
    static BranchGroup symbol;
    
    /** this is a vector of scene roots */    
    static Vector sceneRoots = new Vector();
    /** this is a vector of original graphics */    
    static Vector originalGraphics = new Vector();
    /** this is a vector of pressent graphics */    
    static Vector presentGraphics = new Vector();
    /** this is a vector of symbol branch groups */    
    static Vector symbols = new Vector();
    
    /** the number of this transferable ghraphic.  used to extract the corect objects
     * from the static vectors
     */    
    int transferableGraphicNumber;
    /** total number of transferable graphics created */    
    static int numberOfransferableGraphics = 0;
    
    // BranchGroup sceneRoot;
    // OneGraphic originalGraphic;
    // OneGraphic presentGraphic;
    /** the x position of the grapic in the window */    
    int xPos;
    /** the y position of the grapic in the window */    
    int yPos;
    // ViewPositionModel model;
    /** the size of the scene, max x */    
    double maxX;
    /** the size of the scene, min x */    
    double minX;
    /** the size of the scene, max y */    
    double maxY;
    /** the size of the scene, min y */    
    double minY;
    
    /** the Data flavouras supported for this graphic type */    
    DataFlavor[] dataFlavors = new DataFlavor[1];
    
    /** the metadata of this graphic */    
    private Metadata metadata = null;
    
    /** creates the object, with null values */    
    public TransferableGraphic()
    {
        dataFlavors[0] = new DataFlavor(this.getClass(), "Grout Graphic");
    }
    
    /** creates the object, with all the values set
     * @param sceneRoot the scene root
     * @param originalGraphic the original graphic
     * @param presentGraphic the present graphic
     * @param xPos the x pos in the window
     * @param yPos the y pos in the window
     * @param model the view position model of this graphic
     * @param metadata the metadata
     */    
    public TransferableGraphic(BranchGroup sceneRoot,
    OneGraphic originalGraphic,
    OneGraphic presentGraphic,
    int xPos, int yPos, ViewPositionModel model,
    Metadata metadata)
    {
        // System.out.println("TransferableGraphic = " + this);
        // (new Exception()).printStackTrace();
        this.sceneRoot = sceneRoot;
        this.originalGraphic = originalGraphic;
        this.presentGraphic = presentGraphic;
        
        transferableGraphicNumber = numberOfransferableGraphics;
        ++numberOfransferableGraphics;
        
        if(sceneRoot != null)
        {
            if(sceneRoots.size() < transferableGraphicNumber)
            {
                sceneRoots.setSize(transferableGraphicNumber);
            }
            sceneRoots.add(transferableGraphicNumber, sceneRoot);
        }
        if(originalGraphic != null)
        {
            if(originalGraphics.size() < transferableGraphicNumber)
            {
                originalGraphics.setSize(transferableGraphicNumber);
            }
            // originalGraphics.ensureCapacity(transferableGraphicNumber);
            originalGraphics.add(transferableGraphicNumber, originalGraphic);
        }
        if(presentGraphic != null)
        {
            if(presentGraphics.size() < transferableGraphicNumber)
            {
                presentGraphics.setSize(transferableGraphicNumber);
            }
            // presentGraphics.ensureCapacity(transferableGraphicNumber);
            presentGraphics.add(transferableGraphicNumber, presentGraphic);
        }
        
        symbol = null;
        symbols.add(symbol);
        
        this.xPos = xPos;
        this.yPos = yPos;
        // this.model = model;
        this.metadata = metadata;
                
        if(sceneRoot != null)
        {
            BoundingBox box = Parser.calculateBoundingSphere(sceneRoot);
            Point3d lower = new Point3d();
            Point3d upper = new Point3d();
            box.getLower(lower);
            box.getUpper(upper);
            
            maxX = upper.x;
            minX = lower.x;
            maxY = upper.y;
            minY = lower.y;
        }
        
        dataFlavors[0] = new DataFlavor(this.getClass(), "Grout Graphic");
    }
    
    /** gets the dataflavours
     * @return the dataflavours
     */    
    public DataFlavor[] getTransferDataFlavors()
    {
        return dataFlavors;
    }
    
    /** check if this data flavour is supported
     * @param flavor the data flavor
     * @return whether the data flavour is supported
     */    
    public boolean isDataFlavorSupported(DataFlavor flavor)
    {
        // System.out.println("isDataFlavorSupported flavor = " + flavor);
        // System.out.println("dataFlavors[0] = " + dataFlavors[0]);
        // System.out.println("dataFlavors = " + dataFlavors);
        // System.out.println("originalGraphic = " + originalGraphic);
        
        
        return (flavor.getMimeType().indexOf
								(DataFlavor.javaSerializedObjectMimeType) > -1);
        // return (true);
    }
    
    /** returns the data object in the correct flavour
     * @param flavor the data flavour required
     * @throws UnsupportedFlavorException if the flavour is not supported
     * @throws IOException if the data is no longer available in the requested flavor.
     * @return the data
     */    
    public Object getTransferData(DataFlavor flavor)
    throws UnsupportedFlavorException,
    IOException
    {
        if(isDataFlavorSupported(flavor))
        {
            // System.out.println("returning transferableGraphic = " + this);
            return this;
        }
        else
        {
            throw new UnsupportedFlavorException(flavor);
        }
    }
    
    /** get the scene root
     * @return the scene root
     */    
    public BranchGroup getSceneRoot()
    {
        if(sceneRoots.size() > transferableGraphicNumber)
        {
            return (BranchGroup)
            sceneRoots.elementAt(transferableGraphicNumber);
        }
        else
        {
            return null;
        }
    }
    /** get the original graphic
     * @return the original graphic
     */    
    public OneGraphic getOriginalGraphic()
    {
        // return originalGraphic;
        return (OneGraphic) originalGraphics.elementAt(transferableGraphicNumber);
    }
    /** get the pressent graphic
     * @return the pressent graphic
     */    
    public OneGraphic getPresentGraphic()
    {
        // return presentGraphic;
        return (OneGraphic) presentGraphics.elementAt(transferableGraphicNumber);
    }
    /** get the symbols
     * @return the symbols
     */    
    public BranchGroup getSymbols()
    {
        // return presentGraphic;
        return (BranchGroup) symbols.elementAt(transferableGraphicNumber);
    }
    /** sets the symbols
     * @param symbol the symbols
     */    
    public void setSymbols(BranchGroup symbol)
    {
        symbols.setElementAt(symbol, transferableGraphicNumber);
    }
    /** get the x pos in the window
     * @return the x pos in the window
     */    
    public int getXPos()
    {
        return xPos;
    }
    /** get the y pos in the window
     * @return the y pos in the window
     */    
    public int getYPos()
    {
        return yPos;
    }
    /** gets the metadata
     * @return the metadata
     */    
    public Metadata getMetadata()
    {
        return metadata;
    }
    /** get maxX
     * @return maxX
     */    
    public double getMaxX()
    {
        return maxX;
    }
    /** get minX
     * @return minX
     */    
    public double getMinX()
    {
        return minX;
    }
    /** get maxY
     * @return max Y
     */    
    public double getMaxY()
    {
        return maxY;
    }
    /** get minY
     * @return minY
     */    
    public double getMinY()
    {
        return minY;
    }
    
    /** sets the scene root
     * @param sr the scene root
     */    
    public void setSceneRoot(BranchGroup sr)
    {
        while(sceneRoots.size() <= transferableGraphicNumber)
        {
            sceneRoots.addElement(null);
        }

        BoundingBox box = Parser.calculateBoundingSphere(sr);
        Point3d lower = new Point3d();
        Point3d upper = new Point3d();
        box.getLower(lower);
        box.getUpper(upper);
        
        maxX = upper.x;
        minX = lower.x;
        maxY = upper.y;
        minY = lower.y;
    }
    /** set view position model
     * @param model view position model
     */    
    public void setModel(ViewPositionModel model)
    {
        // System.out.println("setModel = " + model);
        maxX = model.getMaxX();
        minX = model.getMinX();
        maxY = model.getMaxY();
        minY = model.getMinY();
    }
    /** set metadata
     * @param metadata metadata
     */    
    public void setMetadata(Metadata metadata)
    {
        this.metadata = metadata;
    }
    /** set pressent graphic
     * @param pg pressent graphic
     */    
    public void setPresentGraphic(OneGraphic pg)
    {
        // presentGraphic = pg;
        if(presentGraphics.size() < transferableGraphicNumber)
        {
            presentGraphics.setSize(transferableGraphicNumber);
        }
        presentGraphics.add(transferableGraphicNumber, pg);
    }
    /** set original graphic
     * @param pg original graphic
     */    
    public void setOriginalGraphic(OneGraphic pg)
    {
        if(originalGraphics.size() < transferableGraphicNumber)
        {
            originalGraphics.setSize(transferableGraphicNumber);
        }
        //originalGraphics.ensureCapacity(transferableGraphicNumber);
        originalGraphics.add(transferableGraphicNumber, originalGraphic);
    }
    
    /** depricated
     * @param out
     * @throws IOException
     */    
    private void writeObject(java.io.ObjectOutputStream out)
    throws IOException
    {
        // // (new Exception()).printStackTrace();
        // originalGraphic.writeObject(out);
        // presentGraphic.writeObject(out);
        out.defaultWriteObject();
    }
    
    /** depricated
     * @param in
     * @throws IOException
     * @throws ClassNotFoundException
     */    
    private void readObject(java.io.ObjectInputStream in)
    throws IOException, ClassNotFoundException
    {
        // // (new Exception()).printStackTrace();
        // originalGraphic.readObject(in);
        // presentGraphic.readObject(in);
        in.defaultReadObject();
    }
    
    /** I need to overload this because the Drag and drop architecture does
     * copy by value on serialised objects
     * @param obj object to compare this to
     * @return true if the objects have the same scene root
     */
    public boolean equals(Object obj)
    {
        if(obj instanceof TransferableGraphic)
        {
            return getSceneRoot().equals(((TransferableGraphic) obj)
            .getSceneRoot());
        }
        else
        {
            return false;
        }
    }
    
}
