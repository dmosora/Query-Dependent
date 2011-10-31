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

	
	connect( ui.treeWidgetAttributes, SIGNAL(itemSelectionChanged()),
		     this,                    SLOT(SelectionChanged()) );
}

DockWidgetAttributes::~DockWidgetAttributes()
{

}

void DockWidgetAttributes::PopulateTree(DataMgmt* dataMgmt, const QString& sDataName)
{
	QStringList hdr;
	hdr << tr("Attribute") << tr("Type");
	ui.treeWidgetAttributes->setHeaderLabels(hdr);

	//! @todo Get the actual name of the flight file and post the data using that 
	//!       instead of the generic "Flight" label.
	QTreeWidgetItem *parent;
	parent = new QTreeWidgetItem(ui.treeWidgetAttributes);
	parent->setText(0,tr("Flight"));
	
	QTreeWidgetItem* item;
	const ColumnDefList& columns = dataMgmt->GetColumnDefinitions(sDataName);
	for( int i = 0; i < columns.size(); ++i )
	{
		const ColumnDef& col = columns.at(i);
		if( col.bGood )
		{
			item = new QTreeWidgetItem(parent);
			item->setText(0, col.sParamName);
			item->setData(0, Qt::UserRole, QVariant(col.sParamNameComp));
			switch( col.eParamType )
			{
			case ParamType_String:
				item->setText(1, tr("String"));
				break;
			case ParamType_Numeric:
				item->setText(1, tr("Numeric"));
				break;
			default:
				item->setText(1, tr("Unknown"));
			}
		}
	}
	ui.treeWidgetAttributes->sortItems(0,Qt::AscendingOrder);
}


void DockWidgetAttributes::SetModel(DataSelections* selections)
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
			QTreeWidgetItem* item = selections.at(i);
			m_selections->SetSelection(item->data(0, Qt::UserRole).toString());
		}
	}
}
