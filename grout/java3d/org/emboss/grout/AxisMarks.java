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
import java.awt.font.FontRenderContext;
import java.awt.geom.Rectangle2D;

/** This displays the axis marks
 * @author hmorgan2
 */
public class AxisMarks extends Canvas implements MouseListener,
ViewPositionModelListener, ComponentListener, GroutPanelListener
{
    /** true if this graphic has been removed and a white box is being displayed in its
     * place
     */
    private boolean makeAWhiteBox = false;
    
    public static final int HORIZONTAL = 0;
    public static final int VERTICAL = 1;
    //  private static int HORIZONTAL = 0;
    
    /** not used */
    private String label;
    
    /** top value of the display */
    private double max;
    
    /** bottom value of the display */
    private double min;
    
    /** size of the axis marks on the screen */
    private int size;
    /** the oriantation of the axis marks */
    private int orientation = HORIZONTAL;
    
    /** the relative scale compared to full zoom out */
    private double scale;
    /** Distance between original minimum and pressent minimum */
    private double   offset;
    /** position on the z axis */
    private double   zPos;
    /** scale in the x axis */
    private double   xScale;
    /** scale in the y axis */
    private double   yScale;
    /** maximum vales when fully zoomed out */
    private double   originalMax;
    /** minimum vales when fully zoomed out */
    private double   originalMin;
    /** true if this is being redered off screen */
    private boolean offScreen;
    
    /** the image of this component */
    BufferedImage imageToDraw = null;
    
    /** the font being used */
    Font exFont = new Font("TimesRoman",Font.PLAIN,10);
    
    /** th colour of the text */
    private Color foregroundColour = Color.black;
    
    /** strin gdescribing the interval policy */
    private String intervalPolicy = new String("normal");
    /** gap between tick marks */
    private double interval = 0.0;
    /** position of first tick */
    private double firstTick = 0.0;
    
    /** this is made true to turn of the axis */    
    boolean drawAxis = true;
    
    /** not used */    
    private double absoloteMaxTick = 0.0;
    /** the size required by the font, used when oriantation = VERTICAL */    
    private int requiredWidth = 25;
    /** the size required by the font, used when oriantation = HORIZONTAL */    
    private int requiredHeight = 10;
    
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
    
    /** Creates a new instance of AxisMarks
     * @param offScreen
     */
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
            if(orientation == HORIZONTAL)
            {
                imageToDraw = new BufferedImage(400, 10, 
																								BufferedImage.TYPE_INT_ARGB);
            } else if(orientation == VERTICAL)
            {
                imageToDraw = new BufferedImage(25, 400, 
																								BufferedImage.TYPE_INT_ARGB);
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
    
    /** returns the image of the component */    
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
    
    /** sets the max min values of the axis marks */    
    public void setMaxMin(int mx, int mn)
    {
        // System.out.println("setting mx = " +  mx);
        // System.out.println("setting mn = " +  mn);
        originalMax = max = mx;
        originalMin = min = mn;
    }
    
    public void setMin(int mn)
    {
        // System.out.println("setting mn = " +  mn);
        originalMin = min = mn;
    }
    public int getMin()
    {
        return (int) min;
    }
    public double getFirstTick()
    {
        return firstTick;
    }
    public double getInterval()
    {
        return interval;
    }
    public void setMax(int mx)
    {
        // System.out.println("setting mx = " +  mx);
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
            imageToDraw = new BufferedImage(400, 10, 
																						BufferedImage.TYPE_INT_ARGB);
        } else if(orientation == VERTICAL)
        {
            imageToDraw = new BufferedImage(25, 400, 
																						BufferedImage.TYPE_INT_ARGB);
        }
        // // System.out.println("imageToDraw = " + imageToDraw);
    }
    
    public void setDrawAxis(boolean da)
    {
        drawAxis = da;
    }
    
    public void setMakeAWhiteBox(boolean whiteBox)
    {
        makeAWhiteBox = whiteBox;
        repaint();
    }
    
    /** calcules if the font will fit in this comonent size.  If not, it is enlarged */    
    private void checkFontFits()
    {
        if(orientation == VERTICAL)
        {
            String biggestTickString;
            if(originalMax == Math.floor(originalMax))
            {
                biggestTickString = String.valueOf((int) originalMax);
            }
            else
            {
                biggestTickString = String.valueOf(originalMax);
            }
            Rectangle2D fontBounds = exFont.getStringBounds
            (biggestTickString, new FontRenderContext(null, false, true));
            requiredWidth = ((int) Math.floor(fontBounds.getWidth())) + 6;
            int actualWidth = getSize().width;
            
            if(requiredWidth != actualWidth)
            {
                setSize(requiredWidth, getSize().height);
                resizeAllParents();
            }
            
        }
        else if(orientation == HORIZONTAL)
        {
            Rectangle2D fontBounds = exFont.getStringBounds
            ("0", new FontRenderContext(null, false, true));
            requiredHeight = ((int) (Math.floor(fontBounds.getHeight()) / 2) + 5);
            int actualHeight = getSize().height;
            
            if(requiredHeight != actualHeight)
            {
                setSize(getSize().width, requiredHeight);
                resizeAllParents();
            }
        }
    }
    
    /** Convenience method.  Resizes all the parents of this component, so that its new
     * size fits
     */    
    private void resizeAllParents()
    {
        for(Component component = this; component != null;
        component = component.getParent())
        {
            if(component instanceof GroutPanel)
            {
                // ((JComponent) component).revalidate();
                ((GroutPanel) component).removeBlankBoxes(false);
            }
            else if(component instanceof JComponent)
            {
                ((JComponent) component).revalidate();
            }
            else
            {
                component.repaint();
            }
        }
    }
    
    public void paint(Graphics g)
    {
        if(getSize().height > 2)
        {
            // a bit odd, but tests for NaN
            if(!((!((max - min) >= 0)) && (!((max - min) < 0))))
            {
                drawAxis(g);
            }
        }
        if(makeAWhiteBox)
        {
            // System.out.println("paint Axismarks white");
            final Dimension originalSize = getSize();
            g.setColor(Color.white);
            g.fillRect(0, 0, originalSize.width, originalSize.height);
        }
    }
    
    /** do the actual drawing */    
    private void drawAxis(Graphics g)
    {
        //        // // System.out.println("size??? = " +  size);
        Graphics2D g2d = (Graphics2D)g;
        //       Font exFont = new Font("TimesRoman",Font.PLAIN,10);
        if(drawAxis)
        {
            g2d.setFont(exFont);
            // g2d.setColor(Color.black);
            // g2d.setColor(foregroundColour);
            int noOfTickMarks;
            int extent;
            
            // System.out.println("setting max = " +  max);
            // System.out.println("setting min = " +  min);
            // System.out.println("setting (max - min) = " +  (max - min));
            double[] ticks = calculatePolicy(new Double(max - min));
            
            drawTicks(ticks, g);
        }
        
        // g2d.setColor(Color.black);
        if(orientation == HORIZONTAL)
        {
            g2d.drawLine(0, 0, size, 0);
        } else if(orientation == VERTICAL)
        {
            // g2d.drawLine(24, 0, 24, size);
            g2d.drawLine((requiredWidth - 1), 0, (requiredWidth - 1), size);
        }
        
        
    }
    
    /** draw the ticks
     * @param ticks and array giving the numbers that the ticks should hold
     * @param g
     */    
    private void drawTicks(double[] ticks, Graphics g)
    {
        Graphics2D g2d = (Graphics2D) g;
        double gap = max - min;
        // g2d.setColor(Color.black);
        
        String tickStringTemp;
        int lastTick;
        if(ticks[ticks.length - 1] == 0.0)
        {
            lastTick = ticks.length - 2;
        }
        else
        {
            lastTick = ticks.length - 1;
        }
        if(ticks[lastTick] == Math.floor(ticks[lastTick]))
        {
            tickStringTemp = String.valueOf((int) ticks[lastTick]);
        }
        else
        {
            tickStringTemp = String.valueOf(ticks[lastTick]);
        }
        Rectangle2D fontBounds = exFont.getStringBounds
        (tickStringTemp, new FontRenderContext(null, false, true));
        // 		(String.valueOf(ticks[ticks.length - 1]), g);
        double fontHeight = fontBounds.getHeight();
        double fontWidth = fontBounds.getWidth();
        int componentWidth = getSize().width;
        int componentHeight = getSize().height;
        double maxPositionX = ((double) componentWidth) - (fontWidth / 2);
        double charWidth = exFont.getStringBounds
        ("0", new FontRenderContext(null, false, true))
        .getWidth();
        
        
        for(int i = 0; i < ticks.length; ++i)
        {
            double proportionFromZero = (ticks[i] - min) / gap;
            String tickString;
            if(ticks[i] == Math.floor(ticks[i]))
            {
                tickString = String.valueOf(((int) ticks[i]));
            }
            else
            {
                tickString = String.valueOf(ticks[i]);
            }
            if(orientation == HORIZONTAL)
            {
                float mainPos;
                // if(ticks[i] == 0)
                {
                    mainPos = (float) (size * proportionFromZero);
                }
                if(mainPos < maxPositionX)
                {
                    // g2d.drawString(tickString, mainPos, 10.0f);
                    g2d.setColor(foregroundColour);
                    if(ticks[i] == 0)
                    {
                        g2d.drawString(tickString, mainPos, requiredHeight);
                    }
                    else
                    {
                        Rectangle2D fontBoundsTemp = exFont.getStringBounds
                        (String.valueOf(ticks[i]),
                        new FontRenderContext(null, false, true));
                        // 		(String.valueOf(ticks[ticks.length - 1]), g);
                        double fontWidthTemp = fontBoundsTemp.getWidth();
                        g2d.drawString(tickString,
                        ((float) (mainPos -
                        ((fontWidthTemp / 2) -
                        charWidth / 1))),
                        requiredHeight);
                    }
                    g2d.setColor(Color.black);
                    g2d.drawLine(((int) mainPos), 0, ((int) mainPos), 1);
                }
            } else if(orientation == VERTICAL)
            {
                float mainPos = (float) (size * proportionFromZero);
                mainPos = size - mainPos;
                if(((mainPos > (fontHeight / 4)) &&
                (mainPos < (componentHeight - (fontHeight / 4)))) ||
                (ticks[i] == 0))
                {
                    g2d.setColor(foregroundColour);
                    if(ticks[i] == 0)
                    {
                        g2d.drawString(tickString, 2.0f, mainPos);
                    }
                    else
                    {
                        g2d.drawString(tickString, 2.0f,
                        ((float) (mainPos + (fontHeight / 4))));
                    }
                    g2d.setColor(Color.black);
                    g2d.drawLine((requiredWidth - 1), ((int) mainPos),
                    (requiredWidth - 2), ((int) mainPos));
                    
                }
            }
        }
        // add minor tick marks
        for(int i = 1; i < ticks.length; ++i)
        {
            double minorTickPosition = ((ticks[i] - ticks[i -1]) /2)
            + ticks[i -1];
            double proportionFromZero = (minorTickPosition - min) / gap;
            if(orientation == HORIZONTAL)
            {
                float	mainPos = (float) (size * proportionFromZero);
                if(mainPos < maxPositionX)
                {
                    // System.out.println("mainPos  " + mainPos );
                    g2d.setColor(Color.black);
                    g2d.drawLine(((int) mainPos), 0, ((int) mainPos), 1);
                }
            } else if(orientation == VERTICAL)
            {
                float mainPos = (float) (size * proportionFromZero);
                mainPos = size - mainPos;
                if(mainPos > fontHeight)
                {
                    // System.out.println("mainPos  " + mainPos );
                    g2d.setColor(Color.black);
                    g2d.drawLine((requiredWidth - 1), ((int) mainPos),
                    (requiredWidth - 2), ((int) mainPos));
                }
            }
        }
        if(ticks.length > 2)
        {
            int numberOfTicks;
            if((ticks[ticks.length - 1]) == 0 && (ticks.length > 3))
            {
                numberOfTicks = ticks.length -1;
            }
            else
            {
                numberOfTicks = ticks.length;
            }
            
            if((ticks[numberOfTicks - 1] + ((ticks[numberOfTicks - 1] -
            ticks[numberOfTicks - 2]) /2)) < max)
            {
                double minorTickPosition = (ticks[numberOfTicks - 1] +
                ((ticks[numberOfTicks - 1] -
                ticks[numberOfTicks - 2]) /2));
                double proportionFromZero = (minorTickPosition - min) / gap;
                if(orientation == HORIZONTAL)
                {
                    float	mainPos = (float) (size * proportionFromZero);
                    // System.out.println("mainPos  " + mainPos );
                    // if(mainPos < maxPositionX)
                    {
                        g2d.setColor(Color.black);
                        g2d.drawLine(((int) mainPos), 0, ((int) mainPos), 1);
                    }
                } else if(orientation == VERTICAL)
                {
                    float mainPos = (float) (size * proportionFromZero);
                    // System.out.println("mainPos  " + mainPos );
                    mainPos = size - mainPos;
                    // if(mainPos > fontHeight)
                    {
                        g2d.setColor(Color.black);
                        g2d.drawLine((requiredWidth - 1), ((int) mainPos),
                        (requiredWidth - 2), ((int) mainPos));
                    }
                }
            }
        }
    }
    
    /** Calculate what number each of the ticks should be
     * @param gap interval
     * @return array of doubles, with the values of each tick
     */    
    private double[] calculatePolicy(Double gap)
    {
        double[] ticks;
        if(intervalPolicy.equals("normal"))
        {
            // System.out.println("setting gap = " + gap);
            // Double gap = new Double(max - min);
            int extent;
            Dimension size = getSize();
            if(orientation == HORIZONTAL)
            {
                extent = size.width;
            }
            else
            {
                extent = size.height;
            }
            Boolean positive = Boolean.TRUE;
            Integer exponent = new Integer(0); // 10 to the power of
            Double mantisa = new Double(0);  // times by
            
            //calculateScientificNotation(gap, mantisa, exponent, positive);
            {
                int mantisaStart;
                int ordersOfMagnitudeChanged = 0;
                // System.out.print("gap = " + gap);
                // this is a checky way of getting them
                if(((gap.doubleValue() > 0.001) && (gap.doubleValue() < 10000000)) ||
                ((gap.doubleValue() < -0.001) && (gap.doubleValue() > -10000000)))
                {
                    gap = new Double(gap.doubleValue() * 10000000000.0);
                    ordersOfMagnitudeChanged = 10;
                }
                // System.out.println("setting gap = " + gap);
                String gapStr = gap.toString();
                if(gapStr.substring(0, 1).equals("-"))
                {
                    positive = Boolean.FALSE;
                    mantisaStart = 1;
                }
                else
                {
                    positive = Boolean.TRUE;
                    mantisaStart = 0;
                }
                int exponentStart = gapStr.indexOf("E") + 1;
                try
                {
                    mantisa = new Double(gapStr.substring(mantisaStart, 
																													(exponentStart - 1)));
                    int exponentInt = Integer.parseInt
												(gapStr.substring(exponentStart)) 
												- ordersOfMagnitudeChanged;
                    exponent = new Integer(exponentInt);
                }
                catch(NumberFormatException e)
                {
                    e.printStackTrace();
                }
                
            }
            
            int significantFigues;
            int noOfSignificantFigues;
            int numberOfTickMarks;
            int exponentOfTickMarks;
            
            if(extent < 100)
            {
                numberOfTickMarks = 2;
            }
            else if(extent < 200)
            {
                numberOfTickMarks = 3;
            }
            else if(extent < 300)
            {
                numberOfTickMarks = 4;
            }
            else
            {
                numberOfTickMarks = 5;
            }
            
            if(mantisa.doubleValue() < numberOfTickMarks)
            {
                noOfSignificantFigues = 2;
                significantFigues = (int) Math.floor(mantisa.doubleValue() * 10);
                exponentOfTickMarks = exponent.intValue() - 1;
            }
            else
            {
                noOfSignificantFigues = 1;
                significantFigues = (int) Math.floor(mantisa.doubleValue());
                exponentOfTickMarks = exponent.intValue();
            }
            
            // this sometimes causes a figure like 1.7000000000000002 when
            // added twice.  Must be a rounding error somewhere?
                                                /*
                                                        double remainder = Math.IEEEremainder(significantFigues,
                                                        (numberOfTickMarks - 1));
                                                        if(remainder < 0)
                                                        {
                                                        remainder = numberOfTickMarks + remainder - 1;
                                                        }
                                                 */
            
            double tickGap = 0;
            double lowestTick = 0;
            double highestTick = 0;
            
            boolean gotRightNumberOfTicks = false;
            boolean tickMarksReduced = false;
            while(!gotRightNumberOfTicks)
            {
                double remainder = Math.IEEEremainder(significantFigues,
                (numberOfTickMarks - 1));
                if(remainder < 0)
                {
                    remainder = numberOfTickMarks + remainder - 1;
                }
                
                tickGap = ((significantFigues - remainder)
                / (numberOfTickMarks - 1))
                * Math.pow(10, exponentOfTickMarks);
                lowestTick = Math.ceil(min / tickGap) * tickGap;
                highestTick = Math.floor(max / tickGap) * tickGap;
                // int j = 0;
                // for(double i = lowestTick; i < max; i += tickGap)
                
                int numberOfPossibleTickLocations =
                ((int) ((highestTick - lowestTick) / tickGap)) + 1;
                if((numberOfPossibleTickLocations >= 
										(((double) numberOfTickMarks - 1) * 1.5)) 
									 && !tickMarksReduced)
                {
                    numberOfTickMarks -= 1;
                    tickMarksReduced = true;
                    
                }
                else if((numberOfPossibleTickLocations 
												 > (((double) numberOfTickMarks - 1) * 1.5)))
                {
                    numberOfTickMarks += 3;
                    gotRightNumberOfTicks = true;
                    // significantFigues += 1;
                }
                else
                {
                    gotRightNumberOfTicks = true;
                }
            }
            
            
            ticks = new double[numberOfTickMarks];
            
            if(min <= 0 && max >= 0)
            {
                
                double i = lowestTick;
                for(int j = 0; (j < numberOfTickMarks) && (i <= highestTick);
										++j)
                {
                    if(i == -0.0)
                    {
                        i = 0.0;
                    }
                    ticks[j] = i;
                    i += tickGap;
                }
            }
            else
            {
                double relativeSizeOfMaxMin = max / min;
                if(relativeSizeOfMaxMin > 1)
                {
                    relativeSizeOfMaxMin = 1 / relativeSizeOfMaxMin;
                }
                
                // if(relativeSizeOfMaxMin > noOfSignificantFigues)
                if(true)
                {
                    lowestTick = Math.ceil(min / tickGap) * tickGap;
                    highestTick = Math.floor(max / tickGap) * tickGap;
                    // int j = 0;
                    // for(double i = lowestTick; i < max; i += tickGap)
                    double i = lowestTick;
                    for(int j = 0; (j < numberOfTickMarks) && (i <= highestTick);
                    ++j)
                    {
                        ticks[j] = i;
                        i += tickGap;
                        // System.out.println(i);
                    }
                }
                else
                {
                    
                    
                    // double lowestTick;
                    if(noOfSignificantFigues == 1)
                    {
                        lowestTick = 
														Math.pow(10, Math.ceil(Math.log(min) 
																									 / Math.log(10)));
                        // System.out.println("lowestTick = " + lowestTick);
                    }
                    else
                    {
                        lowestTick = min;
                    }
                    for(int i = 0; i < numberOfTickMarks; ++i)
                    {
                        ticks[i] = lowestTick + (tickGap * i);
                    }
                }
            }
            
            // System.out.println("numberOfTicks = " + numberOfTicks);
            firstTick = ticks[0];
            if(ticks.length >= 2)
            {
                interval = ticks[1] - ticks[0];
            }
            else
            {
                interval = 1;
            }
        }
        else // if(intervalPolicy.startsWith("userDefined"))
        {
            double nextTick = firstTick;
            if(interval < 0)
            {
                interval *= -1;
            }
            if(interval != 0)
            {
                int numberOfTicks = ((int) Math.ceil((max - firstTick)
                / interval));
                ticks = new double[numberOfTicks];
                for(int i = 0; i < numberOfTicks; ++i)
                {
                    // System.out.println("ticks[" + i + "] = " + nextTick);
                    ticks[i] = nextTick;
                    nextTick += interval;
                }
            }
            else
            {
                ticks = new double[1];
                ticks[0] = firstTick;
            }
        }
        // System.out.println("intervalPolicy = " + intervalPolicy);
        
        return ticks;
    }
    
    /** Calculates the scientific notation for the number in gap
     * @param gap the number to calculte scientific notation for
     * @param mantisa unused
     * @param exponent unused
     * @param positive unused
     * @return the scientific notation, in the form Double(mantisa), Integer(exponent),
     * Boolean(positive)
     */    
    public static Vector calculateScientificNotation(Double gap,
    Double mantisa,
    Integer  exponent,
    Boolean positive)
    {
        int mantisaStart;
        int ordersOfMagnitudeChanged = 0;
        // this is a checky way of getting them
        // System.out.print("gap = " + gap);
        if(((gap.doubleValue() > 0.001) && (gap.doubleValue() < 10000000)) ||
        ((gap.doubleValue() < -0.001) && (gap.doubleValue() > -10000000)))
        {
            gap = new Double(gap.doubleValue() * 10000000000.0);
            ordersOfMagnitudeChanged = 10;
        }
        String gapStr = gap.toString();
        if(gapStr.substring(0, 1).equals("-"))
        {
            positive = Boolean.FALSE;
            mantisaStart = 1;
        }
        else
        {
            positive = Boolean.TRUE;
            mantisaStart = 0;
        }
        int exponentStart = gapStr.indexOf("E") + 1;
        try
        {
            mantisa = new Double(gapStr.substring(mantisaStart, 
																									(exponentStart - 2)));
            int exponentInt = Integer.parseInt(gapStr.substring(exponentStart))
								- ordersOfMagnitudeChanged;
            exponent = new Integer(exponentInt);
        }
        catch(NumberFormatException e)
        {
            e.printStackTrace();
        }
        
        Vector returnVector = new Vector(3);
        returnVector.addElement(mantisa);
        returnVector.addElement(exponent);
        returnVector.addElement(positive);
        
        return returnVector;
    }
    
    /** calculates if number of tick marks is a factor of significant figures
     * @param numberOfTickMarks
     * @param significantFigues
     * @return
     */    
    private boolean isFactorOf(int numberOfTickMarks, int significantFigues)
    {
        double numberOfTickMarksD = (double) numberOfTickMarks;
        double significantFiguesD = (double) significantFigues;
        
        double divided = significantFigues / numberOfTickMarks;
        
        return (divided == Math.floor(divided));
    }
    
    /** makes the component a clear rectangle */    
    private void clearAxis(Graphics g)
    {
        g.clearRect(0, 0, (int) getSize().getWidth(), (int) getSize().getHeight());
    }
    
    /** not used */    
    int tickNo(int i)
    {
        double x;
        x = (((max - min) / 10) * i) + min;
        int y = (int) x;
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
        if(getSize().height > 2)
        {
            checkFontFits();
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
        }
        
    }
    
    public void componentShown(java.awt.event.ComponentEvent componentEvent)
    {
        System.out.println("componentShown");
        checkFontFits();
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
        axisMarks.setMaxMin(10, 0);
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
						// Is this correct?
            zPos = (double) ((Integer) e.getNewValue()).intValue();
            min = ((offset + originalMin)   * zPos / xScale / yScale);
            max = ((offset + originalMax)   * zPos / xScale / yScale);
            repaint();
        } else if(e.getPropertyName().equals("scaleXFactor") 
									&& (orientation == HORIZONTAL))
        {
            xScale = ((Integer) e.getNewValue()).intValue();
            min = ((offset + originalMin) * zPos / xScale / yScale);
            max = ((offset + originalMax)   * zPos / xScale / yScale);
            repaint();
        }
    }
    
    public void viewPositionModelChanged(ViewPositionModelEvent e)
    {
        ViewPositionModel model = (ViewPositionModel) e.getSource();
        if((e.getPropertyName().equals("transformXFactor") 
						&& (orientation == HORIZONTAL)))
        {
            max = model.getTopX();
            min = model.getBottomX();
            // System.out.println("max = " + max + " min = " + min);
            //		model.printValues();
            repaint();
        } else if((e.getPropertyName().equals("transformYFactor") 
									 && (orientation == VERTICAL)))
        {
            max = model.getTopY();
            min = model.getBottomY();
            repaint();
        } else if(((e.getPropertyName().equals("scaleXFactor")
										&& (orientation == HORIZONTAL)) 
									 || (e.getPropertyName().equals("scaleYFactor") 
											 && (orientation == HORIZONTAL) 
											 && model.getScaleFactorsLocked())))
        {
            max = model.getTopX();
            min = model.getBottomX();
            repaint();
        } else if((e.getPropertyName().equals("scaleYFactor") 
									 && (orientation == VERTICAL)) 
									|| (e.getPropertyName().equals("scaleXFactor")
											&& (orientation == VERTICAL) 
											&& model.getScaleFactorsLocked()))
        {
            max = model.getTopY();
            min = model.getBottomY();
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
        if(e.getPropertyName().startsWith("Parser"))
        {
            // System.out.println("WHAT CAHNGED = " + e.getPropertyName());
        }
        if(e.getPropertyName().startsWith("FontAxisMarks"))
        {
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
    
    /** debug method */    
    public void printValues()
    {
        System.out.println("label = " + label + " max = " + max + " min = " 
													 + min + " size = " + size + " orientation = " 
													 + orientation + " scale = " + scale + " offset = " 
													 + offset);// + "  = " +  + "  = " + );
    }
    
    /** Not used */    
    private String showIntervalChooser()
    {
        String firstTickString = (String)JOptionPane.showInputDialog
        (this,
        "Enter first tick position",
        "Set First Tick",
        JOptionPane.PLAIN_MESSAGE);
        
        double firstTickTemp = Double.NaN;
        
        while(firstTickTemp == Double.NaN)
        {
            try
            {
                firstTickTemp = Double.parseDouble(firstTickString);
            }
            catch(NumberFormatException e)
            {
                firstTickString = (String)JOptionPane.showInputDialog
                (this,
                "Enter first tick position",
                "Enter a real number",
                JOptionPane.PLAIN_MESSAGE);
            }
        }
        
        firstTickTemp = Double.NaN;
        
        String intervalString = (String)JOptionPane.showInputDialog
        (this,
        "Enter Interval for Axis",
        "Set Interval",
        JOptionPane.PLAIN_MESSAGE);
        
        while(firstTickTemp == Double.NaN)
        {
            try
            {
                firstTickTemp = Double.parseDouble(firstTickString);
            }
            catch(NumberFormatException e)
            {
                intervalString = (String)JOptionPane.showInputDialog
                (this,
                "Enter Interval",
                "Enter a real number",
                JOptionPane.PLAIN_MESSAGE);
            }
        }
        
                                /*
                                System.out.println(" = " + );
                                 */
        return "FontAxisMarksSetInterval" + firstTickString + 
						"interval=" + intervalString;
    }
    
    /** alters the size and colour of the font
     * @param whatFont Defines what to change
     * @param colour
     */    
    public void changeFont(String whatFont, Color colour)
    {
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
            checkFontFits();
            repaint();
        }
        
        if(whatFont.substring(13).startsWith("Colour"))
        {
            // System.out.println(whatFont + " in AxisdMarks");
            if(whatFont.substring(19).startsWith("User"))
            {
                foregroundColour = colour;
            }
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
            if(whatFont.substring(24).startsWith("Defualt"))
            {
                // System.out.println(whatFont + " = ");
                intervalPolicy = "normal";
            }
            else
            {
                // System.out.println(whatFont + " = " + e.getNewValue());
                
                // String whatInterval = showIntervalChooser();
                String whatInterval = whatFont;
                // System.out.println("whatInterval = " + whatInterval);
                intervalPolicy = "userDefined";
                try
                {
                    int firstTickStart = 24;
                    int firstTickEnd = whatInterval.indexOf("interval=");
                    int intervalStart = firstTickEnd + 9;
                    int intervalEnd = whatInterval.length();
                    // whatInterval.substring(25)
                    interval = Double.parseDouble
                    (whatInterval.substring(intervalStart, intervalEnd));
                    firstTick = Double.parseDouble
                    (whatInterval.substring(firstTickStart, firstTickEnd));
                }
                catch(NumberFormatException e)
                {
                    e.printStackTrace();
                }
            }
            repaint();
            
            // interval = ((Double) e.getNewValue()).doubleValue();
        }
    }
    
}
