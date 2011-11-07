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
#ifndef _DOCKWIDGETATTRIBUTES_H
#define _DOCKWIDGETATTRIBUTES_H

#include "DataMgmt.h"
#include "DataSelections.h"

#include "ui_DockWidgetAttributes.h"


//! Represents the Main Window of the Information Visualization application.
class DockWidgetAttributes : public QDockWidget
{
   Q_OBJECT

public:
   DockWidgetAttributes(QWidget *parent = 0);
   ~DockWidgetAttributes();

   //! Set the selections that the user interface will be manipulating.
   //! @param selections  Pointer to the data selections to manipulate.
   void SetModel(Data::DataSelections* selections);

   //! Places a root element in the tree to act as a place-holder for a flight
   //! that is not yet populated.
   //! @param sFlightName  Unique identifier for a flight.
   //! @param placeHolder  Widget to be placed in the tree.
   void CreatePlaceHolder(const QString& sFlightName, QWidget* placeHolder);

   //! Populates the attribute tree for the given flight using the given
   //! data management.
   //! @param sFlightName  Unique identifier for a flight.
   //! @param dataMgmt     Pointer to the data manager with the flight data.
   void PopulateTree(const QString& sFlightName, Data::DataMgmt* dataMgmt);

protected slots:
   //! Slot to handle the user click in the selection control.  It maintains
   //! and updates the model.
   void SelectionChanged();

private:
   Ui::DockWidgetAttributes ui; //!< Qt generated from the .ui file from Designer

   Data::DataSelections* m_selections; //!< Selections being manipulated by this widget
};

#endif // _DOCKWIDGETATTRIBUTES_H
