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
/**
 *
 * @author  hmorgan2
 */
public class ViewPositionModel implements ParserListener, 
																					ControlContainerListener, 
																					ViewPositionModelListener, 
		GroutPanelListener,
		RendWindContainerListener
{
  
  private double minX; //lowest x value of scene graph
  private double maxX; //highest x value of scene graph
  private double minY; //lowest y value of scene graph
  private double maxY; //highest y value of scene graph
  private double bottomX; //lowest x value of display
  private double topX; //highest x value of display
  private double bottomY; //lowest y value of display
  private double topY; //highest y value of display
  
  private double diffX; //x extent of scene graph
  private double diffY; //y extent of scene graph
  private double displaySizeX; //x extent of display
  private double displaySizeY; //y extent of display
  
  private double posX; //x centre of display, deviation from bottom of graph
  private double posY; //y centre of display, deviation from bottom of graph
  private double viewPosX; //x centre of display, deviation from bottom of graph, scaled so this is pos of viewpoint
  private double viewPosY; //y centre of display, deviation from bottom of graph, scaled so this is pos of viewpoint
  
  private double scaleXOnly; // total x scale
  private double scaleYOnly; // total y scale
  private double scaleX; // total x scale
  private double scaleY; // total y scale
  private double overallScale; // overall scale, from scale Z factor
  private double overallScaleX; // overall scale, from scale Z factor
  private double overallScaleY; // overall scale, from scale Z factor
  private double overallScaleOriginal; // overall scale when zoomed right out
  private double overallScaleXOriginal; // overall scale when zoomed right out
  private double overallScaleYOriginal; // overall scale when zoomed right out

  private double scaleXForFit; // scale for X axis that makes it fit the view when fully zoomed out
  private double scaleYForFit; // scale for Y axis that makes it fit the view when fully zoomed out

  private double scaleXForFitOriginal; // scale for X axis that makes it fit the view when fully zoomed out Original
  private double scaleYForFitOriginal; // scale for Y axis that makes it fit the view when fully zoomed out Original

  
  private boolean scaleFactorsLocked = false;
  private double xOverY;
  
  private Vector targets = new Vector();
  
  private static double root2overRoot3 = (Math.pow(2, 0.5) / Math.pow(3, 0.5)) * 10;
		private static double RELATIVE_TO_WINDOW_FUDGE_FACTOR = 1.035; // 1.035
		private int windowEyepointPolicy;

		private int graphicNumber;

  /** Creates a new instance of ViewPositionModel */
  public ViewPositionModel()
  {
  }
  
  public double getDiffX()
  {
    return diffX;
  }
  public double getTopX()
  {
    return topX;
  }
  public double getBottomX()
  {
    return bottomX;
  }
  public double getMinX()
  {
    return minX;
  }
  public double getMaxX()
  {
    return maxX;
  }
  public double getDisplaySizeX()
  {
    return displaySizeX;
  }
  public double getPosX()
  {
    return posX;
  }
  public double getViewPosX()
  {
    return viewPosX;
  }
  public double getDiffY()
  {
    return diffY;
  }
  public double getTopY()
  {
    return topY;
  }
  public double getBottomY()
  {
    return bottomY;
  }
  public double getMinY()
  {
    return minY;
  }
  public double getMaxY()
  {
    return maxY;
  }
  public double getDisplaySizeY()
  {
    return displaySizeY;
  }
  public double getPosY()
  {
    return posY;
  }
  public double getViewPosY()
  {
    return viewPosY;
  }
  public double getOverallScale()
  {
    return overallScale;
  }
  public double getScaleX()
  {
    return scaleX;
  }
  public double getScaleY()
  {
    return scaleY;
  }
  public double getScaleXOnly()
  {
    return scaleXOnly;
  }
  public double getScaleYOnly()
  {
    return scaleYOnly;
  }
  public double getScaleXForFit()
  {
    return scaleXForFit;
  }
  public double getScaleYForFit()
  {
    return scaleYForFit;
  }
  public boolean getScaleFactorsLocked()
  {
    return scaleFactorsLocked;
  }
  public double getXOverY()
  {
    return xOverY;
  }
  
  public void setGraphicNumber(int no)
  {
			graphicNumber = no;
			return;
  }

  public void printValues()
  {
    System.out.println("minX = " + minX + " maxX = " + maxX + " minY = " + minY + " maxY = " + maxY + " bottomX = " + bottomX + " topX = " + topX + " diffX = " + diffX + " displaySizeX = " + displaySizeX + " posX = " + posX + " scaleX = " + scaleX + " overallScale = " + overallScale + " displaySizeY = " + displaySizeY);//  + "  = "
		//    System.out.println("minX = " + minX + " maxX = " + maxX + " bottomX = " + bottomX + " topX = " + topX + " diffX = " + diffX + " displaySizeX = " + displaySizeX + " posX = " + posX + " scaleX = " + scaleX + " overallScale = " + overallScale + " scaleXOnly = " + scaleXOnly + " scaleXForFit = " + scaleXForFit + " overallScaleX = " + overallScaleX + " overallScaleXOriginal = " + overallScaleXOriginal + " viewPosX = " + viewPosX);//  + "  = "
    System.out.println("minY = " + minY + " maxY = " + maxY + " bottomY = " + bottomY + " topY = " + topY + " diffY = " + diffY + " displaySizeY = " + displaySizeY + " posY = " + posY + " scaleY = " + scaleY + " overallScale = " + overallScale + " scaleYOnly = " + scaleYOnly + " scaleYForFit = " + scaleYForFit + " overallScaleY = " + overallScaleY + " overallScaleYOriginal = " + overallScaleYOriginal + " viewPosY = " + viewPosY);//  + "  = "
  }
  
  private void recalculateValues()
  {
    // System.out.println("overallScaleOriginal = " + overallScaleOriginal);
    double overallScaleRelativeToFullZoomOut = overallScaleOriginal / overallScale;
    double overallScaleXRelativeToFullZoomOut = overallScaleXOriginal / overallScaleX;
    double overallScaleYRelativeToFullZoomOut = overallScaleYOriginal / overallScaleY;
    // System.out.println("overallScaleRelativeToFullZoomOut = " + overallScaleRelativeToFullZoomOut);
		/*
    scaleX = scaleXOnly * scaleXForFit;
    scaleY = scaleYOnly * scaleYForFit;
		*/
    scaleX = overallScaleXRelativeToFullZoomOut * scaleXOnly;
    scaleY = overallScaleYRelativeToFullZoomOut * scaleYOnly;
    
    // // System.out.println("overallScaleRelativeToFullZoomOut = " + overallScaleRelativeToFullZoomOut);
    
		//		System.out.println("scaleY = overallScaleYRelativeToFullZoomOut");
		//		printValues();
		
    //        scaleX = scaleXOnly * -root2overRoot3;
    bottomX = posX - 0.5 * (diffX  * scaleXForFit/ scaleX);
    topX = posX + 0.5 *(diffX  * scaleXForFit/ scaleX);
    bottomY = posY - 0.5 *(diffY  * scaleYForFit/ scaleY);
    topY = posY + 0.5 *(diffY  * scaleYForFit/ scaleY);
    
		//		System.out.println("topY = posY + 0.5 *(diffY  * scaleYForFit/ scaleY");
		//		printValues();
		
    // System.out.println("checkBounds posX = " + posX + " diffX = " +diffX  + " scaleXForFit = " + scaleXForFit + " scaleX = " + scaleX + " bottomX = " + bottomX);
		/*
    bottomX = posX - 0.5 * (displaySizeX  * scaleXForFit/ scaleX);
    topX = posX + 0.5 *(displaySizeX  * scaleXForFit/ scaleX);
    bottomY = posY - 0.5 *(displaySizeY  * scaleYForFit/ scaleY);
    topY = posY + 0.5 *(displaySizeY  * scaleYForFit/ scaleY);
		*/
    displaySizeX = topX - bottomX;
    displaySizeY = topY - bottomY;
/*		viewPosX = posX * scaleX;
                viewPosY = posY * scaleY; */

		/*
    viewPosX = posX * scaleXOnly * scaleXForFit;
    viewPosY = posY * scaleYOnly * scaleYForFit;
		*/
    viewPosX = posX * scaleXOnly;
    viewPosY = posY * scaleYOnly;
  }
  
  private void recalculateXValues()
  {
    bottomX = posX - 0.5 *(diffX * scaleXForFit / scaleX);
    topX = posX + 0.5 *(diffX * scaleXForFit / scaleX);
    displaySizeX = topX - bottomX;
    viewPosX = posX * scaleX;
  }
  
  private void recalculateYValues()
  {
    bottomY = posY - 0.5 *(diffY * scaleYForFit / scaleY);
    topY = posY + 0.5 *(diffY * scaleYForFit / scaleY);
    displaySizeY = topY - bottomY;
    viewPosY = posY * scaleY;
  }
  
  private void checkBounds(String propertyName)
  {
    if(propertyName.equals("scaleXFactor") || propertyName.equals("transformZFactor") || ((propertyName.equals("scaleYFactor") || propertyName.equals("transformZFactor")) && scaleFactorsLocked))
    {
      if(bottomX < minX)
      {
					// System.out.println("checkBounds bottomX < minX posX = " + posX + " diffX = " + diffX + " scaleX = " + scaleX + " scaleXForFit = " + scaleXForFit + " bottomX = " + bottomX + " minX = " + minX);
        posX = minX + 0.5 *(diffX / scaleX * scaleXForFit);
        recalculateXValues();
					// System.out.println("checkBounds bottomX < minX posX = " + posX + " diffX = " + diffX + " scaleX = " + scaleX + " scaleXForFit = " + scaleXForFit + " bottomX = " + bottomX + " minX = " + minX);
      } else if(topX > maxX)
      {
        posX = maxX - 0.5 *(diffX / scaleX * scaleXForFit);
        recalculateXValues();
      }
    }
    if(propertyName.equals("scaleYFactor") || propertyName.equals("transformZFactor") || ((propertyName.equals("scaleXFactor") || propertyName.equals("transformZFactor")) && scaleFactorsLocked))
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
  
  private void lockScaleFactors()
  {
    // // System.out.println("scaleFactorsLocked = " + scaleFactorsLocked + " to " + xOverY);
    xOverY = scaleXOnly / scaleYOnly;
  }
  
  private void adjustScaleFactorsCosScaleFactorsLocked(String factorChanged)
  {
    if(factorChanged.equals("scaleXFactor"))
    {
				//      System.out.println("scaleFactorsLocked so y factor changed to = " + (scaleXOnly * scaleXForFit  / xOverY));
      scaleYOnly = (scaleXOnly / xOverY) / (scaleXForFit / scaleYForFit) ;
			//      System.out.println("scaleFactorsLocked so y factor changed to = " + scaleYOnly);
			// I guess I have to remove this.  Perhaps I should only do it if the view 
			// is RELATIVE_TO_WINDOW
			/*
      if(scaleYOnly < 1)
      {
        scaleYOnly = 1;
      }
			*/
			//      System.out.println("scaleFactorsLocked 2 so y factor changed to = " + scaleYOnly);
      //			notifyTargets("scaleYFactor", new Double((double) scaleYOnly), new Double((double) (scaleXOnly / xOverY)));
      //		notifyTargets(e.getPropertyName(), (e.getOldValue()), (e.getNewValue()));
      //			scaleYOnly = scaleXOnly * xOverY;
    }
    if(factorChanged.equals("scaleYFactor"))
    {
				//      System.out.println("scaleFactorsLocked so x factor changed to = " + (scaleYOnly * xOverY));
      scaleXOnly = scaleYOnly * xOverY;
			/*
      if(scaleXOnly < 1)
      {
        scaleXOnly = 1;
      }
			*/
      //			notifyTargets("scaleXFactor", new Double((double) scaleXOnly), new Double((double) (scaleYOnly * xOverY)));
      //		notifyTargets(e.getPropertyName(), (e.getOldValue()), (e.getNewValue()));
      //			scaleYOnly = scaleXOnly * xOverY;
    }
    
  }

  private void fitViewToWindow(Dimension renderingWindowSize,
															 Dimension screenSize)
		{
			  // System.out.println("fitViewToWindow dim = " + renderingWindowSize + " screen = " + screenSize);
				if(windowEyepointPolicy == 
					 View.RELATIVE_TO_WINDOW)
				{
						double heightRelativeToScreen = (double) renderingWindowSize.height / (double) screenSize.width;
						double widthRelativeToScreen = (double) renderingWindowSize.width / (double) screenSize.width;

						scaleXForFit = scaleXForFitOriginal * widthRelativeToScreen / RELATIVE_TO_WINDOW_FUDGE_FACTOR;
						scaleYForFit = scaleYForFitOriginal * heightRelativeToScreen / RELATIVE_TO_WINDOW_FUDGE_FACTOR;

						// System.out.println("heightRelativeToScreen = " + heightRelativeToScreen + " widthRelativeToScreen = " + widthRelativeToScreen + " scaleXForFit = " + scaleXForFit + " scaleXForFitOriginal = " + scaleXForFitOriginal + " scaleYForFit = " + scaleYForFit + " scaleYForFitOriginal = " + scaleYForFitOriginal);
						/*
							if(heightRelativeToScreen < widthRelativeToScreen)
							{
							overallScale = overallScaleOriginal / heightRelativeToScreen;
							// System.out.println(" heightRelativeToScreen = " + heightRelativeToScreen + " screenSize.height = " + screenSize.height + " size.height = " + renderingWindowSize.height );
							} 
							else
							{
							overallScale = overallScaleOriginal / widthRelativeToScreen;
							// System.out.println(" widthRelativeToScreen = " + widthRelativeToScreen + " screenSize.width = " + screenSize.width + " size.width = " + renderingWindowSize.width );
							}
							// System.out.println("Reseting overallScale to = " + overallScale);
							//								recalculateValues();
							//								notifyTargets("transformZFactor", new Double(overallScale), 
							//															new Double(overallScale));
							*/
				} 
				else if(windowEyepointPolicy == 
								View.RELATIVE_TO_FIELD_OF_VIEW)
				{
						// System.out.println("windowEyepointPolicy = RELATIVE_TO_FIELD_OF_VIEW");
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
				scaleXOnly = scaleX = scaleXForFit;
				scaleYOnly = scaleY = scaleYForFit;

				// not sure about this one
				overallScaleX = overallScaleXOriginal 
						= overallScaleOriginal * scaleXForFit;
				overallScaleY = overallScaleYOriginal 
						= overallScaleOriginal * scaleYForFit;
				/*
					overallScaleX = overallScaleOriginal * scaleXForFit;
					overallScaleY = overallScaleOriginal * scaleYForFit;
				*/
				

				resetViewPosition();
				//				printValues();
				//				notifyTargets("scaleYFactor", new Double(overallScale), 
				//											new Double(overallScale));
//				notifyTargets("scaleYFactor", new Double(overallScale), 
//											new Double(overallScale));
				notifyTargets("fitViewToWindow", new Double(overallScale), 
											new Double(overallScale));
				
		}

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

  
  public synchronized void addViewPositionModelListener(ViewPositionModelListener listener)
  {
    targets.addElement(listener);
  }
  
  public synchronized void removeViewPositionModelListener(ViewPositionModelListener listener)
  {
    targets.removeElement(listener);
  }
  
  public void parserChanged(ParserEvent e)
  {
    double[] wholeValues = ((Parser)e.getSource()).calculateMaxMinValues();
    double[] indavidualValues = ((Parser)e.getSource()).calculateMaxMinValues(graphicNumber);
    
    minX = wholeValues[0];
    maxX = wholeValues[1];
    minY = indavidualValues[2];
    maxY = indavidualValues[3];
    diffX = maxX - minX;
    diffY = maxY - minY;
    posX = minX + diffX / 2;
    posY = minY + diffY / 2;
    scaleXOnly = scaleYOnly = scaleX = scaleY = 1;
    //		overallScale = overallScaleOriginal = values[4] * Math.pow(3, 0.5);
    overallScale = overallScaleOriginal = Math.max(diffX, diffY) *  Math.pow(6, 0.5) / 2;

		//		System.out.println("parserChanged, maxX = " + maxX + " parser = " + ((Parser)e.getSource()).toString());

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
		
    notifyTargets("parser", ((Parser)e.getSource()), ((Parser)e.getSource()));
  }
  
  public void viewPositionChanged(PropertyChangeEvent e)
  {
			//			System.out.println(e.getPropertyName() + " changed in model to " + e.getNewValue());
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
      // // System.out.println("viewPositionChanged by scaleYFactor to " + scaleYOnly);
      if(scaleFactorsLocked)
      {
        // // System.out.println("so adjustScaleFactorsCosScaleFactorsLocked");
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
    }
    checkBounds(e.getPropertyName());
    notifyTargets(e.getPropertyName(), (e.getOldValue()), (e.getNewValue()));
  }
/*    public void controlContainerChanged(ControlContainerEvent e)
    {
//        printValues();
                // // System.out.println(e.getPropertyName() + " changed in model to " + e.getNewValue());
                if(e.getPropertyName().equals("scaleXFactor"))
        {
            scaleXOnly = (double) ((Double) e.getNewValue()).doubleValue();
            if(scaleFactorsLocked)
                        {
                                adjustScaleFactorsCosScaleFactorsLocked("scaleXFactor");
                        }
                        recalculateValues();
        }
        if(e.getPropertyName().equals("transformXFactor"))
        {
            bottomX = (double) ((Double) e.getNewValue()).doubleValue();
            posX = bottomX + 0.5 *(diffX / scaleX);
            recalculateValues();
        }
        if(e.getPropertyName().equals("scaleYFactor"))
        {
            scaleYOnly = (double) ((Double) e.getNewValue()).doubleValue();
            recalculateValues();
        }
        if(e.getPropertyName().equals("transformYFactor"))
        {
            topY = ((double) ((Double) e.getNewValue()).doubleValue());
            posY = topY - 0.5 *(diffY / scaleY);
            recalculateValues();
        }
        if(e.getPropertyName().equals("transformZFactor"))
        {
            overallScale = (double) ((Double) e.getNewValue()).doubleValue();
            recalculateValues();
                        printValues();
        }
        if(e.getPropertyName().equals("lockScaleFactors"))
        {
            scaleFactorsLocked = ((Boolean) e.getNewValue()).booleanValue();
                        if(scaleFactorsLocked)
                        {
                                lockScaleFactors();
                        }
            recalculateValues();
                        printValues();
        }
                checkBounds(e.getPropertyName());
                notifyTargets(e.getPropertyName(), (e.getOldValue()), (e.getNewValue()));
    }*/
  
  public void controlContainerChanged(ControlContainerEvent e)
  {
			//			System.out.print("No " + graphicNumber + " " + this.toString().substring(34) + " ");
			//			System.out.println(e.getPropertyName());
			//			printValues();
    viewPositionChanged(e);
  }

  public void groutPanelChanged(GroutPanelEvent e)
  {
    viewPositionChanged(e);
  }

  public void viewPositionModelChanged(ViewPositionModelEvent e)
  {
    // // System.out.println("viewPositionModelChanged in viewPositionModel to " + e.getPropertyName());
    viewPositionChanged(e);
  }
  
  public void propertyChange(java.beans.PropertyChangeEvent propertyChangeEvent)
  {
  }
  
	public void rendWindContainerChanged(RendWindContainerEvent e)
		{
				String whatChanged = e.getPropertyName();
				Canvas3D canvas = ((RendWindContainer) e.getSource()).getCanvas();
				windowEyepointPolicy = canvas.getView().getWindowEyepointPolicy();
				Dimension screenSize = canvas.getScreen3D().getSize();
				// System.out.println("whatChanged fitViewToWindow = " + whatChanged);
				/*
				if(whatChanged.startsWith("RendWindContainerResized"))
				{
						Dimension size = (Dimension) e.getNewValue();
						// System.out.println("size = " + size);
						// System.out.println("rendWindContainerChanged");
						// System.out.println("getWindowEyepointPolicy = " + windowEyepointPolicy);
						// System.out.println("RELATIVE_TO_SCREEN = " + View.RELATIVE_TO_SCREEN);
						// System.out.println("RELATIVE_TO_WINDOW = " + View.RELATIVE_TO_WINDOW);
						// System.out.println("RELATIVE_TO_FIELD_OF_VIEW = " + View.RELATIVE_TO_FIELD_OF_VIEW);
						if(windowEyepointPolicy == 
							 View.RELATIVE_TO_WINDOW)
						{
								double heightRelativeToScreen = (double) size.height / (double) screenSize.width;
								double widthRelativeToScreen = (double) size.width / (double) screenSize.width;
								if(heightRelativeToScreen < widthRelativeToScreen)
								{
										overallScale = overallScaleOriginal / heightRelativeToScreen;
										// System.out.println(" heightRelativeToScreen = " + heightRelativeToScreen + " screenSize.height = " + screenSize.height + " size.height = " + size.height );
								} 
								else
								{
										overallScale = overallScaleOriginal / widthRelativeToScreen;
										// System.out.println(" widthRelativeToScreen = " + widthRelativeToScreen + " screenSize.width = " + screenSize.width + " size.width = " + size.width );
								}
								// System.out.println("Reseting overallScale to = " + overallScale);
								recalculateValues();
								notifyTargets("transformZFactor", new Double(overallScale), 
															new Double(overallScale));
						} 
						else if(windowEyepointPolicy == 
										View.RELATIVE_TO_FIELD_OF_VIEW)
						{
								if(size.width > size.height)
								{
										overallScale = overallScaleOriginal * size.width 
												/ size.height;
										recalculateValues();
										notifyTargets("transformZFactor", new Double(overallScale),
																	new Double(overallScale));
										// System.out.println("Reseting overallScale to = " + overallScale);
								} else
								{
										overallScale = overallScaleOriginal;
										recalculateValues();
										notifyTargets("transformZFactor", new Double(overallScale),
																	new Double(overallScale));
								}
						}
				} 
				else
				*/
				if(whatChanged.startsWith("FitViewToWindow"))
				{
						// System.out.println("setting FitViewToWindow in model = " + e);
						Dimension renderingWindowSize = ((RendWindContainer) e.getSource()).getCanvas().getSize();
						fitViewToWindow(renderingWindowSize, screenSize);
				}
		}

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
				/*
				scaleXOnly = scaleX * displaySizeX / diffX;
				scaleYOnly = scaleY * displaySizeY / diffY;
				scaleXOnly = diffX * displaySizeX / scaleX;
				scaleYOnly = diffY * displaySizeY / scaleY;
				scaleXOnly = scaleXForFit / (displaySizeX * (overallScaleXOriginal / overallScaleX));
				scaleYOnly = scaleYForFit / (displaySizeY * (overallScaleYOriginal / overallScaleY));
				*/
				scaleXOnly = diffX / displaySizeX * scaleXForFit;
				scaleYOnly = diffY / displaySizeY * scaleYForFit;

//				System.out.println("recalculateValues setViewPosition to " + top + bottom);
				//				printValues();

				recalculateValues();
				// perhaps sort this out hugh
//				System.out.println("checkBounds setViewPosition to " + top + bottom);
//				printValues();

				checkBounds("scaleX");
				checkBounds("scaleY");
//				System.out.println("done setViewPosition to " + top + bottom);
//				printValues();
				notifyTargets("fitViewToWindow", new Double(overallScale), 
											new Double(overallScale));
//				System.out.println("notifyTargets setViewPosition to " + top + bottom);

		}

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

}
