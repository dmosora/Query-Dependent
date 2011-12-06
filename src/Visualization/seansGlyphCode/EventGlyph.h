	//Sean R 1117 , Liquid Light
// * * *
#ifndef EVENTGLYPH_H
#define EVENTGLYPH_H
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
namespace EGlyph
{
	static const double glyph_pi = 3.1415926535897932;
}

//* * *
	//Forward Declaration
class EventGlyph;
// * * *

class GlyphGraphicsView : public QGraphicsView
{
public:
	GlyphGraphicsView(QGraphicsScene* scene, EventGlyph* glyph);
protected:
	void resizeEvent(QResizeEvent *event);
private:
	EventGlyph* parent_glyph;
};

// * * * 

class GlyphAxisItem : public QGraphicsLineItem
{
public:
	GlyphAxisItem(QGraphicsItem* parent, QGraphicsScene* scene, QLabel* glyph_axis_label, const std::string& attribute_name);
	~GlyphAxisItem(void);
	void SetAxisName(const std::string& attribute_name);

protected:
	void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
	QLabel* axis_label;
	std::string axis_name;
};

// * * *

class GlyphLineItem : public QGraphicsLineItem
{
public:
	GlyphLineItem(QGraphicsItem* parent, QGraphicsScene* scene, EventGlyph* glyph, const std::string& id, const bool warning);
	~GlyphLineItem(void);
	bool CheckWarning(void);
	bool CheckIfMissing(void);
	void SetMissing(void);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
private:
	bool warning_mode;
	bool missing_value;
	EventGlyph* parent_glyph;
	std::string set_id;
};

// * * *

class GlyphPointItem : public QGraphicsEllipseItem
{
public:
	GlyphPointItem(QGraphicsItem* parent, QGraphicsScene* scene, EventGlyph* glyph, const std::string& id, const bool warning);
	~GlyphPointItem(void);
	bool CheckWarning(void);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
private:
	bool warning_mode;
	EventGlyph* parent_glyph;
	std::string set_id;
};

// * * *

class PointSet
{
public:
	PointSet(const std::string& id);
	~PointSet(void);

	void SetState(const bool active);
	bool GetState(void);
	std::string GetID(void);

	void PushLine(GlyphLineItem* line);
	void PushPoint(GlyphPointItem* point);
	
	void Activate(void);
	void Deactivate(void);

private:
	std::vector<GlyphLineItem*> set_lines;
	std::vector<GlyphPointItem*> set_points;

	std::string set_id;
	bool active_state;
};

// * * *

class EventGlyph : public QWidget
{
public:
	EventGlyph(const int x_res, const int y_res, const int number_of_variables);
	~EventGlyph(void);

	void CreatePointSet(const std::vector<float>& data, const std::string& id);
	void SetMinLines(const std::vector<float>& data);
	void SetMaxLines(const std::vector<float>& data);
	void ActivateSet(const std::string& id);
	void DeactivateSet(const std::string& id);
	void ClearSets(void);
	void ClearMinMaxLines(void);
	void SetAttributeNumber(const int number_of_variables);
	void SetAxisLabels(const std::vector<std::string>& axis_names);

	std::string GetActiveSetID(void);
	void ShowGlyph(void);

	QGraphicsView* GetGlyphView(void);

	void RedrawLabelsOnResize(const int x_res, const int y_res);

private:
	void DrawGlyphBackground(void);
	QGraphicsScene *glyph_scene;
	QGraphicsView *glyph_view;
	QLabel *axis_label;
	QLabel *set_label;
	std::deque<QLabel*> name_labels;

	int number_of_attributes;
	int x_resolution, y_resolution;
	float radius;
	std::vector<float> min_data;
	std::vector<float> max_data;

	std::deque<GlyphAxisItem*> background_lines;
	std::deque<QGraphicsLineItem*> min_lines;
	std::deque<QGraphicsLineItem*> max_lines;
	std::map<std::string, PointSet*> point_sets;
	std::string active_set_id;
};

#endif
