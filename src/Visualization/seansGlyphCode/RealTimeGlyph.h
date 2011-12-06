	//Sean R 1117 , Liquid Light
// * * *
#ifndef REALTIMEGLYPH_H
#define REALTIMEGLYPH_H
// * * *
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>
#include <QtGui/QWidget>
#include <QtGui/QGraphicsEllipseItem>
#include <QtGui/QColor>
#include <QtGui/QPen>
#include <QtGui/QLabel>
#include <QtGui/QFont>
// * * *
#include <deque>
#include <map>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <stdlib.h>
// * * *
namespace RTGlyph
{
	static const double glyph_pi = 3.1415926535897932;
}
//* * *
	//Forward Declaration
class RealTimeGlyph;
// * * *

class RTGlyphGraphicsView : public QGraphicsView
{
public:
	RTGlyphGraphicsView(QGraphicsScene* scene, RealTimeGlyph* glyph);
protected:
	void resizeEvent(QResizeEvent *event);
private:
	RealTimeGlyph* parent_glyph;
};

// * * * 

class RTGlyphAxisItem : public QGraphicsLineItem
{
public:
	RTGlyphAxisItem(QGraphicsItem* parent, QGraphicsScene* scene, QLabel* glyph_axis_label, const std::string& attribute_name);
	~RTGlyphAxisItem(void);
	void SetAxisName(const std::string& attribute_name);

protected:
	void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
	QLabel* axis_label;
	std::string axis_name;
};

// * * *

class RealTimeGlyph : public QWidget
{
public:
	RealTimeGlyph(const int x_res, const int y_res, const int number_of_variables);
	~RealTimeGlyph(void);

	void DrawPointSet(const std::vector<float>& data);
	void ClearGlyph(void);
	void SetAttributeNumber(const int number_of_variables);
	void SetAxisLabels(const std::vector<std::string>& axis_names);

	void ShowGlyph(void);
	QGraphicsView* GetGlyphView(void);

	void RedrawLabelsOnResize(const int x_res, const int y_res);

private:
	void DrawGlyphBackground(void);
	QGraphicsScene *glyph_scene;
	QGraphicsView *glyph_view;
	QLabel *axis_label;
	std::deque<QLabel*> name_labels;

	int number_of_attributes;
	int x_resolution, y_resolution;
	float radius;

	std::deque<RTGlyphAxisItem*> background_lines;
	std::deque<QGraphicsLineItem*> line_set;
	std::deque<QGraphicsEllipseItem*> point_set;
};

#endif
