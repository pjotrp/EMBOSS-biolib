/* Jemboss *****************************************************
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
*  @author: Copyright (C) Tim Carver
*
***************************************************************/

package org.emboss.jemboss;


import java.awt.*;
import javax.swing.*;
import javax.swing.event.*;

import java.awt.event.*;
import java.io.*;

import org.emboss.jemboss.gui.startup.*;
import org.emboss.jemboss.gui.filetree.*;   // local files
import uk.ac.mrc.hgmp.embreo.*;             // SOAP settings
import org.emboss.jemboss.gui.*;            // Jemboss graphics
import org.emboss.jemboss.soap.*;

/**
*
*  Java interface to EMBOSS (http://www.emboss.org/)
*  (i)  standalone - with a locally installation of EMBOSS.
*  (ii) client / server mode - download the client from a site,
*       such as the HGMP, which runs the Jemboss server.
*
*  @author Copyright (C) T. J. Carver
*  @version 1.0
*
*/
public class Jemboss 
{

// system properties
  private String fs = new String(System.getProperty("file.separator")); 
  private String ps = new String(System.getProperty("path.separator"));

// path names
  private String plplot;
  private String embossData;
  private String embossBin;
  private String embossPath;
  private String acdDirToParse;

  private File acdDir;

  private String cwd = new String(
                       System.getProperty("user.dir") + fs);
  private String homeDirectory = new String(
                       System.getProperty("user.home") + fs);


// Swing components
  private JFrame f;
  private JSplitPane pmain;
  private JSplitPane ptree;

  private JPanel p1;
  private JPanel p2;
  private JPanel p3;
  public static DragTree tree;
 
/** environment variables */
  private String[] envp = new String[4];

/** SOAP settings */
  static EmbreoParams mysettings;

/** true if in client-server mode (using SOAP) */
  static boolean withSoap;

/** to manage the pending results */
  public static PendingResults resultsManager;


  public Jemboss ()
  {

    Splash splashing=null;
    if(!withSoap)
    {
      JembossParams jp = new JembossParams();
      plplot = jp.getPlplot();
      embossData = jp.getEmbossData();
      embossBin = jp.getEmbossBin();
      embossPath = jp.getEmbossPath();
      acdDirToParse = jp.getAcdDirToParse();
      embossPath = new String("PATH" + ps +
                               embossPath + ps);
      acdDir = new File(acdDirToParse);
      envp[0] = "PATH=" + embossPath;        
      envp[1] = "PLPLOT_LIB=" + plplot;
      envp[2] = "EMBOSS_DATA=" + embossData;
      envp[3] = "HOME=" + homeDirectory;
    }
    else
    {
      splashing = new Splash(mysettings,3);
      resultsManager = new PendingResults(mysettings);
    }
    f = new JFrame("Jemboss");
    File root  = new File(System.getProperty("user.home"));
// make the local file manager
    tree = new DragTree(root, f);
    JScrollPane scrollTree = new JScrollPane(tree);

    p1 = new JPanel(new BorderLayout());
    p2 = new JPanel(new GridLayout());
    JScrollPane scrollProgForm = new JScrollPane(p2);

    JPanel pform = new JPanel(new BorderLayout());
    pform.add(scrollProgForm, BorderLayout.CENTER);

    if(withSoap)
      pform.add(resultsManager.statusPanel(f),BorderLayout.SOUTH);

    p3 = new JPanel(new BorderLayout());
    p3.add(scrollTree, BorderLayout.CENTER);
    
    Dimension d = f.getToolkit().getScreenSize();
    if(d.getWidth()<1024)
      scrollTree.setPreferredSize(new Dimension(130,500));
    else
      scrollTree.setPreferredSize(new Dimension(180,540));
   
    pmain = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
                                  p1,pform);
  
    ptree = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,pmain,p3);
    pmain.setOneTouchExpandable(true);
    ptree.setOneTouchExpandable(true);

    f.getContentPane().add(ptree);     // by default returns JPanel container

    if(d.getWidth()<1024)
      pmain.setPreferredSize(new Dimension(465,500));
    else
      pmain.setPreferredSize(new Dimension(525,540));

    f.setBackground(Color.white);
    new SetUpMenuBar(mysettings, f, envp, cwd, withSoap);

    JPanel pFront = new JPanel(new BorderLayout());
    p2.add(pFront);
    pFront.setBackground(Color.white);

    
//  Box bacross = new Box(BoxLayout.X_AXIS);
//  bacross.add(Box.createHorizontalStrut(10));
//  bacross.add(new JLabel(getFrontHTMLPage()));
    Box bacross = new Box(BoxLayout.Y_AXIS);
    bacross.add(new JembossLogo(120,145,55));

    pFront.add(bacross,BorderLayout.CENTER);

    JLabel lload = new JLabel(getLoadingHTMLPage());
    p1.add(lload, BorderLayout.CENTER);

    f.pack();
    f.setLocation(0,((int)d.getHeight()-f.getHeight())/2);
    ptree.setDividerLocation(1.0);  // hide file tree on start-up

    new BuildProgramMenu(p1,p2,scrollProgForm,embossBin,envp,mysettings,
                           withSoap,cwd,acdDirToParse,f,splashing);

    f.addWindowListener(new winExit());

  }


/**
*
*  Sets html front page
*  @return HTML as a String
*
*/
  private String getFrontHTMLPage()
  {
    String fp = new String("<html>" +
       "<body text=\"#000000\" bgcolor=\"#FF262D\" link=\"#0000EE\" vlink=\"#551A8B\"" +
       "alink=\"#FF0000\">"+
       "<b><font color=\"#FFFFFF\"><font size=+4></font></font></b>&nbsp;<b><font color=\"#" +
       "FFFFFF\"><font size=+4></font></font></b>" +
       "<center>" +
       "<p><b><tt><font color=\"#FFFFFF\"><font size=+4>J<br>" +
       "E<br>M<br>B<br>O<br>S<br>S<br>" +
       "</font></font></tt></b><br>&nbsp;<br>&nbsp;" +
       "<p><b><font color=\"#FFFFFF\"><font size=+2>HGMP-RC</b></font>" +
       "<br><b><font color=\"#FFFFFF\"><font size=+2>Cambridge</b></font>" +
       "<br><b><font color=\"#FFFFFF\"><font size=+2>UK</font></b></center>" +
       "</body></html>");

    return fp;
  }



/**
*
*  Sets html loading page
*  @return HTML as a String
*  
*/
  private String getLoadingHTMLPage()
  {
     String fp = new String("<html>" +
       "<body text=\"#000000\"" +
       "alink=\"#FF0000\">&nbsp;<center>"+
       "<p><b><tt><font color=\"#FFFFFF\"><font size=+1>Loading</font></font></tt></b>" +
       "<br><b><tt><font color=\"#FFFFFF\"><font size=+1>Jemboss</font></font></tt></b><b><tt>" +
       "<font color=\"#FFFFFF\"><font size=+1></font></font></tt></b><p><b><tt><br><br><br>" +
       "<font color=\"#FFFFFF\"><font size=+1>Please Wait...</font></font></tt></b><br><br>" +
       "<font color=\"#FFFFFF\"></font></center></b></body></html>");

    return fp;
  }


/**
*
*  Delete temporary files
*  @param current working directory (local)
*
*/
  private void deleteTmp(File cwd, final String suffix) 
  {

    String tmpFiles[] = cwd.list(new FilenameFilter()
    {
      public boolean accept(File cwd, String name)
      {
        return name.endsWith(suffix);
      };
    });

    for(int h =0;h<tmpFiles.length;h++)
    {
      File tf = new File(tmpFiles[h]);
      tf.delete();
    }
  }


/**
*
* Extends WindowAdapter to close window 
*
*/
  class winExit extends WindowAdapter
  {
     public void windowClosing(WindowEvent we)
     {
        deleteTmp(new File(cwd), ".jembosstmp");
        System.exit(0);
     }
  }


/**
*
* Launches Jemboss in standalone or client-server mode.
*
*/
  public static void main (String args[])
  {
    
    // initialize settings
    mysettings = new EmbreoParams("jemboss");

    if(args.length > 0)
    {
      if(args[0].equalsIgnoreCase("local"))
      {
        withSoap = false; 
        System.out.println("Standalone mode");
      }
      else 
      {
        withSoap = true; 
        System.out.println("Client-server mode");
      }
    }
    else
      withSoap = true;

    new Jemboss();

  }

}

