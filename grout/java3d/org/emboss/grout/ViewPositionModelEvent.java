/*
 * ViewPositionModelEvent.java
 *
 * Created on June 24, 2002, 11:46 AM
 */

package org.emboss.grout;

import java.util.EventObject;
import java.beans.PropertyChangeEvent;

/** An event that indicates that the ViewPosition Model has changed
 * @author hmorgan2
 */
public class ViewPositionModelEvent extends PropertyChangeEvent
{
  
    /** The ViewPositionModel that has changed */    
  private ViewPositionModel source;
  
  /** Creates a new instance of ViewPositionModelEvent
   * @param source The ViewPositionModel that has changed
   * @param propertyName An indication of how the ViewPositionModel has 
	 * changed
   * @param oldValue Probably null
   * @param newValue possibly the new value of what changed, but better to 
	 * get it from source
   */
  public ViewPositionModelEvent(Object source, String propertyName, 
																Object oldValue, Object newValue)
  {
    super(source, propertyName, oldValue, newValue);
    this.source = (ViewPositionModel) source;
    
  }
  
}
