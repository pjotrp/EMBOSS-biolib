/*
 * ParserEvent.java
 *
 * Created on April 24, 2002, 1:21 PM
 */

package org.emboss.grout;

// import javax.media.j3d.BranchGroup;
import java.util.EventObject;
import java.beans.PropertyChangeEvent;
import java.util.Vector;

/**
 *
 * @author  hmorgan2
 */
public class ParserEvent extends PropertyChangeEvent
{
  
  private Parser source;
  
  /** Creates a new instance of ParserEvent with source set */
  public ParserEvent(Object source, String propertyName, Object oldValue, Object newValue)
  {
    super(source, propertyName, oldValue, newValue);
    this.source = (Parser) source;
  }
  
  // convenience method:
  public Vector getGraphics()
  {
    return source.getGraphics();
  }
  
}
