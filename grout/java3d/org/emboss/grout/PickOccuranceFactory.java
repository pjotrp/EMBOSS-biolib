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
