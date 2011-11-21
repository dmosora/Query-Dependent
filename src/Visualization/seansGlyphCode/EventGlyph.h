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

class GlyphLineItem : public QGraphicsLineItem
{
public:
	GlyphLineItem(QGraphicsItem* parent, QGraphicsScene* scene, EventGlyph* glyph, const std::string& id);
	~GlyphLineItem(void);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
private:
	EventGlyph* parent_glyph;
	std::string set_id;
};

// * * *

class GlyphPointItem : public QGraphicsEllipseItem
{
public:
	GlyphPointItem(QGraphicsItem* parent, QGraphicsScene* scene, EventGlyph* glyph, const std::string& id);
	~GlyphPointItem(void);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
private:
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
	void ActivateSet(const std::string& id);
	void ClearSets(void);
	void SetAttributeNumber(const int number_of_variables);

	std::string GetActiveSetID(void);
	void ShowGlyph(void);

private:
	void DrawGlyphBackground(void);
	QGraphicsScene *glyph_scene;
	QGraphicsView *glyph_view;

	int number_of_attributes;
	int x_resolution, y_resolution;
	float radius;

	std::deque<QGraphicsLineItem*> background_lines;
	std::map<std::string, PointSet*> point_sets;
	std::string active_set_id;
};

#endif
