
#include <QTableView>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSqlError>
#include <QPushButton>
#include <QDialogButtonBox>

#include "TableEditor.h"


TableEditor::TableEditor(const QString& sConnectionName, const QString& sDataName, QWidget *parent)
	: QDialog(parent)
{
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

	connect(submitButton, SIGNAL(clicked()), this, SLOT(submit()));
	connect(revertButton, SIGNAL(clicked()), model, SLOT(revertAll()));
	connect(insertRowButton, SIGNAL(clicked()), this, SLOT(insertRow()));
	connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addWidget(view);
	mainLayout->addWidget(buttonBox);
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
