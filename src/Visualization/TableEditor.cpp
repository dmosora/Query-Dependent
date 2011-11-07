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

#include <QTableView>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSqlError>
#include <QPushButton>
#include <QDialogButtonBox>

#include "TableEditor.h"


// Macro enabling buttons to modify data.
//#define USE_MODIFICATION_BUTTONS


TableEditor::TableEditor(const QString& sConnectionName, const QString& sDataName, QWidget *parent)
   : QDialog(parent)
{
   QHBoxLayout *mainLayout = new QHBoxLayout;

   model = new QSqlTableModel( this, QSqlDatabase::database(sConnectionName) );
   model->setTable(sDataName);
   model->setEditStrategy(QSqlTableModel::OnManualSubmit);
   model->select();
   //!@todo There is a parameter definition in the system already.  Need to
   //!      get that into here and populate the nice names as the column 
   //!      headers.
   //! model->setHeaderData(0, Qt::Horizontal, tr("Nice Name"));
   QTableView *view = new QTableView;
   view->setModel(model);
   // Resizing takes a lot of time...
   //view->resizeColumnsToContents();
   mainLayout->addWidget(view);

#  if defined(USE_MODIFICATION_BUTTONS)
   {
      submitButton = new QPushButton(tr("Submit"));
      submitButton->setDefault(true);
      insertRowButton = new QPushButton(tr("&Insert Row"));
      revertButton = new QPushButton(tr("&Revert"));
      quitButton = new QPushButton(tr("Quit"));

      buttonBox = new QDialogButtonBox(Qt::Vertical);
      buttonBox->addButton(submitButton, QDialogButtonBox::ActionRole);
      buttonBox->addButton(revertButton, QDialogButtonBox::ActionRole);
      buttonBox->addButton(insertRowButton, QDialogButtonBox::ActionRole);
      buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);
      mainLayout->addWidget(buttonBox);

      connect(submitButton, SIGNAL(clicked()), this, SLOT(submit()));
      connect(revertButton, SIGNAL(clicked()), model, SLOT(revertAll()));
      connect(insertRowButton, SIGNAL(clicked()), this, SLOT(insertRow()));
      connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
   }
#  endif

   setLayout(mainLayout);

   setWindowTitle(tr("Cached Table"));
}

TableEditor::~TableEditor()
{

}

void TableEditor::submit()
{
   model->database().transaction();
   if (model->submitAll()) {
      model->database().commit();
   } else {
      model->database().rollback();
      QMessageBox::warning
         ( this
         , tr("Cached Table")
         , tr("The database reported an error: %1").arg(model->lastError().text())
         );
   }
}

void TableEditor::insertRow()
{
   model->insertRow( model->rowCount() );
}
