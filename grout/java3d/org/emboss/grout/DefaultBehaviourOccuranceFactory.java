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
 * DefaultBehaviourOccuranceFactory.java
 *
 * Created on Sept 5, 2003, 11:46 AM
 */

package org.emboss.grout;

import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import java.awt.Component;
import javax.swing.JPopupMenu;
import javax.swing.JMenuItem;
import javax.swing.JMenu;
import java.util.Vector;
import java.awt.dnd.DragGestureEvent;

import com.sun.j3d.utils.picking.*;
import javax.media.j3d.*;
import javax.vecmath.Point3d;
import javax.vecmath.Point2d;
import javax.vecmath.Color4f;
import javax.vecmath.Color3f;
import javax.vecmath.Vector3d;
import java.awt.Point;
import java.awt.Dimension;
import java.awt.Color;
import com.sun.j3d.utils.geometry.*;
import javax.media.j3d.Canvas3D;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
/*
import ;
 */
/** This is the only behaviour occurance factory used at the moment */
public class DefaultBehaviourOccuranceFactory implements PickOccuranceFactory,
MouseListener, ColourMenuUser
{
    
    private Vector selectedGraphics = new Vector();
    /** USed to detect double click (depricated) */    
    private long lastClickTime;
    
    private static ShowSequenceOccuranceFactory showSequenceOccuranceFactory;
    private ZoomToOccuranceFactory zoomToOccuranceFactory;
    
    private Vector targets = new Vector();
    /** Panel than this behaviour relates to */    
    private GroutPanel parentPanel = null;
    
    /** Run when the mouse is pressed to start a zoom action.  Needs to draw a rubber
     * band, but does nothing at the moment
     */    
    public void startZoom(DragGestureEvent e)
    {
        
    }
    
    private void showRightClickMenu(final MouseEvent me,
    final TransferableGraphic tr)
    {
        // System.out.println("showRightClickMenu");
        
        final Component component = me.getComponent();
        
        Canvas3D canvas = ((Canvas3D) me.getComponent());
        RendWindContainer rendWindContainer = ((RendWindContainer) canvas.getParent());
        OneGraphic graphicWindow = ((OneGraphic) rendWindContainer.getParent());
        GroutPanel panel = ((GroutPanel) graphicWindow.getParent());
        
        JPopupMenu popup = new JPopupMenu();
        
        if(tr != null)
        {
            JMenuItem	menuItem = new JMenuItem("Delete Graphic");
            menuItem.addActionListener(new java.awt.event.ActionListener()
            {
                public void actionPerformed(java.awt.event.ActionEvent e)
                {
                    tr.getPresentGraphic().removeThisGraphic(tr);
                    // originator.validate();
                    RendWindContainer rendWindContainer =
                    ((RendWindContainer) component.getParent());
                    
                    if(rendWindContainer.getScenes().size() == 0)
                    {
                        java.awt.Container parent = tr.getPresentGraphic().getParent();
                        parent.remove(tr.getOriginalGraphic());
                        parent.validate();
                    }
                    notifyTargets("DeletedGraphic", null, tr);
                }
            }
            );
            popup.add(menuItem);
            
            ColourMenu submenu = new ColourMenu("Change Graphic Colour");
            submenu.setParentEtc(this, me, tr);
            
            popup.add(submenu);
            
            menuItem = new JMenuItem("Add symbol");
            menuItem.addActionListener(new java.awt.event.ActionListener()
            {
                public void actionPerformed(java.awt.event.ActionEvent e)
                {
                    addSymbols(tr, me, e);
                }
            }
            );
            popup.add(menuItem);
            // This adds the 'Seperate' item.  Does not work
            menuItem = new JMenuItem("Open in separate window");
            menuItem.addActionListener(new java.awt.event.ActionListener()
            {
                public void actionPerformed(java.awt.event.ActionEvent e)
                {
                    RendWindContainer parent =
                    ((RendWindContainer) component.getParent());
                    // RendWindContainer parent = (RendWindContainer) getParent();
                    OneGraphic grandParent = ((OneGraphic) parent.getParent());
                    GroutPanel greatGrandParent =
                    ((GroutPanel) grandParent.getParent());
                    OneGraphic originalOneGraphic = tr.getOriginalGraphic();
                    
                    greatGrandParent.newGroutWindow
                    (originalOneGraphic.getGraphicNumber());
                    //		(grandParent.getGraphicNumber());
                    // System.out.println("addThisGraphic " + tr + " to " + greatGrandParent);
                }
            }
            );
            popup.add(menuItem);
            popup.addSeparator();
        }
        
        JMenu submenu2 = new JMenu("Main 2 Title");
        submenu2.setMnemonic(KeyEvent.VK_M);
        
        JMenu submenu3 = new JMenu("Set Font Size");
        submenu3.setMnemonic(KeyEvent.VK_S);
        
        JMenuItem	menuItem = new JMenuItem("8");
        menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize8");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("10");
        menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize10");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("12");
        menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize12");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("14");
        menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize14");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("16");
        menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize16");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("18");
        menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize18");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("20");
        menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize20");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("24");
        menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize24");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("28");
        menuItem.getAccessibleContext().setAccessibleDescription("FontMainTitleSize28");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        
        submenu2.add(submenu3);
        
        ColourMenu colourMenu = new ColourMenu("Set Colour");
        colourMenu.setParentEtc(this, me, null, "MainTitle");
        submenu2.add(colourMenu);
        
        popup.add(submenu2);
        
        submenu2 = new JMenu("X Title");
        submenu2.setMnemonic(KeyEvent.VK_M);
        
        submenu3 = new JMenu("Set Font Size");
        submenu3.setMnemonic(KeyEvent.VK_S);
        
        menuItem = new JMenuItem("8");
        menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize8");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("10");
        menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize10");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("12");
        menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize12");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("14");
        menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize14");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("16");
        menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize16");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("18");
        menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize18");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("20");
        menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize20");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("24");
        menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize24");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("28");
        menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSize28");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        
        submenu2.add(submenu3);
        
        colourMenu = new ColourMenu("Set Colour");
        colourMenu.setParentEtc(this, me, null, "XTitle");
        submenu2.add(colourMenu);
        
        menuItem = new JMenuItem("Set Text");
        menuItem.getAccessibleContext().setAccessibleDescription("FontXTitleSetText");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu2.add(menuItem);
        
        popup.add(submenu2);
        
        submenu2 = new JMenu("Y Title");
        submenu2.setMnemonic(KeyEvent.VK_M);
        
        submenu3 = new JMenu("Set Font Size");
        submenu3.setMnemonic(KeyEvent.VK_S);
        
        menuItem = new JMenuItem("8");
        menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize8");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("10");
        menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize10");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("12");
        menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize12");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("14");
        menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize14");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("16");
        menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize16");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("18");
        menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize18");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("20");
        menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize20");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("24");
        menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize24");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("28");
        menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSize28");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        
        submenu2.add(submenu3);
        
        colourMenu = new ColourMenu("Set Colour");
        colourMenu.setParentEtc(this, me, null, "YTitle");
        submenu2.add(colourMenu);
        
        
        menuItem = new JMenuItem("Set Text");
        menuItem.getAccessibleContext().setAccessibleDescription("FontYTitleSetText");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu2.add(menuItem);
        
        popup.add(submenu2);
        //    menu.add(submenu);
        
        submenu2 = new JMenu("Axis Marks");
        submenu2.setMnemonic(KeyEvent.VK_M);
        
        submenu3 = new JMenu("Set Font Size");
        submenu3.setMnemonic(KeyEvent.VK_S);
        
        menuItem = new JMenuItem("8");
        menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize8");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("10");
        menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize10");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("12");
        menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize12");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("14");
        menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize14");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("16");
        menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize16");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("18");
        menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize18");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("20");
        menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize20");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("24");
        menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize24");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        menuItem = new JMenuItem("28");
        menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSize28");
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu3.add(menuItem);
        
        submenu2.add(submenu3);
        
        colourMenu = new ColourMenu("Set Colour");
        colourMenu.setParentEtc(this, me, null, "AxisMarks");
        submenu2.add(colourMenu);
        
        menuItem = new JMenuItem("Set Tick Marks");
        menuItem.getAccessibleContext().setAccessibleDescription("FontAxisMarksSetInterval");
        // menuItem.addActionListener(new MenuListener(graphicWindow));
        menuItem.addActionListener(new MenuListener(graphicWindow));
        submenu2.add(menuItem);
        
        popup.add(submenu2);
        
        
        popup.show(component, me.getX(), me.getY());
        
    }
    
    /** Brings up the menu for adding the symols */    
    private void addSymbols(TransferableGraphic tr, MouseEvent me, ActionEvent e)
    {
        if(tr != null)
        {
            SymbolsMenu symbolsMenu = new SymbolsMenu(tr, me, e);
            symbolsMenu.pack();
            symbolsMenu.show();
            
        }
    }
    
    /** Calculates the points before bringing up the symbols menu */    
    private void addSymbolToThesePoints(Vector points, TransferableGraphic tr,
    MouseEvent me,
    PickResult pickResult,
    PickIntersection pickIntersection,
    GeometryArray geometryArray)
    {
        
        SceneGraphPath sceneGraphPath = pickResult.getSceneGraphPath();
        
        // System.out.println("sceneGraphPath = " + sceneGraphPath);
        
        BranchGroup branchGroup = null;
        
        for(int i = 0;
        ((i < sceneGraphPath.nodeCount()) && (branchGroup == null));
        ++i)
        {
            Node node = sceneGraphPath.getNode
            (sceneGraphPath.nodeCount() - i - 1);
            if(node instanceof BranchGroup)
            {
                branchGroup = ((BranchGroup) node);
            }
        }
        
        // System.out.println("branchGroup = " + branchGroup);
                                /*
                                System.out.println(" = " + );
                                 */
        
        double labelXSize = (tr.getMaxX() - tr.getMinX()) / 100;
        double labelYSize = (tr.getMaxY() - tr.getMinY()) / 100;
        
        for(int i = 0; i < points.size(); ++i)
        {
            Appearance appearance = new Appearance();
            Material material = new Material();

            material.setDiffuseColor(0.0f, 0.0f, 0.0f);
            material.setAmbientColor(0.0f, 0.0f, 0.0f);
            material.setSpecularColor(0.0f, 0.0f, 0.0f);
            material.setEmissiveColor(0.0f, 0.0f, 0.0f);
            appearance.setMaterial(material);
            
            Box label = new Box((float) labelXSize, (float) labelYSize,
            0.01f, appearance);
            for(int k = 0; k <= 5; ++k)
            {
                label.getShape(k).setCapability(Shape3D.ALLOW_GEOMETRY_READ);
                label.getShape(k).setCapability(Shape3D.ALLOW_BOUNDS_READ);
                label.getShape(k).setCapability(Shape3D.ALLOW_APPEARANCE_READ);
                label.getShape(k).setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
            }
            
            TransformGroup transformGroup = new TransformGroup();
            transformGroup.setCapability(Group.ALLOW_CHILDREN_READ);
            transformGroup.setCapability(Group.ALLOW_CHILDREN_WRITE);
            transformGroup.setCapability(Group.ALLOW_CHILDREN_EXTEND);
            
            Transform3D transform3D = new Transform3D();
            Vector3d trans = new Vector3d((Point3d) points.elementAt(i));
            transform3D.setTranslation(trans);
            
            transformGroup.setTransform(transform3D);
            transformGroup.addChild(label);
            
            BranchGroup intermidiateGroup = new BranchGroup();
            intermidiateGroup.setCapability(Group.ALLOW_CHILDREN_READ);
            intermidiateGroup.setCapability(Group.ALLOW_CHILDREN_WRITE);
            intermidiateGroup.setCapability(Group.ALLOW_CHILDREN_EXTEND);
            intermidiateGroup.addChild(transformGroup);
            branchGroup.addChild(intermidiateGroup);
        }
        
    }
    
    /** Not Used */    
    private Color3f selectColour(MouseEvent me)
    {
        // new ColourMenu("Colour for Graphic");
        
        Color3f colour = new Color3f(Color.pink);
        
        return colour;
    }
    
    /** Changes the colour of a scene group only */    
    public void changeThisGraphicColour(Color colourSwing,
    MouseEvent me,
    TransferableGraphic tr)
    {
        changeThisGraphicColour(colourSwing, me, tr, "");
        // System.out.println("changeThisGraphicColour me = " + me);
        // System.out.println("tr = " + tr);
    }
    
    /** Changes the colour of a scene group or a label */    
    public void changeThisGraphicColour(Color colourSwing,
    MouseEvent me,
    TransferableGraphic tr,
    String eventDescription)
    {
        // System.out.println("changeThisGraphicColour me = " + me);
        // System.out.println("tr = " + tr);
        // (new Exception()).printStackTrace();
        
        if(tr != null)
        {
            Color3f colour = new Color3f(colourSwing);
            
            Canvas3D canvas = (Canvas3D) me.getSource();
            OneGraphic graphic1 = (OneGraphic) canvas.getParent().getParent();
            PickCanvas pickCanvas = new PickCanvas
            (canvas,
            graphic1.getRendWindContainer().getUniverse().getLocale());
            pickCanvas.setMode(PickTool.GEOMETRY_INTERSECT_INFO);
            pickCanvas.setTolerance(5.0f);
            pickCanvas.setShapeLocation(me);
            
            PickResult pickResult = pickCanvas.pickClosest();
            
            // System.out.println("pickResult = " + pickResult);
            
            if (pickResult != null)
            {
                graphic1.graphicColourChanged(tr, colourSwing);
                if(thisIsALine(pickResult))
                {
                    PickIntersection pickIntersection =
                    pickResult.getClosestIntersection
                    (pickCanvas.getStartPosition());
                    GeometryArray geometryArray =
                    pickIntersection.getGeometryArray();
                    
                    
                    for(int i = 0; i < geometryArray.getVertexCount(); ++i)
                    {
                        geometryArray.setColor(i, colour);
                    }
                }
                else
                {
                    SceneGraphPath sceneGraphPath = pickResult.getSceneGraphPath();
                                        
                    Node shape = sceneGraphPath.getObject();
                    
                    if(shape instanceof Shape3D)
                    {
                        Appearance appearance = ((Shape3D) shape).getAppearance();
                        // System.out.println("appearance = " + appearance);
                        Material material = appearance.getMaterial();
                        // System.out.println("material = " + material);
                        
                        material.setDiffuseColor(colour);
                    }
                }
            }
        }
        else
        {
            
            String whatChanged = "Font" + eventDescription + "ColourUser";
            Component canvas = ((Component) me.getSource());
            Component renderer = canvas.getParent();
            Component parent = renderer.getParent();
            // System.out.println("whatChanged = " + whatChanged);
            // System.out.println("canvas = " + canvas);
            // System.out.println("renderer = " + renderer);
            // System.out.println("parent = " + parent);
            
            if(parent instanceof OneGraphic)
            {
                // System.out.println("parent instanceof OneGraphic");
                ((OneGraphic) parent).changeFont(whatChanged, colourSwing);
            }
        }
    }
    
    /** Calcultes if this graphic is a line (specificly an IndexedLineStripArray) */    
    private boolean thisIsALine(PickResult pickResult)
    {
        SceneGraphPath sceneGraphPath = pickResult.getSceneGraphPath();
        
        
        Node shape = sceneGraphPath.getObject();
        
        if(shape instanceof Shape3D)
        {
            Geometry geometry = ((Shape3D) shape).getGeometry();
            // System.out.println("geometry = " + geometry);
            if(geometry instanceof IndexedLineStripArray)
            {
                return true;
            }
        }
        return false;
    }
    
    public boolean thisEventIsDoubleClick(MouseEvent e)
    {
        return (e.getClickCount() > 1);
    }
    
    /** Not used */    
    public void selectThisGraphic(TransferableGraphic graphic)
    {
        selectedGraphics.addElement(graphic);
    }
    
    
    /** Used when you select "Open new alignment viewer" */    
    public void newShowSequenceOccuranceFactory(GroutPanel panel)
    {
        // System.out.println("newShowSequenceOccuranceFactory");
        showSequenceOccuranceFactory = new ShowSequenceOccuranceFactory(panel,
        this);
    }
    
    /** Display the alignment editor */    
    private void showSequence(MouseEvent e)
    {
        // System.out.println("showSequence in behaviour " + e);
        Canvas3D canvas = ((Canvas3D) e.getComponent());
        // System.out.println("done canvas " + canvas);
        RendWindContainer rendWindContainer = 
						((RendWindContainer) canvas.getParent());
        // System.out.println("done rendWindContainer " + rendWindContainer);
        OneGraphic graphicWindow = ((OneGraphic) rendWindContainer.getParent());
        // System.out.println("done graphicWindow " + graphicWindow);
        parentPanel = ((GroutPanel) graphicWindow.getParent());
        // System.out.println("done panel " + parentPanel);
        if(showSequenceOccuranceFactory == null)
        {
            showSequenceOccuranceFactory =
            new ShowSequenceOccuranceFactory(parentPanel, this);
        }
        showSequenceOccuranceFactory.mouseClicked(e);
    }
    
    public synchronized void addDefaultBehaviourOccuranceFactoryListener
    (DefaultBehaviourOccuranceFactoryListener listener)
    {
        targets.addElement(listener);
    }
    
    public synchronized void removeDefaultBehaviourOccuranceFactoryListener
    (DefaultBehaviourOccuranceFactoryListener listener)
    {
        targets.removeElement(listener);
    }
    
    protected void notifyTargets(String propertyName, Object oldValue,
    Object newValue)
    {
        // System.out.println("notifyTargets");
        Vector l;
        DefaultBehaviourOccuranceFactoryEvent s =
        new DefaultBehaviourOccuranceFactoryEvent((Object) this, propertyName,
        oldValue, newValue);
        synchronized(this)
        {
            l = (Vector) targets.clone();
        }
        for (int i = 0; i < l.size(); i++)
        {
            DefaultBehaviourOccuranceFactoryListener sl =
            (DefaultBehaviourOccuranceFactoryListener) l.elementAt(i);
            //			System.out.println("viewPositionModelChanged " + sl);
            sl.defaultBehaviourOccuranceFactoryChanged(s);
        }
    }
    
    /** Used when you select "Open new alignment viewer" */    
    public void changeShowSequenceOccuranceFactory
    (ShowSequenceOccuranceFactory calledFrom)
    {
        
        if(calledFrom == showSequenceOccuranceFactory)
        {
            showSequenceOccuranceFactory =
            new ShowSequenceOccuranceFactory(parentPanel, this);
        }
    }
    
    public void mouseClicked(MouseEvent e)
    {
        // System.out.println("me.getClickCount() = " + e.getClickCount());
        // System.out.println("mouseClicked at " + e);
        Component component = e.getComponent();
        // if(e.getButton() == MouseEvent.BUTTON3)
        if(javax.swing.SwingUtilities.isRightMouseButton(e))
        {
            // System.out.println("MouseEvent.BUTTON3 ");
            TransferableGraphic graphic =
            ((RendWindContainer) component.getParent()).pickGraphic
            (((Canvas3D)component), e.getPoint());
            
            // if(graphic != null)
            {
                // System.out.println("mouseClicked at graphic");
                showRightClickMenu(e, graphic);
            }
            // System.out.println("mouseClicked at " + graphic);
        }
        else
        {
            // System.out.println("not MouseEvent.BUTTON3 ");
            if(thisEventIsDoubleClick(e))
            {
                // System.out.println("thisEventIsDoubleClick");
                showSequence(e);
            }
            else
            {
                TransferableGraphic graphic =
                ((RendWindContainer) component.getParent()).pickGraphic
                (((Canvas3D)component), e.getPoint());
                
                if(graphic != null)
                {
                    selectThisGraphic(graphic);
                }
            }
        }
    }
    public void mousePressed(MouseEvent e)
    {
        if(e.isShiftDown())
        {
            Canvas3D canvas = ((Canvas3D) e.getComponent());
            RendWindContainer rendWindContainer = 
								((RendWindContainer) canvas.getParent());
            OneGraphic graphicWindow = 
								((OneGraphic) rendWindContainer.getParent());
            GroutPanel panel = ((GroutPanel) graphicWindow.getParent());
            zoomToOccuranceFactory = new ZoomToOccuranceFactory(panel);
            zoomToOccuranceFactory.mousePressed(e);
        }
        // System.out.println("mousePressed 1 e = " + e);
    }
    public void mouseReleased(MouseEvent e)
    {
        if(e.isShiftDown())
        {
            if(zoomToOccuranceFactory != null)
            {
                zoomToOccuranceFactory.mouseReleased(e);
            }
        }
        else
        {
            zoomToOccuranceFactory = null;
        }
        // System.out.println("mouseReleased e = " + e);
    }
    public void mouseEntered(MouseEvent e)
    {
        // System.out.println("mouseEntered");
    }
    public void mouseExited(MouseEvent e)
    {
        // System.out.println("mouseExited");
    }
    
    
    public void PerformPick(double x1, double y1, double x2, double y2)
    {
    }
    public void PerformPick(double x, double y)
    {
    }
    
    /** Handles all the right click menu actions */    
    class MenuListener implements ActionListener
    {
        OneGraphic graphic = null;
        
        public MenuListener(OneGraphic gr)
        {
            graphic = gr;
        }
        
        public MenuListener()
        {
        }
        
        public void actionPerformed(ActionEvent e)
        {
            System.out.println("ActionEvent 1 in " + this + " = " + e);
            JMenuItem source = (JMenuItem)e.getSource();
            String whatChanged = source.getAccessibleContext().
            getAccessibleDescription();
            if(graphic != null)
            {
                graphic.changeFont(whatChanged, null);
            }
        }
    }
}
