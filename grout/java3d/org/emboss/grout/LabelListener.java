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
 * LabelListener.java
 *
 */

package org.emboss.grout;

import java.awt.event.ComponentListener;

import javax.swing.*;
import java.awt.event.*;
import java.awt.*;


/** LabelListener.java
 *
 * This Handles clicks on the titles and axis marks
 *
 *
 * Created: Sun Oct 05 22:48:41 2003
 * @author <a href="mailto:"></a>
 * @version 1.0
 */
public class LabelListener extends MouseAdapter implements ColourMenuUser
{
    
    /** Depricated */    
    private long lastClickTime;
    /** If this is listening to an axis marks, then this is them */    
    private AxisMarks axisMarks;
    /** The one graphic this label is in */    
    private OneGraphic oneGraphic;
    /** What sort of label this is listening to */    
    private String type;
    
    public LabelListener()
    {
        
    } // LabelListener constructor
    
    public LabelListener(AxisMarks axisMarks, OneGraphic oneGraphic,
    String type)
    {
        this.axisMarks = axisMarks;
        this.oneGraphic = oneGraphic;
        this.type = type;
    } // LabelListener constructor
    
    public boolean thisEventIsDoubleClick(MouseEvent e)
    {
        return (e.getClickCount() > 1);
    }
    
    /** Change this graphics colour */    
    public void changeThisGraphicColour(Color colourSwing,
    MouseEvent me,
    TransferableGraphic tr)
    {
        String newWhatChanged = "Font" + type + "ColourUser";
        System.out.println(newWhatChanged);
        if(oneGraphic != null)
        {
            oneGraphic.changeFont(newWhatChanged, colourSwing);
        }
        if(axisMarks != null)
        {
            axisMarks.changeFont(newWhatChanged, colourSwing);
        }
        
    }
    
    public void mouseClicked(MouseEvent e)
    {
        if(javax.swing.SwingUtilities.isRightMouseButton(e))
        {
            final Component component = e.getComponent();
            JPopupMenu popup = new JPopupMenu();
            
            JMenu submenu3 = new JMenu("Set Font Size");
            submenu3.setMnemonic(KeyEvent.VK_S);
            
            JMenuItem menuItem = new JMenuItem("8");
            menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize8");
            menuItem.addActionListener(new MenuListener());
            submenu3.add(menuItem);
            menuItem = new JMenuItem("10");
            menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize10");
            menuItem.addActionListener(new MenuListener());
            submenu3.add(menuItem);
            menuItem = new JMenuItem("12");
            menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize12");
            menuItem.addActionListener(new MenuListener());
            submenu3.add(menuItem);
            menuItem = new JMenuItem("14");
            menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize14");
            menuItem.addActionListener(new MenuListener());
            submenu3.add(menuItem);
            menuItem = new JMenuItem("16");
            menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize16");
            menuItem.addActionListener(new MenuListener());
            submenu3.add(menuItem);
            menuItem = new JMenuItem("18");
            menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize18");
            menuItem.addActionListener(new MenuListener());
            submenu3.add(menuItem);
            menuItem = new JMenuItem("20");
            menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize20");
            menuItem.addActionListener(new MenuListener());
            submenu3.add(menuItem);
            menuItem = new JMenuItem("24");
            menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize24");
            menuItem.addActionListener(new MenuListener());
            submenu3.add(menuItem);
            menuItem = new JMenuItem("28");
            menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize28");
            menuItem.addActionListener(new MenuListener());
            submenu3.add(menuItem);
            
            popup.add(submenu3);
            
            ColourMenu colourMenu = new ColourMenu("Set Colour");
            colourMenu.setParentEtc(this, e, null);
            
            popup.add(colourMenu);
            
            if(type.equals("AxisMarks"))
            {
                menuItem = new JMenuItem("Set Tick Marks");
                menuItem.getAccessibleContext().setAccessibleDescription
                ("FontXTitleSetInterval250interval=2500");
                // menuItem.addActionListener(new MenuListener());
                menuItem.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent e)
                    {
                        (new IntervalMenu(axisMarks)).show();
                    }
                }
                );
                popup.add(menuItem);
            }
            else
            {
                menuItem = new JMenuItem("Set Text");
                menuItem.getAccessibleContext().setAccessibleDescription
                ("FontXTitleSetText");
                menuItem.addActionListener(new MenuListener());
                popup.add(menuItem);
            }
            
            popup.show(component, e.getX(), e.getY());
            
        }
    }
    
    class MenuListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            JMenuItem source = (JMenuItem)e.getSource();
            String whatChanged = 
								source.getAccessibleContext().getAccessibleDescription();
            //				String whatChanged = e.getPropertyName();
            int xTitleStart = whatChanged.indexOf("XTitle");
            String newWhatChanged = whatChanged.substring(0, xTitleStart)
            + type + whatChanged.substring(xTitleStart + 6);
            System.out.println(newWhatChanged);
            if(oneGraphic != null)
            {
                oneGraphic.changeFont(newWhatChanged, null);
            }
            if(axisMarks != null)
            {
                axisMarks.changeFont(newWhatChanged, null);
            }
        }
    }
    
} // LabelListener
