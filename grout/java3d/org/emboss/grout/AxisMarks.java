/*
 * AxisMarks.java
 *
 * Created on June 25, 2002, 11:25 AM
 */

package org.emboss.grout;

import java.awt.*;
import java.awt.event.*;
import java.applet.*;
import javax.swing.*;
import java.util.*;
import java.awt.image.BufferedImage;

/**
 *
 * @author  hmorgan2
 */
//public class AxisMarks extends Canvas implements MouseListener, ComponentListener, ControlContainerListener
public class AxisMarks extends Canvas implements MouseListener, 
		ViewPositionModelListener, ComponentListener, GroutPanelListener
{
  
  public static final int HORIZONTAL = 0;
  public static final int VERTICAL = 1;
  //  private static int HORIZONTAL = 0;
  
  private String label;
  
  private double max;
  
  private double min;
  
  private int size;
  private int orientation = HORIZONTAL;
  
  private double scale, offset, zPos, xScale, yScale, originalMax, originalMin;
  
  private boolean offScreen;
  
  BufferedImage imageToDraw = null;
  
  Font exFont = new Font("TimesRoman",Font.PLAIN,10);

	private Color foregroundColour = Color.black;

		private String intervalPolicy = new String("normal");
		private double interval = 0.0;

  /** Creates a new instance of AxisMarks */
  public AxisMarks()
  {
    //        this.AxisMarks(100); why does this not work?
    super();
    setVisible(true);
    setBackground(Color.white);
		//    setBackground(Color.black);
    //		setForeground(Color.white);
    scale = zPos = xScale = yScale = 1;
    offset = 0;
		//    addMouseListener(this);
    addComponentListener(this);
    //        orientation = HORIZONTAL;
    if(orientation == HORIZONTAL)
    {
      size = (int) getSize().getWidth();
      // // System.out.println("size x = " + size);
      
    } else if(orientation == VERTICAL)
    {
      size = (int) getSize().getHeight();
      // // System.out.println("size y = " + size);
    }
  }
  
  /** Creates a new instance of AxisMarks */
  public AxisMarks(boolean offScreen)
  {
    //        this.AxisMarks(100); why does this not work?
    super();
    setVisible(true);
    setBackground(Color.white);
		//    setBackground(Color.black);
    //		setForeground(Color.white);
    scale = zPos = xScale = yScale = 1;
    offset = 0;
		//    addMouseListener(this);
    addComponentListener(this);
    //        orientation = HORIZONTAL;
    // // System.out.println("Making image offScreen = " + offScreen);
    if(offScreen)
    {
      this.offScreen = offScreen;
      size = 400;
      // // System.out.println("Making image offscreen orientation = " + orientation);
      if(orientation == HORIZONTAL)
      {
        imageToDraw = new BufferedImage(400, 10, BufferedImage.TYPE_INT_ARGB);
        // // System.out.println("imageToDraw HORIZONTAL = " + imageToDraw);
      } else if(orientation == VERTICAL)
      {
        imageToDraw = new BufferedImage(25, 400, BufferedImage.TYPE_INT_ARGB);
        // // System.out.println("imageToDraw VERTICAL = " + imageToDraw);
      }
      
    } else
    {
      if(orientation == HORIZONTAL)
      {
        size = (int) getSize().getWidth();
      } else if(orientation == VERTICAL)
      {
        size = (int) getSize().getHeight();
      }
    }
    // // System.out.println("imageToDraw = " + imageToDraw);
  }
  
  public Image createImage(int width, int height)
  {
    // // System.out.println("createing Image%^ offScreen = " + offScreen);
    
    if(offScreen)
    {
      // // System.out.println("imageToDraw%^ = " + imageToDraw);
      return (Image) imageToDraw;
    } else
    {
      return super.createImage(width, height);
    }
  }
  
/*    public AxisMarks(int sz)
    {
        setSize(sz, 30);
        //        size = sz;
        //        size = sz;
        setVisible(true);
        scale = 1;
        addMouseListener(this);
        addComponentListener(this);
    } */
  
  public void setMaxMin(int mx, int mn)
  {
    originalMax = max = mx;
    originalMin = min = mn;
  }
  
  public void setMin(int mn)
  {
    originalMin = min = mn;
  }
  public int getMin()
  {
    return (int) min;
  }
  public void setMax(int mx)
  {
    originalMax = max = mx;
  }
  public int getMax()
  {
    return (int) max;
  }
  
  public void setSize(int x, int y)
  {
    //        size = y;
    super.setSize(x, y);
  }
  
  public void setOrientation(int o)
  {
    orientation = o;
    // // System.out.println("orientation = " + orientation);
    // // System.out.println("imageToDraw = " + imageToDraw);
    if(orientation == HORIZONTAL)
    {
      imageToDraw = new BufferedImage(400, 10, BufferedImage.TYPE_INT_ARGB);
    } else if(orientation == VERTICAL)
    {
      imageToDraw = new BufferedImage(25, 400, BufferedImage.TYPE_INT_ARGB);
    }
    // // System.out.println("imageToDraw = " + imageToDraw);
  }
  
  public void paint(Graphics g)
  {
    drawAxis(g);
  }
  
  private void drawAxis(Graphics g)
  {
    //        // // System.out.println("size??? = " +  size);
    Graphics2D g2d = (Graphics2D)g;
    //       Font exFont = new Font("TimesRoman",Font.PLAIN,10);
    g2d.setFont(exFont);
    // g2d.setColor(Color.black);
    g2d.setColor(foregroundColour);
		int noOfTickMarks;
		int extent;
		{
		Dimension size = getSize();
		if(orientation == HORIZONTAL)
		{
				extent = size.width;
		}
		else
		{
				extent = size.height;
		}
		if(extent > 200)
		{
				noOfTickMarks = 10;
		}
		else if(extent > 50)
		{
				noOfTickMarks = 4;
		}
		else
		{
				noOfTickMarks = 2;
		}
		}
		//    g2d.setColor(Color.white);

		// drawTickMarks(g);
						for(int i = 0; i < noOfTickMarks; ++i)
						{
								//            g2d.drawString(String.valueOf(tickNo(i)),((float) ((size / (10 / scale) ) * (i + 3)) + 1),8.0f);
								//            float mainPos = (float) ((size / (10 / scale) ) * (i ));
								if(orientation == HORIZONTAL)
								{
										float mainPos = (float) ((size / (noOfTickMarks / scale) ) * (i ));
										g2d.drawString(String.valueOf(tickNo(i)), mainPos, 10.0f);
										g2d.drawLine(((int) mainPos), 0, ((int) mainPos), 1);
								} else if(orientation == VERTICAL)
								{
										float mainPos = (float) ((size / (noOfTickMarks / scale) ) * (i ));
										mainPos = size - mainPos;
										g2d.drawString(String.valueOf(tickNo(i)), 2.0f, mainPos);
										g2d.drawLine(24, ((int) mainPos), 23, ((int) mainPos));
								}
						}
    g2d.setColor(Color.black);
      if(orientation == HORIZONTAL)
      {
        g2d.drawLine(0, 0, extent, 0);
      } else if(orientation == VERTICAL)
      {
        g2d.drawLine(24, 0, 24, extent);
      }
  }
		/*
		private void drawTickMarks(Graphics g)
		{
				if(intervalPolicy.equals("normal"));
				{
						for(int i = 0; i < noOfTickMarks; ++i)
						{
								//            g2d.drawString(String.valueOf(tickNo(i)),((float) ((size / (10 / scale) ) * (i + 3)) + 1),8.0f);
								//            float mainPos = (float) ((size / (10 / scale) ) * (i ));
								if(orientation == HORIZONTAL)
								{
										float mainPos = (float) ((size / (noOfTickMarks / scale) ) * (i ));
										g2d.drawString(String.valueOf(tickNo(i)), mainPos, 10.0f);
										g2d.drawLine(((int) mainPos), 0, ((int) mainPos), 1);
								} else if(orientation == VERTICAL)
								{
										float mainPos = (float) ((size / (noOfTickMarks / scale) ) * (i ));
										mainPos = size - mainPos;
										g2d.drawString(String.valueOf(tickNo(i)), 2.0f, mainPos);
										g2d.drawLine(24, ((int) mainPos), 23, ((int) mainPos));
								}
						}
				}
				if(intervalPolicy.equals("userDefined"));
				{
				}
		}
		*/
  private void clearAxis(Graphics g)
  {
    g.clearRect(0, 0, (int) getSize().getWidth(), (int) getSize().getHeight());
  }
  
  int tickNo(int i)
  {
    double x;
    x = (((max - min) / 10) * i) + min;
    int y = (int) x;
/*		// System.out.print("i = " +  i);
                // System.out.print(" x = " +  x);
                // // System.out.println(" y = " +  y);
                // // System.out.println("max = " + max );
                // // System.out.println("min = " +  min);
                // // System.out.println("(max - min) = " + (max - min) );
                // // System.out.println("((max - min) / 10) = " + ((max - min) / 10) );
                // // System.out.println("(((max - min) / 10) * i) = " + (((max - min) / 10) * i) );
                // // System.out.println("(((max - min) / 10) * i) + min = " + (((max - min) / 10) * i) + min );*/
    return y;
    //		return ((int) (((max - min) / 10) * i) + min);
  }
  
  /**
   * When user clicks mouse,
   * increment k
   */
  public void mouseReleased(MouseEvent e)
  {
    scale *= 1.1;
/*	xPos = e.getX();
        yPos = e.getY();*/
    repaint();
    //        // // System.out.println("size = "+ size + " actual = " + getSize().getWidth());
  }
  
  /**
   * Unused.
   */
  public void mouseExited(MouseEvent e)
  {
  }
  
  /**
   * Reset scale.
   */
  public void mouseEntered(MouseEvent e)
  {
    scale = 1;
    repaint();
  }
  
  /**
   * Unused.
   */
  public void mousePressed(MouseEvent e)
  {
  }
  
  /**
   * Unused.
   */
  public void mouseClicked(MouseEvent e)
  {
  }
  
  public void componentHidden(java.awt.event.ComponentEvent componentEvent)
  {
  }
  
  public void componentMoved(java.awt.event.ComponentEvent componentEvent)
  {
  }
  
  public void componentResized(java.awt.event.ComponentEvent componentEvent)
  {
    // // System.out.println("componentResized getWidth() = " + getSize().getWidth() + " getHeight() = " + getSize().getHeight() + " source = " + componentEvent.getSource());
    if(orientation == HORIZONTAL)
    {
      size = (int) getSize().getWidth();
    } else if(orientation == VERTICAL)
    {
      size = (int) getSize().getHeight();
    }
    if(offScreen)
    {
      clearAxis(imageToDraw.getGraphics());
      drawAxis(imageToDraw.getGraphics());
    }
    //        repaint();
    //        // // System.out.println("resize size = "+ size + " actual = " + getSize().getWidth());
  }
  
  public void componentShown(java.awt.event.ComponentEvent componentEvent)
  {
  }
  
  public static void main(String[] args)
  {
    Frame frame = new Frame("test Axis");
    frame.addWindowListener(new java.awt.event.WindowAdapter()
    {
      public void windowClosing(java.awt.event.WindowEvent evt)
      {
        System.exit(0);
      }
    });
    frame.setSize(400, 800);
    AxisMarks axisMarks = new AxisMarks();
    //        // // System.out.println("AxisMarks.orientation = " + axisMarks.orientation);
    //        axisMarks.setOrientation(AxisMarks.VERTICAL);
    axisMarks.setMaxMin(10, 0);
    //        // // System.out.println("AxisMarks.orientation = " + axisMarks.orientation);
    frame.add(axisMarks);
    frame.show();
    
    
  }
  
  public void controlContainerChanged(ControlContainerEvent e)
  {
    if((e.getPropertyName().equals("transformXFactor") && (orientation == HORIZONTAL)) || (e.getPropertyName().equals("transformYFactor") && (orientation == VERTICAL)))
    {
      offset = ((Integer) e.getNewValue()).intValue();
      min = ((offset + originalMin)  * zPos / xScale / yScale);
      max = ((offset + originalMax)   * zPos / xScale / yScale);
      repaint();
    } else if(e.getPropertyName().equals("transformZFactor"))
    {
      // This is bollocks, I shall have to figure out the calculation I have to do
      zPos = (double) ((Integer) e.getNewValue()).intValue();
      min = ((offset + originalMin)   * zPos / xScale / yScale);
      max = ((offset + originalMax)   * zPos / xScale / yScale);
      repaint();
    } else if(e.getPropertyName().equals("scaleXFactor") && (orientation == HORIZONTAL))
    {
      //            pos = new Vector3f(0, 0, ((Integer) e.getNewValue()).intValue());
      xScale = ((Integer) e.getNewValue()).intValue();
      min = ((offset + originalMin) * zPos / xScale / yScale);
      max = ((offset + originalMax)   * zPos / xScale / yScale);
      repaint();
/*            eyeTrans.setScale(scale);
            examineGroup.setTransform(eyeTrans); */
    }
  }
  
  public void viewPositionModelChanged(ViewPositionModelEvent e)
  {
    ViewPositionModel model = (ViewPositionModel) e.getSource();
		// System.out.println("getPropertyName = " + e.getPropertyName() + " model = " + model);
    //        if((e.getPropertyName().equals("transformXFactor") && (orientation == HORIZONTAL)) || (e.getPropertyName().equals("transformYFactor") && (orientation == VERTICAL)))
    if((e.getPropertyName().equals("transformXFactor") && (orientation == HORIZONTAL)))
    {
      max = model.getTopX();
      min = model.getBottomX();
		// System.out.println("max = " + max + " min = " + min);
			//		model.printValues();
      repaint();
    } else if((e.getPropertyName().equals("transformYFactor") && (orientation == VERTICAL)))
    {
      max = model.getTopY();
      min = model.getBottomY();
      repaint();
    } else if(((e.getPropertyName().equals("scaleXFactor") && (orientation == HORIZONTAL)) || (e.getPropertyName().equals("scaleYFactor") && (orientation == HORIZONTAL) && model.getScaleFactorsLocked())))
    {
      //	    	// System.out.println("scaled x axis cos HORIZONTAL = " + (e.getPropertyName().equals("scaleXFactor") && (orientation == HORIZONTAL)) + " or cos scale factors locked = " + (e.getPropertyName().equals("scaleYFactor") && (orientation == VERTICAL) && model.getScaleFactorsLocked()));
      //	    	// System.out.println("scaled x axis cos VERTICAL = " + (e.getPropertyName().equals("scaleYFactor") && (orientation == VERTICAL)) + " or cos scale factors locked = " + (e.getPropertyName().equals("scaleXFactor") && (orientation == HORIZONTAL) && model.getScaleFactorsLocked()));
      //	    	// System.out.println("model.getTopX() = " + model.getTopX() + " model.getBottomX() = " + model.getBottomX());
      max = model.getTopX();
      min = model.getBottomX();
      //			// System.out.println("xAxis max = " + max + " min = " + min + " orientation = " + orientation + " (HORIZONTAL = " + HORIZONTAL);
      repaint();
    } else if((e.getPropertyName().equals("scaleYFactor") && (orientation == VERTICAL)) || (e.getPropertyName().equals("scaleXFactor") && (orientation == VERTICAL) && model.getScaleFactorsLocked()))
    {
      //	    	// System.out.println("scaled x axis cos VERTICAL = " + (e.getPropertyName().equals("scaleYFactor") && (orientation == VERTICAL)) + " or cos scale factors locked = " + (e.getPropertyName().equals("scaleXFactor") && (orientation == HORIZONTAL) && model.getScaleFactorsLocked()));
      //	    	// System.out.println("model.getTopX() = " + model.getTopY() + " model.getBottomX() = " + model.getBottomY());
      max = model.getTopY();
      min = model.getBottomY();
      //			// System.out.println("yAxis max = " + max + " min = " + min + " orientation = " + orientation + " (HORIZONTAL = " + HORIZONTAL);
      repaint();
    } else if(e.getPropertyName().equals("parser"))
    {
      setMaxMin(((int)model.getTopX()), ((int)model.getBottomX()));
      if(orientation == HORIZONTAL)
      {
        setSize(100, 10);
      } else if(orientation == VERTICAL)
      {
        setSize(25, 100);
      }
      repaint();
    } else if(e.getPropertyName().equals("transformZFactor"))
    {
/*            // This is bollocks, I shall have to figure out the calculation I have to do
            zPos = (double) ((Integer) e.getNewValue()).intValue();
            min = ((offset + originalMin)   * zPos / xScale / yScale);
            max = ((offset + originalMax)   * zPos / xScale / yScale);
            repaint(); */
      
      max = model.getTopY();
      min = model.getBottomY();
      // // System.out.println("yAxis max = " + max + " min = " + min);
      //			model.printValues();
      repaint();
      
    }
    if(offScreen)
    {
      clearAxis(imageToDraw.getGraphics());
      drawAxis(imageToDraw.getGraphics());
    }
    
  }

  public void groutPanelChanged(GroutPanelEvent e)
	{
			if(e.getPropertyName().startsWith("FontAxisMarks"))
			{
					// System.out.println("FontAxisMarks viewPositionModelChanged" + e.getPropertyName() + " by " + e);
					String whatFont = e.getPropertyName();
					// System.out.println("whatFont = " + whatFont);
					if(whatFont.substring(13).startsWith("Size"))
					{
							int size = exFont.getSize();
							try
							{
									String sizeString = whatFont.substring(17);
									size = Integer.parseInt(sizeString);
									// System.out.println("size = " + size);
							}
							catch(NumberFormatException exc)
							{
									// this really should never happen
									exc.printStackTrace();
							}
							exFont = new Font(exFont.getName(), 
																exFont.getStyle(), size);
							repaint();
					}
					
					if(whatFont.substring(13).startsWith("Colour"))
					{
							if(whatFont.substring(19).startsWith("Black"))
							{
									foregroundColour = Color.black;
							}
							if(whatFont.substring(19).startsWith("White"))
							{
									foregroundColour = Color.white;
							}
							if(whatFont.substring(19).startsWith("Pink"))
							{
									foregroundColour = Color.pink;
							}
							if(whatFont.substring(19).startsWith("Red"))
							{
									foregroundColour = Color.red;
							}
							repaint();
					}
					
					if(whatFont.substring(13).startsWith("SetInterval"))
					{
							// System.out.println(whatFont + " = " + e.getNewValue());
							intervalPolicy = "userDefined";
							interval = ((Double) e.getNewValue()).doubleValue();
					}

			}
	}
}
