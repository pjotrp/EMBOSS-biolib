/***************************************************************
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*  @author: Copyright (C) Hugh Morgan
*
***************************************************************/

/*
 * PositionLabel.java
 *
 */

package org.emboss.grout;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.Vector;
import java.awt.geom.Point2D;
import javax.swing.BorderFactory; 
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;

// public class PositionLabel extends JLabel implements DragableCanvas3DListener
/** This gives the x y coords while the mouse is over a canvas */
public class PositionLabel extends JPanel implements DragableCanvas3DListener
{

    /** the chars to put between the x and y coords */    
		private String betweenXAndY = " ";
		private JLabel label;
		/** not used as the "1st graphic overlaps the 2nd" bug messes it up */
		private Border border;
		private String text = new String("x= " + betweenXAndY + "y=");
		private Font exFont = new Font("TimesRoman",Font.PLAIN,10);

		public PositionLabel()
		{
				super(new BorderLayout());
				label = new JLabel();
				border = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
				// label.setBorder(border);
				setBorder(border);
				label.setText("x= " + betweenXAndY + "y=");
				exFont = label.getFont();
				// setPreferredSize(new Dimension(27, 789));
				add(label, BorderLayout.WEST);
				// addComponentListener(this);
				revalidate();
		}

		public void paint(Graphics g)
		{
				Graphics2D g2d = (Graphics2D)g;
				Dimension size = getSize();
				// System.out.println("size paint = " + size);
				g2d.clearRect(0, 0, size.width, size.height);
				g2d.setColor(Color.gray);
				g2d.drawRect(1, 1, (size.width - 2), (size.height - 2));
				g2d.setColor(Color.black);
				g2d.setFont(exFont);
				g2d.drawString(text, 10, 14);
				
		}

		public void setText(String tx)
		{
				text = tx;
		}

		public void dragableCanvas3DChanged(DragableCanvas3DEvent e)
		{
				if(e.getPropertyName().equals("mouseMoved"))
				{
						// System.out.println("size b4 = " + getSize());
						Point2D point = ((Point2D) e.getNewValue());
						// label.setText("x=" + formatDouble(point.getX()) + betweenXAndY
						// 							+ "y=" + formatDouble(point.getY()));
						setText("x=" + formatDouble(point.getX()) + betweenXAndY
										+ "y=" + formatDouble(point.getY()));
						repaint();
						// System.out.println("size af = " + getSize());
				}
				else if(e.getPropertyName().equals("mouseExited"))
				{
						// System.out.println("mouseExited size af = " + getSize());
						setText("x=" + betweenXAndY + "y=");						
						repaint();
				}
		}

		private String formatDouble(double value)
		{
				String returnString = null;
				if((value <= 0.0) && (value >= -0.000000001))
				{
						returnString = "0.0";
				}
				else if((value >= 10000.0) || (value <= -10000.0) || 
					 ((value <= 0.001) && (value >= -0.001)))
				{
						Double gap = new Double(value);
						Double mantisa = new Double(0.0);
						Integer exponent = new Integer(0);
						Boolean positive = new Boolean(true);

						Vector number = AxisMarks.calculateScientificNotation(gap,
																									mantisa,
																									exponent, 
																									positive);
						mantisa = (Double) number.elementAt(0);
						exponent = (Integer) number.elementAt(1);
						positive = (Boolean) number.elementAt(2);

						returnString = new String();
						if(!positive.booleanValue())
						{
								returnString += "-";
						}
						returnString += mantisa.toString().substring(0, 3);
						// returnString += Integer.toString((int) Math.floor
						// 																 ((mantisa.doubleValue() * 10)
						// 																	+ 0.5));
						returnString += "E";
						returnString += exponent.toString();
						// returnString += Integer.toString(exponent.intValue() - 1);

						// return returnString;
				}
				else if((value >= 10.0) || (value <= -10.0))
				{
						returnString = Integer.toString((int) Math.floor(value + 0.5));
				}
				else if(value < 0)
				{
						returnString = Double.toString(value).substring(0, 5);
				}
				else
				{
						returnString = Double.toString(value);
						if(returnString.length() > 4)
						{
								returnString = Double.toString(value).substring(0, 4);
						}
				}
				if(returnString.equals("-4E-13"))
				{
						System.out.println("returnString = " + returnString + " value = " + value);
				}
				return returnString;
		}

}
