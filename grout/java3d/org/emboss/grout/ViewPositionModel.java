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
 * ViewPositionModel.java
 *
 * Created on July 22, 2002, 1:37 PM
 */

package org.emboss.grout;

import java.util.Vector;
import java.beans.PropertyChangeEvent;
import java.awt.Dimension;

import javax.media.j3d.Canvas3D;
import javax.media.j3d.View;
import java.awt.Point;
import javax.vecmath.Point3d;
/** This handles the abstract and real view position parameters.  All the view
 * position change events go through this object.
 * @author hmorgan2
 */
public class ViewPositionModel implements ParserListener,
ControlContainerListener,
ViewPositionModelListener,
GroutPanelListener,
RendWindContainerListener
{
    
    /** lowest x value of scene graph */    
    private double minX;
    /**  highest x value of scene graph */    
    private double maxX;
    /**  //lowest y value of scene graph */    
    private double minY;
    /**  //highest y value of scene graph */    
    private double maxY;
    /**  //lowest x value of display */    
    private double bottomX;
    /**  //highest x value of display */    
    private double topX; 
    /**  //lowest y value of display */    
    private double bottomY;
    /** //highest y value of display */    
    private double topY; 
    
    /**  //x extent of scene graph */    
    private double diffX;
    /**  //y extent of scene graph */    
    private double diffY;
    /**  //x extent of display */    
    private double displaySizeX;
    /**  //y extent of display */    
    private double displaySizeY;
    
    /**  //x centre of display, deviation from bottom of graph */    
    private double posX;
    /**  //y centre of display, deviation from bottom of graph */    
    private double posY;
    /**  //x centre of display, deviation from bottom of graph, scaled so this 
     * is pos of viewpoint */    
    private double viewPosX;
    /**  //y centre of display, deviation from bottom of graph, scaled so this 
     *  is pos of viewpoint */    
    private double viewPosY;
    
    /**  // total x scale */    
    private double scaleXOnly;
    /**  // total y scale */    
    private double scaleYOnly;
    /**  // total x scale */    
    private double scaleX;
    /**  // total y scale */    
    private double scaleY;
    /**  // overall scale, from scale Z factor */    
    private double overallScale;
    /**  // overall scale, from scale Z factor */    
    private double overallScaleX;
    /**  // overall scale, from scale Z factor */    
    private double overallScaleY;
    /**  // overall scale when zoomed right out */    
    private double overallScaleOriginal;
    /**  // overall scale when zoomed right out */    
    private double overallScaleXOriginal;
    /**  // overall scale when zoomed right out */    
    private double overallScaleYOriginal;
    
    /** scale for X axis that makes it fit the view when fully zoomed out */    
    private double scaleXForFit;
    /** scale for Y axis that makes it fit the view when fully zoomed out */    
    private double scaleYForFit;
    
    /** scale for X axis that makes it fit the view when fully zoomed out 
		 * Original */    
    private double scaleXForFitOriginal;
    /** scale for Y axis that makes it fit the view when fully zoomed out 
		 * Original */    
    private double scaleYForFitOriginal;
    
    
    /** are the scale Factors Locked */    
    private boolean scaleFactorsLocked = false;
    /** what are the scale Factors Locked to */    
    private double xOverY;
    
    /** targets to send events to */    
    private Vector targets = new Vector();
    
    /** double value of root 2 over Root 3 */    
    private static double root2overRoot3 = 
				(Math.pow(2, 0.5) / Math.pow(3, 0.5)) * 10;
    /** factor required to make fitting to window work (something to do with the
     * difference between the window size and the display area size? */    
    private static double RELATIVE_TO_WINDOW_FUDGE_FACTOR = 1.035; // 1.035
    /** the window Eyepoint Policy */    
    private int windowEyepointPolicy;
    
    /** the graphic number that this view refers to */    
    private int graphicNumber;
    
    /** Creates a new instance of ViewPositionModel */
    public ViewPositionModel()
    {
    }
    
    /** gets diffX
     * @return diffX
     */    
    public double getDiffX()
    {
        return diffX;
    }
    /** gets topX
     * @return topX
     */    
    public double getTopX()
    {
        return topX;
    }
    /** gets bottomX
     * @return bottomX
     */    
    public double getBottomX()
    {
        return bottomX;
    }
    /** gets minX
     * @return minX
     */    
    public double getMinX()
    {
        return minX;
    }
    /** gets maxX
     * @return maxX
     */    
    public double getMaxX()
    {
        return maxX;
    }
    /** gets displaySizeX
     * @return displaySizeX
     */    
    public double getDisplaySizeX()
    {
        return displaySizeX;
    }
    /** gets posX
     * @return posX
     */    
    public double getPosX()
    {
        return posX;
    }
    /** gets viewPosX
     * @return viewPosX
     */    
    public double getViewPosX()
    {
        return viewPosX;
    }
    /** gets diffY
     * @return diffY
     */    
    public double getDiffY()
    {
        return diffY;
    }
    /** gets topY
     * @return topY
     */    
    public double getTopY()
    {
        return topY;
    }
    /** gets bottomY
     * @return bottomY
     */    
    public double getBottomY()
    {
        return bottomY;
    }
    /** gets minY
     * @return minY
     */    
    public double getMinY()
    {
        return minY;
    }
    /** gets maxY
     * @return maxY
     */    
    public double getMaxY()
    {
        return maxY;
    }
    /** gets displaySixeY
     * @return displaySixeY
     */    
    public double getDisplaySizeY()
    {
        return displaySizeY;
    }
    /** gets posY
     * @return posY
     */    
    public double getPosY()
    {
        return posY;
    }
    /** gets viewPosY
     * @return viewPosY
     */    
    public double getViewPosY()
    {
        return viewPosY;
    }
    /** gets overalScale
     * @return overalScale
     */    
    public double getOverallScale()
    {
        return overallScale;
    }
    /** gets scaleX
     * @return scaleX
     */    
    public double getScaleX()
    {
        return scaleX;
    }
    /** gets scaleY
     * @return scaleY
     */    
    public double getScaleY()
    {
        return scaleY;
    }
    /** gets scaleXOnly
     * @return scaleXOnly
     */    
    public double getScaleXOnly()
    {
        return scaleXOnly;
    }
    /** gets scaleYOnly
     * @return scaleYOnly
     */    
    public double getScaleYOnly()
    {
        return scaleYOnly;
    }
    /** gets scaleXForFit
     * @return scaleXForFit
     */    
    public double getScaleXForFit()
    {
        return scaleXForFit;
    }
    /** gets scaleYForFit
     * @return scaleYForFit
     */    
    public double getScaleYForFit()
    {
        return scaleYForFit;
    }
    /** gets scaleFactorsLocked
     * @return scaleFactorsLocked
     */    
    public boolean getScaleFactorsLocked()
    {
        return scaleFactorsLocked;
    }
    /** gets xOverY
     * @return xOverY
     */    
    public double getXOverY()
    {
        return xOverY;
    }
    
    /** Calculates the x position for the view to fit a specific screen size and
     * positioned at a point
     * @param screenPosition Point for the centre of the view
     * @param screenSize The screen size to fit to
     * @return the x position
     */    
    public double transformScreenToModelX(Point screenPosition,
    Dimension screenSize)
    {
        double xProportion = ((double) screenPosition.x) /
        ((double) screenSize.width);
        double xPosition = bottomX + (displaySizeX * xProportion);
        
        return xPosition;
    }
    
    /** Calculates the y position for the view to fit a specific screen size and
     * positioned at a point
     * @param screenPosition Point for the centre of the view
     * @param screenSize thew screen size
     * @return the y position
     */    
    public double transformScreenToModelY(Point screenPosition,
    Dimension screenSize)
    {
        double yProportion = (((double) screenSize.height) -
        ((double) screenPosition.y)) /
        ((double) screenSize.height);
        double yPosition = bottomY + (displaySizeY * yProportion);
                                /*
                                System.out.println(" = " + );
                                System.out.println(" = " + );
                                 */
        
        return yPosition;
    }
    
    /** sets the graphic number this view refers to
     * @param no the graphic number
     */    
    public void setGraphicNumber(int no)
    {
        graphicNumber = no;
        return;
    }
    /** sets the maxX
     * @param value maxX
     */    
    public void setMaxX(double value)
    {
        maxX = value;
        minMaxValuesChanged();
        resetViewPosition();
        recalculateValues();
    }
    /** sets the minX
     * @param value minX
     */    
    public void setMinX(double value)
    {
        minX = value;
        minMaxValuesChanged();
        resetViewPosition();
        recalculateValues();
    }
    /** sets the maxY
     * @param value maxY
     */    
    public void setMaxY(double value)
    {
        maxY = value;
        minMaxValuesChanged();
        resetViewPosition();
        recalculateValues();
    }
    /** sets the minY
     * @param value minY
     */    
    public void setMinY(double value)
    {
        minY = value;
        minMaxValuesChanged();
        //		resetViewPosition();
        //		recalculateValues();
    }
    
    /** Debug method.  Prints out all the view parameters */    
    public void printValues()
    {
        System.out.println("minX = " + minX + " maxX = " + maxX + " minY = " +
													 minY + " maxY = " + maxY + " bottomX = " + bottomX 
													 + " topX = " + topX + " diffX = " + diffX + 
													 " displaySizeX = " + displaySizeX + " posX = " + 
													 posX + " scaleX = " + scaleX + " overallScale = " +
													 overallScale + " displaySizeY = " + displaySizeY);
        System.out.println("minY = " + minY + " maxY = " + maxY + " bottomY = "
													 + bottomY + " topY = " + topY + " diffY = " + diffY 
													 + " displaySizeY = " + displaySizeY + " posY = " + 
													 posY + " scaleY = " + scaleY + " overallScale = " + 
													 overallScale + " scaleYOnly = " + scaleYOnly + 
													 " scaleYForFit = " + scaleYForFit + 
													 " overallScaleY = " + overallScaleY + 
													 " overallScaleYOriginal = " + overallScaleYOriginal 
													 + " viewPosY = " + viewPosY + 
													 " scaleYForFitOriginal = " + scaleYForFitOriginal);
    }
    
    /** recaluculate vales for the view */    
    private void recalculateValues()
    {
        double overallScaleRelativeToFullZoomOut = 
						overallScaleOriginal / overallScale;
        double overallScaleXRelativeToFullZoomOut = 
						overallScaleXOriginal / overallScaleX;
        double overallScaleYRelativeToFullZoomOut = 
						overallScaleYOriginal / overallScaleY;
        scaleX = overallScaleXRelativeToFullZoomOut * scaleXOnly;
        scaleY = overallScaleYRelativeToFullZoomOut * scaleYOnly;
        
        bottomX = posX - 0.5 * (diffX  * scaleXForFit/ scaleX);
        topX = posX + 0.5 *(diffX  * scaleXForFit/ scaleX);
        bottomY = posY - 0.5 *(diffY  * scaleYForFit/ scaleY);
        topY = posY + 0.5 *(diffY  * scaleYForFit/ scaleY);
        
        displaySizeX = topX - bottomX;
        displaySizeY = topY - bottomY;

        viewPosX = posX * scaleXOnly;
        viewPosY = posY * scaleYOnly;
    }
    
    /** recalculate x vales */    
    private void recalculateXValues()
    {
        bottomX = posX - 0.5 *(diffX * scaleXForFit / scaleX);
        topX = posX + 0.5 *(diffX * scaleXForFit / scaleX);
        displaySizeX = topX - bottomX;
        viewPosX = posX * scaleX;
    }
    
    /** recaluculate y values */    
    private void recalculateYValues()
    {
        bottomY = posY - 0.5 *(diffY * scaleYForFit / scaleY);
        topY = posY + 0.5 *(diffY * scaleYForFit / scaleY);
        displaySizeY = topY - bottomY;
        viewPosY = posY * scaleY;
    }
    
    /** check the bounds of the view, to make sure they have not been move beyond the
     * bounds of the scene
     * @param propertyName what changed to cause this check
     */    
    private void checkBounds(String propertyName)
    {
        if(propertyName.equals("scaleXFactor") ||
        propertyName.equals("transformZFactor") ||
        ((propertyName.equals("scaleYFactor") ||
        propertyName.equals("transformZFactor")) && scaleFactorsLocked))
        {
            if(bottomX < minX)
            {
                posX = minX + 0.5 *(diffX / scaleX * scaleXForFit);
                recalculateXValues();
            } else if(topX > maxX)
            {
                posX = maxX - 0.5 *(diffX / scaleX * scaleXForFit);
                recalculateXValues();
            }
        }
        if(propertyName.equals("scaleYFactor") ||
        propertyName.equals("transformZFactor") ||
        ((propertyName.equals("scaleXFactor") ||
        propertyName.equals("transformZFactor")) && scaleFactorsLocked))
        {
            if(bottomY < minY)
            {
                posY = minY + 0.5 *(diffY / scaleY * scaleYForFit);
                recalculateYValues();
            } else if(topY > maxY)
            {
                posY = maxY - 0.5 *(diffY / scaleY * scaleYForFit);
                recalculateYValues();
            }
        }
    }
    
    /** convenience method to emit events
     * @param propertyName The programmatic name of the property that was changed.
     * @param oldValue The old value for the property, expressed as an Object.
     * @param newValue The new value for the property, expressed as an Object.
     */    
    protected void notifyTargets(String propertyName, Object oldValue, Object newValue)
    {
        //			System.out.println("notifyTargets");
        Vector l;
        ViewPositionModelEvent s = new ViewPositionModelEvent((Object) this, propertyName, oldValue, newValue);
        synchronized(this)
        {
            l = (Vector) targets.clone();
        }
        for (int i = 0; i < l.size(); i++)
        {
            ViewPositionModelListener sl = (ViewPositionModelListener) l.elementAt(i);
            //			System.out.println("viewPositionModelChanged " + sl);
            sl.viewPositionModelChanged(s);
        }
    }
    
    /** locks the scale factors */    
    private void lockScaleFactors()
    {
        xOverY = scaleXOnly / scaleYOnly;
    }
    
    /** alter the view parameters because the scale factors are locked
     * @param factorChanged what changed to cause the view parameters to change
     */    
    private void adjustScaleFactorsCosScaleFactorsLocked(String factorChanged)
    {
        if(factorChanged.equals("scaleXFactor"))
        {
            scaleYOnly = scaleXOnly / xOverY;
        }
        if(factorChanged.equals("scaleYFactor"))
        {
            scaleXOnly = scaleYOnly * xOverY;
        }
        
    }
    
    /** fit the view to exactly fill the window
     * @param renderingWindowSize the size of the canvas to which the veiw is displayed
     * @param screenSize the size of the screen
     */    
    public void fitViewToWindow(Dimension renderingWindowSize,
    Dimension screenSize)
    {
        if((renderingWindowSize.width != 0) &&
        (renderingWindowSize.height != 0))
        {
            // System.out.print("b4 ");
            // printValues();
            double xSliderPosition = scaleXOnly / scaleXForFit;
            double ySliderPosition = scaleYOnly / scaleYForFit;
            
            if(windowEyepointPolicy == View.RELATIVE_TO_WINDOW)
            {
                double heightRelativeToScreen = 
										(double) renderingWindowSize.height
										/ (double) screenSize.width;
                double widthRelativeToScreen = 
										(double) renderingWindowSize.width
										/ (double) screenSize.width;
                
                scaleXForFit = scaleXForFitOriginal * widthRelativeToScreen
                / RELATIVE_TO_WINDOW_FUDGE_FACTOR;
                scaleYForFit = scaleYForFitOriginal * heightRelativeToScreen
                / RELATIVE_TO_WINDOW_FUDGE_FACTOR;
                
            }
            else if(windowEyepointPolicy == View.RELATIVE_TO_FIELD_OF_VIEW)
            {
                if(renderingWindowSize.width > renderingWindowSize.height)
                {
                    scaleXForFit = scaleXForFitOriginal;
                    scaleYForFit = scaleYForFitOriginal
                    / renderingWindowSize.width
                    * renderingWindowSize.height;
                }
                else
                {
                    scaleYForFit = scaleYForFitOriginal;
                    scaleXForFit = scaleXForFitOriginal
                    / renderingWindowSize.height
                    * renderingWindowSize.width;
                }
            }
            //				recalculateValues();
            scaleXOnly = scaleX = scaleXForFit * xSliderPosition;
            scaleYOnly = scaleY = scaleYForFit * ySliderPosition;
            
            // not sure about this one
            overallScaleX = overallScaleXOriginal
            = overallScaleOriginal * scaleXForFit;
            overallScaleY = overallScaleYOriginal
            = overallScaleOriginal * scaleYForFit;
            
            
            // resetViewPosition();
            recalculateValues();
            
            // System.out.print("af ");
            // printValues();
            //				notifyTargets("scaleYFactor", new Double(overallScale),
            //											new Double(overallScale));
            //				notifyTargets("scaleYFactor", new Double(overallScale),
            //											new Double(overallScale));
            notifyTargets("fitViewToWindow", new Double(overallScale),
            new Double(overallScale));
        }
        
    }
    
    /** sets the view position to full zoom out */    
    private void resetViewPosition()
    {
        // // System.out.println("Model Reset");
        //			printValues();
        diffX = maxX - minX;
        diffY = maxY - minY;
        posX = minX + diffX / 2;
        posY = minY + diffY / 2;
        //      scaleXOnly = scaleYOnly = scaleX = scaleY = 1;
        scaleXOnly = scaleX = scaleXForFit;
        scaleYOnly = scaleY = scaleYForFit;
        overallScale = overallScaleOriginal;
        xOverY = 1;
        recalculateValues();
        //			printValues();
    }
    
    
    /** adds listener
     * @param listener the listener
     */    
    public synchronized void addViewPositionModelListener
				(ViewPositionModelListener listener)
    {
        targets.addElement(listener);
    }
    
    /** removes a listener
     * @param listener the listener
     */    
    public synchronized void removeViewPositionModelListener(ViewPositionModelListener listener)
    {
        targets.removeElement(listener);
    }
    
    /** respond to a parser changed event
     * @param e the event
     */    
    public void parserChanged(ParserEvent e)
    {
        double[] wholeValues = ((Parser)e.getSource()).calculateMaxMinValues();
        double[] indavidualValues =
						((Parser)e.getSource()).calculateMaxMinValues(graphicNumber);
        
                /*
                System.out.println(" = " + );
                 */
        
        if(wholeValues[0] == wholeValues[1])
        {
            minX = -1;
            maxX = 1;
        }
        else
        {
            minX = wholeValues[0];
            maxX = wholeValues[1];
        }
        if(indavidualValues[2] == indavidualValues[3])
        {
            minY = -1;
            maxY = 1;
        }
        else
        {
            minY = indavidualValues[2];
            maxY = indavidualValues[3];
        }
        
        minMaxValuesChanged();
        
        notifyTargets("parser", ((Parser)e.getSource()), ((Parser)e.getSource()));
    }
    
    /** respond to the min max values changing (if the scene has changed) */    
    public void minMaxValuesChanged()
    {
        
        diffX = maxX - minX;
        diffY = maxY - minY;
        posX = minX + diffX / 2;
        posY = minY + diffY / 2;
        scaleXOnly = scaleYOnly = scaleX = scaleY = 1;

        overallScale = overallScaleOriginal = Math.max(diffX, diffY) *
        Math.pow(6, 0.5) / 2;
        
        
        if(diffX > diffY)
        {
            scaleYForFit = scaleYForFitOriginal = diffX / diffY;
            scaleXForFit = scaleXForFitOriginal = 1;
        } else
        {
            scaleXForFit = scaleXForFitOriginal = diffY / diffX;
            scaleYForFit = scaleYForFitOriginal = 1;
        }
        
        scaleXOnly = scaleX = scaleXForFit;
        scaleYOnly = scaleY = scaleYForFit;
        
        overallScaleX = overallScaleXOriginal = overallScaleOriginal * scaleXForFit;
        overallScaleY = overallScaleYOriginal = overallScaleOriginal * scaleYForFit;
        
        // System.out.println("overallScale123 = " + overallScale);
        recalculateValues();
        
    }
    
    /** respond to a change, used for many of the functions below
     * @param e the event
     */    
    public void viewPositionChanged(PropertyChangeEvent e)
    {
        //			printValues();
        if(e.getPropertyName().equals("scaleXFactor"))
        {
            scaleXOnly = ((double) ((Double) e.getNewValue()).doubleValue()) * scaleXForFit;
            if(scaleFactorsLocked)
            {
                adjustScaleFactorsCosScaleFactorsLocked("scaleXFactor");
            }
            recalculateValues();
        }
        if(e.getPropertyName().equals("transformXFactor"))
        {
            bottomX = (double) ((Double) e.getNewValue()).doubleValue();
            posX = bottomX + 0.5 * (diffX / scaleX * scaleXForFit);
            recalculateValues();
        }
        if(e.getPropertyName().equals("scaleYFactor"))
        {
            scaleYOnly = ((double) ((Double) e.getNewValue()).doubleValue()) * scaleYForFit;
            if(scaleFactorsLocked)
            {
                adjustScaleFactorsCosScaleFactorsLocked("scaleYFactor");
            }
            recalculateValues();
        }
        if(e.getPropertyName().equals("transformYFactor"))
        {
            topY = ((double) ((Double) e.getNewValue()).doubleValue());
            posY = topY - 0.5 *(diffY / scaleY * scaleYForFit);
            recalculateValues();
        }
        if(e.getPropertyName().equals("transformZFactor"))
        {
            overallScale = (double) ((Double) e.getNewValue()).doubleValue();
            recalculateValues();
            //			printValues();
        }
        if(e.getPropertyName().equals("lockScaleFactors"))
        {
            scaleFactorsLocked = ((Boolean) e.getNewValue()).booleanValue();
            if(scaleFactorsLocked)
            {
                lockScaleFactors();
            }
            recalculateValues();
            //			printValues();
        }
        if(e.getPropertyName().equals("reset"))
        {
            resetViewPosition();
            // System.out.println("reset in vpm");
        }
        checkBounds(e.getPropertyName());
        notifyTargets(e.getPropertyName(), (e.getOldValue()), (e.getNewValue()));
    }
    
    /** respond to a controls change
     * @param e the event
     */    
    public void controlContainerChanged(ControlContainerEvent e)
    {
        viewPositionChanged(e);
    }
    
    /** respond to a GroutPanel change
     * @param e the event
     */    
    public void groutPanelChanged(GroutPanelEvent e)
    {
        viewPositionChanged(e);
    }
    
    /** respond to a view poisition change event
     * @param e the event
     */    
    public void viewPositionModelChanged(ViewPositionModelEvent e)
    {
        viewPositionChanged(e);
    }
    
    /** depricated
     * @param propertyChangeEvent the event
     */    
    public void propertyChange(java.beans.PropertyChangeEvent propertyChangeEvent)
    {
    }
    
    /** respond to a change to the rendWindContainer
     * @param e the event
     */    
    public void rendWindContainerChanged(RendWindContainerEvent e)
    {
        String whatChanged = e.getPropertyName();
        Canvas3D canvas = ((RendWindContainer) e.getSource()).getCanvas();
        windowEyepointPolicy = canvas.getView().getWindowEyepointPolicy();
        Dimension screenSize = canvas.getScreen3D().getSize();
        
        if(whatChanged.startsWith("FitViewToWindow"))
        {
            Dimension renderingWindowSize = 
								((RendWindContainer) e.getSource()).getCanvas().getSize();
            // fitViewToWindow(renderingWindowSize, screenSize);
            // dirty hack, but works hugh
            fitViewToWindow(renderingWindowSize, new Dimension(1280, 1024));
        }
    }
    
    /** sets the view to incude the area given by the top and bottom points
     * @param bottom the bottom point
     * @param top the top point
     */    
    public void setViewPosition(Point3d bottom, Point3d top)
    {
        //				System.out.println("setViewPosition to " + top + bottom);
        //				printValues();
        topX = top.x;
        topY = top.y;
        bottomX = bottom.x;
        bottomY = bottom.y;
        
        displaySizeX = topX - bottomX;
        displaySizeY = topY - bottomY;
        
        posX = (topX + bottomX) / 2;
        posY = (topY + bottomY) / 2;
        scaleXOnly = diffX / displaySizeX * scaleXForFit;
        scaleYOnly = diffY / displaySizeY * scaleYForFit;
        
        recalculateValues();
        
        checkBounds("scaleX");
        checkBounds("scaleY");
        //				printValues();
        notifyTargets("fitViewToWindow", new Double(overallScale),
        new Double(overallScale));
        
    }
    
    /** sets the view to include a proportion of the pressent view
     * @param changeBottomXBy proportion to change Bottom X value By
     * @param changeBottomYBy proportion to change Bottom Y value By
     * @param changeTopXBy proportion to change Top X value By
     * @param changeTopYBy proportion to change Top Y value By
     */    
    public void setFractionalWindow(double changeBottomXBy,
    double changeBottomYBy,
    double changeTopXBy,
    double changeTopYBy)
    {
        Point3d top = new Point3d();
        Point3d bottom = new Point3d();
        
        top.x = changeTopXBy * displaySizeX + bottomX;
        bottom.x = changeBottomXBy * displaySizeX + bottomX;
        top.y = changeTopYBy * displaySizeY + bottomY;
        bottom.y = changeBottomYBy * displaySizeY + bottomY;
        
        setViewPosition(bottom, top);
    }
    
    /** makes a copy of this object, includeing the Exception handling
     * @return the copy
     */    
    public ViewPositionModel makeCopy()
    {
        try
        {
            return (ViewPositionModel) clone();
        }
        catch(CloneNotSupportedException e)
        {
            e.printStackTrace();
            return null;
        }
    }
    
}
