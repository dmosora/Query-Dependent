// Written by David Sheets

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


	void SetModel(DataSelections* selections);
	void PopulateTree(DataMgmt* dataMgmt, const QString& sDataName);
	
protected slots:
	void SelectionChanged();

private:
	Ui::DockWidgetAttributes ui; //!< Qt generated from the .ui file from Designer
	
	DataSelections* m_selections; //!< Selections being manipulated by this widget
};

#endif // _DOCKWIDGETATTRIBUTES_H
