/*
 * GroutFileChooser.java
 *
 * Created on Feb 4th 2003
 */

package org.emboss.grout;

import java.io.File;
import javax.swing.JFileChooser;
import java.awt.*;

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
				int returnVal = chooser.showOpenDialog(parentComponent);
				if(returnVal == JFileChooser.APPROVE_OPTION) 
				{
						// System.out.println(" file = " + 
						// chooser.getSelectedFile().getAbsolutePath());
						return chooser.getSelectedFile();
				}	
				return null;
    }
}
