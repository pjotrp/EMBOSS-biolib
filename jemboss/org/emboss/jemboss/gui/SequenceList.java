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


public class SequenceList extends JFrame
{

  private JTable table;
  private SequenceListTableModel seqModel;

  public SequenceList()
  {
    super("Sequence List");
    setSize(500,200);
    table = new JTable();
    seqModel = new SequenceListTableModel();
    table.setAutoCreateColumnsFromModel(false);
    table.setModel(seqModel);
    table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);

//create the columns of the table
    for(int i=0;i<SequenceListTableModel.modelColumns.length;i++)
    {
      TableCellRenderer renderer;
      if(i==SequenceListTableModel.COL_DEF)    
        renderer = new CheckCellRenderer();
      else
      {
        DefaultTableCellRenderer text = new DefaultTableCellRenderer();
        text.setHorizontalAlignment(SequenceListTableModel.modelColumns[i].alignment);
        renderer = text;
      }

      TableCellEditor editor;
      if(i==SequenceListTableModel.COL_NAME ||
         i==SequenceListTableModel.COL_PATH)
        editor = new DefaultCellEditor(new JTextField());
      else
        editor = new DefaultCellEditor(new JCheckBox());

      TableColumn column = new TableColumn(i,
                        SequenceListTableModel.modelColumns[i].width,
                        renderer,editor);
      table.addColumn(column);
    }

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

    getContentPane().add(buttonPanel, BorderLayout.SOUTH);
  }

}

class CheckCellRenderer extends JCheckBox implements TableCellRenderer
{
  protected static Border m_noFocusBorder;

  public CheckCellRenderer() 
  {
    super();
    m_noFocusBorder = new EmptyBorder(1, 2, 1, 2);
    setOpaque(true);
    setBorder(m_noFocusBorder);
  }

  public Component getTableCellRendererComponent(JTable table,
   Object value, boolean isSelected, boolean hasFocus,
   int row, int column)
  {
    if (value instanceof Boolean) 
    {
      Boolean b = (Boolean)value;
      setSelected(b.booleanValue());
    }

    setBackground(isSelected && !hasFocus ?
      table.getSelectionBackground() : table.getBackground());
    setForeground(isSelected && !hasFocus ?
      table.getSelectionForeground() : table.getForeground());

    setFont(table.getFont());
    setBorder(hasFocus ? UIManager.getBorder(
      "Table.focusCellHighlightBorder") : m_noFocusBorder);

    return this;
  }
}

/**
*
* Content of each column in the JTable
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
* Content of each row in the JTable
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
    new ColumnData("Name",100,JLabel.LEFT),
    new ColumnData("Path",150,JLabel.LEFT),
    new ColumnData("Default",15,JLabel.LEFT)
  };

  public static final int COL_NAME = 0;
  public static final int COL_PATH = 1;
  public static final int COL_DEF  = 2;

  public void setDefaultData()
  {
    modelVector.removeAllElements();
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


