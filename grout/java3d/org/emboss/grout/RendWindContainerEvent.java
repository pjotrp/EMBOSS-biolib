package org.emboss.grout;

/**
 * RendWindowContainerEvent.java
 *
 *
 * Created: Thu Mar 13 14:44:13 2003
 *
 * @author <a href="mailto:">Mr H. Morgan</a>
 * @version
 */
import java.util.EventObject;
import java.beans.PropertyChangeEvent;

public class RendWindContainerEvent extends PropertyChangeEvent
{
  
  private RendWindContainer source;
  
  /** Creates a new instance of RendWindContainerEvent with source set */
  public RendWindContainerEvent(Object source, String propertyName, 
																Object oldValue, Object newValue)
  {
    super(source, propertyName, oldValue, newValue);
    this.source = (RendWindContainer) source;
  }
  
}
