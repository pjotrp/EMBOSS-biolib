/*
 * PickOccuranceFactory.java
 *
 * Created on Feb 4th 2003
 */

package org.emboss.grout;

import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;

public interface PickOccuranceFactory extends MouseListener
{
		public void PerformPick(double x, double y);
		public void PerformPick(double x1, double y1, double x2, 
														double y2);


}
