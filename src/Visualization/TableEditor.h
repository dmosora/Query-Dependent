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

#ifndef TABLEEDITOR_H
#define TABLEEDITOR_H

#include <QSqlTableModel>
#include <QtGui/QDialog>

class QPushButton;
class QDialogButtonBox;
class QSqlTableModel;


class TableEditor : public QDialog
{
	Q_OBJECT

public:
	TableEditor(const QString& sConnectionName, const QString& sDataName, QWidget *parent = 0);
	virtual ~TableEditor();

 private slots:
     void submit();
     void insertRow();

 private:
     QPushButton *submitButton;
     QPushButton *insertRowButton;
     QPushButton *revertButton;
     QPushButton *quitButton;
     QDialogButtonBox *buttonBox;
     QSqlTableModel *model;
};

#endif // TABLEEDITOR_H
