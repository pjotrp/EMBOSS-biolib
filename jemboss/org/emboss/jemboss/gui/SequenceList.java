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


package org.emboss.jemboss.gui;

import java.awt.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.table.*;
import javax.swing.border.*;
import java.awt.event.*;

import org.emboss.jemboss.gui.filetree.*;
import java.awt.datatransfer.*;
import java.awt.dnd.*;


public class SequenceList extends JFrame
{

  private DragJTable table;
  private SequenceListTableModel seqModel;

  public SequenceList()
  {
    super("Sequence List");
    setSize(400,200);
    table = new DragJTable();
    seqModel = new SequenceListTableModel();
    table.setModel(seqModel);
    table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);

    JScrollPane scrollpane = new JScrollPane(table);
    scrollpane.setSize(300,100);
    getContentPane().add(scrollpane, BorderLayout.CENTER);

    JPanel buttonPanel = new JPanel(new FlowLayout());
   
    JButton addSeq = new JButton("Add sequence");
    addSeq.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        int row = table.getSelectedRow();
        seqModel.insertRow(row+1);
        table.tableChanged(new TableModelEvent(seqModel, row+1, row+1, 
                TableModelEvent.ALL_COLUMNS, TableModelEvent.INSERT));
//      table.repaint();
      }
    });
    buttonPanel.add(addSeq);

    JButton deleteSeq = new JButton("Delete  sequence");
    deleteSeq.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        int row = table.getSelectedRow();
        if(seqModel.deleteRow(row))
        {
          table.tableChanged(new TableModelEvent(seqModel, row, row,
                TableModelEvent.ALL_COLUMNS, TableModelEvent.DELETE));
//        table.repaint();
        }
      }
    });
    buttonPanel.add(deleteSeq);

    JButton closeFrame = new JButton("Close");
    closeFrame.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        setVisible(false);
      }
    });
    buttonPanel.add(closeFrame);

    getContentPane().add(buttonPanel, BorderLayout.SOUTH);
  }

}


class DragJTable extends JTable implements DragGestureListener,
                           DragSourceListener, DropTargetListener
{

  public DragJTable()
  {
    super();
    DragSource dragSource = DragSource.getDefaultDragSource();
    dragSource.createDefaultDragGestureRecognizer(
           this,                             // component where drag originates
           DnDConstants.ACTION_COPY_OR_MOVE, // actions
           this);
    setDropTarget(new DropTarget(this,this));
  }

// drag source
  public void dragGestureRecognized(DragGestureEvent e)
  {
  }
  public void dragDropEnd(DragSourceDropEvent e) {}
  public void dragEnter(DragSourceDragEvent e) {}
  public void dragExit(DragSourceEvent e) {}
  public void dragOver(DragSourceDragEvent e) {}
  public void dropActionChanged(DragSourceDragEvent e) {}

// drop sink
  public void dragEnter(DropTargetDragEvent e)
  {
    if(e.isDataFlavorSupported(RemoteFileNode.REMOTEFILENODE) ||
       e.isDataFlavorSupported(FileNode.FILENODE))
      e.acceptDrag(DnDConstants.ACTION_COPY_OR_MOVE);
  }

  public void drop(DropTargetDropEvent e)
  {
    Transferable t = e.getTransferable();

    if(t.isDataFlavorSupported(FileNode.FILENODE))
    {
      try
      {
        final FileNode fn =
           (FileNode)t.getTransferData(FileNode.FILENODE);
        System.out.println("Detected local drop of " + fn.getFile().getCanonicalPath()); 
      }
      catch(UnsupportedFlavorException ufe){}
      catch(IOException ioe){}
    }
    else if(t.isDataFlavorSupported(RemoteFileNode.REMOTEFILENODE))
    {
      try
      {
        Point ploc = e.getLocation();
        int row = rowAtPoint(ploc);
        final RemoteFileNode fn =
            (RemoteFileNode)t.getTransferData(RemoteFileNode.REMOTEFILENODE);
        e.getDropTargetContext().dropComplete(true);
        System.out.println("Detected remote drop of " + fn.getRootDir() + "/" + fn.getFile());
        System.out.println("Detected remote drop of " + fn.getServerName());
      }
      catch (Exception exp)
      {
        e.rejectDrop();
      }
    }
  }

  public void dragOver(DropTargetDragEvent e)
  { 
    Point ploc = e.getLocation();
    int row = rowAtPoint(ploc);
    setRowSelectionInterval(row,row);
  }
  public void dropActionChanged(DropTargetDragEvent e) {}
  public void dragExit(DropTargetEvent e){}

}

/**
*
* Content of each column in the DragJTable
*
*/
class ColumnData
{
  protected String title;
  protected int width;
  protected int alignment;
  public ColumnData(String title, int width, int alignment)
  {
    this.title = title;
    this.width = width;
    this.alignment = alignment;
  }
}

/**
*
* Content of each row in the DragJTable
*
*/
class SequenceData
{
  public String s_name;
  public String s_path;
  public Boolean s_default;

  public SequenceData()
  {
    s_name = new String();
    s_path = new String();
    s_default = new Boolean(false);
  }

  public SequenceData(String name, String path, Boolean def)
  {
    s_name = name;
    s_path = path;
    s_default = def;
  }
}


class SequenceListTableModel extends AbstractTableModel
{

  private Vector modelVector;

  public SequenceListTableModel()
  {
    modelVector = new Vector();
    setDefaultData();
  }

  public static final ColumnData modelColumns[] =
  {
    new ColumnData("File",100,JLabel.LEFT),
    new ColumnData("Path",150,JLabel.LEFT),
    new ColumnData("Default",15,JLabel.LEFT)
  };

/*
* DragJTable uses this method to determine the default renderer/
* editor for each cell.  If we didn't implement this method,
* then the last column would contain text ("true"/"false"),
* rather than a check box.
*/
  public Class getColumnClass(int c) 
  {
    return getValueAt(0, c).getClass();
  }


  public static final int COL_NAME = 0;
  public static final int COL_PATH = 1;
  public static final int COL_DEF  = 2;

  public void setDefaultData()
  {
    modelVector.removeAllElements();
    modelVector.addElement(new SequenceData("-","-",new Boolean(false))); 
    modelVector.addElement(new SequenceData("-","-",new Boolean(false))); 
    modelVector.addElement(new SequenceData("-","-",new Boolean(false))); 
  }
  
  public int getRowCount()
  {
    return modelVector==null ? 0 : modelVector.size();
  }

  public int getColumnCount()
  {
    return modelColumns.length;
  }

  public String getColumnName(int c)
  {
    return modelColumns[c].title;
  }

  public boolean isCellEditable(int nRow, int nCol)
  {
    return true;
  }

  public Object getValueAt(int nRow, int nCol)
  {
    if(nRow < 0 || nCol>=getRowCount())
      return "";

    SequenceData row = (SequenceData)modelVector.elementAt(nRow);
    switch(nCol)
    { 
      case COL_NAME: return row.s_name;
      case COL_PATH: return row.s_path;
      case COL_DEF:  return row.s_default;
    }
    return ""; 
  }

  public void setValueAt(Object value, int nRow, int nCol)
  {
    if(nRow < 0 || nCol>=getRowCount())
      return ;

    SequenceData row = (SequenceData)modelVector.elementAt(nRow);
    String svalue = value.toString();
    
    switch(nCol)
    {
      case COL_NAME: 
        row.s_name = svalue;
        break;
      case COL_PATH: 
        row.s_path = svalue;
        break;
      case COL_DEF:  
        row.s_default = (Boolean)value;
        break;
    }
  }

  public void insertRow(int row)
  {
    if(row < 0)
      row = 0;
    if(row > modelVector.size())
      row = modelVector.size();
    modelVector.insertElementAt(new SequenceData(), row);
  }
  
  public boolean deleteRow(int row)
  {
    if(row < 0 || row>=modelVector.size())
      return false;
    modelVector.remove(row);
    return true;
  }

}


