/**
 * MyScrollBarEvent.java
 *
 *
 * Created: Mon May 12 09:19:16 2003
 *
 * @author <a href="mailto:">Mr H. Morgan</a>
 * @version
 */

package org.emboss.grout;

import java.util.EventObject;
import java.beans.PropertyChangeEvent;

public class MyScrollBarEvent extends PropertyChangeEvent
{
  
  private MyScrollBar source;
  
  /** Creates a new instance of MyScrollBarEvent */
  public MyScrollBarEvent(Object source, String propertyName, 
													Object oldValue, Object newValue)
  {
    super(source, propertyName, oldValue, newValue);
    this.source = (MyScrollBar) source;
  }
		
}
