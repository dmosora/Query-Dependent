// Written by David Sheets
// Visualization product for analyzing data, flight data in particular.
// Copyright (C) 2011  David Sheets (dsheets4@kent.edu)
//
// Visualization is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "DockWidgetAttributes.h"


DockWidgetAttributes::DockWidgetAttributes(QWidget *parent)
   : QDockWidget(parent)
   , m_selections(0)
{
   ui.setupUi(this);

   // Create the header columns for the tree view.
   QStringList hdr;
   hdr << tr("Attribute") << tr("Type");
   ui.treeWidgetAttributes->setHeaderLabels(hdr);

   // Connect up to the UI to handle the items.
   connect
      ( ui.treeWidgetAttributes, SIGNAL(itemSelectionChanged())
      , this,                    SLOT(SelectionChanged()) );
}

DockWidgetAttributes::~DockWidgetAttributes()
{
}

void DockWidgetAttributes::CreatePlaceHolder(const QString& sFlightName, QWidget* placeHolder)
{
   // Qt documentation discourage this approach for dynamic items...
   QTreeWidgetItem *parent;
   parent = new QTreeWidgetItem(ui.treeWidgetAttributes);
   parent->setText(0,sFlightName);

   if( placeHolder )
   {
      ui.treeWidgetAttributes->setItemWidget( parent, 1, placeHolder );
   }
}

void DockWidgetAttributes::PopulateTree(const QString& sFlightName, Data::DataMgmt* dataMgmt)
{
   // First, check to see if the item is already in the tree 
   // (presumably a placeholder)
   QList<QTreeWidgetItem*> results = ui.treeWidgetAttributes->findItems( sFlightName, Qt::MatchExactly, 0 );
   Q_ASSERT( results.size() < 2 ); // Should not have multiple items of the same flight name.
   QTreeWidgetItem *parent;
   if( results.size() > 0 )
   {
      parent = results.at(0);
   }
   else
   {
      parent = new QTreeWidgetItem(ui.treeWidgetAttributes);
      parent->setText(0,sFlightName);
   }

   QTreeWidgetItem* item;
   const Data::ColumnDefList& columns = dataMgmt->GetColumnDefinitions(sFlightName);
   for( int i = 0; i < columns.size(); ++i )
   {
      const Data::ColumnDef& col = columns.at(i);
      if( col.bGood )
      {
         item = new QTreeWidgetItem(parent);
         item->setText(0, col.sParamName);
         item->setData(0, Qt::UserRole, QVariant(col.sParamNameComp));
         switch( col.eParamType )
         {
         case Data::ParamType_String:
            item->setText(1, tr("String"));
            break;
         case Data::ParamType_Numeric:
            item->setText(1, tr("Numeric"));
            break;
         default:
            item->setText(1, tr("Unknown"));
         }
      }
   }
   ui.treeWidgetAttributes->sortItems(0,Qt::AscendingOrder);
}


void DockWidgetAttributes::SetModel(Data::DataSelections* selections)
{
   m_selections = selections;
}

void DockWidgetAttributes::SelectionChanged()
{
   QList<QTreeWidgetItem*> selections;
   if( m_selections )
   {
      // Clear out the previous selections
      m_selections->ClearSelections();

      // Build up a new list of selections
      selections = ui.treeWidgetAttributes->selectedItems();
      for( int i = 0; i < selections.size(); ++i )
      {
         QTreeWidgetItem* item   = selections.at(i);
		 QTreeWidgetItem* parent = item->parent();
		 if( item && parent )
		 {
			m_selections->SetSelection(parent->text(0), item->data(0, Qt::UserRole).toString());
		 }
      }
   }
}
