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

/**
* Creates a file tree which is a drag source
*/
public class DragTree extends JTree implements DragGestureListener,
                           DragSourceListener 
{

    public static DefaultTreeModel model;
    private String fs = new String(System.getProperty("file.separator"));
    private FileNode rootNode;
    private Hashtable openNodeDir;

    final Cursor cbusy = new Cursor(Cursor.WAIT_CURSOR);
    final Cursor cdone = new Cursor(Cursor.DEFAULT_CURSOR);


    public DragTree(final File root, final JFrame f) 
    {
      openNodeDir = new Hashtable();

      DragSource dragSource = DragSource.getDefaultDragSource();

      dragSource.createDefaultDragGestureRecognizer(
               this,                             // component where drag originates
               DnDConstants.ACTION_COPY_OR_MOVE, // actions
               this);                            // drag gesture recognizer

      model = createTreeModel(root);
      setModel(model);
      createTreeModelListener();


      this.getSelectionModel().setSelectionMode
                    (TreeSelectionModel.SINGLE_TREE_SELECTION);

      //Listen for when a file is selected

      MouseListener mouseListener = new MouseAdapter() {
        public void mouseClicked(MouseEvent me) 
        {
          if(me.getClickCount() == 2 && isFileSelection()) 
          {
            f.setCursor(cbusy);
            JTree t = (JTree)me.getSource();
            TreePath p = t.getSelectionPath();
            DefaultMutableTreeNode node = (DefaultMutableTreeNode)
                                         t.getLastSelectedPathComponent();
            String selected = node.toString();

            TreeNode tn = node.getParent();
            selected = tn.toString().concat(fs + selected);
            while((tn = tn.getParent()) != null) 
              selected = tn.toString().concat(fs + selected);
            int sep = selected.indexOf(fs);
            selected = selected.substring(sep+1,selected.length());
            selected = root.toString().concat(fs +selected);
            showFilePane(selected);
            f.setCursor(cdone);
          }
        }
      };

      this.addMouseListener(mouseListener);


      addTreeExpansionListener(new TreeExpansionListener(){
        public void treeCollapsed(TreeExpansionEvent e) 
        {
        }
        public void treeExpanded(TreeExpansionEvent e) 
        {
          TreePath path = e.getPath();
          if(path != null) 
          {
            FileNode node = (FileNode)path.getLastPathComponent();

            if(!node.isExplored()) 
            {  
              model = (DefaultTreeModel)getModel();
              node.explore(openNodeDir);
              model.nodeStructureChanged(node);
            }
          }
        }
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
    public void dragDropEnd(DragSourceDropEvent e) {}
    public void dragEnter(DragSourceDragEvent e) {}
    public void dragExit(DragSourceEvent e) {}
    public void dragOver(DragSourceDragEvent e) {}
    public void dropActionChanged(DragSourceDragEvent e) {}

    public String getFilename()
    {
      TreePath path = getLeadSelectionPath();
      FileNode node = (FileNode)path.getLastPathComponent();
      return ((File)node.getUserObject()).getAbsolutePath();
    }

    public boolean isFileSelection() 
    {
      TreePath path = getLeadSelectionPath();
      FileNode node = (FileNode)path.getLastPathComponent();
      return !node.isDirectory();
    }


    private DefaultTreeModel createTreeModel(File root) 
    {
      rootNode = new FileNode(root,openNodeDir);
      rootNode.explore(openNodeDir);
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
    public DefaultMutableTreeNode addObject(String child, String path)
    {

      FileNode parentNode = null;
      Enumeration enum = openNodeDir.keys();
      while(enum.hasMoreElements())
      {
        String thiskey = (String)enum.nextElement();
        if(thiskey.equals(path))
        {
//        System.out.println("OPEN NODE EQUALS " + thiskey +  " :::: " + path  +  " :::: " + child);
          parentNode = (FileNode)openNodeDir.get(thiskey);
          break;
        }
      }

      if(parentNode == null)
        return null;
      else if(!parentNode.isExplored()) 
      {
        model = (DefaultTreeModel)getModel();
        parentNode.explore(openNodeDir);
        model.nodeStructureChanged(parentNode);
      }

      File newleaf = new File(child);
      int index = parentNode.getAnIndex(child);
      FileNode childNode = new FileNode(newleaf,openNodeDir);
      model.insertNodeInto(childNode, parentNode, index);

     // Make sure the user can see the new node.
      this.scrollPathToVisible(new TreePath(childNode.getPath()));

      return childNode;
    }


/**
*
* Opens a JFrame with the file contents displayed.
* @param the file name
*
*/
    public void showFilePane(String filename)
    {
      JFrame ffile = new JFrame(filename);
      JPanel pfile = (JPanel)ffile.getContentPane();
      pfile.setLayout(new BorderLayout());
      JPanel pscroll = new JPanel(new BorderLayout());
      JScrollPane rscroll = new JScrollPane(pscroll);

      FileEditorDisplay fed = new FileEditorDisplay(ffile, filename);
      new ResultsMenuBar(ffile,fed);
      pfile.add(rscroll, BorderLayout.CENTER);
      JTextPane seqText = fed.getJTextPane();
      pscroll.add(seqText, BorderLayout.CENTER);
      seqText.setCaretPosition(0);
      ffile.setSize(450,400);
      ffile.setVisible(true);
    }

}


class FileNode extends DefaultMutableTreeNode 
{
    private boolean explored = false;
//  private File[] children;
    private String[] childrenNames;
  
    public FileNode(File file, Hashtable openNodeDir)
    { 
      if(file.isDirectory())
        openNodeDir.put(file.getAbsolutePath(),this);
      setUserObject(file); 
    }

    public boolean getAllowsChildren() { return isDirectory(); }
    public boolean isLeaf() { return !isDirectory(); }
    public File getFile() { return (File)getUserObject(); }

    public boolean isExplored() { return explored; }

    public boolean isDirectory() 
    {
      File file = getFile();
      return file.isDirectory();
    }

    public String toString() 
    {
      File file = (File)getUserObject();
      String filename = file.toString();
      int index = filename.lastIndexOf(File.separator);

      return (index != -1 && index != filename.length()-1) ? 
                          filename.substring(index+1) : 
                                            filename;
    }

    public void explore(Hashtable openNodeDir) 
    {
      if(!isDirectory())
        return;

      if(!isExplored()) {
        File file = getFile();

      File[] children;
// filter out dot files
      children = file.listFiles(new FilenameFilter() 
      {
        public boolean accept(File d, String n) 
        {
          return !n.startsWith(".");
        }
      });

// sort into alphabetic order
      java.util.Arrays.sort(children);

      for(int i=0; i < children.length; ++i) 
        add(new FileNode(children[i],openNodeDir));
      explored = true;

      childrenNames = new String[children.length];
      for(int i=0; i < children.length; ++i)
        childrenNames[i] = children[i].getName();
     }
   }

   protected int getAnIndex(String newleaf)
   {
     String newChildren[] = new String[childrenNames.length+1];


     for(int i=0; i < childrenNames.length; ++i)
       newChildren[i] = childrenNames[i];

     newChildren[childrenNames.length] = newleaf;
     java.util.Arrays.sort(newChildren);
       
     int index;
     for(index=0; index < newChildren.length; ++index)
       if(newChildren[index].equals(newleaf))
         break;

     childrenNames = new String[newChildren.length];
     childrenNames[newChildren.length-1] = new String(newleaf);
     for(int i=0; i < childrenNames.length-1; ++i)
       childrenNames[i] = newChildren[i];

     return index;
   }

}



