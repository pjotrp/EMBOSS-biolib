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
 * RemovedScene.java
 *
 * Created on June 24, 2002, 11:46 AM
 */

package org.emboss.grout;

import javax.media.j3d.BranchGroup;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;

import java.io.IOException;
import java.awt.datatransfer.UnsupportedFlavorException;

public class RemovedScene implements Transferable
{
    
    BranchGroup sceneRoot;
    OneGraphic originalGraphic;
    int xPos;
    int yPos;
    
    DataFlavor[] dataFlavors = new DataFlavor[1];
    
    public RemovedScene(BranchGroup sceneRoot, OneGraphic originalGraphic,
    int xPos, int yPos)
    {
        this.sceneRoot = sceneRoot;
        this.originalGraphic = originalGraphic;
        this.xPos = xPos;
        this.yPos = yPos;
        
        dataFlavors[0] = new DataFlavor(this.getClass(), "Grout Graphic");
    }
    
    public DataFlavor[] getTransferDataFlavors()
    {
        return dataFlavors;
    }
    
    public boolean isDataFlavorSupported(DataFlavor flavor)
    {
        // I am sure this is not the way to do it
        return (flavor.getHumanPresentableName().equals("Grout Graphic"));
    }
    
    public Object getTransferData(DataFlavor flavor)
    throws UnsupportedFlavorException,
    IOException
    {
        return new Object();
    }
    
}
