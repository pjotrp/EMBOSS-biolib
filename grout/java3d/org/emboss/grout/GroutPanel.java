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
 * GroutPanel.java
 *
 * Created on July 15, 2002, 4:20 PM
 */

package org.emboss.grout;

import java.awt.GridBagLayout;
import java.util.Vector;
import java.awt.image.BufferedImage;
import java.util.Enumeration;
import java.util.StringTokenizer;
import java.util.Hashtable;
//import ;

// the ones from MiniFrame
import java.beans.*;
import javax.swing.*;

// the ones from loader
// Standard imports
import java.applet.Applet;
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.media.j3d.*;
import javax.vecmath.*;
import java.net.URL;
import java.net.MalformedURLException;

// Application specific imports
import com.sun.j3d.utils.applet.MainFrame;
import com.sun.j3d.utils.universe.*;
import com.sun.j3d.utils.behaviors.mouse.MouseRotate;
import com.sun.j3d.utils.behaviors.mouse.MouseZoom;
import com.sun.j3d.utils.behaviors.mouse.MouseTranslate;

import com.sun.j3d.loaders.Scene;

import org.ietf.uri.ContentHandlerFactory;
import org.ietf.uri.FileNameMap;
import org.ietf.uri.URI;

import org.web3d.j3d.loaders.VRML97Loader;
import org.web3d.net.content.VRMLContentHandlerFactory;
import org.web3d.net.content.VRMLFileNameMap;

// Picking imports

import com.sun.j3d.utils.picking.*;

//temp imports

import org.web3d.vrml.renderer.DefaultNodeFactory;

/** Main class of Grout
 * @author hmorgan2
 */
public class GroutPanel extends javax.swing.JPanel implements
java.io.Serializable, ControlContainerListener, ParserListener,
WindowListener, ComponentListener, OneGraphicListener
{
    
    private Vector targets = new Vector();
    
    //  private RendWindContainer canvas;
    private Parser parser;
    /** The panel to put all the graphics */    
    private JPanel renderPanel;
    private GridBagLayout gridbag = new GridBagLayout();
    private GridBagConstraints c = new GridBagConstraints();
    
    /** Used when there is a shared X-axis */    
    private JPanel xAxis;
    /** Used when there is a shared X-axis */    
    private AxisMarks xAxisMarks = null;
    //  private AxisMarks xAxisMarks = null, yAxisMarks = null;
    //  private SequenceView xSequence = null;
    private ViewPositionModel viewPositionModel;
    /** Used when there is a shared X-axis */    
    private ControlContainer controls;
    
    private Vector graphics = new Vector();
    private Vector removedGraphics = new Vector();
    
    //  private Vector controls = new Vector();
    //  private Vector viewPositionModel = new Vector();
    
    private JMenuBar menuBar;
    private JToolBar toolBar;
    // private JMenu graphicsMenu;
    /** Not used */    
    private JMenu removedGraphicsMenu;
    private Vector toolbarLabels = new Vector();
    
    private boolean scaleFactorsLocked = false;
    /** true if this is being drawn off screen */    
    private boolean offScreen = false;
    
    /** The window this grout panel is being displayed in */    
    private Window ownerWindow = null;
    
    private JCheckBoxMenuItem useOneXAxis = new JCheckBoxMenuItem("Use one X axis");
    private JMenuItem menuItemUndo;
    private JMenuItem menuItemRedo;
    
    private	PickOccuranceFactory pickFactory;
    
    private Vector undoables = new Vector();
    private	int undoableIndex = -1;
    
    private String xSequenceString = null;
    private String ySequenceString = null;
    
    public static int RIGHT_INDENT = 8;
    private boolean usePositionLabel = false;
    /** Used to make bits of the screen the right colour */    
    private JComponent paddingBox;
    
    /** Creates new form GroutPanel */
    public GroutPanel()
    {
        super();
        // System.out.println("GroutPanel()");
        setLayout(gridbag);
        try
        {
            loadSystemLibrarys();
        } catch(Exception e)
        {
            e.printStackTrace();
        }
        // loadProperties();
        parser = new Parser();
        parser.setOwnerPanel(this);
        //    setupRenderingPanel(false);
        setupControls();
        setupviewPositionModel();
        setupToolbarAndMenu();
        //    setupListeners();
        //    attachPickBehavior(canvas.getCanvas());
    }
    
    /** Creates new form GroutPanel
     * @param filename filename
     */
    public GroutPanel(String filename)
    {
        super();
        // System.out.println("GroutPanel(String filename)");
        setLayout(gridbag);
        
        try
        {
            loadSystemLibrarys();
        } catch(Exception e)
        {
            e.printStackTrace();
        }
        // loadProperties();
        parser = new Parser();
        parser.setOwnerPanel(this);
        
        //        contentPane.setLayout(gridbag);
        //    setupRenderingPanel(false);
        setupControls();
        setupviewPositionModel();
        setupToolbarAndMenu();
        //    setupListeners();
        //    attachPickBehavior(canvas.getCanvas());
        
        parser.setFilename(filename);
    }
    
    /** Sets this panel to read the x3d file from a string
     * @param x3DFile
     */    
    public void setX3DFile(String x3DFile)
    {
        parser.setMetaFile(x3DFile);
    }
    
    public void setFilename(String x3DFile)
    {
        parser.setFilename(x3DFile);
    }
    
    public JMenuBar getMenuBar()
    {
        return menuBar;
    }
    
    public JToolBar getToolBar()
    {
        return toolBar;
    }
    
    public String getXSequence()
    {
        return xSequenceString;
    }
    
    public String getYSequence()
    {
        return ySequenceString;
    }
    
    public Vector getGraphicsVector()
    {
        return graphics;
    }
    
    public void addUndoableOccurance(UndoableOccurance undoableOccurance)
    {
        if(!menuItemUndo.isEnabled())
        {
            menuItemUndo.setEnabled(true);
        }
        if(undoableIndex < (undoables.size() - 1))
        {
            // this works, but has to be slow hugh
            undoables.setSize(undoableIndex + 1);
        }
        undoables.addElement(undoableOccurance);
        undoableIndex++;
    }
    
    /** Creates new form GroutPanel
     * @param filename
     * @param source
     * @param offScreenInstance
     */    
    public GroutPanel(String filename, String source, boolean offScreenInstance)
    {
        super();
        try
        {
            loadSystemLibrarys();
        } catch(Exception e)
        {
            e.printStackTrace();
        }
        // loadProperties();
        // System.out.println("source = " +  source);
        if(offScreenInstance)
        {
            offScreen = true;
            setLayout(gridbag);
            // // System.out.println("source = " +  source);
            
            parser = new Parser();
            parser.setOwnerPanel(this);
            //      setupRenderingPanel(true);
            setupviewPositionModel();
            //      setupListenersForOffScreen();
            parser.setFilename(filename);
            
        } else
        {
            setLayout(gridbag);
            
            parser = new Parser();
            parser.setOwnerPanel(this);
            //        contentPane.setLayout(gridbag);
            //      setupRenderingPanel(false);
            //      setupRenderingPanel(false, 1);
            setupControls();
            setupviewPositionModel();
            setupToolbarAndMenu();
            //      setupListeners();
            
            if(source.equals("File"))
            {
                parser.setFilename(filename);
            } else if(source.equals("String"))
            {
                parser.setMetaFile(filename);
            } else if(source.equals("StringFromFile"))
            {
                parser.setMetaFile(loadFile(filename));
            } else if(source.equals("stdin"))
            {
                parser.setMetaFile(loadFileFromStdin());
            } else
            {
            }
        }
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String args[])
    {
        
        // java.util.Properties properties = System.getProperties();
        // properties.list(System.out);
        // System.out.println(System.getProperties());
        // System.out.println("Running grout panel");
        
                        /*
                try
                {
                        loadSystemLibrarys();
                } catch(Exception e)
                {
                        e.printStackTrace();
                }
                         */
        // // System.out.println("args.length = " +  args.length);
        boolean offScreen = false;
        String format = null;
        String source = null;
        String filename = null;
        JFrame window = new JFrame("Grout Window");
        window.getContentPane().setLayout(new BorderLayout());
        GroutPanel panel = null;
        
        for(int arg = 0; arg < args.length; ++arg)
        {
            if(args[arg].startsWith("-OffScreen=") && (args[arg].length() > 11))
            {
                offScreen = true;
                format = args[arg].substring(11);
            } else if(args[arg].startsWith("-source") && 
											((args[arg].length() > 8) || !offScreen))
            {
                if(args[arg].length() > 8)
                {
                    source = args[arg].substring(8);
                } else
                {
                    source = JOptionPane.showInputDialog(window, "Enter source");
                }
            } else if(args[arg].startsWith("-file") && 
											((args[arg].length() > 6) || !offScreen))
            {
                if(args[arg].length() > 6)
                {
                    filename = args[arg].substring(6);
                } else
                {
                    filename = GroutFileChooser.showInputDialog
												(window, "Enter file name", 
												 "/people/hmorgan2/code/samples/xml/newXml/dotmatcherOutput.x3d");
                }
            } else
            {
                System.err.println("unrecognised option " + args[arg] + 
																	 System.getProperty("line.separator") + 
																	 "Use = java GroutPanel filename [use][-format]");
            }
        }
        
        //    System.err.println("source = " + source);
        
        if(source == null)
        {
            source = "File";
        }
        
        if((filename == null) && !(source.equals("stdin")))
        {
            if(offScreen)
            {
                System.err.println("No File specified");
                System.exit(0);
            } else
            {
                filename = GroutFileChooser.showInputDialog
										(window, "Enter file name", "~/code/samples/ajb2.wrl");
            }
        }
        
        if(source == null)
        {
            panel = new GroutPanel();
            // panel.loadProperties();
            //			panel.setOwnerWindow(window);
            // // System.out.println("filename = " + filename);
            panel.setFilename(filename);
            window.setJMenuBar(panel.getMenuBar());
            window.getContentPane().add(panel.getToolBar(), BorderLayout.NORTH);
            window.getContentPane().add(panel, BorderLayout.CENTER);
            
            window.addWindowListener(new java.awt.event.WindowAdapter()
            {
                public void windowClosing(java.awt.event.WindowEvent evt)
                {
                    System.exit(0);
                }
            });
            window.pack();
            window.setVisible(true);
        } else
        {
            panel = new GroutPanel(filename, source, offScreen);
            // panel.loadProperties();
            panel.setOwnerWindow(window);
            window.setJMenuBar(panel.getMenuBar());
            window.getContentPane().add(panel.getToolBar(), BorderLayout.NORTH);
            window.getContentPane().add(panel, BorderLayout.CENTER);
            
            window.addWindowListener(new java.awt.event.WindowAdapter()
            {
                public void windowClosing(java.awt.event.WindowEvent evt)
                {
                    System.exit(0);
                }
            });
            window.pack();
            if(offScreen)
            {
                panel.saveImage(format);
                
                System.exit(0);
            } else
            {
                window.setVisible(true);
            }
        }
    }
    
    /** Returns the image of the panel
     * @return the image
     */    
    public BufferedImage getImage()
    {
        BufferedImage[] allImages;
        int[] xCoords;
        int[] yCoords;
        
        int numberOfGraphicsDisplayed = 0;
        Vector displayedGraphics = new Vector();
        
        for (int i = 0; i < graphics.size(); ++i)
        {
            // System.out.println("doneing number = " + i);
            OneGraphic graphic = (OneGraphic) graphics.elementAt(i);
            if(this.isAncestorOf(graphic))
            {
                // System.out.println("this is displayed = " + graphic);
                ++numberOfGraphicsDisplayed;
                displayedGraphics.addElement(graphic);
            }
        }
        
        if(useOneXAxis.getState())
        {
            allImages = new BufferedImage[numberOfGraphicsDisplayed + 1];
            xCoords = new int[numberOfGraphicsDisplayed + 1];
            yCoords = new int[numberOfGraphicsDisplayed + 1];
            
            Point xAxisPos = xAxis.getLocation();
            BufferedImage wholeImage = 
								ImageWriter.createBufferedImageFromComponent(this);
            BufferedImage bottomPanel = wholeImage.getSubimage
								(0, xAxisPos.y, wholeImage.getWidth(),
								 wholeImage.getHeight() - xAxisPos.y);
            
            allImages[numberOfGraphicsDisplayed] = bottomPanel;
            xCoords[numberOfGraphicsDisplayed] = 0;
            yCoords[numberOfGraphicsDisplayed] = numberOfGraphicsDisplayed;
        }
        else
        {
            allImages = new BufferedImage[numberOfGraphicsDisplayed];
            xCoords = new int[numberOfGraphicsDisplayed];
            yCoords = new int[numberOfGraphicsDisplayed];
        }
        
        for (int i = 0; i < numberOfGraphicsDisplayed; ++i)
        {
            OneGraphic graphic = (OneGraphic) displayedGraphics.elementAt(i);
            allImages[i] = graphic.getImage(!useOneXAxis.getState());
            xCoords[i] = 0;
            yCoords[i] = i;
        }
        
        BufferedImage wholeImage = ImageWriter.joinImages(allImages, 
																													xCoords, yCoords);
        
        if(useOneXAxis.getState())
        {
            Rectangle centreBounds = ((OneGraphic) displayedGraphics
            .elementAt(0)).getControls()
            .getCentrePanel().getBounds();
            Rectangle axisMarksBounds = xAxisMarks.getBounds();
            Rectangle lastGraphicBounds = ((OneGraphic) displayedGraphics
            .lastElement()).getBounds();
            Rectangle lockButtonBounds =
            controls.getLockButtonPanel().getBounds();

            int x = centreBounds.x + centreBounds.width;
            int y = 0;
            int w = wholeImage.getWidth() - centreBounds.width;
            // int h = lastGraphicBounds.y + lastGraphicBounds.height
            // 		+ axisMarksBounds.height;
            int h = wholeImage.getHeight() - lockButtonBounds.height;
            // why does it cut the bottom off the image when you 
						// have merged graphics?
            wholeImage = wholeImage.getSubimage(x, y, w, h);
        }
        return wholeImage;
    }
    
    
    /** Writes the imamge of this panel to a file.  It uses a dialog to allow you to
     * select the file name.
     * @param format the format to save the file to
     */    
    public void saveImage(String format)
    {
        BufferedImage wholeImage = getImage();
        File file = null;
        if(!format.equals("print"))
        {
            file = GroutFileChooser.showInputDialogFile
            (this, "Save as", "image");
        }
        ImageWriter.writeImageToFile(wholeImage, file, format);
        
    }
    
    private void doPrintPreview()
    {
        BufferedImage image = getImage();
        ImageIcon imageIcon = new ImageIcon(image);
        JLabel imageLabel = new JLabel(imageIcon);
        final JFrame previewWindow = new JFrame("Print Preview");
        JMenuBar menuBar = new JMenuBar();
        
        JMenu menu = new JMenu("File");
        
        JMenuItem menuItem;
        
        JMenu submenu = new JMenu("Print");
        submenu.setMnemonic(KeyEvent.VK_P);
        
        if(ImageWriter.isJava14orMore() || ImageWriter.hasJAI())
        {
            JMenu submenu2 = new JMenu("File");
            submenu.setMnemonic(KeyEvent.VK_F);
            
            menuItem = new JMenuItem("Jpeg Image");
            menuItem.setAccelerator(KeyStroke.getKeyStroke
																		(KeyEvent.VK_S, ActionEvent.ALT_MASK));
            menuItem.getAccessibleContext().setAccessibleDescription("jpeg");
            menuItem.addActionListener(new PrintPreviewMenuListener(image));
            submenu2.add(menuItem);
            
            menuItem = new JMenuItem("Png Image");
            menuItem.getAccessibleContext().setAccessibleDescription("png");
            menuItem.addActionListener(new PrintPreviewMenuListener(image));
            submenu2.add(menuItem);
            
            if(ImageWriter.hasJAI())
            {
                menuItem = new JMenuItem("Bmp Image");
                menuItem.getAccessibleContext().setAccessibleDescription("bmp");
                menuItem.addActionListener(new PrintPreviewMenuListener(image));
                submenu2.add(menuItem);
                
                menuItem = new JMenuItem("Tiff Image");
                menuItem.getAccessibleContext().setAccessibleDescription("tiff");
                menuItem.addActionListener(new PrintPreviewMenuListener(image));
                submenu2.add(menuItem);
                
                menuItem = new JMenuItem("Pnm Image");
                menuItem.getAccessibleContext().setAccessibleDescription("pnm");
                menuItem.addActionListener(new PrintPreviewMenuListener(image));
                submenu2.add(menuItem);
            }
            
            submenu.add(submenu2);
        }
        
        menuItem = new JMenuItem("Printer or Postscript");
        menuItem.getAccessibleContext().setAccessibleDescription("print");
        menuItem.addActionListener(new PrintPreviewMenuListener(image));
        submenu.add(menuItem);
        
        menu.add(submenu);
        
        menuItem = new JMenuItem("Close");
        menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_E, 
																											 ActionEvent.CTRL_MASK));
        menuItem.getAccessibleContext().setAccessibleDescription
        ("Close Window");
        menuItem.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                previewWindow.dispose();
            }
        }
        );
        menu.add(menuItem);
        
        menuBar.add(menu);
        
        previewWindow.setJMenuBar(menuBar);
        
        previewWindow.addWindowListener(new java.awt.event.WindowAdapter()
        {
            public void windowClosing(java.awt.event.WindowEvent evt)
            {
                evt.getWindow().dispose();
            }
        });
        previewWindow.getContentPane().add(imageLabel);
        previewWindow.pack();
        previewWindow.show();
    }
    
    /** Loads the properties files. */    
    private void loadProperties()
    {
        // Also, I try and explicetly load the properties file
        
        
        org.web3d.vrml.renderer.DefaultLocator nf = new org.web3d.vrml.renderer.DefaultLocator();
        ClassLoader nfcl = nf.getClass().getClassLoader();
        java.io.InputStream is = (InputStream) nfcl.getResourceAsStream("resources/xj3d.properties");
        
        try
        {
            int i = 0;
            for(; is.available() > i; ++i)
            {
                // Just loops through and reads the file
            }
        } catch(java.io.IOException e)
        { e.printStackTrace(); }
        
        // System.out.println("done xj3d.properties");
        
        is = (InputStream) nfcl.getResourceAsStream
						("resources/immersive.properties");
        try
        {
            int i = 0;
            for(; is.available() > i; ++i)
            {
                // System.out.print(((char) is.read()));
                // Just loops through and reads the file
            }
        } catch(java.io.IOException e)
        { e.printStackTrace(); }
        // System.out.println("done immersive.properties");
        
        is = (InputStream) nfcl.getResourceAsStream
						("resources/interactive.properties");
        try
        {
            int i = 0;
            for(; is.available() > i; ++i)
            {
                // System.out.print(((char) is.read()));
                // Just loops through and reads the file
            }
        } catch(java.io.IOException e)
        { e.printStackTrace(); }
    }
    
    /** Loads the system librarys.  Needed for Java Web Start only
     * @throws Exception
     */    
    private static void loadSystemLibrarys() throws Exception
    {
        //
        // Web Start needs explicit native lib loading!
        //
        
        // System.getProperties().list(System.out);
        
        if(System.getProperty("jnlpx.home") != null)
        {
            // System.out.println("loadSystemLibrarys");
            String os = System.getProperty( "os.name" );
            // System.out.println("loading " + os + " native libraries ..");
            if ( os.startsWith( "Windows" ) )
            {
                // order matters here!
                // load those libs that are required by other libs first!
                // System.out.print( "j3daudio.dll .. " );
                // drop ".dll" suffix here
                System.loadLibrary( "j3daudio" );
                // // System.out.println( "OK" );
                // System.out.print( "J3D.dll .. " );
                System.loadLibrary( "J3D" );
                // // System.out.println( "OK" );
                // System.out.print( "libJ3DUtils.dll .. " );
                System.loadLibrary( "J3DUtils" );
                // // System.out.println( "OK" );
            } else if ( os.equals( "Linux" ) )
            {
                // System.out.print( "libj3daudio.so .. " );
                // drop "lib" prefix and ".so" suffix
                System.loadLibrary( "j3daudio" );
                // // System.out.println( "OK" );
                // System.out.print( "libJ3D.so .. " );
                System.loadLibrary( "J3D" );
                // // System.out.println( "OK" );
                // System.out.print( "libJ3DUtils.so .. " );
                System.loadLibrary( "J3DUtils" );
                // // System.out.println( "OK" );
            } else if ( os.equals( "SunOS" ) )
            {
                System.out.print( "libj3daudio.so .. " );
                // drop "lib" prefix and ".so" suffix
                System.loadLibrary( "j3daudio" );
                // // System.out.println( "OK" );
                // System.out.print( "libJ3D.so .. " );
                System.loadLibrary( "J3D" );
                // // System.out.println( "OK" );
                // System.out.print( "libJ3DUtils.so .. " );
                System.loadLibrary( "J3DUtils" );
                // // System.out.println( "OK" );
            } else
            {
                throw new Exception( "OS '" + os + "' not yet supported." );
            }
        }
    }
    
    /** not used */    
    private void setPicking()
    {
        if(pickFactory == null)
        {
            pickFactory = new ZoomToOccuranceFactory(this);
        }
        
    }
    
    /** Sets all the listeners for the various components */    
    private void setupListeners()
    {
        for (Enumeration e = graphics.elements() ; e.hasMoreElements() ;)
        {
            OneGraphic graphic = (OneGraphic) e.nextElement();
            graphic.setParserListeners(parser);
            graphic.setGroutPanelListeners(this);
            addGroutPanelListener(graphic);
            graphic.attachPickBehavior(pickFactory);
            graphic.addOneGraphicListener(this);
            //							graphic.setControlListeners(controls);
        }
        parser.addParserListener(viewPositionModel);
        viewPositionModel.addViewPositionModelListener(xAxisMarks);
        viewPositionModel.addViewPositionModelListener(controls);
        controls.addControlContainerListener(viewPositionModel);
        addGroutPanelListener(controls);
        addComponentListener(this);
        
    }
    
    /** Sets all the listeners for the various components for off screen */    
    private void setupListenersForOffScreen()
    {
                /*
    parser.addParserListener(canvas);
    parser.addParserListener(viewPositionModel);
    viewPositionModel.addViewPositionModelListener(xAxisMarks);
    viewPositionModel.addViewPositionModelListener(yAxisMarks);
    viewPositionModel.addViewPositionModelListener(xSequence);
    viewPositionModel.addViewPositionModelListener(ySequence);
    addGroutPanelListener(viewPositionModel);
    viewPositionModel.addViewPositionModelListener(canvas);
                 */
    }
    
    /** */    
    private void setupviewPositionModel()
    {
                /*
    viewPositionModel = new ViewPositionModel();
                 */
    }
    
    /** adds a graphic
     * @param tr the graphic
     */    
    public void addThisGraphic(TransferableGraphic tr)
    {
        c.gridx = 0;
        c.gridy = tr.getYPos() + 1;
        //							c.weightx = 1.0;
        //							c.weightx = GridBagConstraints.REMAINDER;
        c.weightx = 1;
        c.weighty = 1.0;
        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        gridbag.setConstraints(tr.getOriginalGraphic(), c);
        add(tr.getOriginalGraphic());
        
        // System.out.println("tr.getYPos() + 1 = " + (tr.getYPos() + 1));
        
        c.weightx = 0.0;
        c.weighty = 0.0;
        revalidate();
    }
    
    /** lays out this panel
     * @param offScreen true if offScreen
     */    
    private void setupRenderingPanel(boolean offScreen)
    {
        PositionLabel positionLabel = new PositionLabel();
        for(int i = 0; i<parser.getNumberOfGraphics(); ++i)
        {
            OneGraphic graphic = new OneGraphic((GroutPanel) (this));
            // System.out.println("new OneGraphic = " + graphic);
            // System.out.println("this = " + this);
            // OneGraphic graphic = new OneGraphic();
            graphic.setGraphicNumber(i);
            graphics.addElement(graphic);
            
            graphic.setLocationListener(positionLabel);
            
            c.gridx = 0;
            c.gridy = 1 + i;
            c.weightx = 1;
            c.weighty = 1.0;
            c.gridwidth = GridBagConstraints.REMAINDER;
            c.fill = GridBagConstraints.BOTH;
            gridbag.setConstraints(graphic, c);
            add(graphic);
            // System.out.println("adding = " + graphic + " at " + c.gridy);
            c.weightx = 0.0;
            c.weighty = 0.0;
        }
        
        
        c.gridx = 0;
        c.gridy = graphics.size() + 3;
        c.gridheight = 1;
        c.fill = GridBagConstraints.BOTH;
        c.insets.right = 1;
        c.insets.left = 1;
        c.insets.top = 1;
        c.insets.bottom = 1;
        // c.anchor = GridBagConstraints.WEST;
        gridbag.setConstraints(positionLabel, c);
        add(positionLabel);
        c.insets.right = 0;
        c.insets.left = 0;
        c.insets.top = 0;
        c.insets.bottom = 0;
        c.gridheight = 1;
        
    }
    
    /** not used
     * @param offScreen true if offScreen
     * @param panelNumber
     */    
    private void setupRenderingPanel(boolean offScreen, int panelNumber)
    {
    }
    
    /** Sets up the controls */    
    private void setupControls()
    {
        
        controls = new ControlContainer();
        xAxisMarks = new AxisMarks();
        // xSequence = new SequenceView();
        viewPositionModel = new ViewPositionModel();
        
        xAxis = new JPanel(new BorderLayout());
        xAxis.add(controls.getBottomPanel(), BorderLayout.SOUTH);
        xAxis.add(xAxisMarks, BorderLayout.NORTH);
    }
    private void setupToolbarAndMenu()
    {
        //        JPanel menuAndTools = new JPanel();
        // we do not like the toolbar
        toolBar = new JToolBar();
        // toolBar.setToolTipText("Try me!");
        
        addMenuBar();
    }
    
    private void addMenuBar()
    {
        JPopupMenu.setDefaultLightWeightPopupEnabled(false);
        
        JMenu menu, submenu, submenu2, submenu3, submenu4;
        JMenuItem menuItem;
        JCheckBoxMenuItem cbMenuItem;
        JRadioButtonMenuItem rbMenuItem;
        
        //Create the menu bar.
        menuBar = new JMenuBar();
        //        setJMenuBar(menuBar);
        
        //Build the first menu.
        menu = new JMenu("File");
        menu.setMnemonic(KeyEvent.VK_A);
        menu.getAccessibleContext().setAccessibleDescription(
        "File menu");
        menuBar.add(menu);
        
        menuItem = new JMenuItem("Open");
        menuItem.setMnemonic(KeyEvent.VK_O);
        menuItem.getAccessibleContext().setAccessibleDescription("Open");
        menuItem.addActionListener(new MenuListener());
        menu.add(menuItem);
        
        menuItem = new JMenuItem("Save");
        menuItem.setMnemonic(KeyEvent.VK_S);
        menuItem.getAccessibleContext().setAccessibleDescription("Save");
        menuItem.addActionListener(new MenuListener());
        menu.add(menuItem);
        
        submenu = new JMenu("Print");
        submenu.setMnemonic(KeyEvent.VK_P);
        
        if(ImageWriter.isJava14orMore() || ImageWriter.hasJAI())
        {
            submenu2 = new JMenu("File");
            submenu.setMnemonic(KeyEvent.VK_F);
            
            menuItem = new JMenuItem("Jpeg Image");
            menuItem.setAccelerator
								(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.ALT_MASK));
            menuItem.getAccessibleContext().setAccessibleDescription("jpeg");
            menuItem.addActionListener(new MenuListener());
            submenu2.add(menuItem);
            
            menuItem = new JMenuItem("Png Image");
            menuItem.getAccessibleContext().setAccessibleDescription("png");
            menuItem.addActionListener(new MenuListener());
            submenu2.add(menuItem);
            
            if(ImageWriter.hasJAI())
            {
                menuItem = new JMenuItem("Bmp Image");
                menuItem.getAccessibleContext().setAccessibleDescription("bmp");
                menuItem.addActionListener(new MenuListener());
                submenu2.add(menuItem);
                
                menuItem = new JMenuItem("Tiff Image");
                menuItem.getAccessibleContext().setAccessibleDescription("tiff");
                menuItem.addActionListener(new MenuListener());
                submenu2.add(menuItem);
                
                menuItem = new JMenuItem("Pnm Image");
                menuItem.getAccessibleContext().setAccessibleDescription("pnm");
                menuItem.addActionListener(new MenuListener());
                submenu2.add(menuItem);
            }
            
            submenu.add(submenu2);
        }
        menuItem = new JMenuItem("Printer or Postscript");
        menuItem.getAccessibleContext().setAccessibleDescription("print");
        menuItem.addActionListener(new MenuListener());
        submenu.add(menuItem);
        
        menu.add(submenu);
        
        menuItem = new JMenuItem("Print Preview");
        menuItem.getAccessibleContext().setAccessibleDescription("PrintPreview");
        menuItem.addActionListener(new MenuListener());
        menu.add(menuItem);
        
        addDisposeOfGroutPanelMenuItem(menu);
        
        menu = new JMenu("View");
        menu.setMnemonic(KeyEvent.VK_A);
        menu.getAccessibleContext().setAccessibleDescription(
        "This controls the View");
        menuBar.add(menu);
        
        //a group of JMenuItems
        // This just does the same as the Reset button, so remove it
        // No, lets just not add it to a menu,
        JMenuItem ResetViewMenuItem = new JMenuItem("Reset View", KeyEvent.VK_R);
        ResetViewMenuItem.setAccelerator(KeyStroke.getKeyStroke
        (KeyEvent.VK_1, ActionEvent.ALT_MASK));
        ResetViewMenuItem.getAccessibleContext().setAccessibleDescription
        ("This Resets the View");
        ResetViewMenuItem.addActionListener(new MenuListener());
        menu.add(ResetViewMenuItem);
        
        
        menuItem = new JMenuItem("Fit View to Window");
        menuItem.getAccessibleContext().setAccessibleDescription("RemoveBlankBoxes");
        menuItem.addActionListener(new MenuListener());
        menuItem.setMnemonic(KeyEvent.VK_R);
        menu.add(menuItem);
        
        
        ButtonGroup group;
        
        // this is no longer required
                /*
    submenu = new JMenu("Set View Policy");
    submenu.setMnemonic(KeyEvent.VK_V);
                 
    group = new ButtonGroup();
                //    group = new ButtonGroup();
    rbMenuItem = new JRadioButtonMenuItem("RELATIVE_TO_SCREEN");
    rbMenuItem.getAccessibleContext().setAccessibleDescription("RELATIVE_TO_SCREEN");
    rbMenuItem.setSelected(false);
    rbMenuItem.setMnemonic(KeyEvent.VK_S);
                rbMenuItem.addActionListener(new MenuListener());
    group.add(rbMenuItem);
    submenu.add(rbMenuItem);
                 
    rbMenuItem = new JRadioButtonMenuItem("RELATIVE_TO_WINDOW");
    rbMenuItem.getAccessibleContext().setAccessibleDescription("RELATIVE_TO_WINDOW");
    rbMenuItem.setSelected(true);
    rbMenuItem.setMnemonic(KeyEvent.VK_W);
                rbMenuItem.addActionListener(new MenuListener());
    group.add(rbMenuItem);
    group.add(rbMenuItem);
    submenu.add(rbMenuItem);
                 
    rbMenuItem = new JRadioButtonMenuItem("RELATIVE_TO_FIELD_OF_VIEW");
    rbMenuItem.getAccessibleContext().setAccessibleDescription("RELATIVE_TO_FIELD_OF_VIEW");
    rbMenuItem.setSelected(false);
    rbMenuItem.setMnemonic(KeyEvent.VK_V);
                rbMenuItem.addActionListener(new MenuListener());
    group.add(rbMenuItem);
    group.add(rbMenuItem);
    submenu.add(rbMenuItem);
    menu.add(submenu);
                 */
        
                /*
    submenu = new JMenu("Set Pick Policy");
    submenu.setMnemonic(KeyEvent.VK_V);
                 
    group = new ButtonGroup();
    rbMenuItem = new JRadioButtonMenuItem("Zoom To");
    rbMenuItem.getAccessibleContext().setAccessibleDescription("SetPickPolicyZoomTo");
    rbMenuItem.setSelected(false);
    rbMenuItem.setMnemonic(KeyEvent.VK_V);
                rbMenuItem.addActionListener(new MenuListener());
    group.add(rbMenuItem);
    submenu.add(rbMenuItem);
                 
                // group = new ButtonGroup();
    rbMenuItem = new JRadioButtonMenuItem("Show Sequence");
    rbMenuItem.getAccessibleContext().setAccessibleDescription("SetPickPolicyShowSequence");
    rbMenuItem.setSelected(false);
    rbMenuItem.setMnemonic(KeyEvent.VK_S);
                rbMenuItem.addActionListener(new MenuListener());
    group.add(rbMenuItem);
    submenu.add(rbMenuItem);
                 
    rbMenuItem = new JRadioButtonMenuItem("Merge Graphics");
    rbMenuItem.getAccessibleContext().setAccessibleDescription("SetPickPolicyMergeGraphics");
    rbMenuItem.setSelected(false);
    rbMenuItem.setMnemonic(KeyEvent.VK_M);
                rbMenuItem.addActionListener(new MenuListener());
    group.add(rbMenuItem);
    submenu.add(rbMenuItem);
                 
                submenu2 = new JMenu("Change Colour");
                 
    rbMenuItem = new JRadioButtonMenuItem("White");
    rbMenuItem.getAccessibleContext().setAccessibleDescription("SetPickPolicyChangeColourWhite");
    rbMenuItem.setSelected(false);
                rbMenuItem.addActionListener(new MenuListener());
    group.add(rbMenuItem);
    submenu2.add(rbMenuItem);
                 
    rbMenuItem = new JRadioButtonMenuItem("Black");
    rbMenuItem.getAccessibleContext().setAccessibleDescription("SetPickPolicyChangeColourBlack");
    rbMenuItem.setSelected(false);
                rbMenuItem.addActionListener(new MenuListener());
    group.add(rbMenuItem);
    submenu2.add(rbMenuItem);
                 
    rbMenuItem = new JRadioButtonMenuItem("Blue");
    rbMenuItem.getAccessibleContext().setAccessibleDescription("SetPickPolicyChangeColourBlue");
    rbMenuItem.setSelected(false);
                rbMenuItem.addActionListener(new MenuListener());
    group.add(rbMenuItem);
    submenu2.add(rbMenuItem);
                 
    rbMenuItem = new JRadioButtonMenuItem("Pink");
    rbMenuItem.getAccessibleContext().setAccessibleDescription("SetPickPolicyChangeColourPink");
    rbMenuItem.setSelected(false);
                rbMenuItem.addActionListener(new MenuListener());
    group.add(rbMenuItem);
    submenu2.add(rbMenuItem);
                 
    submenu.add(submenu2);
                 */
                /*
    rbMenuItem = new JRadioButtonMenuItem("Change Colour");
    rbMenuItem.getAccessibleContext().setAccessibleDescription("SetPickPolicyChangeColour");
    rbMenuItem.setSelected(false);
    rbMenuItem.setMnemonic(KeyEvent.VK_V);
                rbMenuItem.addActionListener(new MenuListener());
    group.add(rbMenuItem);
    submenu.add(rbMenuItem);
                 */
        
                /*
    menu.add(submenu);
                 */
        
        submenu = new JMenu("Set Background Colour");
        submenu.setMnemonic(KeyEvent.VK_V);
        
        group = new ButtonGroup();
        rbMenuItem = new JRadioButtonMenuItem("White");
        rbMenuItem.setSelected(true);
        rbMenuItem.setMnemonic(KeyEvent.VK_W);
        rbMenuItem.getAccessibleContext().setAccessibleDescription("ColourWhite");
        rbMenuItem.addActionListener(new MenuListener());
        group.add(rbMenuItem);
        submenu.add(rbMenuItem);
        
        rbMenuItem = new JRadioButtonMenuItem("Black");
        rbMenuItem.setSelected(false);
        rbMenuItem.setMnemonic(KeyEvent.VK_B);
        rbMenuItem.getAccessibleContext().setAccessibleDescription("ColourBlack");
        rbMenuItem.addActionListener(new MenuListener());
        group.add(rbMenuItem);
        submenu.add(rbMenuItem);
        
        rbMenuItem = new JRadioButtonMenuItem("Pink");
        rbMenuItem.setSelected(false);
        rbMenuItem.setMnemonic(KeyEvent.VK_P);
        rbMenuItem.getAccessibleContext().setAccessibleDescription("ColourPink");
        rbMenuItem.addActionListener(new MenuListener());
        group.add(rbMenuItem);
        submenu.add(rbMenuItem);
        
        menuItemUndo = new JMenuItem("Undo");
        menuItemUndo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_U, ActionEvent.CTRL_MASK));
        menuItemUndo.setEnabled(false);
        menuItemUndo.getAccessibleContext().setAccessibleDescription("Undo");
        menuItemUndo.addActionListener(new MenuListener());
        menu.add(menuItemUndo);
        
        menuItemRedo = new JMenuItem("Redo");
        menuItemRedo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_R, ActionEvent.CTRL_MASK));
        menuItemRedo.setEnabled(false);
        menuItemRedo.getAccessibleContext().setAccessibleDescription("Redo");
        menuItemRedo.addActionListener(new MenuListener());
        menu.add(menuItemRedo);
        
        menuBar.add(menu);
    }
    
    /** this is here so the user of this class can override it
     * @param menu the menu to aadd the item to
     */    
    protected void addDisposeOfGroutPanelMenuItem(JMenu menu)
    {
        JMenuItem menuItem = new JMenuItem("Exit");
        menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_E, ActionEvent.CTRL_MASK));
        menuItem.setMnemonic(KeyEvent.VK_E);
        menuItem.getAccessibleContext().setAccessibleDescription("Exit");
        menuItem.addActionListener(new MenuListener());
        menu.add(menuItem);
    }
    
    private void attachPickBehavior(final Canvas3D canvas3D)
    {
    }
    
    public void paint(Graphics g)
    {
        super.paint(g);
        if(useOneXAxis.getState())
        {
            
            int axisMarksHeight = xAxisMarks.getSize().height;
            JComponent lastGraphic = null;
            for(int i = (graphics.size() - 1);
            ((i >= 0) && (lastGraphic == null)) ; --i)
            {
                if(isAncestorOf((Component) graphics.elementAt(i)))
                {
                    lastGraphic	= ((JComponent) graphics.elementAt(i));
                }
            }
            int xStartPos = lastGraphic.getSize().width - RIGHT_INDENT;
            int yStartPos = lastGraphic.getSize().height + lastGraphic.getY();
            
            g.setColor(Color.white);
            g.fillRect(xStartPos, yStartPos, RIGHT_INDENT,
            axisMarksHeight);
        }
    }
    
    
    /** Sets either a shared x axis or indavidul ones for each graphic
     * @param one True if shared x axis
     */    
    public void alterOneOrManyXAxis(boolean one)
    {
        if(one)
        {
            // settup gridbag;
            // add x Axis stuff;
            notifyTargets("xAxisChange", new Boolean(!one), new Boolean(one));
            int rendWindSize = ((OneGraphic) graphics.lastElement())
								.getRendWindContainerSize();
            if(rendWindSize != 0)
            {
                xAxis.setMinimumSize(new Dimension(rendWindSize, 70));
            }
            else
            {
                xAxis.setMinimumSize(new Dimension(925, 70));
            }
            c.gridx = 1;
            c.gridy = graphics.size() + 1;
            c.gridheight = 2;
            c.fill = GridBagConstraints.NONE;
            c.anchor = GridBagConstraints.NORTHEAST;
            c.insets.right = RIGHT_INDENT;
            gridbag.setConstraints(xAxis, c);
            add(xAxis);
            c.insets.right = 0;
            c.gridheight = 1;
            
            
            c.gridwidth = 1;
            c.gridx = 0;
            // c.gridy = graphics.size() + 1;
            c.gridy = graphics.size() + 2;
            c.fill = GridBagConstraints.HORIZONTAL;
            // c.anchor = GridBagConstraints.WEST;
            gridbag.setConstraints(controls.getLockButtonPanel(), c);
            add(controls.getLockButtonPanel());
            
            // why do you need the text here? hugh
            if(usePositionLabel)
            {
                paddingBox = new PositionLabel();
                for (int i = 0; i < graphics.size(); ++i)
                {
                    ((OneGraphic) graphics.elementAt(i))
                    .setLocationListener(((PositionLabel) paddingBox));
                }
            }
            else if(true) // change thisa to use a JPanel
            {
                paddingBox = (new JLabel("test")
                {
                    public void paint(Graphics g)
                    {
                        // super.paint(g);
                        int axisMarksHeight = xAxisMarks.getSize().height;
                        int yControlsWidth = 0;
                        for(int i = (graphics.size() - 1);
                        ((i >= 0) && (yControlsWidth == 0)) ; --i)
                        {
                            int thisYControlsWidth =
                            ((OneGraphic)graphics.elementAt(i))
                            .getControls().getCentrePanel().getSize()
                            .width;
                            
                            if(thisYControlsWidth != 0)
                            {
                                yControlsWidth = thisYControlsWidth;
                            }
                        }
                        int lockButtonsWidth =
                        controls.getLockButtonPanel().getSize().width;
                        // int windowWidth = getSize().width;
                        
                        g.setColor(Color.white);
                        g.fillRect(yControlsWidth, 0, lockButtonsWidth,
                        axisMarksHeight);
                    }
                }
                
                );
                paddingBox.setBackground(Color.white);
            }
            else
            {
                paddingBox = new JPanel();
            }
            // RIGHT_INDENT = paddingBox.getSize().width;
            
            c.gridwidth = 0;
            c.gridx = 0;
            c.gridy = graphics.size() + 1;
            c.fill = GridBagConstraints.BOTH;
            c.anchor = GridBagConstraints.WEST;
            gridbag.setConstraints(paddingBox, c);
            add(paddingBox);
            
            validate();
            
        } else
        {
            remove(xAxis);
            remove(controls.getLockButtonPanel());
            notifyTargets("xAxisChange", new Boolean(!one), new Boolean(one));
            validate();
        }
    }
    
    /** This is setting up the window that cannot be done until the window is initialy
     * displayed
     */    
    private void stuffToDoWhenWindowIsDisplayed()
    {
        if(useOneXAxis.getState())
        {
            //					alterOneOrManyXAxis(true);
        }
        notifyTargets("FitViewToWindow", (Object) new Boolean(true),
        (Object) new Boolean(false));
    }
    
    public ControlContainer getControls()
    {
        return controls;
    }
    
    public void controlContainerChanged(ControlContainerEvent e)
    {
    }
    
    public void parserChanged(ParserEvent e)
    {
    }
    
    private String loadFileFromStdin()
    {
        StringBuffer inputReader = new StringBuffer();
        try
        {
            BufferedReader br = new BufferedReader
								(new InputStreamReader(System.in));
            String text;
            while ((text = br.readLine()) != null)
            {
                inputReader = inputReader.append(text).append("\n");
            }
            // Always close a file after opening.
            br.close();
        } catch(java.io.IOException e)
        {
            e.printStackTrace();
        }
        return inputReader.toString();
    }
    
    private String loadFile(String filename)
    {
        StringBuffer inputReader = new StringBuffer();
        try
        {
            BufferedReader br = new BufferedReader(new FileReader(filename));
            String text;
            while ((text = br.readLine()) != null)
            {
                inputReader = inputReader.append(text).append("\n");
            }
            // Always close a file after opening.
            br.close();
        } catch(java.io.IOException e)
        {
            e.printStackTrace();
        }
        return inputReader.toString();
    }
    
    /** This is run when the graphics have been split */    
    public void graphicsSplit()
    {
        JMenu eachGraphicMenu, subMenu;
        JMenuItem menuItem;
        JCheckBoxMenuItem cbMenuItem;
        JRadioButtonMenuItem rbMenuItem;
        
        setupRenderingPanel(offScreen);
        if(offScreen)
        {
            setupListenersForOffScreen();
        }
        else
        {
            setupListeners();
        }
        
        if(parser.getNumberOfGraphics() == 1)
        {
            useOneXAxis.setEnabled(false);
        }
        else
        {
            useOneXAxis.setEnabled(true);
        }
        
        for(int i =0; i<parser.getNumberOfGraphics(); ++i)
        {
        }
        
        return;
    }
    
    /** Not used */    
    private JPopupMenu getMenuTypeMakeIfNot(JPopupMenu thisGraphicsMenu, 
																						String name)
    {
        MenuElement[] components = thisGraphicsMenu.getSubElements();
        Component item = null;
        Component returnItem = null;
        for(int i = 0; ((i < components.length) && (returnItem == null)); ++i)
        {
            item = (Component) components[i];
            if(item.getName().equals(name))
            {
                returnItem = item;
            }
        }
        if(returnItem == null)
        {
            returnItem = new JPopupMenu("name");
        }
        return (JPopupMenu) returnItem;
    }
    
    /** Not used */    
    private JMenu getMenuTypeMakeIfNot(JMenu thisGraphicsMenu, String name)
    {
        Component[] components = thisGraphicsMenu.getMenuComponents();
        JMenuItem item = null;
        Component returnItem = null;
        for(int i = 0; ((i < components.length) && (returnItem == null)); ++i)
        {
            item = (JMenuItem) components[i];
            // System.out.println("item " + i + " = " + item);
            if(item.getText().equals(name))
            {
                returnItem = item;
            }
        }
        if(returnItem == null)
        {
            returnItem = new JMenu(name);
            thisGraphicsMenu.add(returnItem);
        }
        return (JMenu) returnItem;
    }
    
    /** Not used */    
    private void removeItemNamedThisFromThisMenu(JMenu menu, String name)
    {
        Component[] menus = menu.getMenuComponents();
        for(int j = 0; j < menus.length; ++j)
        {
            // System.out.println("eachGraphicMenu " + j + " = " + menus[j]);
            if(((JMenuItem) menus[j]).getText().equals(name))
            {
                menu.remove(menus[j]);
            }
        }
        
    }
    
    /** Not used */    
    private void removeThisGraphicFromAllTheseMenus(JMenu graphicsMenu, 
																										JMenuItem origin)
    {
        String originName = origin.getText();
        Component[] components = graphicsMenu.getMenuComponents();
        JMenuItem item = null;
        Component returnItem = null;
        for(int i = 0; ((i < components.length) && (returnItem == null)); ++i)
        {
            item = (JMenuItem) components[i];
            // System.out.println("item " + i + " = " + item);
            // System.out.println("item.getText() " + item.getText());
            JMenu mergeWith = getMenuTypeMakeIfNot((JMenu)item, "Merge with");
            removeItemNamedThisFromThisMenu(mergeWith, originName);
        }
        removeItemNamedThisFromThisMenu(graphicsMenu, originName);
    }
    
    /**
     * Refreash the screen, and remove the black boes if argument is true
     * @param doRemove remove any black boxes
     */
    public void removeBlankBoxes(final boolean doRemove)
    {
        final Runnable doRemoving = new Runnable()
        {
            public void run()
            {
                if(doRemove)
                {
                    int limit = graphics.size();
                    for(int i = 0; i < limit; ++i)
                    {
                        OneGraphic thisGraphic =
                        ((OneGraphic) graphics.elementAt(i));
                        if(thisGraphic.isAWhiteBox())
                        {
                            remove(thisGraphic);
                        }
                    }
                }
                revalidate();
            }
        };
        
        Thread appThread = new Thread()
        {
            public void run()
            {
                try
                {
                    SwingUtilities.invokeAndWait(doRemoving);
                    Thread.currentThread().sleep(100);
                    notifyTargets("FitViewToWindow",
                    (Object) new Boolean(true),
                    (Object) new Boolean(false));
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                }
            }
        };
        appThread.start();
    }
    
   
    
    /** Not used (since drag and drop) */    
    public void mergeGraphics(int addTo, int addThis, ActionEvent e)
    {
        // remove j from panel
        // removeGraphic((Component) graphics.elementAt(addThis));
        remove((Component) graphics.elementAt(addThis));
        // remove scenegraph from j
        BranchGroup scene = 
						((OneGraphic) graphics.elementAt(addThis)).detachScene(0);
        
        // make a new RemovedScene
        RemovedScene removedScene = new RemovedScene(scene,
        ((OneGraphic) graphics.elementAt(addThis)),
        addThis, 0);
        
        // add j's scene graph to i
        ((OneGraphic) graphics.elementAt(addTo))
        .addScene(scene, ((OneGraphic) graphics.elementAt(addThis)));
        // validate
        validate();
        
    }
    
    /**
     * This one is for use by the Drag and drop behaviour
     **/
    public void mergeGraphics(int addTo, int addThis)
    {
        // remove j from panel
        remove((Component) graphics.elementAt(addThis));
        // remove scenegraph from j
        BranchGroup scene = 
						((OneGraphic) graphics.elementAt(addThis)).detachScene(0);
        // add j's scene graph to i
        // ((OneGraphic) graphics.elementAt(addTo)).addScene(scene);
        ((OneGraphic) graphics.elementAt(addTo))
        .addScene(scene, ((OneGraphic) graphics.elementAt(addThis)));
        // validate
        validate();
        
    }
    
    public void removeGraphic(Component graphic)
    {
        removedGraphics.addElement(graphic);
        remove(graphic);
        
    }
    
    /** Removes all the data for this x3d file */    
    private void clearAllData()
    {
        for (Enumeration e = graphics.elements() ; e.hasMoreElements() ;)
        {
            OneGraphic graphic = (OneGraphic) e.nextElement();
            remove(graphic);
        }
        if(!graphics.isEmpty())
        {
            graphics.removeAllElements();
        }
        parser = new Parser();
        parser.setOwnerPanel(this);
        targets.removeAllElements();
        
        xSequenceString = null;
        ySequenceString = null;
        
    }
    
    public void setOwnerWindow(Window window)
    {
        ownerWindow = window;
        ownerWindow.addWindowListener(this);
    }
    
    public synchronized void addGroutPanelListener(GroutPanelListener listener)
    {
        targets.addElement(listener);
    }
    
    public synchronized void removeGroutPanelListener(GroutPanelListener listener)
    {
        targets.removeElement(listener);
    }
    
    protected void notifyTargets(String propertyName, Object oldValue, 
																 Object newValue)
    {
        // System.out.println("notifyTargets in GroutPanel = " + propertyName);
        
        Vector l;
        GroutPanelEvent s = new GroutPanelEvent((Object) this, 
																								propertyName, oldValue, 
																								newValue);
        synchronized(this)
        {
            l = (Vector) targets.clone();
        }
        for (int i = 0; i < l.size(); i++)
        {
            GroutPanelListener sl = (GroutPanelListener) l.elementAt(i);
            sl.groutPanelChanged(s);
            // System.out.println("Event in GroutPanel to = " + sl);
        }
    }
    
    public void propertyChange(java.beans.PropertyChangeEvent propertyChangeEvent)
    {
    }
    
    
    /** Sets up the panel accroding to the options from the x3d file */    
    public void setOptions(Metadata groutOptions)
    {
        for(Enumeration e = groutOptions.keys() ; e.hasMoreElements() ;)
        {
            String key = (String) e.nextElement();
            String value = groutOptions.getValue(key);
            if(key.equals("showOneXAxis"))
            {
                alterOneOrManyXAxis(true);
                useOneXAxis.setState(true);
                // System.out.println("UseOneXAxis = " + locked.getState());
                notifyTargets("UseOneXAxis", (Object) new Boolean(false),
                (Object) new Boolean(true));
                // System.out.println("showOneXAxis 1 groutOptions");
            }
            if(key.equals("pickType"))
            {
                if(value.equals("ZoomTo"))
                {
                    // System.out.println("Add PickOccuranceFactory ZoomTo");
                    changePickFactory(new ZoomToOccuranceFactory(this));
                }
                if(value.equals("ShowSequence"))
                {
                    // System.out.println("Add PickOccuranceFactory ZoomTo");
                }
            }
            if(key.endsWith("Sequence"))
            {
                if(key.equals("xSequence"))
                {
                    xSequenceString = value;
                }
                if(key.equals("ySequence"))
                {
                    ySequenceString = value;
                }
            }
        }
        
    }
    
    public PickOccuranceFactory getPickFactory()
    {
        return pickFactory;
    }
    
    public void changePickFactory(PickOccuranceFactory factory)
    {
        // System.out.println("changePickFactory" + factory);
        boolean removeOldOne = (pickFactory != null);
        for (Enumeration e = graphics.elements() ; e.hasMoreElements() ;)
        {
            OneGraphic graphic = (OneGraphic) e.nextElement();
            if(removeOldOne)
            {
                graphic.removePickBehavior(pickFactory);
            }
            graphic.attachPickBehavior(factory);
        }
        pickFactory = factory;
    }
    
    /** undo the last undoable occurance */    
    private void undoOne()
    {
        // System.out.println("undo " + undoableIndex + " " + undoables.size());
        if(undoableIndex >= 0)
        {
            ((UndoableOccurance) undoables.elementAt(undoableIndex)).undo();
            --undoableIndex;
            if(!menuItemRedo.isEnabled())
            {
                menuItemRedo.setEnabled(true);
            }
            if(undoableIndex < 0)
            {
                menuItemUndo.setEnabled(false);
            }
        }
        else
        {
            System.err.println("No undoable events have yet occured");
        }
    }
    
    /** redo the last undone occurance */    
    private void redoOne()
    {
        if(!((undoableIndex == (undoables.size() - 1))
        || (undoables.size() == 0)))
        {
            ((UndoableOccurance) undoables.elementAt(++undoableIndex)).redo();
            if(undoableIndex == (undoables.size() - 1))
            {
                menuItemRedo.setEnabled(false);
            }
            if(!menuItemUndo.isEnabled())
            {
                menuItemUndo.setEnabled(true);
            }
        }
        else
        {
            System.err.println("No redoable events have yet occured");
        }
    }
    
    /** adds a sequence view marker */    
    public Hashtable addSinglePositionMarker(double sequencePosition,
    OneGraphic graphic)
    {
        Hashtable markers = new Hashtable();
        if(useOneXAxis.getState())
        {
            for(int i = 0; i < graphics.size(); ++i)
            {
                markers.put(((OneGraphic) graphics.elementAt(i))
														.addSinglePositionMarker(sequencePosition), 
														((OneGraphic) graphics.elementAt(i)));
            }
        }
        else
        {
            markers.put(graphic.addSinglePositionMarker(sequencePosition),
            graphic);
        }
        return markers;
    }
    
    /** adds a sequence view marker */    
    public Hashtable addYPositionMarker(double sequencePositionY,
    OneGraphic graphic)
    {
        Hashtable markers = new Hashtable();
        markers.put(graphic.addYPositionMarker(sequencePositionY), graphic);
        
        return markers;
    }
    
    /** removes this sequence view marker */    
    public void removeLine(Shape3D line, OneGraphic graphic)
    {
        graphic.removeLine(line);
    }
    
    /** removes this sequence view marker */    
    public void removeLine(Shape3D line)
    {
        ((OneGraphic) graphics.elementAt(0)).removeLine(line);
    }
    
    /** adds a key member refering to a graphic
     * @param colour make it this colour
     * @param title give it this title
     */    
    public void addThisGraphicToKey(String colour, String title)
    {
        float red = 0;
        float green = 0;
        float blue = 0;
        
        char comma = (new String(",")).charAt(0);
        char space = (new String(" ")).charAt(0);
        
        // System.out.print("colour = " + colour);
        colour = colour.replace(comma, space);
        StringTokenizer st = new StringTokenizer(colour);
        red = Float.valueOf(st.nextToken().trim()).floatValue();
        green = Float.valueOf(st.nextToken().trim()).floatValue();
        blue = Float.valueOf(st.nextToken().trim()).floatValue();
        
        Color imageColour = new Color(red, green, blue);
        
        BufferedImage image = new BufferedImage(10, 10,
        BufferedImage.TYPE_INT_RGB);
        Graphics drawHere = image.getGraphics();
        drawHere.setColor(imageColour);
        drawHere.fillRect(0, 0, 10, 10);
        
        ImageIcon picture = new ImageIcon(image);
        if(title.length() > 10)
        {
            title = title.substring(0, 10);
        }
        JLabel newLabel = new JLabel(title, picture, SwingConstants.CENTER);
        toolbarLabels.addElement(newLabel);
        
        toolBar.add(newLabel);
    }
    
                /*
                public void doLayout()
                {
                                super.doLayout();
                 
                                // System.out.println("doLayout");
                }
                 */
    
    /**
     * If this method returns true, <code>revalidate</code> calls by
     * descendants of this component will cause the entire tree
     * beginning with this root to be validated.
     * Returns false by default.  <code>JScrollPane</code> overrides
     * this method and returns true.
     *
     * Hugh - Pinched from JComponent, try returning true
     *
     * @return always returns false
     * @see #revalidate
     * @see java.awt.Component#invalidate
     * @see java.awt.Container#validate
    public boolean isValidateRoot()
    {
        return false;
    }
     */
    
    /** just used to examine the "1st graphic overlaps the second" bug */    
    private void debugMethod()
    {
        int totalHeight = 0;
        for(int i = 0; i<graphics.size(); ++i)
        {
            OneGraphic thisGraphic = ((OneGraphic) graphics.elementAt(i));
            
            java.awt.Dimension graphicSize = thisGraphic.getSize();
            System.out.println("graphic " + i + " = " + graphicSize);
            totalHeight += graphicSize.height;
            System.out.println("graphic " + i + " is at = " + ((OneGraphic) graphics.elementAt(i)).getLocation());
            
            // thisGraphic.setSize(graphicSize);
            // thisGraphic.setMinimumSize(graphicSize);
            // thisGraphic.setMaximumSize(graphicSize);
            
            // this is the one you need
            // thisGraphic.setPreferredSize(thisGraphic.getSize());
        }
        
        
        int i = 0;
        
        revalidate();
        
        for(Component[] components = getComponents();
        i < components.length;
        ++i)
        {
            // if(components[i] instanceof JComponent)
            if((components[i] instanceof OneGraphic) ||
            ((i == 0) && (components[i] instanceof JComponent)))
            {
                ((JComponent) components[i]).setPreferredSize
										(components[i].getSize());
                // components[i].invalidate();
            }
            else if(false)
            {
                // components[i].setSize(components[i].getSize());
                components[i].invalidate();
            }
            System.out.println("components[" + i + "] = " + components[i]);
        }
        
        // revalidate();
        // validateTree();
        // invalidate();
        // validateTree();
        // revalidate();
        // validate();
        // doLayout();
        // repaint();
        System.out.println("totalHeight = " + totalHeight);
        System.out.println("graphics.size() = " + graphics.size());
        
        int topPixel = ((OneGraphic) graphics.elementAt(0)).getSize().height;
        
        OneGraphic lastGraphic =
        ((OneGraphic) graphics.elementAt(graphics.size() - 1));
        
        int totalHeightAvailable = lastGraphic.getLocation().y +
        lastGraphic.getSize().height;
        
        System.out.println("totalHeightAvailable = " + totalHeightAvailable);
        double totalHeightEach = ((double) totalHeightAvailable) /
        ((double) graphics.size());
        System.out.println("totalHeightEach = " + totalHeightEach);
        double totalHeightEachRoundedDown = Math.ceil(totalHeightEach);
        double remiander = totalHeightAvailable - 
						(totalHeightEachRoundedDown * graphics.size());
        System.out.println("totalHeightEachRoundedDown = " + 
													 totalHeightEachRoundedDown);
        System.out.println("remiander = " + remiander);
        System.out.println("(remiander + totalHeightEachRoundedDown) = " + 
													 (remiander + totalHeightEachRoundedDown));
        
                                        /*
                                        System.out.println(" = " + );
                                         */
        
    }
    
    public AxisMarks getXAxisMarks()
    {
        System.out.println("getXAxisMarks() = " + xAxisMarks);
        return xAxisMarks;
    }
    
    /** Used by open in seperate window option
     * @param graphicNumber the graphic to copy to new window
     */    
    public void newGroutWindow(int graphicNumber)
    {
        JFrame window = new JFrame("Grout Window");
        window.getContentPane().setLayout(new BorderLayout());
        GroutPanel newPanel = new GroutPanel();
        newPanel.setX3DFile(parser.getFile());
        
        Vector newGraphics = newPanel.getGraphicsVector();
        
        for(int i = 0; i < newGraphics.size(); ++i)
        {
            if(i != graphicNumber)
            {
                newPanel.remove((Component) newGraphics.elementAt(i));
            }
            else
            {
                Component keepThis = newPanel.getToolBar().getComponent(i);
                newPanel.getToolBar().removeAll();
                newPanel.getToolBar().add(keepThis);
                // System.out.println("keepThis = " + newGraphics.elementAt(i));
            }
        }
        
        newPanel.revalidate();
        
        window.setJMenuBar(newPanel.getMenuBar());
        window.getContentPane().add(newPanel.getToolBar(), BorderLayout.NORTH);
        window.getContentPane().add(newPanel, BorderLayout.CENTER);
        window.addWindowListener(new java.awt.event.WindowAdapter()
        {
            public void windowClosing(java.awt.event.WindowEvent evt)
            {
                evt.getWindow().dispose();
            }
        });
        window.pack();
        window.setVisible(true);
        window.requestFocus();
    }
    
    /** useed to hadle the menu event of the print preview window */    
    class PrintPreviewMenuListener implements ActionListener
    {
        /** The image displayed in the print preview window */        
        BufferedImage image;
        
        public PrintPreviewMenuListener(BufferedImage img)
        {
            image = img;
        }
        
        public void actionPerformed(ActionEvent e)
        {
            JMenuItem source = (JMenuItem)e.getSource();
            String format = source.getAccessibleContext().
            getAccessibleDescription();
            
            File file = null;
            if(!format.equals("print"))
            {
                file = GroutFileChooser.showInputDialogFile
                (source, "Save as", "image");
            }
            ImageWriter.writeImageToFile(image, file, format);
        }
    }
    
    /** This handles all the meun events for the Grout Panel MenuBar */    
    class MenuListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            // // System.out.println("ActionEvent in Grout Panel");
            JMenuItem source = (JMenuItem)e.getSource();
            String whatChanged = source.getAccessibleContext()
								.getAccessibleDescription();
            // System.out.println("whatChanged = " + whatChanged);
            if(whatChanged.equals("UseOneXAxis"))
            {
                JCheckBoxMenuItem locked = (JCheckBoxMenuItem)e.getSource();
                alterOneOrManyXAxis(locked.getState());
                // System.out.println("UseOneXAxis = " + locked.getState());
                notifyTargets("UseOneXAxis", 
															(Object) new Boolean(!locked.getState()),  
															(Object) new Boolean(locked.getState()));
            }
            if(whatChanged.equals("This Resets the View"))
            {
                JMenuItem locked = (JMenuItem)e.getSource();
                // System.out.println("reseet in Grout Window = ");
                notifyTargets("reset", (Object) new Boolean(false),  
															(Object) new Boolean(true));
            }
            if(whatChanged.equals("lockScaleFactors"))
            {
                JCheckBoxMenuItem locked = (JCheckBoxMenuItem)e.getSource();
                notifyTargets("lockScaleFactors", 
															(Object) new Boolean(!locked.getState()),
															(Object) new Boolean(locked.getState()));
            }
            if(whatChanged.equals("useButton"))
            {
                // // System.out.println("useButton in Grout Window");
                notifyTargets("useButton", (Object) new Boolean(false),  
															(Object) new Boolean(true));
            }
            if(whatChanged.equals("useSlider"))
            {
                // // System.out.println("useSlider in Grout Window");
                notifyTargets("useSlider", (Object) new Boolean(false),  
															(Object) new Boolean(true));
            }
            if(whatChanged.equals("jpeg") || whatChanged.equals("print") 
							 || whatChanged.equals("png") || whatChanged.equals("bmp") 
							 || whatChanged.equals("tiff") || whatChanged.equals("pnm"))
            {
                saveImage(whatChanged);
            }
            if(whatChanged.equals("PrintPreview"))
            {
                doPrintPreview();
            }
            if(whatChanged.equals("RELATIVE_TO_SCREEN"))
            {
                notifyTargets("WindowEyepointPolicy", (Object) new Integer(0),
															(Object) new Integer(View.RELATIVE_TO_SCREEN));
            }
            if(whatChanged.equals("RELATIVE_TO_WINDOW"))
            {
                notifyTargets("WindowEyepointPolicy", (Object) new Integer(0),
															(Object) new Integer(View.RELATIVE_TO_WINDOW));
            }
            if(whatChanged.equals("RELATIVE_TO_FIELD_OF_VIEW"))
            {
                notifyTargets("WindowEyepointPolicy", (Object) new Integer(0), 
															(Object) new Integer(View.RELATIVE_TO_FIELD_OF_VIEW));
            }
            if(whatChanged.startsWith("Merge"))
            {
                try
                {
                    int i, j;
                    StringTokenizer st = new StringTokenizer(whatChanged);
                    st.nextToken();
                    i = Integer.parseInt(st.nextToken());
                    j = Integer.parseInt(st.nextToken());
                    mergeGraphics(i, j, e);
                }
                catch(Exception ex)
                {
                    ex.printStackTrace();
                }
            }
            if(whatChanged.startsWith("AddGraphic"))
            {
                // System.out.println(whatChanged);
                int graphicNumber = -1;
                try
                {
                    graphicNumber = Integer.parseInt(whatChanged.substring(10));
                }
                catch(NumberFormatException ex)
                {
                    ex.printStackTrace();
                }
                
                c.gridx = 0;
                c.gridy = 1 + 
										graphics.indexOf(removedGraphics.elementAt(graphicNumber));
                c.weightx = 1;
                c.weighty = 1.0;
                c.gridwidth = GridBagConstraints.REMAINDER;
                c.fill = GridBagConstraints.BOTH;
                gridbag.setConstraints
                (((Component)removedGraphics.elementAt(graphicNumber)), c);
                add((Component)removedGraphics.elementAt(graphicNumber));
                c.weightx = 0.0;
                c.weighty = 0.0;
                
                validate();
                
                // System.out.println(" e.getSource() = " +   e.getSource());
                
                removedGraphicsMenu.remove((JMenuItem) e.getSource());
                
            }
            if(whatChanged.startsWith("Debug"))
            {
                debugMethod();
            }
            if(whatChanged.startsWith("FitViewToWindow"))
            {
                notifyTargets("FitViewToWindow", (Object) new Boolean(true),
                (Object) new Boolean(false));
            }
            if(whatChanged.startsWith("RemoveBlankBoxes"))
            {
                removeBlankBoxes(true);
            }
            if(whatChanged.startsWith("Colour"))
            {
                notifyTargets(whatChanged, (Object) new Boolean(true),
                (Object) new Boolean(false));
            }
            if(whatChanged.equals("Exit"))
            {
                System.exit(0);
            }
            if(whatChanged.startsWith("Font"))
            {
                // System.out.println("GroutPanel event = " + whatChanged);
                if(whatChanged.equals("FontAxisMarksSetInterval"))
                {
                    String interval = (String)JOptionPane.showInputDialog(
                    ((Component) e.getSource()),
                    "Enter Interval",
                    "Customized Dialog",
                    JOptionPane.PLAIN_MESSAGE,
                    null,
                    null,
                    null);
                    Object newValue = null;
                    while(newValue == null)
                    {
                        try
                        {
                            newValue = Double.valueOf(interval);
                        }
                        catch(NumberFormatException ex)
                        {
                            interval = (String)JOptionPane.showInputDialog(
                            ((Component) e.getSource()),
                            "Please enter a number",
                            "Customized Dialog",
                            JOptionPane.PLAIN_MESSAGE,
                            null,
                            null,
                            null);
                            
                        }
                    }
                    notifyTargets(whatChanged,(Object) new Boolean(false), 
																	newValue);
                }
                else
                {
                    notifyTargets(whatChanged, (Object)  new Boolean(true),
                    (Object) new Boolean(false));
                }
                // System.out.println("GroutPanel done");
            }
            if(whatChanged.startsWith("Open"))
            {
                String filename = GroutFileChooser.
                showInputDialog((Component) e.getSource(),
                "Enter file name",
                "/people/hmorgan2/code/samples/xml/newXml/pepinfoOutput1.x3d");
                // System.out.println("filename = " + filename);
                if(filename != null)
                {
                    clearAllData();
                    //								parser.clearAllData();
                    parser.setFilename(filename);
                    validate();
                    ownerWindow.pack();
                    
                }
            }
            if(whatChanged.startsWith("Save"))
            {
                parser.saveFile();
            }
            if(whatChanged.startsWith("newGroutWindow"))
            {
                int graphicNumber = 0;
                try
                {
                    graphicNumber = Integer.parseInt(whatChanged.substring(21));
                }
                catch(NumberFormatException ex)
                {
                    ex.printStackTrace();
                }
                
                newGroutWindow(graphicNumber);
            }
            if(whatChanged.startsWith("SetPickPolicy"))
            {
                // System.out.println("whatChanged = " + whatChanged);
                // System.out.println("whatChanged.substring(13) = " + whatChanged.substring(13));
                if(whatChanged.endsWith("ZoomTo"))
                {
                    //							changePickFactory(new ZoomToOccuranceFactory(this));
                    changePickFactory("ZoomTo");
                }
                if(whatChanged.substring(13).startsWith("ChangeColour"))
                {
                    //							changePickFactory(new ZoomToOccuranceFactory(this));
                    changePickFactory(whatChanged.substring(13));
                }
                if(whatChanged.endsWith("ShowSequence"))
                {
                    //							changePickFactory(new ZoomToOccuranceFactory(this));
                    changePickFactory("ShowSequence");
                }
                if(whatChanged.endsWith("MergeGraphics"))
                {
                    changePickFactory("MergeGraphics");
                }
            }
            if(whatChanged.equals("Undo"))
            {
                undoOne();
            }
            if(whatChanged.equals("Redo"))
            {
                redoOne();
            }
        }
    }
    
    public void changePickFactory(String type)
    {
        // System.out.println("changePickFactory to " + type);
        
        if(type.startsWith("ZoomTo"))
        {
            changePickFactory(new ZoomToOccuranceFactory(this));
        }
        if(type.startsWith("ShowSequence"))
        {
            changePickFactory(new ShowSequenceOccuranceFactory(this));
        }
        if(type.startsWith("MergeGraphics"))
        {
            changePickFactory(new MergeGraphicsOccuranceFactory(this));
        }
        if(type.startsWith("ChangeColour"))
        {
            changePickFactory(new ChangeColourOccuranceFactory(this,
            type.substring(12)));
        }
    }
    
    /** The tool bar could do somethign, but it does not yet */    
    class ToolbarListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            // // System.out.println("ActionEvent in Grout Window");
            JButton source = (JButton)e.getSource();
            String whatChanged = source.getAccessibleContext().getAccessibleDescription();
            // // System.out.println("whatChanged = " + whatChanged);
            // None of these commands are relavent any longer
        }
    }
    
    public void windowActivated(WindowEvent e)
    {
    }
    
    public void windowClosed(WindowEvent e)
    {
    }
    
    public void windowClosing(WindowEvent e)
    {
    }
    
    public void windowDeactivated(WindowEvent e)
    {
    }
    
    public void windowDeiconified(WindowEvent e)
    {
    }
    
    public void windowIconified(WindowEvent e)
    {
    }
    
    public void windowOpened(WindowEvent e)
    {
        //				((JFrame)e.getSource()).notifyAll();
        //				Runtime.getRuntime().gc();
        //				for(int i = 0; i < 10000; ++i) { ++i; }
        stuffToDoWhenWindowIsDisplayed();
    }
    
    public void componentResized(ComponentEvent e)
    {
        int limit = graphics.size();
        int rendWindSize = 0;
        // for(int i = 0; ((i < limit) && (rendWindSize == 0)); ++i)
        for(int i = 0; i < limit; ++i)
        {
            if(((OneGraphic) graphics.elementAt(i)).isShowing())
            {
                rendWindSize = ((OneGraphic) graphics.elementAt(i))
										.getRendWindContainerSize();
            }
        }
        xAxis.setMinimumSize(new Dimension(rendWindSize, 70));
        xAxis.setPreferredSize(new Dimension(rendWindSize, 70));
        // xAxis.setMinimumSize(new Dimension (rendWindSize,
        // 																		xAxis.getSize().height));
        xAxis.revalidate();
        notifyTargets("FitViewToWindow", (Object) new Boolean(true),
        (Object) new Boolean(false));
        repaint();
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
    
				// Variables declaration - do not modify//GEN-BEGIN:variables
				// End of variables declaration//GEN-END:variables
    
    public void oneGraphicChanged(OneGraphicEvent e)
    {
        String whatChanged = e.getPropertyName();
        if(whatChanged.startsWith("graphicColourChanged"))
        {
            
            int graphicNumber = -1;
            try
            {
                graphicNumber = Integer.parseInt(whatChanged.substring(20));
            }
            catch(NumberFormatException ex)
            {
                ex.printStackTrace();
            }
            
            if(graphicNumber != -1)
            {
                JLabel label = ((JLabel) toolbarLabels.elementAt(graphicNumber));
                
                Color imageColour = ((Color) e.getNewValue());
                BufferedImage image = new BufferedImage(10, 10,
                BufferedImage.TYPE_INT_RGB);
                Graphics drawHere = image.getGraphics();
                drawHere.setColor(imageColour);
                drawHere.fillRect(0, 0, 10, 10);
                
                ImageIcon picture = new ImageIcon(image);
                label.setIcon(picture);
            }
            
        }
        // parser.oneGraphicChanged(whatChanged);
    }
    
}
