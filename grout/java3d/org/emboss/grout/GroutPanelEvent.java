/*
 * GroutPanelEvent.java
 *
 * Created on October 4th, 2002
 */

package org.emboss.grout;

import java.util.EventObject;
import java.beans.PropertyChangeEvent;

/**
 *
 * @author  hmorgan2
 */
public class GroutPanelEvent extends PropertyChangeEvent
{
  
  private GroutPanel source;
  
  /** Creates a new instance of GroutPanelEvent */
  public GroutPanelEvent(Object source, String propertyName, Object oldValue, Object newValue)
  {
    super(source, propertyName, oldValue, newValue);
    this.source = (GroutPanel) source;
    
  }
  
}
