/********************************************************************
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Library General Public
*  License as published by the Free Software Foundation; either
*  version 2 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Library General Public License for more details.
*
*  You should have received a copy of the GNU Library General Public
*  License along with this library; if not, write to the
*  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
*  Boston, MA  02111-1307, USA.
*
*  @author: Copyright (C) Tim Carver
*
********************************************************************/

package org.emboss.jemboss.gui.filetree;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import java.awt.dnd.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.tree.*;
import java.io.*;
import java.util.*;

import org.emboss.jemboss.gui.ResultsMenuBar;
import uk.ac.mrc.hgmp.embreo.*;
import uk.ac.mrc.hgmp.embreo.filemgr.*;

/**
* Creates a file tree which is a drag source
*/
public class RemoteDragTree extends JTree implements DragGestureListener,
                           DragSourceListener, DropTargetListener 
{

  public static DefaultTreeModel model;
  private String fs = new String(System.getProperty("file.separator"));
  private RemoteFileNode rootNode;
  private EmbreoParams mysettings; 
  private EmbreoFileRoots froots;

  final Cursor cbusy = new Cursor(Cursor.WAIT_CURSOR);
  final Cursor cdone = new Cursor(Cursor.DEFAULT_CURSOR);


  public RemoteDragTree(EmbreoParams mysettings, EmbreoFileRoots froots,
                        final JPanel viewPane) 
  {

    this.mysettings = mysettings;
    this.froots = froots;

    DragSource dragSource = DragSource.getDefaultDragSource();
    dragSource.createDefaultDragGestureRecognizer(
             this,                             // component where drag originates
             DnDConstants.ACTION_COPY_OR_MOVE, // actions
             this);                            // drag gesture recognizer

    setDropTarget(new DropTarget(this,this));
    model = createTreeModel(" ");
    setModel(model);
    createTreeModelListener();

    this.getSelectionModel().setSelectionMode
                  (TreeSelectionModel.SINGLE_TREE_SELECTION);

    //Listen for when a file is selected
    addMouseListener(new MouseListener()
    {
      public void mouseClicked(MouseEvent me)
      {
        if(me.getClickCount() == 2 && isFileSelection())
        {
          RemoteFileNode node = (RemoteFileNode)getLastSelectedPathComponent();
          if(node==null)
            return;
          if(node.isLeaf())
            showFilePane(node.getFullName());
        }
      }
      public void mousePressed(MouseEvent me){}
      public void mouseEntered(MouseEvent me){}
      public void mouseExited(MouseEvent me){}
      public void mouseReleased(MouseEvent me){}
    });

    addTreeExpansionListener(new TreeExpansionListener()
    {
      public void treeExpanded(TreeExpansionEvent e) 
      {
        TreePath path = e.getPath();
        if(path != null) 
        {
          RemoteFileNode node = (RemoteFileNode)path.getLastPathComponent();

          if(!node.isExplored()) 
          {  
            model = (DefaultTreeModel)getModel();
            node.explore();
            model.nodeStructureChanged(node);
          }
        }
      }
      public void treeCollapsed(TreeExpansionEvent e){}
    });

  }


  public void dragGestureRecognized(DragGestureEvent e) 
  {
    // drag only files 
    if(isFileSelection())
      e.startDrag(DragSource.DefaultCopyDrop, // cursor
          new StringSelection(getFilename()), // transferable
                                       this); // drag source listener
  }

// Source
  public void dragDropEnd(DragSourceDropEvent e) {}
  public void dragEnter(DragSourceDropEvent e){} 
  public void dragEnter(DragSourceDragEvent e){}
  public void dragExit(DragSourceEvent e) {}
  public void dragOver(DragSourceDropEvent e) {}
  public void dragOver(DragSourceDragEvent e){}
  public void dropActionChanged(DragSourceDragEvent e) {}

// Target
  public void dragEnter(DropTargetDragEvent e)
  {
    if(e.isDataFlavorSupported(DataFlavor.stringFlavor))
    {
      e.acceptDrag(DnDConstants.ACTION_COPY_OR_MOVE);
    }
    System.out.println("dragEnter");
  }

  public void drop(DropTargetDropEvent e)
  {
    Transferable t = e.getTransferable();
    if(t.isDataFlavorSupported(DataFlavor.stringFlavor))
    {
//    try
//    {
//      System.out.println("DROPPED SUCCESS!!!" + 
//          (String)t.getTransferData(DataFlavor.stringFlavor));       
//    }
//    catch (Exception ex) {}
    } 
    else
    {
      e.rejectDrop();
      return;
    }
    return;
  }

/**
*
* When a suitable DataFlavor is offered over a remote file 
* node the node is highlighted/selected and the drag
* accepted. Otherwise the drag is rejected.
*
*/
  public void dragOver(DropTargetDragEvent e) 
  {
    if (e.isDataFlavorSupported(DataFlavor.stringFlavor)) 
    {
      Point hereiam = e.getLocation();
      TreePath ePath = getPathForLocation(hereiam.x,hereiam.y);
      if (ePath == null) 
        e.rejectDrag();
      else
      {
        setSelectionPath(eventPath);
        e.acceptDrag(DnDConstants.ACTION_COPY_OR_MOVE);
      }
    } 
    else
    {
      e.rejectDrag();
    }
  }

  public void dropActionChanged(DropTargetDragEvent e) {}
  public void dragExit(DropTargetEvent e){}

//
  public boolean isFileSelection()
  {
    TreePath path = getLeadSelectionPath();
    if(path == null)
      return false;

    RemoteFileNode node = (RemoteFileNode)path.getLastPathComponent();
    return !node.isDirectory();
  }

  public String getFilename()
  {
    TreePath path = getLeadSelectionPath();
    RemoteFileNode node = (RemoteFileNode)path.getLastPathComponent();
    return node.getServerName();
  }

  private DefaultTreeModel createTreeModel(String root) 
  {
    rootNode = new RemoteFileNode(root,null,null);
    rootNode.explore();
    return new DefaultTreeModel(rootNode);
  }

  public DefaultTreeModel getTreeModel () 
  {
    return model;
  }


/**
*
* Adding a file (or directory) to the file tree manager.
* This looks to see if the directory has already been opened
* and updates the filetree if it has.
* @param file to add to the tree
*
*/
//  public DefaultMutableTreeNode addObject(String child, String path)
//  {

//    return childNode;
//  }


/**
*
* Opens a JFrame with the file contents displayed.
* @param the file name
*
*/
  public void showFilePane(String filename)
  {
    setCursor(cbusy);

    try
    {
      JFrame ffile = new JFrame(filename);
      JPanel pfile = (JPanel)ffile.getContentPane();
      pfile.setLayout(new BorderLayout());
      JPanel pscroll = new JPanel(new BorderLayout());
      JScrollPane rscroll = new JScrollPane(pscroll);

      EmbreoFileGet efg = new EmbreoFileGet(mysettings, froots.getCurrentRoot(),filename);
      FileEditorDisplay fed = new FileEditorDisplay(ffile,filename,efg.contents());
      new ResultsMenuBar(ffile,fed);
      pfile.add(rscroll, BorderLayout.CENTER);
      JTextPane seqText = fed.getJTextPane();
      pscroll.add(seqText, BorderLayout.CENTER);
      seqText.setCaretPosition(0);
      ffile.setSize(450,400);
      ffile.setVisible(true);
    }
    catch(EmbreoAuthException eae)
    {
      setCursor(cdone);
    }
    setCursor(cdone);
  }


  class RemoteFileNode extends DefaultMutableTreeNode 
  {
    private boolean explored = false;
    private boolean isDir = false;
    private EmbreoFileList parentList;
    private String[] childrenNames;
    private String fs = new String(System.getProperty("file.separator"));
    private String fullname;
    private Vector children;

    public RemoteFileNode(String file, EmbreoFileList parentList, String parent)
    { 
      this.parentList = parentList;

      if(file.equals(" "))
        isDir = true;

      if(parentList != null)
      {
        fullname = parent + fs + file;
        if(parentList.isDirectory(file))
          isDir = true;
      }
      else
        fullname = ".";

      setUserObject(file); 
    }

    public boolean getAllowsChildren() { return isDir; }
    public boolean isLeaf() { return !isDir; }
    public boolean isDirectory() { return isDir; }
//  public String getFile() { return (String)getUserObject(); }
    public String getFullName() { return fullname; }
    public boolean isExplored() { return explored; }
    public String getServerName() 
    { 
      String prefix = (String)froots.getRoots().get(froots.getCurrentRoot());
      return prefix + fs + fullname;
    }

    public void explore() 
    {
      if(!isDir)
        return;

      if(!explored)
      {
        try
        {
//        System.out.println(froots.getCurrentRoot() + " :: " + fullname);
          EmbreoFileList efl = new EmbreoFileList(mysettings,
                                   froots.getCurrentRoot(),fullname);
          children = efl.fileVector();
          for(int i=0;i<children.size();i++)
            add(new RemoteFileNode((String)children.get(i),efl,fullname));
        }
        catch(EmbreoAuthException eae) {}
      }
      explored = true;
    }
  }

}
