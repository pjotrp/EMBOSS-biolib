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

import org.emboss.jemboss.parser.*;
import org.emboss.jemboss.soap.CallAjax;

import uk.ac.mrc.hgmp.embreo.EmbreoParams;
import uk.ac.mrc.hgmp.embreo.EmbreoAuthException;


import org.emboss.jemboss.gui.filetree.*;
import java.awt.datatransfer.*;
import java.awt.dnd.*;


public class SequenceList extends JFrame
{

  private DragJTable table;
  private SequenceListTableModel seqModel;
  final Cursor cbusy = new Cursor(Cursor.WAIT_CURSOR);
  final Cursor cdone = new Cursor(Cursor.DEFAULT_CURSOR);


  public SequenceList(final boolean withSoap,final EmbreoParams mysettings)
  {
    super("Sequence List");
    setSize(400,155);
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

//  MouseListener popupListener = new PopupListener(); 
//  table.addMouseListener(popupListener);

    JScrollPane scrollpane = new JScrollPane(table);
    scrollpane.setSize(300,100);
    getContentPane().add(scrollpane, BorderLayout.CENTER);

    JMenuBar menuPanel = new JMenuBar();
    new BoxLayout(menuPanel,BoxLayout.X_AXIS);
    setJMenuBar(menuPanel);

    JMenu fileMenu = new JMenu("File");
    fileMenu.setMnemonic(KeyEvent.VK_F);
    menuPanel.add(fileMenu);
    JMenu toolMenu = new JMenu("Tools");
    toolMenu.setMnemonic(KeyEvent.VK_T);
    menuPanel.add(toolMenu);

    JMenuItem openMenuItem = new JMenuItem("Open");
    openMenuItem.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        int nrow = table.getSelectedRow();
        String fileName = (String)table.getValueAt(nrow,
                table.convertColumnIndexToView(SequenceListTableModel.COL_NAME));

        SequenceData row = (SequenceData)seqModel.modelVector.elementAt(nrow);

        if(!(row.s_remote.booleanValue()))
          DragTree.showFilePane(fileName);        //local file
        else
          RemoteDragTree.showFilePane(fileName);  //remote file
      }
    });
    fileMenu.add(openMenuItem);

    JMenuItem addSeq = new JMenuItem("Add sequence");
    addSeq.setMnemonic(KeyEvent.VK_A);
    addSeq.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        int row = table.getSelectedRow();
        seqModel.insertRow(row+1);
        table.tableChanged(new TableModelEvent(seqModel, row+1, row+1, 
                TableModelEvent.ALL_COLUMNS, TableModelEvent.INSERT));
      }
    });
    toolMenu.add(addSeq);

    JMenuItem deleteSeq = new JMenuItem("Delete  sequence");
    deleteSeq.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        int row = table.getSelectedRow();
        if(seqModel.deleteRow(row))
          table.tableChanged(new TableModelEvent(seqModel, row, row,
                TableModelEvent.ALL_COLUMNS, TableModelEvent.DELETE));
        
      }
    });
    toolMenu.add(deleteSeq);
 
    JMenuItem ajaxSeq = new JMenuItem("Calculate sequence attributes");
    ajaxSeq.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        setCursor(cbusy);
        int row = table.getSelectedRow();
        String fn = table.getFileName(row);
        if(table.isListFile(row).booleanValue())
          fn = "list::"+fn;
        
        String fc = AjaxUtil.getFileOrDatabaseForAjax(fn,
                      BuildProgramMenu.getDatabaseList(),
                      null,withSoap);
        
        boolean ok = false;
        int ajaxLength=0;
        float ajaxWeight;
        boolean ajaxProtein;

        if(!withSoap && fc!=null)    //Ajax without SOAP
        {
          Ajax aj = new Ajax();
          ok = aj.seqType(fc);
          if(ok)
          {
            ajaxLength  = aj.length;
            ajaxWeight  = aj.weight;
            ajaxProtein = aj.protein;
          }
        }
        else if(fc!=null)    //Ajax with SOAP
        {
          try
          {  
            CallAjax ca = new CallAjax(fc,"sequence",mysettings);
            if(ca.getStatus().equals("0"))
            {
              ajaxLength  = ca.getLength();
              ajaxWeight  = ca.getWeight();
              ajaxProtein = ca.isProtein();
              ok = true;
            }
          }
          catch (EmbreoAuthException eae)
          {
            System.out.println("Call to Ajax library failed");
            setCursor(cdone);
          }
        }

        if(!ok && fc!=null)                       //Ajax failed
          JOptionPane.showMessageDialog(null,
                   "Sequence not found." +
                   "\nCheck the sequence entered.",
                   "Error Message", JOptionPane.ERROR_MESSAGE);
        else
        {
          seqModel.setValueAt(new Integer(1),row,
                      SequenceListTableModel.COL_BEG);
          seqModel.setValueAt(new Integer(ajaxLength),row,
                      SequenceListTableModel.COL_END);
          table.repaint();
        }
        setCursor(cdone);
      }
    });
    toolMenu.add(ajaxSeq);


    JMenuItem reset = new JMenuItem("Reset Table");
    reset.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        seqModel.setDefaultData();
        table.repaint();
      }
    });
    fileMenu.add(reset);

    fileMenu.addSeparator();

    JMenuItem closeFrame = new JMenuItem("Close");
    closeFrame.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        setVisible(false);
      }
    });
    fileMenu.add(closeFrame);

  }


  private int getDefaultRow()
  {
    int nrow = seqModel.getRowCount();
    for(int i =0;i<nrow;i++)
    {
      Boolean isDef = (Boolean)seqModel.getValueAt(i,
                        SequenceListTableModel.COL_DEF);
      if(isDef.booleanValue())
        return i;
    }
    return -1;
  }

  public String getDefaultSequenceName()
  {
    int ndef = getDefaultRow();
    if(ndef<0)
      return null;

    return (String)seqModel.getValueAt(ndef,
             SequenceListTableModel.COL_NAME);
  }

//class PopupListener extends MouseAdapter 
//{
//  public void mousePressed(MouseEvent e) {
//    maybeShowPopup(e);
//  }

//  public void mouseReleased(MouseEvent e) 
//  {
//    maybeShowPopup(e);
//  }

//  private void maybeShowPopup(MouseEvent e) 
//  {
//    if(e.isPopupTrigger()) 
//      popMenu.show(e.getComponent(),
//            e.getX(), e.getY());
//  }
//}
}



class DragJTable extends JTable implements DragGestureListener,
             DragSourceListener, DropTargetListener
{

  private SequenceListTableModel seqModel;
  public static DataFlavor DRAGJTABLE =
           new DataFlavor(DragJTable.class, "Sequence JTable");
  static DataFlavor flavors[] = { DRAGJTABLE , DataFlavor.stringFlavor };

  public DragJTable(SequenceListTableModel seqModel)
  {
    super();
    this.seqModel = seqModel;
    DragSource dragSource = DragSource.getDefaultDragSource();
    dragSource.createDefaultDragGestureRecognizer(
           this,                            // component where drag originates
           DnDConstants.ACTION_COPY,        // actions
           this);
    setDropTarget(new DropTarget(this,this));
  }

// drag source
  public void dragGestureRecognized(DragGestureEvent e)
  {
    Point p = e.getDragOrigin();
    int ncol = columnAtPoint(p);

    if(ncol == convertColumnIndexToView(SequenceListTableModel.COL_NAME))
    {
      int nrow = getSelectedRow();
//    String fileName = getFileName(nrow);
//    if(isListFile(nrow).booleanValue())
//      fileName="@".concat(fileName);

      e.startDrag(DragSource.DefaultCopyDrop,             // cursor
            (Transferable)seqModel.getSequenceData(nrow), // transferable data
                  this);                                  // drag source listener

//    e.startDrag(DragSource.DefaultCopyDrop,  // cursor
//       new StringSelection(fileName),        // transferable data
//                               this);        // drag source listener
    }
  }
  public void dragDropEnd(DragSourceDropEvent e) {}
  public void dragEnter(DragSourceDragEvent e) {}
  public void dragExit(DragSourceEvent e) {}
  public void dragOver(DragSourceDragEvent e) {}
  public void dropActionChanged(DragSourceDragEvent e) {}

  public String getFileName(int row)
  {
    return (String)seqModel.getValueAt(row,
                  SequenceListTableModel.COL_NAME);
  }

  public Boolean isListFile(int row)
  {
    return (Boolean)seqModel.getValueAt(row,
                  SequenceListTableModel.COL_LIST);
  }


// drop sink
  public void dragEnter(DropTargetDragEvent e)
  {
    if(e.isDataFlavorSupported(RemoteFileNode.REMOTEFILENODE) ||
       e.isDataFlavorSupported(FileNode.FILENODE) ||
       e.isDataFlavorSupported(DataFlavor.stringFlavor) )
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
    else if(t.isDataFlavorSupported(DataFlavor.stringFlavor))
    {
      System.out.println("HERE DataFlavor.stringFlavor");
    }
    else if(t.isDataFlavorSupported(SequenceData.SEQUENCEDATA))
    {
      try
      {
        SequenceData seqData = (SequenceData)
           t.getTransferData(SequenceData.SEQUENCEDATA);
        System.out.println(seqData.s_name);
        System.out.println(seqData.s_beg);
      }
      catch (UnsupportedFlavorException ufe){}
      catch (IOException ioe){}
    }

  }


  public void insertData(SequenceListTableModel seqModel, Point ploc,
                         String fileName, Boolean bremote)
  {
    int row = rowAtPoint(ploc);
    seqModel.modelVector.insertElementAt(new SequenceData(fileName,"","",
                      new Boolean(false), new Boolean(false),bremote),row);

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
    new ColumnData("List File",15,JLabel.LEFT),
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
  public static final int COL_LIST = 3;
  public static final int COL_DEF  = 4;

  public void setDefaultData()
  {
    modelVector.removeAllElements();
    Boolean bdef = new Boolean(false);
    for(int i=0;i<getColumnCount();i++)
      modelVector.addElement(new SequenceData("","","",bdef,bdef,bdef)); 
    
  }
  

  public SequenceData getSequenceData(int nrow)
  {
    return (SequenceData)modelVector.get(nrow);
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
      case COL_LIST:  return row.s_listFile;
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
      case COL_LIST:
        row.s_listFile = (Boolean)value;
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


