#ifndef _CHART_H_
#define _CHART_H_

#include <QWidget>

enum ViewType
{
	Manufacturer,
	Time,
	Both,
	None
};

class Chart : public QWidget
{
	Q_OBJECT

public:

	Chart(const QString& sConnectionName, const QString& sDataName, QWidget *parent = 0);
	virtual ~Chart();

	void draw();

	void SetChartView(ViewType view);
	
 protected:
     void paintEvent(QPaintEvent* event);

	 QString  m_sConnectionName;
	 QString  m_sDataName;
	 ViewType m_eView;
};

#endif // _CHART_H_
