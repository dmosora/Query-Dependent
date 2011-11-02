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

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include "DataSelections.h"


namespace Data
{
   DataSelections::DataSelections()
   {
   }

   DataSelections::~DataSelections()
   {

   }

   void DataSelections::ClearSelections()
   {
      m_selections.clear();
   }


   const QStringList& DataSelections::GetSelectedAttributes()
   {
      return m_selections;
   }

   void DataSelections::SetSelection(const QString& column)
   {
      m_selections.push_back(column);
   }
};
