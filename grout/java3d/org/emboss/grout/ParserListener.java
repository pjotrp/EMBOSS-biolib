/*
 * ParserListener.java
 *
 * Created on April 24, 2002, 1:42 PM
 */

package org.emboss.grout;

import java.beans.PropertyChangeListener;
/**
 *
 * @author  hmorgan2
 */
public interface ParserListener
{
  
  public abstract void parserChanged(ParserEvent e);
  
}

