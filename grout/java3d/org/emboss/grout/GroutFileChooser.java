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
 * GroutFileChooser.java
 *
 * Created on Feb 4th 2003
 */

package org.emboss.grout;

import java.io.File;
import javax.swing.JFileChooser;
import java.awt.*;

/** This handles peopel selecting files for the grout panel. The methods just
 * override the ones of JFileChooser
 */
public class GroutFileChooser extends java.lang.Object
{
    
    GroutFileChooser()
    {
        
    }
    
    public static String showInputDialog(Component parentComponent,
    Object message,
    Object initialSelectionValue)
    {
        //				System.getProperties().list(System.out);
        File userDir = new File(System.getProperty("user.dir"));
        JFileChooser chooser = new JFileChooser(userDir);
        // Note: source for ExampleFileFilter can be found in FileChooserDemo,
        // under the demo/jfc directory in the Java 2 SDK, Standard Edition.
        int returnVal = chooser.showOpenDialog(parentComponent);
        if(returnVal == JFileChooser.APPROVE_OPTION)
        {
            // System.out.println(" file = " +
            // chooser.getSelectedFile().getAbsolutePath());
            return chooser.getSelectedFile().getAbsolutePath();
        }
        return null;
    }
    
    public static File showInputDialogFile(Component parentComponent,
    Object message,
    Object initialSelectionValue)
    {
        //				System.getProperties().list(System.out);
        File userDir = new File(System.getProperty("user.dir"));
        JFileChooser chooser = new JFileChooser(userDir);
        // Note: source for ExampleFileFilter can be found in FileChooserDemo,
        // under the demo/jfc directory in the Java 2 SDK, Standard Edition.
        int returnVal = chooser.showDialog(parentComponent, "Save");
        if(returnVal == JFileChooser.APPROVE_OPTION)
        {
            // System.out.println(" file = " +
            // chooser.getSelectedFile().getAbsolutePath());
            return chooser.getSelectedFile();
        }
        return null;
    }
}
