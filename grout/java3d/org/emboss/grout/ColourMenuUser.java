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

package org.emboss.grout;

import java.awt.Color;
import java.awt.event.MouseEvent;

/** ColourMenuUser.java
 *
 * Interface for an object that instansiates a Colour Menu
 *
 *
 * Created: Thu Mar 13 14:42:13 2003
 * @author <a href="mailto:">Mr H. Morgan</a>
 * @version
 */

public interface ColourMenuUser
{
    /** The colour menu runs this to change the graphic colour
     * @param colourSwing The colour to change it to
     * @param me The mouse event that generated the menu
     * @param tr The transferable graphic that was clicked on
     */    
    public abstract void changeThisGraphicColour(Color colourSwing,
    MouseEvent me,
    TransferableGraphic tr);
}

