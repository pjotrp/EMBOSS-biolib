/*
 * ShowSequenceOccurance.java
 *
 * Created on June 24, 2002, 11:46 AM
 */

package org.emboss.grout;

public class ShowSequenceOccurance implements PickOccurance
{
  
		private double x1;
		private double y1;
		private double x2;
		private double y2;

		public void ShowSequenceOccurance(double x, double y)
		{
				x1 = x;
				y1 = y;
				x2 = y2 = Integer.MIN_VALUE;
		}

		public void ShowSequenceOccurance(double x1, double y1, double x2, 
																			double y2)
		{
				this.x1 = x1;
				this.y1 = y1;
				this.x2 = x2;
				this.y2 = y2;
		}

		public void PerformPick()
		{
				
		}

}
