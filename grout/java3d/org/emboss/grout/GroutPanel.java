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


/**
 *
 * @author  hmorgan2
 */
public class GroutPanel extends javax.swing.JPanel implements java.io.Serializable, ControlContainerListener, ParserListener, WindowListener, ComponentListener
{
  
  private Vector targets = new Vector();
  
		//  private RendWindContainer canvas;
  private Parser parser;
  private JPanel renderPanel;
  private GridBagLayout gridbag = new GridBagLayout();
  private GridBagConstraints c = new GridBagConstraints();

  private JPanel xAxis;
  private AxisMarks xAxisMarks = null;
		//  private AxisMarks xAxisMarks = null, yAxisMarks = null;
		//  private SequenceView xSequence = null;
  private ViewPositionModel viewPositionModel;
  private ControlContainer controls;

  private Vector graphics = new Vector();

		//  private Vector controls = new Vector();
		//  private Vector viewPositionModel = new Vector();

  private JMenuBar menuBar;
  private JMenu graphicsMenu;
  
  private boolean scaleFactorsLocked = false;
  private boolean offScreen = false;
  
  private Window ownerWindow = null;
  
	private JCheckBoxMenuItem useOneXAxis = new JCheckBoxMenuItem("Use one X axis");

		private	PickOccuranceFactory pickFactory;

  private Vector undoables = new Vector();
	private	int undoableIndex = -1;

		private String xSequenceString = null;
		private String ySequenceString = null;

  /** Creates new form GroutPanel */
  public GroutPanel()
  {
    super();
    setLayout(gridbag);
    parser = new Parser();
		parser.setOwnerPanel(this);
		//    setupRenderingPanel(false);
    setupControls();
    setupviewPositionModel();
    setupToolbarAndMenu();
		//    setupListeners();
		//    attachPickBehavior(canvas.getCanvas());
  }
  
  /** Creates new form GroutPanel */
  public GroutPanel(String filename)
  {
    super();
    setLayout(gridbag);
    
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
  
  public String getXSequence()
  {
    return xSequenceString;
  }
  
  public String getYSequence()
  {
    return ySequenceString;
  }
  
  public void addUndoableOccurance(UndoableOccurance undoableOccurance)
  {
			if(undoableIndex < (undoables.size() - 1))
			{
					//					undoables.removeRange((undoableIndex + 1), (undoables.size() - 1));
					//					undoables.subList((undoableIndex + 1), (undoables.size() - 1)).clear();
					// this works, but has to be slow hugh
					undoables.setSize(undoableIndex + 1);
			}
			undoables.addElement(undoableOccurance);
			undoableIndex++;
  }
  
  public GroutPanel(String filename, String source, boolean offScreenInstance)
  {
    super();
    // System.out.println("source = " +  source);
    // // System.out.println("source.startsWith(OffScreen) = " +  source.startsWith("OffScreen"));
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
        // // System.out.println("Unknown option " + source + ". Please try again.");
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
    GroutPanel panel = null;
    
    for(int arg = 0; arg < args.length; ++arg)
    {
      if(args[arg].startsWith("-OffScreen=") && (args[arg].length() > 11))
      {
        offScreen = true;
        format = args[arg].substring(11);
      } else if(args[arg].startsWith("-source") && ((args[arg].length() > 8) || !offScreen))
      {
        if(args[arg].length() > 8)
        {
          source = args[arg].substring(8);
        } else
        {
	    //          source = JOptionPane.showInputDialog(window, "Enter source", "File");
          source = JOptionPane.showInputDialog(window, "Enter source");
        }
      } else if(args[arg].startsWith("-file") && ((args[arg].length() > 6) || !offScreen))
      {
        if(args[arg].length() > 6)
        {
          filename = args[arg].substring(6);
        } else
        {
	    //          filename = JOptionPane.showInputDialog(window, "Enter file name", "/people/hmorgan2/code/samples/xml/newXml/dotmatcherOutput.x3d");
          filename = GroutFileChooser.showInputDialog(window, "Enter file name", "/people/hmorgan2/code/samples/xml/newXml/dotmatcherOutput.x3d");
        }
      } else
      {
        System.err.println("unrecognised option " + args[arg] + System.getProperty("line.separator") + "Use = java GroutPanel filename [use][-format]");
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
        filename = GroutFileChooser.showInputDialog(window, "Enter file name", "~/code/samples/ajb2.wrl");
      }
    }
    
    // // System.out.println("filename = " + filename);
    // // System.out.println("offScreen = " + offScreen);
    // // System.out.println("source = " + source);
    // // System.out.println("format = " + format);
    
/*		File tempFile = new File(filename);
                // // System.out.println("file exists = " + tempFile.exists()); */
    
    if(source == null)
    {
      panel = new GroutPanel();
			//			panel.loadProperties();
			//			panel.setOwnerWindow(window);
      // // System.out.println("filename = " + filename);
      panel.setFilename(filename);
      window.getContentPane().add(panel);
      window.setJMenuBar(panel.getMenuBar());
      
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
			panel.setOwnerWindow(window);
      window.getContentPane().add(panel);
      window.setJMenuBar(panel.getMenuBar());
      
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
/*				if(args[1].equals("OffScreen"))
                                {
                                        panel.saveImage("jpeg");
                                } else if(args[1].startsWith("OffScreen-"))
                                { */
        panel.saveImage(format);
/*				} else
                                {
                                        System.err.println("unrecognised option " + args[1] + System.getProperty("line.separator") + "Use = java GroutPanel filename [use][-format]");
                                } */
        //				// // System.out.println(" System.getProperty(java.specification.version) = " + System.getProperty("java.specification.version"));
        
        System.exit(0);
      } else
      {
        window.setVisible(true);
      }
    } /* else
                {
                        System.err.println("Use = java GroutPanel filename [use][-format]");
                        System.exit(0);
                } */
  }
  
  public void saveImage(String format)
		{
				BufferedImage[] allImages;
				int[] xCoords;
				int[] yCoords;
				if(useOneXAxis.getState())
				{
						allImages = new BufferedImage[parser.getNumberOfGraphics() + 1];
						xCoords = new int[parser.getNumberOfGraphics() + 1];
						yCoords = new int[parser.getNumberOfGraphics() + 1];

						Point xAxisPos = xAxis.getLocation();
						BufferedImage wholeImage = ImageWriter.createBufferedImageFromComponent(this);
						BufferedImage bottomPanel = wholeImage.getSubimage(0, xAxisPos.y, 
																															 wholeImage.getWidth(),
																															 wholeImage.getHeight() - xAxisPos.y);

						allImages[parser.getNumberOfGraphics()] = bottomPanel;
						xCoords[parser.getNumberOfGraphics()] = 0;
						yCoords[parser.getNumberOfGraphics()] = parser.getNumberOfGraphics();
				}
				else
				{
						allImages = new BufferedImage[parser.getNumberOfGraphics()];
						xCoords = new int[parser.getNumberOfGraphics()];
						yCoords = new int[parser.getNumberOfGraphics()];
				}
			
				for (int i = 0; i < parser.getNumberOfGraphics(); ++i)
				{
						OneGraphic graphic = (OneGraphic) graphics.elementAt(i);
						allImages[i] = graphic.getImage();
						xCoords[i] = 0;
						yCoords[i] = i;
				}

				BufferedImage wholeImage = ImageWriter.joinImages(allImages, xCoords, yCoords);
				/*
				String filename = GroutFileChooser.showInputDialog(this, "Save as", "image");
				ImageWriter.writeImageToFile(wholeImage, new File(filename + "." + format), format);
				*/
				File file = GroutFileChooser.showInputDialogFile(this, "Save as", "image");
				ImageWriter.writeImageToFile(wholeImage, file, format);

				// // System.out.println("Doing rendering");
				// // System.out.println("Getting mainCanvasImage");
				// this is all rubish now
				/*
					BufferedImage mainCanvasImage = null;
					BufferedImage xAxisPanelImage = null;
					BufferedImage yAxisPanelImage = null;
					if(offScreen)
					{
					canvas.renderOffScreenBuffer();
					mainCanvasImage = canvas.getOffScreenBuffer();
					xAxisPanelImage = (BufferedImage) xAxisMarks.createImage(400, 1000);
					yAxisPanelImage = (BufferedImage) yAxisMarks.createImage(25, 400);
					} else
					{
					mainCanvasImage = ImageWriter.createBufferedImageFromCanvas3D(canvas.getCanvas());
					xAxisPanelImage = ImageWriter.createBufferedImageFromComponent((Component) xAxisMarks);
					yAxisPanelImage = ImageWriter.createBufferedImageFromComponent((Component) yAxisMarks);
					}
    
					BufferedImage[] images =
					{ yAxisPanelImage, mainCanvasImage, xAxisPanelImage };
					int[] xCoords =
					{ 0, 1, 1 };
					int[] yCoords =
					{ 0, 0, 1 };
    
					BufferedImage wholeImage = ImageWriter.joinImages(images, xCoords, yCoords);
					ImageWriter.writeImageToFile(wholeImage, new File("image." + format), format);
				*/
				/*		// // System.out.println("got mainCanvasImage = " + mainCanvasImage);
				// // System.out.println("Getting renderPanelImage = " + renderPanel);
				BufferedImage renderPanelImage = ImageWriter.createBufferedImageFromComponent((Component) renderPanel);
				// // System.out.println("got renderPanelImage = " + renderPanelImage);
				// // System.out.println("Adding 2 images together");
				ImageWriter.add3DImage(renderPanelImage, mainCanvasImage);
				// // System.out.println("Saving  renderPanelImage");
				ImageWriter.writeImageToFile(renderPanelImage, new File("image." + format), format);
				// // System.out.println("Saved  renderPanelImage");
				ImageWriter.writeImageToFile(renderPanelImage, new File("image." + format), format);
				// // System.out.println("Saved  " + "image." + format);
 
				// // System.out.println("ySequence%%% = " + ySequence);
				BufferedImage ySequenceImage = ImageWriter.createBufferedImageFromComponent((Component) ySequence);
				ImageWriter.writeImageToFile(ySequenceImage, new File("imageYSeq." + format), format);
				//		BufferedImage xAxisPanelImage = ImageWriter.createBufferedImageFromComponent((Component) xAxisMarks);
				BufferedImage xAxisPanelImage = (BufferedImage) xAxisMarks.createImage(400, 800);
				//		ImageWriter.writeImageToFile(xAxisPanelImage, new File("imageX." + format), format);
				//		BufferedImage yAxisPanelImage = ImageWriter.createBufferedImageFromComponent((Component) yAxisMarks);
				BufferedImage yAxisPanelImage = (BufferedImage) yAxisMarks.createImage(400, 400);
				//		ImageWriter.writeImageToFile(yAxisPanelImage, new File("imageY." + format), format);
				//		BufferedImage xSequenceImage = ImageWriter.createBufferedImageFromComponent((Component) xSequence);
				//		ImageWriter.writeImageToFile(xSequenceImage, new File("imageXSeq." + format), format);
 
				BufferedImage[] images = { yAxisPanelImage, mainCanvasImage, xAxisPanelImage };
				int[] xCoords = { 0, 1, 1 };
				int[] yCoords = { 0, 0, 1 };
 
				BufferedImage wholeImage = ImageWriter.joinImages(images, xCoords, yCoords);
				ImageWriter.writeImageToFile(wholeImage, new File("image." + format), format);
				*/
		}
  

  private void loadProperties()
  {
    // Also, I try and explicetly load the properties file
    // Shit place to do it, but we shall see
    getClass().getClassLoader().getResource("interactive.properties");
	}

  private static void loadSystemLibrarys() throws Exception
  {
    //
    // Web Start needs explicit native lib loading!
    //
    String os = System.getProperty( "os.name" );
    System.out.println("loading " + os + " native libraries ..");
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
  
  private void setPicking()
  {
			if(pickFactory == null)
			{
					pickFactory = new ZoomToOccuranceFactory(this);
			}
			
	}

  private void setupListeners()
  {
			/*    parser.addParserListener(canvas);
    //        controls.addControlContainerListener(this);
    //        controls.addControlContainerListener(canvas);
    //        parser.addParserListener(controls);
    parser.addParserListener(viewPositionModel);
    controls.addControlContainerListener(viewPositionModel);
    viewPositionModel.addViewPositionModelListener(controls);
    //		viewPositionModel.addViewPositionModelListener(viewPositionModel);
    viewPositionModel.addViewPositionModelListener(xAxisMarks);
    viewPositionModel.addViewPositionModelListener(yAxisMarks);
    //		viewPositionModel.addViewPositionModelListener(xSequence);
    //		viewPositionModel.addViewPositionModelListener(ySequence);
    addGroutPanelListener(controls);
    addGroutPanelListener(viewPositionModel);
    viewPositionModel.addViewPositionModelListener(canvas);
    //		viewPositionModel.addviewPositionModelListener(axisMarks);
		*/
			for (Enumeration e = graphics.elements() ; e.hasMoreElements() ;) 
					{
							OneGraphic graphic = (OneGraphic) e.nextElement();
							graphic.setParserListeners(parser);
							graphic.setGroutPanelListeners(this);
							addGroutPanelListener(graphic);
              // graphic.attachPickBehavior(graphic.getRendWindContainer().getCanvas());
              graphic.attachPickBehavior(pickFactory);
							//							graphic.setControlListeners(controls);
					}
			parser.addParserListener(viewPositionModel);
			viewPositionModel.addViewPositionModelListener(xAxisMarks);
			viewPositionModel.addViewPositionModelListener(controls);
			controls.addControlContainerListener(viewPositionModel);
			addGroutPanelListener(controls);
			addComponentListener(this);
			//			System.out.println("grout controls = " + controls);
			//			ownerWindow.addComponentListener(this);
			//			viewPositionModel.addViewPositionModelListener(xSequence);
			
			//		graphic.setParserListeners(parser);
			//		graphic.setControlListeners(controls);
		
  }
  
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
  
  private void setupviewPositionModel()
  {
		/*
    viewPositionModel = new ViewPositionModel();
		*/
  }
  
  private void setupRenderingPanel(boolean offScreen)
  {
			for(int i = 0; i<parser.getNumberOfGraphics(); ++i)
					{
							OneGraphic graphic = new OneGraphic();
							graphic.setGraphicNumber(i);
							graphics.addElement(graphic);
    
							c.gridx = 0;
							c.gridy = 1 + i;
							//							c.weightx = 1.0;
							//							c.weightx = GridBagConstraints.REMAINDER;
							c.weightx = 1;
							c.weighty = 1.0;
							c.gridwidth = GridBagConstraints.REMAINDER;
							c.fill = GridBagConstraints.BOTH;
							gridbag.setConstraints(graphic, c);
							add(graphic);
							c.weightx = 0.0;
							c.weighty = 0.0;
					}

  }

  private void setupRenderingPanel(boolean offScreen, int panelNumber)
  {
			/*
				GridBagLayout gridbagRP = new GridBagLayout();
				GridBagConstraints cRP = new GridBagConstraints();
    
				RendWindContainer canvas2 = new RendWindContainer(offScreen);
				JPanel renderPanel2 = new JPanel(gridbagRP);
				renderPanel2.setBackground(Color.black);
				cRP.gridx = 2;
				cRP.gridy = 0;
				cRP.weightx = 1.0;
				cRP.weighty = 1.0;
				cRP.fill = GridBagConstraints.BOTH;
				gridbagRP.setConstraints(canvas2, cRP);
				renderPanel2.add(canvas2);
				cRP.weightx = 0.0;
				cRP.weighty = 0.0;
    
				JPanel xAxis = new JPanel(new BorderLayout());
				// // System.out.println("Making x axis marks offScreen = " + offScreen);
				AxisMarks xAxisMarks2 = new AxisMarks(offScreen);
				// // System.out.println("xAxisMarks2 = " + xAxisMarks2);
				xAxis.add(xAxisMarks2, BorderLayout.NORTH);
				SequenceView xSequence2 = new SequenceView();
				xSequence2.setSequenceFile("resources/sequence.txt");
				xAxis.add(xSequence2, BorderLayout.SOUTH);
				cRP.gridx = 2;
				cRP.gridy = 1;
				cRP.fill = GridBagConstraints.HORIZONTAL;
				gridbagRP.setConstraints(xAxis, cRP);
				renderPanel2.add(xAxis);
    
				// // System.out.println("Making y axis marks");
				AxisMarks yAxisMarks2 = new AxisMarks(offScreen);
				// // System.out.println("yAxisMarks2 = " + yAxisMarks2);
				yAxisMarks2.setOrientation(AxisMarks.VERTICAL);
				SequenceView ySequence2 = new SequenceView();
				ySequence2.setOrientation(SequenceView.VERTICAL);;
				ySequence2.setSequenceFile("resources/sequence.txt");
				cRP.gridx = 0;
				cRP.gridy = 0;
				cRP.fill = GridBagConstraints.VERTICAL;
				gridbagRP.setConstraints(ySequence2, cRP);
				renderPanel2.add(ySequence2);
				cRP.gridx = 1;
				cRP.gridy = 0;
				gridbagRP.setConstraints(yAxisMarks2, cRP);
				renderPanel2.add(yAxisMarks2);
    
				c.gridx = 1 + panelNumber;
				c.gridy = 1;
				c.weightx = 1.0;
				c.weighty = 1.0;
				c.gridwidth = 1;
				c.fill = GridBagConstraints.BOTH;
				gridbag.setConstraints(renderPanel2, c);
				add(renderPanel2);
				c.weightx = 0.0;
				c.weighty = 0.0;
			*/
  }

  private void setupControls()
  {
    // Also, I try and explicetly load the properties file
    // Shit place to do it, but we shall see
    // System.out.println("interactive.properties = " + getClass().getClassLoader().getResource("interactive.properties"));

    controls = new ControlContainer();
		xAxisMarks = new AxisMarks();
		// xSequence = new SequenceView();
		xAxis = new JPanel(new BorderLayout());
		viewPositionModel = new ViewPositionModel();
		
		xAxis.add(controls.getBottomPanel(), BorderLayout.SOUTH);
		xAxis.add(xAxisMarks, BorderLayout.NORTH);
//		xAxis.add(xSequence, BorderLayout.CENTER);
			/*
    c.gridx = 0;
    c.gridy = 1;
    c.fill = GridBagConstraints.VERTICAL;
    c.weightx = 0.0;
    gridbag.setConstraints(controls.getCentrePanel(), c);
    add(controls.getCentrePanel());

    c.gridx = 1;
    c.gridy = 2;
    c.fill = GridBagConstraints.HORIZONTAL;
    gridbag.setConstraints(controls.getBottomPanel(), c);
    add(controls.getBottomPanel());
    //        getContentPane().add(controls, BorderLayout.WEST);
			*/
  }
  private void setupToolbarAndMenu()
  {
    //        JPanel menuAndTools = new JPanel();
			// we do not like the toolbar
			/*
    JToolBar toolBar = new JToolBar();
    JButton button = null;
    button = new JButton(new ImageIcon("resources/pics/home_ns_.gif"));
    button.getAccessibleContext().setAccessibleDescription("reset");
    button.addActionListener(new ToolbarListener());
    toolBar.add(button);
    button = new JButton(new ImageIcon("resources/pics/go_next_.gif"));
    toolBar.add(button);
    button = new JButton(new ImageIcon("resources/pics/purplestar.GIF"));
    button.getAccessibleContext().setAccessibleDescription("lockScaleFactors");
    button.addActionListener(new ToolbarListener());
    button.setPreferredSize(new Dimension(20, 20));
    toolBar.add(button);
    button = new JButton(new ImageIcon("resources/pics/go_prev_.gif"));
    toolBar.add(button);
    button = new JButton(new ImageIcon("resources/pics/go_prev_.gif"));
    toolBar.add(button);
    button = new JButton(new ImageIcon("resources/pics/go_prev_.gif"));
    toolBar.add(button);
    //        menuAndTools.add(toolBar, BorderLayout.SOUTH);
    
    //        getContentPane().add(menuAndTools, BorderLayout.NORTH);
    c.gridx = 0;
    c.gridy = 0;
    c.gridwidth = GridBagConstraints.REMAINDER;
    gridbag.setConstraints(toolBar, c);
		//    add(toolBar);
		*/
    
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
    
    submenu = new JMenu("Output to");
    submenu.setMnemonic(KeyEvent.VK_S);
    
    menuItem = new JMenuItem("Jpeg Image");
    menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.ALT_MASK));
    menuItem.getAccessibleContext().setAccessibleDescription("jpeg");
    menuItem.addActionListener(new MenuListener());
    submenu.add(menuItem);
    
    menuItem = new JMenuItem("Png Image");
    menuItem.getAccessibleContext().setAccessibleDescription("png");
    menuItem.addActionListener(new MenuListener());
    submenu.add(menuItem);
    
    menuItem = new JMenuItem("Bmp Image");
    menuItem.getAccessibleContext().setAccessibleDescription("bmp");
    menuItem.addActionListener(new MenuListener());
    submenu.add(menuItem);
    
    menuItem = new JMenuItem("Tiff Image");
    menuItem.getAccessibleContext().setAccessibleDescription("tiff");
    menuItem.addActionListener(new MenuListener());
    submenu.add(menuItem);
    
    menuItem = new JMenuItem("Pnm Image");
    menuItem.getAccessibleContext().setAccessibleDescription("pnm");
    menuItem.addActionListener(new MenuListener());
    submenu.add(menuItem);
    
    menuItem = new JMenuItem("Printer or Postscript");
    menuItem.getAccessibleContext().setAccessibleDescription("print");
    menuItem.addActionListener(new MenuListener());
    submenu.add(menuItem);
    menu.add(submenu);

		addDisposeOfGroutPanelMenuItem(menu);

    menu = new JMenu("View");
    menu.setMnemonic(KeyEvent.VK_A);
    menu.getAccessibleContext().setAccessibleDescription(
    "This controls the View");
    menuBar.add(menu);
    
    //a group of JMenuItems
    menuItem = new JMenuItem("Reset View", KeyEvent.VK_R);
    menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_1, ActionEvent.ALT_MASK));
    menuItem.getAccessibleContext().setAccessibleDescription("This Resets the View");
    menuItem.addActionListener(new MenuListener());
    menu.add(menuItem);
    
    //        menuItem = new JMenuItem("Both text and icon",
    //        new ImageIcon("middle.gif"));
    //        menuItem.setMnemonic(KeyEvent.VK_B);
    //        menu.add(menuItem);
    //
    //        menuItem = new JMenuItem(new ImageIcon("images/middle.gif"));
    //        menuItem.setMnemonic(KeyEvent.VK_D);
    //        menu.add(menuItem);
    
    //a group of radio button menu items
		// no longer use slider zoom
		/*
    menu.addSeparator();
    ButtonGroup group = new ButtonGroup();
    rbMenuItem = new JRadioButtonMenuItem("Use slider zoom");
    rbMenuItem.setMnemonic(KeyEvent.VK_S);
    rbMenuItem.getAccessibleContext().setAccessibleDescription("useSlider");
    group.add(rbMenuItem);
    rbMenuItem.addActionListener(new MenuListener());
    menu.add(rbMenuItem);
    
    rbMenuItem = new JRadioButtonMenuItem("Use lock button");
    rbMenuItem.setSelected(true);
    rbMenuItem.setMnemonic(KeyEvent.VK_L);
    rbMenuItem.getAccessibleContext().setAccessibleDescription("useButton");
    group.add(rbMenuItem);
    rbMenuItem.addActionListener(new MenuListener());
    menu.add(rbMenuItem);
		*/
    
    //a group of check box menu items
		//    menu.addSeparator();
		// this is no longer required
		/*
    cbMenuItem = new JCheckBoxMenuItem("Lock X and Y axis");
    cbMenuItem.getAccessibleContext().setAccessibleDescription("lockScaleFactors");
    cbMenuItem.setMnemonic(KeyEvent.VK_C);
    cbMenuItem.addActionListener(new MenuListener());
    menu.add(cbMenuItem);
		*/
    
    cbMenuItem = new JCheckBoxMenuItem("Use one X axis");
		//    JCheckBoxMenuItem useOneXAxis = new JCheckBoxMenuItem("Use one X axis");
    useOneXAxis.getAccessibleContext().setAccessibleDescription("UseOneXAxis");
    useOneXAxis.setMnemonic(KeyEvent.VK_U);
    useOneXAxis.addActionListener(new MenuListener());
    menu.add(useOneXAxis);
    
    menuItem = new JMenuItem("Fit View to Window");
    menuItem.getAccessibleContext().setAccessibleDescription("FitViewToWindow");
		menuItem.addActionListener(new MenuListener());
    menuItem.setMnemonic(KeyEvent.VK_F);
    menu.add(menuItem);
    
    menu.addSeparator();

    submenu = new JMenu("Titles");
    submenu.setMnemonic(KeyEvent.VK_A);

    submenu2 = new JMenu("Main Title");
    submenu2.setMnemonic(KeyEvent.VK_M);

    submenu3 = new JMenu("Set Font Size");
    submenu3.setMnemonic(KeyEvent.VK_S);

    menuItem = new JMenuItem("8");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize8");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("10");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize10");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("12");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize12");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("14");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize14");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("16");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize16");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("18");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize18");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("20");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize20");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("24");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize24");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("28");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize28");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    submenu2.add(submenu3);

    submenu3 = new JMenu("Set Colour");
    submenu3.setMnemonic(KeyEvent.VK_C);

    menuItem = new JMenuItem("Black");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleColourBlack");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    menuItem = new JMenuItem("White");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleColourWhite");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    menuItem = new JMenuItem("Pink");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleColourPink");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    menuItem = new JMenuItem("Red");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleColourRed");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    submenu2.add(submenu3);

    menuItem = new JMenuItem("Set Text");
    menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSetText");
    menuItem.addActionListener(new MenuListener());
    submenu2.add(menuItem);
		

    submenu.add(submenu2);
    menu.add(submenu);

    submenu2 = new JMenu("X Title");
    submenu2.setMnemonic(KeyEvent.VK_M);

    submenu3 = new JMenu("Set Font Size");
    submenu3.setMnemonic(KeyEvent.VK_S);

    menuItem = new JMenuItem("8");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize8");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("10");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize10");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("12");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize12");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("14");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize14");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("16");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize16");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("18");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize18");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("20");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize20");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("24");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize24");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("28");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize28");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    submenu2.add(submenu3);

    submenu3 = new JMenu("Set Colour");
    submenu3.setMnemonic(KeyEvent.VK_C);

    menuItem = new JMenuItem("Black");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleColourBlack");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    menuItem = new JMenuItem("White");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleColourWhite");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    menuItem = new JMenuItem("Pink");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleColourPink");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    menuItem = new JMenuItem("Red");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleColourRed");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    submenu2.add(submenu3);

    menuItem = new JMenuItem("Set Text");
    menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSetText");
    menuItem.addActionListener(new MenuListener());
    submenu2.add(menuItem);
		
    submenu.add(submenu2);
    menu.add(submenu);

    submenu2 = new JMenu("Y Title");
    submenu2.setMnemonic(KeyEvent.VK_M);

    submenu3 = new JMenu("Set Font Size");
    submenu3.setMnemonic(KeyEvent.VK_S);

    menuItem = new JMenuItem("8");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize8");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("10");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize10");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("12");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize12");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("14");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize14");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("16");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize16");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("18");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize18");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("20");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize20");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("24");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize24");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("28");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize28");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    submenu2.add(submenu3);

    submenu3 = new JMenu("Set Colour");
    submenu3.setMnemonic(KeyEvent.VK_C);

    menuItem = new JMenuItem("Black");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleColourBlack");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    menuItem = new JMenuItem("White");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleColourWhite");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    menuItem = new JMenuItem("Pink");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleColourPink");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    menuItem = new JMenuItem("Red");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleColourRed");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    submenu2.add(submenu3);

    menuItem = new JMenuItem("Set Text");
    menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSetText");
    menuItem.addActionListener(new MenuListener());
    submenu2.add(menuItem);
		
    submenu.add(submenu2);
		//    menu.add(submenu);

    submenu2 = new JMenu("Axis Marks");
    submenu2.setMnemonic(KeyEvent.VK_M);

    submenu3 = new JMenu("Set Font Size");
    submenu3.setMnemonic(KeyEvent.VK_S);

    menuItem = new JMenuItem("8");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize8");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("10");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize10");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("12");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize12");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("14");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize14");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("16");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize16");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("18");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize18");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("20");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize20");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("24");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize24");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);
    menuItem = new JMenuItem("28");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize28");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    submenu2.add(submenu3);

    submenu3 = new JMenu("Set Colour");
    submenu3.setMnemonic(KeyEvent.VK_C);

    menuItem = new JMenuItem("Black");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksColourBlack");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    menuItem = new JMenuItem("White");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksColourWhite");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    menuItem = new JMenuItem("Pink");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksColourPink");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    menuItem = new JMenuItem("Red");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksColourRed");
    menuItem.addActionListener(new MenuListener());
    submenu3.add(menuItem);

    submenu2.add(submenu3);

    menuItem = new JMenuItem("Set Interval");
    menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSetInterval");
    menuItem.addActionListener(new MenuListener());
    submenu2.add(menuItem);
		
    submenu.add(submenu2);
    menu.add(submenu);


    //        menuItem = new JMenuItem("Save Image", KeyEvent.VK_S);
    //        menuItem.getAccessibleContext().setAccessibleDescription("saveImage");
    //        menuItem.addActionListener(new MenuListener());
    //        menu.add(menuItem);
    
    //        cbMenuItem = new JCheckBoxMenuItem("Another one");
    //        cbMenuItem.setMnemonic(KeyEvent.VK_H);
    //        menu.add(cbMenuItem);
    
    //a submenu

    //        submenu = new JMenu("Save Image", KeyEvent.VK_S); How do I add a key shortcut to a submenu?
    
    //Build second menu in the menu bar.
    // No don't
		/*
    menu = new JMenu("Setup");
    menu.setMnemonic(KeyEvent.VK_N);
    menu.getAccessibleContext().setAccessibleDescription(
    "This menu does nothing");
		*/
    
		// this is no longer required
		/*
    submenu = new JMenu("Set View Policy");
    submenu.setMnemonic(KeyEvent.VK_V);
    
    ButtonGroup group = new ButtonGroup();
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

    submenu = new JMenu("Set Pick Policy");
    submenu.setMnemonic(KeyEvent.VK_V);
    
    ButtonGroup group = new ButtonGroup();
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

		/*
    rbMenuItem = new JRadioButtonMenuItem("Change Colour");
    rbMenuItem.getAccessibleContext().setAccessibleDescription("SetPickPolicyChangeColour");
    rbMenuItem.setSelected(false);
    rbMenuItem.setMnemonic(KeyEvent.VK_V);
		rbMenuItem.addActionListener(new MenuListener());
    group.add(rbMenuItem);
    submenu.add(rbMenuItem);
		*/

    menu.add(submenu);

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
    
    menuItem = new JMenuItem("Undo");
    menuItem.getAccessibleContext().setAccessibleDescription("Undo");
    menuItem.addActionListener(new MenuListener());
    menu.add(menuItem);

    menuItem = new JMenuItem("Redo");
    menuItem.getAccessibleContext().setAccessibleDescription("Redo");
    menuItem.addActionListener(new MenuListener());
    menu.add(menuItem);

    menuBar.add(menu);
  }
  
	protected void addDisposeOfGroutPanelMenuItem(JMenu menu)
	{
    JMenuItem menuItem = new JMenuItem("Exit");
    menuItem.setMnemonic(KeyEvent.VK_E);
    menuItem.getAccessibleContext().setAccessibleDescription("Exit");
    menuItem.addActionListener(new MenuListener());
    menu.add(menuItem);				
	}

  private void attachPickBehavior(final Canvas3D canvas3D)
  {
			// this all has to go in OneGraphic
			/*
    canvas3D.addMouseListener(new MouseAdapter()
    {
      public void mouseClicked(MouseEvent me)
      {
        //                // // System.out.println("in mouse clicked");
        
        final PickCanvas pickCanvas = new PickCanvas(canvas3D, canvas.getUniverse().getLocale());
        pickCanvas.setMode(PickTool.GEOMETRY_INTERSECT_INFO);
        pickCanvas.setTolerance(5.0f);
        pickCanvas.setShapeLocation(me);
        
        PickResult pickResult = pickCanvas.pickClosest();
        
        if (pickResult != null)
        {
          //                    // // System.out.println("in pick result");
          
          PickIntersection pickIntersection = pickResult.getClosestIntersection( pickCanvas.getStartPosition());
          
          if (pickIntersection != null)
          {
            Point3d p3d1 = pickIntersection.getClosestVertexCoordinates();
            Point3d[] coords = pickIntersection.getPrimitiveCoordinates();
            int index = pickIntersection.getClosestVertexIndex();
            int[] arrayIndex = pickIntersection.getPrimitiveCoordinateIndices();
            GeometryArray geometry = pickIntersection.getGeometryArray();
            
            Point3d p3d2 = new Point3d();
            geometry.getCoordinate(index, p3d2);
            
            
            System.out.print("Match between");
            for(int i = 0; i < coords.length; ++i)
            {
              System.out.print(" point " + i + " = " + coords[i]);
            }
            // System.out.println("");
          }
        } else
        {
          // System.out.println("Cannot find line, please try again.");
        }
      }
    });
			*/
  }

  public void alterOneOrManyXAxis(boolean one)
  {
			if(one)
			{
					// settup gridbag;
					// add x Axis stuff;
					/*
					c.gridx = 0;
					c.gridy = graphics.size() + 1;
					c.fill = GridBagConstraints.HORIZONTAL;
					gridbag.setConstraints(controls.getLockButtonPanel(), c);
					add(controls.getLockButtonPanel());
					c.gridwidth = 1;

					c.gridx = 1;
					*/
					notifyTargets("xAxisChange", new Boolean(!one), new Boolean(one));
					int rendWindSize = ((OneGraphic) graphics.lastElement()).getRendWindContainerSize();
					if(rendWindSize != 0)
					{
							xAxis.setMinimumSize(new Dimension (rendWindSize, 70));
					}
					else
					{
							xAxis.setMinimumSize(new Dimension (425, 70));
					}
					c.gridx = 1;
					c.gridy = graphics.size() + 1;
					c.fill = GridBagConstraints.NONE;
					c.anchor = GridBagConstraints.EAST;
					c.insets.right = 5;
					gridbag.setConstraints(xAxis, c);
					add(xAxis);
					c.insets.right = 0;

					c.gridx = 0;
					c.gridy = graphics.size() + 1;
					//					c.fill = GridBagConstraints.HORIZONTAL;
					c.anchor = GridBagConstraints.WEST;
					gridbag.setConstraints(controls.getLockButtonPanel(), c);
					add(controls.getLockButtonPanel());

					validate();
			} else
			{
					remove(xAxis);
					remove(controls.getLockButtonPanel());
					notifyTargets("xAxisChange", new Boolean(!one), new Boolean(one));
					validate();
			}
  }
  
  private void stuffToDoWhenWindowIsDisplayed()
  {
			if(useOneXAxis.getState())
			{
					// System.out.println("alterOneOrManyXAxis in stuffToDoWhenWindowIsDisplayed");
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
			BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
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
				
				graphicsMenu = new JMenu("Graphics");
				graphicsMenu.setMnemonic(KeyEvent.VK_N);
				menuBar.add(graphicsMenu);

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
								((OneGraphic) graphics.elementAt(i)).setMetadata((Metadata) parser.getMetadatas().elementAt(i));
								String mainTitleFrom = ((Metadata) parser.getMetadatas().elementAt(i)).getValue("Graph.mainTitle");
								if(mainTitleFrom != null)
										{
											eachGraphicMenu	 = new JMenu(mainTitleFrom);
										}
								else
										{
												eachGraphicMenu = new JMenu("Graphic no. " + (i + 1));
										}
								subMenu = new JMenu("Merge with");
								for(int j =0; j<parser.getNumberOfGraphics(); ++j)
										{
												if(i != j)
														{
																// System.out.println("parser.getMetadatas().size() = " + parser.getMetadatas().size() + " parser.getNumberOfGraphics() = " + parser.getNumberOfGraphics());
																String mainTitleTo = ((Metadata) parser.getMetadatas().elementAt(j)).getValue("Graph.mainTitle");
																if(mainTitleTo != null)
																		{
																				menuItem = new JMenuItem(mainTitleTo);
																		}
																else
																		{
																				menuItem = new JMenuItem("Graphic no. " 
																																 + (j + 1));
																		}
																menuItem.getAccessibleContext().setAccessibleDescription("Merge " + i + " " + j);
																menuItem.addActionListener(new MenuListener());
																subMenu.add(menuItem);
														}
										}
								eachGraphicMenu.add(subMenu);
								graphicsMenu.add(eachGraphicMenu);
						}

				return;
		}
  
		private JPopupMenu getMenuTypeMakeIfNot(JPopupMenu thisGraphicsMenu, String name)
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

		private JMenu getMenuTypeMakeIfNot(JMenu thisGraphicsMenu, String name)
		{
				Component[] components = thisGraphicsMenu.getMenuComponents();
				JMenuItem item = null;
				Component returnItem = null;
				for(int i = 0; ((i < components.length) && (returnItem == null)); ++i)
						{
								item = (JMenuItem) components[i];
								// System.out.println("item " + i + " = " + item);
								// System.out.println("item.getText() " + item.getText() + " name = " + name);
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

		private void removeThisGraphicFromAllTheseMenus(JMenu graphicsMenu, JMenuItem origin)
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

		private void mergeGraphics(int addTo, int addThis, ActionEvent e)
		{
				// System.out.println("mergeGraphics addTo = " + addTo + " addThis = " + addThis);
				// remove j from panel
				remove((Component) graphics.elementAt(addThis));
				// remove scenegraph from j
				BranchGroup scene = ((OneGraphic) graphics.elementAt(addThis)).detachScene(0);
				// add j's scene graph to i
				((OneGraphic) graphics.elementAt(addTo)).addScene(scene);
				// validate
				validate();


				//				JMenuItem graphicsMenu = getMenuTypeMakeIfNot(menuBar, "Graphics");
				// System.out.println("graphicsMenu = " + graphicsMenu);
				JMenu thisGraphicsMenu = (JMenu) (graphicsMenu.getMenuComponents())[addTo];
				// System.out.println("thisGraphicsMenu = " + thisGraphicsMenu);
				JMenu seperateMenu = getMenuTypeMakeIfNot(thisGraphicsMenu, 
																											 "Seperate");
				JMenu mergeWith = getMenuTypeMakeIfNot(thisGraphicsMenu, 
																							 "Merge with");
				JMenuItem origin = (JMenuItem) e.getSource();
				// mergeWith.remove(origin);
				removeThisGraphicFromAllTheseMenus(graphicsMenu, origin);
				String command = origin.getAccessibleContext().getAccessibleDescription();
				// System.out.println("command 4 = " + command);
				// command = command.replaceFirst("Merge", "Seperate");
				command = "Seperate";
				// System.out.println("command a = " + command);
				origin.getAccessibleContext().setAccessibleDescription(command);
				seperateMenu.add(origin);

		}

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
				if(menuBar.isAncestorOf(graphicsMenu))
				{
						menuBar.remove(graphicsMenu);
				}
  
				xSequenceString = null;
				ySequenceString = null;
				
				/*
					if( != null)
					{
					= null;
					}
					if( != null)
					{
					= null;
					}
				*/
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
  
  protected void notifyTargets(String propertyName, Object oldValue, Object newValue)
  {
			// System.out.println("notifyTargets in GroutPanel = " + propertyName);

    Vector l;
    GroutPanelEvent s = new GroutPanelEvent((Object) this, propertyName, oldValue, newValue);
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
  
/*	private void saveImage(String format)
        {
                BufferedImage mainCanvasImage = ImageWriter.createBufferedImageFromCanvas3D(canvas.getCanvas());
                // // System.out.println("mainCanvasImage = " + mainCanvasImage.getRGB(200, 200) + " | " + mainCanvasImage.getRGB(0, 0));
//		BufferedImage ySequenceImage = ImageWriter.createBufferedImageFromComponent((Component) ySequence);
//		BufferedImage xSequenceImage = ImageWriter.createBufferedImageFromComponent((Component) xSequence);
//		BufferedImage xAxisMarksImage = ImageWriter.createBufferedImageFromComponent((Component) xAxisMarks);
//		BufferedImage yAxisMarksImage = ImageWriter.createBufferedImageFromComponent((Component) yAxisMarks);
 
                BufferedImage renderPanelImage = ImageWriter.createBufferedImageFromComponent((Component) renderPanel);
                ImageWriter.add3DImage(renderPanelImage, mainCanvasImage);
 
                ImageWriter.writeImageToFile(renderPanelImage, new File("image." + format), format);
        } */
  
		public void setOptions(Metadata groutOptions)
		{
				for(Enumeration e = groutOptions.keys() ; e.hasMoreElements() ;)
						{
								String key = (String) e.nextElement();
								String value = groutOptions.getValue(key);
								//								if(key.equals("showOneXAxis") && value.equals("true"))
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
										// System.out.println("Add PickOccuranceFactory value " + value);
										if(value.equals("ZoomTo"))
										{
												// System.out.println("Add PickOccuranceFactory ZoomTo");
												changePickFactory(new ZoomToOccuranceFactory(this));
										}
										if(value.equals("ShowSequence"))
										{
												// System.out.println("Add PickOccuranceFactory ZoomTo");
												changePickFactory(new ShowSequenceOccuranceFactory(this));
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

		private void undoOne()
		{
				// System.out.println("undo " + undoableIndex + " " + undoables.size());
				if(undoableIndex >= 0)
				{
						((UndoableOccurance) undoables.elementAt(undoableIndex)).undo();
						--undoableIndex;
				}
				else
				{
						System.err.println("No undoable events have yet occured");
				}
		}
				
		private void redoOne()
		{
				// System.out.println("redo " + undoableIndex + " " + undoables.size());
				if(!((undoableIndex == (undoables.size() - 1)) 
						 || (undoables.size() == 0)))
				{
						((UndoableOccurance) undoables.elementAt(++undoableIndex)).redo();
				}
				else
				{
						System.err.println("No redoable events have yet occured");
				}
		}

		public Hashtable addSinglePositionMarker(double sequencePosition, 
																					OneGraphic graphic)
		{
				Hashtable markers = new Hashtable();
				if(useOneXAxis.getState())
				{
						for(int i = 0; i < graphics.size(); ++i)
						{
								markers.put(((OneGraphic) graphics.elementAt(i)).addSinglePositionMarker(sequencePosition), ((OneGraphic) graphics.elementAt(i)));
						}
				}
				else
				{
						markers.put(graphic.addSinglePositionMarker(sequencePosition), 
												graphic);
				}
				return markers;
		}

		public Hashtable addYPositionMarker(double sequencePositionY, 
																		 OneGraphic graphic)
		{
				Hashtable markers = new Hashtable();
				markers.put(graphic.addYPositionMarker(sequencePositionY), graphic);

				return markers;
		}

		public void removeLine(Shape3D line, OneGraphic graphic)
		{
				graphic.removeLine(line);
		}

		public void removeLine(Shape3D line)
		{
				((OneGraphic) graphics.elementAt(0)).removeLine(line);
		}

  class MenuListener implements ActionListener
  {
    public void actionPerformed(ActionEvent e)
    {
      // // System.out.println("ActionEvent in Grout Panel");
      JMenuItem source = (JMenuItem)e.getSource();
      String whatChanged = source.getAccessibleContext().getAccessibleDescription();
      // System.out.println("whatChanged = " + whatChanged);
      if(whatChanged.equals("UseOneXAxis"))
      {
        JCheckBoxMenuItem locked = (JCheckBoxMenuItem)e.getSource();
				alterOneOrManyXAxis(locked.getState());
        // System.out.println("UseOneXAxis = " + locked.getState());
        notifyTargets("UseOneXAxis", (Object) new Boolean(!locked.getState()),  (Object) new Boolean(locked.getState()));
      }
      if(whatChanged.equals("This Resets the View"))
      {
        JMenuItem locked = (JMenuItem)e.getSource();
        // // System.out.println("reseet in Grout Window = ");
        notifyTargets("reset", (Object) new Boolean(false),  (Object) new Boolean(true));
      }
      if(whatChanged.equals("lockScaleFactors"))
      {
        JCheckBoxMenuItem locked = (JCheckBoxMenuItem)e.getSource();
        // // System.out.println("scaleFactorsLocked in Grout Window = " + locked.getState());
        notifyTargets("lockScaleFactors", (Object) new Boolean(!locked.getState()),  (Object) new Boolean(locked.getState()));
      }
      if(whatChanged.equals("useButton"))
      {
        // // System.out.println("useButton in Grout Window");
        notifyTargets("useButton", (Object) new Boolean(false),  (Object) new Boolean(true));
      }
      if(whatChanged.equals("useSlider"))
      {
        // // System.out.println("useSlider in Grout Window");
        notifyTargets("useSlider", (Object) new Boolean(false),  (Object) new Boolean(true));
      }
      if(whatChanged.equals("jpeg") || whatChanged.equals("print") || whatChanged.equals("png") || whatChanged.equals("bmp") || whatChanged.equals("tiff") || whatChanged.equals("pnm"))
      {
        saveImage(whatChanged);
      }
      if(whatChanged.equals("RELATIVE_TO_SCREEN"))
      {
        notifyTargets("WindowEyepointPolicy", (Object) new Integer(0),  (Object) new Integer(View.RELATIVE_TO_SCREEN));
      }
      if(whatChanged.equals("RELATIVE_TO_WINDOW"))
      {
        notifyTargets("WindowEyepointPolicy", (Object) new Integer(0),  (Object) new Integer(View.RELATIVE_TO_WINDOW));
      }
      if(whatChanged.equals("RELATIVE_TO_FIELD_OF_VIEW"))
      {
        notifyTargets("WindowEyepointPolicy", (Object) new Integer(0),  (Object) new Integer(View.RELATIVE_TO_FIELD_OF_VIEW));
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
      if(whatChanged.startsWith("FitViewToWindow"))
      {
        notifyTargets("FitViewToWindow", (Object) new Boolean(true),  
											(Object) new Boolean(false));					
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
							notifyTargets(whatChanged,(Object) new Boolean(false), newValue);
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
        String filename = GroutFileChooser.showInputDialog((Component) e.getSource(), 
																													 "Enter file name",
																													 "/people/hmorgan2/code/samples/xml/newXml/pepinfoOutput1.x3d");
					//        String filename = JOptionPane.showInputDialog((Component) e.getSource(), "Enter file name");
				// System.out.println("filename = " + filename);
				if(filename != null)
						{
								clearAllData();
								//								parser.clearAllData();
								parser.setFilename(filename);
								validate();
								ownerWindow.pack();
						
						}
/*			JButton source = (JButton)e.getSource();
            if(source.equals(lockScaleFactors))
            {
                                scaleFactorsLocked = !scaleFactorsLocked;
                                // // System.out.println("scaleFactorsLocked = " + scaleFactorsLocked);
                                notifyTargets("lockScaleFactors", (Object) new Boolean(!scaleFactorsLocked),  (Object) new Boolean(scaleFactorsLocked));
//				LockUnlockScaleFactors(scaleFactorsLocked);
                }
            if(source.equals(reset))
            {
                                notifyTargets("reset", (Object) new Boolean(false),  (Object) new Boolean(true));
                } */
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
				if(type.startsWith("ChangeColour"))
				{
						changePickFactory(new ChangeColourOccuranceFactory(this, 
																															 type.substring(12)));
				}
		}
  
				class ToolbarListener implements ActionListener
				{
						public void actionPerformed(ActionEvent e)
						{
								// // System.out.println("ActionEvent in Grout Window");
								JButton source = (JButton)e.getSource();
								String whatChanged = source.getAccessibleContext().getAccessibleDescription();
								// // System.out.println("whatChanged = " + whatChanged);
								// None of these commands are relavent any longer
								/*
									if(whatChanged.equals("reset"))
									{
									// // System.out.println("reseet in Grout Window = ");
									notifyTargets("reset", (Object) new Boolean(false),  (Object) new Boolean(true));
									}
									if(whatChanged.equals("lockScaleFactors"))
									{
									scaleFactorsLocked = !scaleFactorsLocked;
									// // System.out.println("scaleFactorsLocked in Grout Window = " + scaleFactorsLocked);
									notifyTargets("lockScaleFactors", (Object) new Boolean(!scaleFactorsLocked),  (Object) new Boolean(scaleFactorsLocked));
									}
									if(whatChanged.equals("useButton"))
									{
									// // System.out.println("useButton in Grout Window");
									notifyTargets("useButton", (Object) new Boolean(false),  (Object) new Boolean(true));
									}
									if(whatChanged.equals("useSlider"))
									{
									// // System.out.println("useSlider in Grout Window");
									notifyTargets("useSlider", (Object) new Boolean(false),  (Object) new Boolean(true));
									}
								*/
								/*			JButton source = (JButton)e.getSource();
												if(source.equals(lockScaleFactors))
												{
												scaleFactorsLocked = !scaleFactorsLocked;
												// // System.out.println("scaleFactorsLocked = " + scaleFactorsLocked);
												notifyTargets("lockScaleFactors", (Object) new Boolean(!scaleFactorsLocked),  (Object) new Boolean(scaleFactorsLocked));
												//				LockUnlockScaleFactors(scaleFactorsLocked);
												}
												if(source.equals(reset))
												{
												notifyTargets("reset", (Object) new Boolean(false),  (Object) new Boolean(true));
												} */
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
								//				System.out.println("componentShown by " + e.getSource() + " to " + this);
								//				((JFrame)e.getSource()).notifyAll();
								//				Runtime.getRuntime().gc();
								//				for(int i = 0; i < 10000; ++i) { ++i; }
								stuffToDoWhenWindowIsDisplayed();
						}

				public void componentResized(ComponentEvent e)
						{
								int rendWindSize = ((OneGraphic) graphics.lastElement()).getRendWindContainerSize();
								// System.out.println("componentResized by " + e.getSource() + " to " + rendWindSize);
								xAxis.setMinimumSize(new Dimension (rendWindSize, 70));
								xAxis.revalidate();
								notifyTargets("FitViewToWindow", (Object) new Boolean(true),  
															(Object) new Boolean(false));					
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
  
		}
