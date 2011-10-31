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

#ifndef _DATASELECTIONS_H_
#define _DATASELECTIONS_H_


#include "DataMgmt.h"


//! Class to track the data selections for a particular purpose.  The
//! long-term application of this class will be toward abstracting the 
//! SQL access from the system.
class DataSelections
{
public:

	DataSelections();
	virtual ~DataSelections();
	
	void ClearSelections();
	void SetSelection(const QString& name);

	const QStringList& GetSelectedAttributes();

	
 protected:
	QStringList m_selections;
};

#endif // _DATASELECTIONS_H_
