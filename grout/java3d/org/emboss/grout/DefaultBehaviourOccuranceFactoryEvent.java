/*
 * DefaultBehaviourOccuranceFactoryEvent.java
 *
 * Created on June 24, 2002, 11:46 AM
 */

package org.emboss.grout;

import java.util.EventObject;
import java.beans.PropertyChangeEvent;

/**
 *
 * @author  hmorgan2
 */
public class DefaultBehaviourOccuranceFactoryEvent extends PropertyChangeEvent
{
  
  private DefaultBehaviourOccuranceFactory source;
  
  /** Creates a new instance of DefaultBehaviourOccuranceFactoryEvent */
  public DefaultBehaviourOccuranceFactoryEvent(Object source, String propertyName, Object oldValue, Object newValue)
  {
    super(source, propertyName, oldValue, newValue);
    this.source = (DefaultBehaviourOccuranceFactory) source;
    
  }
  
}
