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
*  @author: Copyright (C) Tim Carver
*
***************************************************************/

package org.emboss.jemboss.gui.form;

import javax.swing.*;
import java.awt.*;

public class ApplicationNamePanel extends JPanel
{

  private int x;
  private int y;
  private int fontSize;
  private String name;

  public ApplicationNamePanel(String name, int x, int y, 
                                  int fontSize)
  {
    this.name = name;
    this.x = x;
    this.y = y;
    this.fontSize = fontSize;
    
    Font font = new Font("Dialog", Font.BOLD, fontSize);
    setFont(font);
    FontMetrics metrics = getFontMetrics(font);
    int width = metrics.stringWidth(name);
    setPreferredSize(new Dimension(width, fontSize));   
    setBackground(Color.white);
  } 

  public void paintComponent(Graphics g)
  {
    super.paintComponent(g);
    int line_spacing = 4;

//  g.setFont(new Font("Dialog", Font.BOLD, fontSize));
    Color top_color = new Color(200,10,0);
    Color side_color = new Color(200,10,0);

    for (int i = 0; i < 2; i++)
    {
      g.setColor(top_color);
      g.drawString(name, ShiftEast(x, i), ShiftNorth(ShiftSouth(y, i), 1));
      g.setColor(side_color);
      g.drawString(name, ShiftWest(ShiftEast(x, i), 1), ShiftSouth(y, i));
    }
    g.setColor(Color.red);
    g.drawString(name, ShiftEast(x, 1), ShiftSouth(y, 1));

  }

  

  int ShiftNorth(int p, int distance) 
  {
    return (p - distance);
  }

  int ShiftSouth(int p, int distance) 
  {
    return (p + distance);
  }

  int ShiftEast(int p, int distance) 
  {
    return (p + distance);
  }

  int ShiftWest(int p, int distance) 
  {
    return (p - distance);
  }

}

