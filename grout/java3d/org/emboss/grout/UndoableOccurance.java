/*
 * UndoableOccurance.java
 *
 * Created on June 24, 2002, 11:41 AM
 */

package org.emboss.grout;

/**
 *
 * @author  hmorgan2
 */
public interface UndoableOccurance
{
  public abstract void undo(); 
  public abstract void redo(); 
}
