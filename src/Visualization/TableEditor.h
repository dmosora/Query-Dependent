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
