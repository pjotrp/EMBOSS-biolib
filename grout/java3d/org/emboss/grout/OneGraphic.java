/*
 * OneGraphic.java
 *
 * Created on Feb 3 2003
 */

package org.emboss.grout;

import java.awt.GridBagLayout;
import java.util.Vector;
import java.util.Enumeration;
import java.awt.image.BufferedImage;
import javax.swing.JLabel;
import javax.swing.SwingConstants;
//import ;
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
 * A Panel to display One Graphic
 *
 * @author  hmorgan2
 */
public class OneGraphic extends javax.swing.JPanel implements java.io.Serializable, ParserListener, GroutPanelListener
{
		private boolean offScreen = false;
		private int graphicNumber = 0;

		private RendWindContainer canvas;
    private JPanel xAxis;
    private AxisMarks xAxisMarks;
    // private SequenceView xSequence;
    private AxisMarks yAxisMarks;
    // private SequenceView ySequence;
    private JLabel mainTitle;
    private JLabel xTitle;
    private LabelWithVerticalText yTitle;
		//    private JLabel yTitle;
		//    private JEditorPane yTitle;

		private Font mainTitleFont;
		private Font xTitleFont;
		private Font yTitleFont;

		private ViewPositionModel viewPositionModel;
		private ControlContainer controls;

		private Metadata metadata = null;
	 
		private GridBagLayout gridbag = null;
		private GridBagConstraints c = null;
 
    /**
     * Standard NULL constructor
     *
     */
		public OneGraphic()
		{
				super();
				doSetup();
				return;
		}

		private RendWindContainer getCanvas()
		{
				return canvas;
		}

		public String getXTitle()
		{
				// System.out.println("xTitle = " + xTitle);
				return xTitle.getText();
		}

		public String getYTitle()
		{
				// System.out.println("yTitle = " + yTitle);
				return yTitle.getText();
		}

		private void doSetup()
		{
			  gridbag = new GridBagLayout();
			  c = new GridBagConstraints();

				setLayout(gridbag);

				viewPositionModel = new ViewPositionModel();
				controls = new ControlContainer();

				canvas = new RendWindContainer(offScreen);
				canvas.setGraphicNumber(graphicNumber);
				xAxis = new JPanel(new BorderLayout());
				xAxisMarks = new AxisMarks(offScreen);
				// xSequence = new SequenceView();
				yAxisMarks = new AxisMarks(offScreen);
				// ySequence = new SequenceView();

				//				setBackground(Color.black);
				setBackground(Color.white);

				c.gridx = 0;
				c.gridy = 0;
				c.weightx = 0.0;
				c.weighty = 0.0;
				c.gridheight = 5;
				c.fill = GridBagConstraints.VERTICAL;
				gridbag.setConstraints(controls.getCentrePanel(), c);
				add(controls.getCentrePanel());
				c.gridheight = 1;
				
				c.gridx = 2;
				c.gridy = 5;
				c.gridwidth = 3;
				c.gridheight = 2;
				c.fill = GridBagConstraints.HORIZONTAL;
				//				c.insets.right = 5;
				gridbag.setConstraints(controls.getBottomPanel(), c);
				add(controls.getBottomPanel());
				//				c.insets.right = 0;

				c.gridx = 0;
				c.gridy = 5;
				c.gridwidth = 2;
				c.gridheight = 2;
				c.fill = GridBagConstraints.BOTH;
				gridbag.setConstraints(controls.getLockButtonPanel(), c);
				add(controls.getLockButtonPanel());
				c.gridwidth = 1;
				c.gridheight = 1;

				c.gridx = 4;
				c.gridy = 1;
				c.weightx = 1.0;
				c.weighty = 1.0;
				c.fill = GridBagConstraints.BOTH;
				c.insets.right = 5;
				gridbag.setConstraints(canvas, c);
				add(canvas);
				c.weightx = 0.0;
				c.weighty = 0.0;

				c.gridx = 4;
				c.gridy = 2;
				c.fill = GridBagConstraints.HORIZONTAL;
				gridbag.setConstraints(xAxisMarks, c);
				add(xAxisMarks);
				c.insets.right = 0;
				// xSequence.setSequenceFile("resources/sequence.txt");
				// c.gridx = 4;
				// c.gridy = 3;
				// c.fill = GridBagConstraints.HORIZONTAL;
				// gridbag.setConstraints(xSequence, c);
				// add(xSequence);
				

				yAxisMarks.setOrientation(AxisMarks.VERTICAL);
				// ySequence.setOrientation(SequenceView.VERTICAL);;
				// ySequence.setSequenceFile("resources/sequence.txt");
				// c.gridx = 2;
				// c.gridy = 1;
				// c.fill = GridBagConstraints.VERTICAL;
				// gridbag.setConstraints(ySequence, c);
				// add(ySequence);
				c.gridx = 3;
				c.gridy = 1;
				c.fill = GridBagConstraints.VERTICAL;
				gridbag.setConstraints(yAxisMarks, c);
				add(yAxisMarks);

				//		addMainTitle("Title");

				setInternalListeners();

				return;
		}

		public void addMainTitle(String title)
		{
				// System.out.println("addMainTitle = " + title);

			  mainTitle = new JLabel(title);
				mainTitle.setPreferredSize(new Dimension(400, 30));
				//				mainTitle.setForeground(Color.WHITE);
				mainTitle.setForeground(Color.black);
				mainTitleFont = new Font("TimesRoman",Font.PLAIN,18);
				mainTitle.setFont(mainTitleFont);
				mainTitle.setHorizontalAlignment(SwingConstants.CENTER);
				//				mainTitle.add(text);
				c.gridx = 4;
				c.gridy = 0;
				c.fill = GridBagConstraints.HORIZONTAL;
				gridbag.setConstraints(mainTitle, c);
				if(!isAncestorOf(mainTitle))
						{
								add(mainTitle);
						}
		}

		public void addXTitle(String title)
		{
			  xTitle = new JLabel(title);
				xTitle.setPreferredSize(new Dimension(400, 20));
				//				xTitle.setForeground(Color.WHITE);
				xTitle.setForeground(Color.black);
				xTitleFont = new Font("TimesRoman",Font.PLAIN,12);
				xTitle.setFont(xTitleFont);
				xTitle.setHorizontalAlignment(SwingConstants.CENTER);
				c.gridx = 4;
				c.gridy = 4;
				c.fill = GridBagConstraints.HORIZONTAL;
				gridbag.setConstraints(xTitle, c);
				if(!isAncestorOf(xTitle))
						{
								add(xTitle);
						}
		}

		public void addYTitle(String title)
		{
				String verticalTitle = new String();
//				for(int i = 0; i<title.length(); ++i)
//						{
//								verticalTitle = verticalTitle.concat
//										(title.substring(i, i+1));
//								verticalTitle = verticalTitle.concat("\n");
//						}
//				// System.out.println("verticalTitle = |" + verticalTitle + "|");
			  yTitle = new LabelWithVerticalText(title);
				//			  yTitle = new JLabel(verticalTitle);
				//			  yTitle = new JEditorPane();
				//			  yTitle.setText(title);
				yTitle.setPreferredSize(new Dimension(15, 400));
				// I do not know why this line is needed, but else you lose the axis
				yTitle.setMinimumSize(new Dimension(15, 400));
				//				yTitle.setForeground(Color.WHITE);
				yTitle.setBackground(Color.white);
				yTitle.setForeground(Color.black);
				yTitleFont = new Font("TimesRoman",Font.PLAIN,12);
				yTitle.setFont(yTitleFont);
				c.gridx = 1;
				c.gridy = 1;
				c.fill = GridBagConstraints.VERTICAL;
				gridbag.setConstraints(yTitle, c);
				if(!isAncestorOf(yTitle))
						{
								add(yTitle);
						}
				repaint();
		}


    /**
     *  Constructor for offscreen instance
     *
     * @param os True for offscreen instance
     */
		public OneGraphic(boolean os)
		{
				super();
				offScreen = os;
				doSetup();
				return;
		}

    /**
     *  Sets the graphic number that this displays
     *
     * @param no the graphic number
     */
		public void setGraphicNumber(int no)
		{
				graphicNumber = no;
				canvas.setGraphicNumber(graphicNumber);
				viewPositionModel.setGraphicNumber(graphicNumber);
				return;
		}

    /**
     *  Gets the graphic number that this displays
     *
     * @return int the graphic number
     */
		public int getGraphicNumber()
		{
				return graphicNumber;
		}

		public RendWindContainer getRendWindContainer()
		{
				return canvas;
		}

    /**
     *  Gets the size of the rendering window
     *
     * @return int the size of the rendering window
     */
		public int getRendWindContainerSize()
		{
				// System.out.println("getWidth = " + canvas.getCanvas().getWidth());
				return canvas.getCanvas().getWidth();
		}

		public ViewPositionModel getViewPositionModel()
		{
				return viewPositionModel;
		}

    /**
     *  Sets up the internal Listeners
     *
     */
		private void setInternalListeners()
		{
				viewPositionModel.addViewPositionModelListener(xAxisMarks);
				viewPositionModel.addViewPositionModelListener(yAxisMarks);
				viewPositionModel.addViewPositionModelListener(canvas);
				viewPositionModel.addViewPositionModelListener(controls);

				controls.addControlContainerListener(viewPositionModel);

				canvas.addRendWindContainerListener(viewPositionModel);

				//		viewPositionModel.addViewPositionModelListener(xSequence);
				//		viewPositionModel.addViewPositionModelListener(ySequence);

				return;
		}

    /**
     *  Sets up the parser Listeners
     *
     * @param parser the parser
     */
		public void setParserListeners(Parser parser)
		{
				parser.addParserListener(canvas);
				parser.addParserListener(viewPositionModel);
				parser.addParserListener(this);

				return;
		}

    /**
     *  Sets up the GroutPanel Listeners
     *
     * @param GroutPanel the GroutPanel
     */
		public void setGroutPanelListeners(GroutPanel panel)
		{
				panel.addGroutPanelListener(canvas);
				panel.addGroutPanelListener(xAxisMarks);
				panel.addGroutPanelListener(yAxisMarks);

				return;
		}

    /**
     *  Sets up the ControlContainer Listeners
     *
     * @param controlContainer the ControlContainer
     */
		public void setControlListeners(ControlContainer controlContainer)
		{
				controlContainer.addControlContainerListener(viewPositionModel);
				viewPositionModel.addViewPositionModelListener(controlContainer);

				return;
		}

		public void	setMetadata(Metadata md)
		{
				// System.out.println("setMetadata addMainTitle");
				metadata = md;
				setTitles();
		}

		public void	setTitles()
		{
				// System.out.println("setTitles addMainTitle");
				for (Enumeration e = metadata.keys() ; e.hasMoreElements() ;)
						{
								String key = (String) e.nextElement();
								if(key.equals("Graph.mainTitle"))
										{
												addMainTitle(metadata.getValue(key));
										}
								if(key.equals("Graph.xTitle"))
										{
												addXTitle(metadata.getValue(key));
										}
								if(key.equals("Graph.yTitle"))
										{
												addYTitle(metadata.getValue(key));
										}
								// add rest of metadata hugh
						}
		}

		private void removeXAxis()
		{
				// System.out.println("removeXAxis");
				remove(xAxisMarks);
				// remove(xSequence);
				remove(controls.getLockButtonPanel());
				remove(controls.getBottomPanel());
				remove(xTitle);
				validate();
		}

		private void listenToThisControl(ControlContainer controls)
		{
				//				viewPositionModel.addViewPositionModelListener(controls);
				controls.addControlContainerListener(viewPositionModel);
		}

		private void addXAxis()
		{
				c.gridx = 3;
				c.gridy = 5;
				c.fill = GridBagConstraints.HORIZONTAL;
				gridbag.setConstraints(controls.getBottomPanel(), c);
				add(controls.getBottomPanel());

				c.gridx = 0;
				c.gridy = 5;
				c.gridwidth = 3;
				c.gridheight = 2;
				c.fill = GridBagConstraints.HORIZONTAL;
				gridbag.setConstraints(controls.getLockButtonPanel(), c);
				add(controls.getLockButtonPanel());
				c.gridwidth = 1;
				c.gridheight = 1;

				c.gridx = 3;
				c.gridy = 2;
				c.fill = GridBagConstraints.HORIZONTAL;
				gridbag.setConstraints(xAxis, c);
				add(xAxis);

				c.gridx = 3;
				c.gridy = 3;
				c.fill = GridBagConstraints.HORIZONTAL;
				gridbag.setConstraints(xTitle, c);
				add(xTitle);

				validate();
		}

		private void listenToOwnControl()
		{
				//				viewPositionModel.removeViewPositionModelListener(controls);
				controls.removeControlContainerListener(viewPositionModel);
		}

		public BranchGroup detachScene(int sceneNumber)
		{
				return canvas.detachScene();
		}

  public void addScene(BranchGroup sc)
		{
				canvas.addScene(sc);
		}

  public void attachPickBehavior(MouseListener lis)
  {
			// System.out.println("attachPickBehavior = " + lis);
			canvas.getCanvas().addMouseListener(lis);
	}

  public void removePickBehavior(MouseListener lis)
  {
			canvas.getCanvas().removeMouseListener(lis);
	}

  public void attachPickBehavior(final Canvas3D canvas3D)
  {
			
        // System.out.println("attachPickBehavior");
    canvas3D.addMouseListener(new MouseAdapter()
    {
      public void mouseClicked(MouseEvent me)
      {
        // System.out.println("in mouse clicked");
        
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
            
            
            // System.out.print("Match between");
            for(int i = 0; i < coords.length; ++i)
            {
              System.err.print(" point " + i + " = " + coords[i]);
            }
            System.err.println("");
          }
        } else
        {
          System.err.println("Cannot find line, please try again.");
        }
      }
    });
  }

		public BufferedImage getImage_old()
		{
				BufferedImage mainCanvasImage = null;
				BufferedImage xAxisMarksImage = null;
				BufferedImage yAxisMarksImage = null;
				BufferedImage xAxisTitleImage = null;
				// BufferedImage xSequenceImage = null;
				BufferedImage yAxisTitleImage = null;
				// BufferedImage ySequenceImage = null;
				BufferedImage mainTitleImage = null;

				mainCanvasImage = ImageWriter.createBufferedImageFromCanvas3D(canvas.getCanvas());
				yAxisMarksImage = ImageWriter.createBufferedImageFromComponent((Component) yAxisMarks);
				// ySequenceImage = ImageWriter.createBufferedImageFromComponent((Component) ySequence);
				yAxisTitleImage = ImageWriter.createBufferedImageFromComponent((Component) yTitle);
				mainTitleImage = ImageWriter.createBufferedImageFromComponent((Component) mainTitle);
				//				 = ImageWriter.createBufferedImageFromComponent((Component) );
				
				BufferedImage wholeImage;

				if(yTitle.getHeight() != 0 && yTitle.getWidth() != 0)
				{
						yAxisTitleImage = ImageWriter.createBufferedImageFromComponent((Component) yTitle);

						if(this.isAncestorOf(xAxisMarks))
						{
								xAxisMarksImage = ImageWriter.createBufferedImageFromComponent((Component) xAxisMarks);
								// xSequenceImage = ImageWriter.createBufferedImageFromComponent((Component) xSequence);
								xAxisTitleImage = ImageWriter.createBufferedImageFromComponent((Component) xTitle);
								BufferedImage[] images =
										{ mainTitleImage, yAxisMarksImage, yAxisTitleImage, mainCanvasImage, xAxisMarksImage, xAxisTitleImage };
								int[] xCoords =	{ 2, 0, 1, 2, 2, 2, 2 };
								int[] yCoords =	{ 0, 1, 1, 1, 2, 3, 4 };
								wholeImage = ImageWriter.joinImages(images, xCoords, yCoords);
						}
						else
						{
								BufferedImage[] images =
										{ yAxisMarksImage, mainCanvasImage };
								int[] xCoords =	{ 0, 1 };
								int[] yCoords =	{ 0, 0 };
								wholeImage = ImageWriter.joinImages(images, xCoords, yCoords);
						}
				}
				else
				{
						if(this.isAncestorOf(xAxisMarks))
						{
								xAxisMarksImage = ImageWriter.createBufferedImageFromComponent((Component) xAxisMarks);
								// xSequenceImage = ImageWriter.createBufferedImageFromComponent((Component) xSequence);
								xAxisTitleImage = ImageWriter.createBufferedImageFromComponent((Component) xTitle);
								BufferedImage[] images =
										{ mainTitleImage, yAxisMarksImage, mainCanvasImage, 
											xAxisMarksImage, xAxisTitleImage };
								int[] xCoords =	{ 2, 0, 2, 2, 2, 2 };
								int[] yCoords =	{ 0, 1, 1, 2, 3, 4 };
								wholeImage = ImageWriter.joinImages(images, xCoords, yCoords);
						}
						else
						{
								BufferedImage[] images =
										{ yAxisMarksImage, mainCanvasImage };
								int[] xCoords =	{ 0, 1 };
								int[] yCoords =	{ 0, 0 };
								wholeImage = ImageWriter.joinImages(images, xCoords, yCoords);
						}
				}
				return wholeImage;
		}

		public BufferedImage getImage_old2()
		{
				ImageJoiner joiner = new ImageJoiner();
				// arguments are component, parent, x, y, xWeight, yWeight, width, height, anchor, fill
				joiner.addComponent(mainTitle, this, 0, 0, 1, 1, GridBagConstraints.REMAINDER, 1, 
														GridBagConstraints.CENTER, GridBagConstraints.NONE);
				joiner.addComponent(yTitle, this, 0, 1, 1, 1, 1, 1, 
														GridBagConstraints.CENTER, GridBagConstraints.NONE);
				joiner.addComponent(yAxisMarks, this, 1, 1, 1, 1, 1, 1, 
														GridBagConstraints.CENTER, GridBagConstraints.NONE);
				joiner.addCanvas3D(canvas.getCanvas(), this, 2, 1, 1, 1, 1, 1, 
														GridBagConstraints.CENTER, GridBagConstraints.NONE);
				joiner.addComponent(xAxisMarks, this, 2, 2, 1, 1, 1, 1, 
														GridBagConstraints.CENTER, GridBagConstraints.NONE);
				joiner.addComponent(xTitle, this, 2, 3, 1, 1, 1, 1, 
														GridBagConstraints.CENTER, GridBagConstraints.NONE);
				return joiner.joinImage();
		}

		public BufferedImage getImage()
		{
				BufferedImage image = ImageWriter.createBufferedImageFromComponent(this);
				BufferedImage canvasImage = ImageWriter.createBufferedImageFromCanvas3D(canvas.getCanvas());
				Graphics2D graphics = (Graphics2D) image.getGraphics();
				graphics.drawImage(canvasImage, null, canvas.getLocation().x, canvas.getLocation().y);

				return image;
		}

		public Shape3D addSinglePositionMarker(double sequencePosition)
		{
				double maxY = viewPositionModel.getMaxY();
				double minY = viewPositionModel.getMinY();

				// canvas.addLine(sequencePosition, minY, sequencePosition, maxY);
				
				return canvas.addLine(sequencePosition, minY, sequencePosition, maxY);
		}

		public Shape3D addYPositionMarker(double sequencePosition)
		{
				double maxX = viewPositionModel.getMaxX();
				double minX = viewPositionModel.getMinX();

				// canvas.addLine(sequencePosition, minY, sequencePosition, maxY);
				
				return canvas.addLine(minX, sequencePosition, maxX, sequencePosition);
		}

		public void removeLine(Shape3D line)
		{
				canvas.removeLine(line);
		}

		public void parserChanged(ParserEvent e)
		{
				//    setMetadata((Metadata) ((Parser) e.getSource()).getMetadatas().elementAt(graphicNumber));
				// System.out.println("Parser changed in oneGraphic setSceneGroup to no " + graphicNumber);
		}
  
		public void groutPanelChanged(GroutPanelEvent e)
		{
				// System.out.println("groutPanelChanged in oneGraphic by " +e.getPropertyName() );
				// System.out.println("y title is child? = " + this.isAncestorOf(yTitle));
				// System.out.println("y title = " + (yTitle));
				//				System.out.println("hieght = " + yTitle.getHeight() + " width = " + yTitle.getWidth() + " pos = " + yTitle.getSize() );
				GroutPanel source = (GroutPanel) e.getSource();
				if(e.getPropertyName().equals("xAxisChange"))
				{
						if(((Boolean) e.getNewValue()).booleanValue())
						{
								removeXAxis();
								listenToThisControl(source.getControls());
						} else
						{
								addXAxis();
								listenToOwnControl();
						}
				}
				if(e.getPropertyName().startsWith("Font"))
				{
						String whatFont = e.getPropertyName();
						if(whatFont.substring(4).startsWith("XTitle"))
						{
								if(whatFont.substring(10).startsWith("Size"))
								{
										int size = yTitleFont.getSize();
										try
												{
														String sizeString = whatFont.substring(14);
														size = Integer.parseInt(sizeString);
														// System.out.println("size = " + size);
												}
										catch(NumberFormatException exc)
												{
														// this really should never happen
														exc.printStackTrace();
												}
										xTitleFont = new Font(xTitleFont.getName(), 
																					xTitleFont.getStyle(), size);
										xTitle.setFont(xTitleFont);
								}
								if(whatFont.substring(10).startsWith("Colour"))
								{
										if(whatFont.substring(16).startsWith("Pink"))
										{
												xTitle.setForeground(Color.pink);
										}
										if(whatFont.substring(16).startsWith("Red"))
										{
												xTitle.setForeground(Color.red);
										}
										if(whatFont.substring(16).startsWith("White"))
										{
												xTitle.setForeground(Color.white);
										}
										if(whatFont.substring(16).startsWith("Black"))
										{
												xTitle.setForeground(Color.black);
										}
								}
								if(whatFont.substring(10).startsWith("SetText"))
								{
										String newText = (String)JOptionPane.showInputDialog(
                             this,
                             "Enter text for X Title",
                             "Customized Dialog",
                             JOptionPane.PLAIN_MESSAGE,
                             null,
                             null,
                             xTitle.getText());
										xTitle.setText(newText);
								}
						} else if(whatFont.substring(4).startsWith("YTitle"))
						{
								if(whatFont.substring(10).startsWith("Size"))
								{
										int size = yTitleFont.getSize();
										try
												{
														String sizeString = whatFont.substring(14);
														size = Integer.parseInt(sizeString);
														// System.out.println("size = " + size);
												}
										catch(NumberFormatException exc)
												{
														// this really should never happen
														exc.printStackTrace();
												}
										yTitleFont = new Font(yTitleFont.getName(), 
																						 yTitleFont.getStyle(), size);
										yTitle.setFont(yTitleFont);
								}
								if(whatFont.substring(10).startsWith("Colour"))
								{
										if(whatFont.substring(16).startsWith("Pink"))
										{
												yTitle.setForeground(Color.pink);
										}
										if(whatFont.substring(16).startsWith("Red"))
										{
												yTitle.setForeground(Color.red);
										}
										if(whatFont.substring(16).startsWith("White"))
										{
												yTitle.setForeground(Color.white);
										}
										if(whatFont.substring(16).startsWith("Black"))
										{
												yTitle.setForeground(Color.black);
										}
								}
								if(whatFont.substring(10).startsWith("SetText"))
								{
										String newText = (String)JOptionPane.showInputDialog(
                             this,
                             "Enter text for Y Title",
                             "Customized Dialog",
                             JOptionPane.PLAIN_MESSAGE,
                             null,
                             null,
                             yTitle.getText());
										yTitle.setText(newText);
								}
						} else if(whatFont.substring(4).startsWith("MainTitle"))
						{
								if(whatFont.substring(13).startsWith("Size"))
								{
										int size = mainTitleFont.getSize();
										try
												{
														String sizeString = whatFont.substring(17);
														size = Integer.parseInt(sizeString);
												}
										catch(NumberFormatException exc)
												{
														// this really should never happen
														exc.printStackTrace();
												}
										mainTitleFont = new Font(mainTitleFont.getName(), 
																						 mainTitleFont.getStyle(), size);
										mainTitle.setFont(mainTitleFont);
								}
								if(whatFont.substring(13).startsWith("Colour"))
								{
										if(whatFont.substring(19).startsWith("Pink"))
										{
												mainTitle.setForeground(Color.pink);
										}
										if(whatFont.substring(19).startsWith("Red"))
										{
												mainTitle.setForeground(Color.red);
										}
										if(whatFont.substring(19).startsWith("White"))
										{
												mainTitle.setForeground(Color.white);
										}
										if(whatFont.substring(19).startsWith("Black"))
										{
												mainTitle.setForeground(Color.black);
										}
								}
								if(whatFont.substring(13).startsWith("SetText"))
								{
										String newText = (String)JOptionPane.showInputDialog(
                             this,
                             "Enter text for Main Title",
                             "Customized Dialog",
                             JOptionPane.PLAIN_MESSAGE,
                             null,
                             null,
                             mainTitle.getText());
										mainTitle.setText(newText);
								}
						}
				}
		}
  

}

class ImageJoiner
{
		Vector images = new Vector();
		Vector xCoords = new Vector();
		Vector yCoords = new Vector();
		Vector xWeights = new Vector();
		Vector yWeights = new Vector();
		Vector widths = new Vector();
		Vector heights = new Vector();
		Vector anchors = new Vector();
		Vector fills = new Vector();

		public void addComponent(Component component, JPanel parent, int xCoord, 
												int yCoord, double xWeight, double yWeight,
												int width, int height, int anchor, int fill)
		{
				if((parent.isAncestorOf(component))
					 && (component.getHeight() != 0 && component.getWidth() != 0))
				{
						images.addElement
								(ImageWriter.createBufferedImageFromComponent((Component) component));
						xCoords.addElement(new Integer(xCoord));
						yCoords.addElement(new Integer(yCoord));
						xWeights.addElement(new Double(xWeight));
						yWeights.addElement(new Double(yWeight));
						widths.addElement(new Integer(width));
						heights.addElement(new Integer(height));
						anchors.addElement(new Integer(anchor));
						fills.addElement(new Integer(fill));
				}
		}

		public void addCanvas3D(Canvas3D component, JPanel parent, int xCoord, 
														int yCoord, double xWeight, double yWeight,
														int width, int height, int anchor, int fill)
		{
				if((parent.isAncestorOf(component))
					 && (component.getHeight() != 0 && component.getWidth() != 0))
				{
						images.addElement
								(ImageWriter.createBufferedImageFromCanvas3D(component));
						xCoords.addElement(new Integer(xCoord));
						yCoords.addElement(new Integer(yCoord));
						xWeights.addElement(new Double(xWeight));
						yWeights.addElement(new Double(yWeight));
						widths.addElement(new Integer(width));
						heights.addElement(new Integer(height));
						anchors.addElement(new Integer(anchor));
						fills.addElement(new Integer(fill));
				}
		}

		public BufferedImage joinImage()
		{
				BufferedImage[] imageArray = vectorToImageArray(images);
				int[] xCoordsArray = vectorToIntArray(xCoords);
				int[] yCoordsArray = vectorToIntArray(yCoords);
				double[] xWeightsArray = vectorToDoubleArray(xWeights);
				double[] yWeightsArray = vectorToDoubleArray(yWeights);
				int[] widthsArray = vectorToIntArray(widths);
				int[] heightsArray = vectorToIntArray(heights);
				int[] anchorsArray = vectorToIntArray(anchors);
				int[] fillsArray = vectorToIntArray(fills);
				
				// System.out.println("BufferedImage joinImage");

				return ImageWriter.joinImagesGirdBag(imageArray, xCoordsArray, 
																						 yCoordsArray, xWeightsArray,
																						 yWeightsArray, widthsArray, 
																						 heightsArray, anchorsArray, 
																						 fillsArray, null, null, null);
		}

		private int[] vectorToIntArray(Vector vector)
		{
				int[] returnArray = new int[vector.size()];
				for(int i = 0; i < vector.size(); ++i)
				{
						returnArray[i] = ((Integer) vector.elementAt(i)).intValue();
				}
				return returnArray;
		}

		private double[] vectorToDoubleArray(Vector vector)
		{
				double[] returnArray = new double[vector.size()];
				for(int i = 0; i < vector.size(); ++i)
				{
						returnArray[i] = ((Double) vector.elementAt(i)).doubleValue();
				}
				return returnArray;
		}

		private BufferedImage[] vectorToImageArray(Vector vector)
		{
				BufferedImage[] returnArray = new BufferedImage[vector.size()];
				for(int i = 0; i < vector.size(); ++i)
				{
						returnArray[i] = (BufferedImage) vector.elementAt(i);
				}
				return returnArray;
		}

}
