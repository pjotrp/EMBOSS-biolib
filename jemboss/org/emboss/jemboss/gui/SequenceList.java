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
  private JPopupMenu popMenu = new JPopupMenu();

  public SequenceList()
  {
    super("Sequence List");
    setSize(400,200);
    seqModel = new SequenceListTableModel();
    table = new DragJTable(seqModel);
    table.setModel(seqModel);
    table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);

    //column width
    for(int i=0;i<SequenceListTableModel.modelColumns.length;i++)
    {
      TableColumn column = table.getColumn(
            SequenceListTableModel.modelColumns[i].title);
      column.setPreferredWidth(
             SequenceListTableModel.modelColumns[i].width);
    }

    JMenuItem openMenuItem = new JMenuItem("Open");
    openMenuItem.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        int nrow = table.getSelectedRow();
        String fileName = (String)table.getValueAt(nrow,
                         SequenceListTableModel.COL_NAME);
        System.out.println("FILE NAME IS " +fileName);
        SequenceData row = (SequenceData)seqModel.modelVector.elementAt(nrow);
        if(!(row.s_remote.booleanValue()))
          DragTree.showFilePane(fileName);
        else
          RemoteDragTree.showFilePane(fileName);
      }
    });
    popMenu.add(openMenuItem);
    
    MouseListener popupListener = new PopupListener(); 
    table.addMouseListener(popupListener);

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

  class PopupListener extends MouseAdapter 
  {
    public void mousePressed(MouseEvent e) {
      maybeShowPopup(e);
    }

    public void mouseReleased(MouseEvent e) 
    {
      maybeShowPopup(e);
    }

    private void maybeShowPopup(MouseEvent e) 
    {
      if(e.isPopupTrigger()) 
        popMenu.show(e.getComponent(),
              e.getX(), e.getY());
    }
  }
}


class DragJTable extends JTable implements DragGestureListener,
                           DragSourceListener, DropTargetListener
{

  private SequenceListTableModel seqModel;
  public DragJTable(SequenceListTableModel seqModel)
  {
    super();
    this.seqModel = seqModel;
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
        insertData(seqModel,e.getLocation(),fn.getFile().getCanonicalPath(),
                   new Boolean(false));
        e.getDropTargetContext().dropComplete(true);
      }
      catch(UnsupportedFlavorException ufe){}
      catch(IOException ioe){}
    }
    else if(t.isDataFlavorSupported(RemoteFileNode.REMOTEFILENODE))
    {
      try
      {
        String dropS = (String)t.getTransferData(DataFlavor.stringFlavor);
        insertData(seqModel,e.getLocation(),dropS,new Boolean(true));
        e.getDropTargetContext().dropComplete(true);
      }
      catch (Exception exp)
      {
        e.rejectDrop();
      }
    }
  }

  public void insertData(SequenceListTableModel seqModel, Point ploc,
                         String fileName, Boolean bremote)
  {
    int row = rowAtPoint(ploc);
    seqModel.insertRow(row);
    seqModel.modelVector.insertElementAt(new SequenceData(fileName,"","",
                                   new Boolean(false),bremote),row);

    tableChanged(new TableModelEvent(seqModel, row+1, row+1,
            TableModelEvent.ALL_COLUMNS, TableModelEvent.INSERT));
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
  public String s_name;       //seq location
  public String s_beg;        //seq start
  public String s_end;        //seq end
  public Boolean s_default;   //use as the default
  public Boolean s_remote;    //file on remote file system

  public SequenceData()
  {
    s_name = new String();
    s_beg = new String();
    s_end = new String();
    s_default = new Boolean(false);
    s_remote = new Boolean(false);
  }

  public SequenceData(String name, String beg, String end,
                      Boolean def, Boolean remote)
  {
    s_name = name;
    s_beg = beg;
    s_end = end;
    s_default = def;
    s_remote = remote;
  }
}


class SequenceListTableModel extends AbstractTableModel
{

  protected static Vector modelVector;

  public SequenceListTableModel()
  {
    modelVector = new Vector();
    setDefaultData();
  }

  public static final ColumnData modelColumns[] =
  {
    new ColumnData("File",170,JLabel.LEFT),
    new ColumnData("Start",45,JLabel.LEFT),
    new ColumnData("End",45,JLabel.LEFT),
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
  public static final int COL_BEG  = 1;
  public static final int COL_END  = 2;
  public static final int COL_DEF  = 3;

  public void setDefaultData()
  {
    modelVector.removeAllElements();
    Boolean bdef = new Boolean(false);
    modelVector.addElement(new SequenceData("-","","",bdef,bdef)); 
    modelVector.addElement(new SequenceData("-","","",bdef,bdef)); 
    modelVector.addElement(new SequenceData("-","","",bdef,bdef)); 
    modelVector.addElement(new SequenceData("-","","",bdef,bdef)); 
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
      case COL_BEG: return row.s_beg;
      case COL_END: return row.s_end;
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
      case COL_BEG: 
        row.s_beg = svalue;
        break;
      case COL_END:
        row.s_end = svalue;
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


