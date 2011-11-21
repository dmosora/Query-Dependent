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

// * * *

class RealTimeGlyph : public QWidget
{
public:
	RealTimeGlyph(const int x_res, const int y_res, const int number_of_variables);
	~RealTimeGlyph(void);

	
	void DrawPointSet(const std::vector<float>& data);
	void ClearGlyph(void);
	void SetAttributeNumber(const int number_of_variables);

	void ShowGlyph(void);

private:
	void DrawGlyphBackground(void);
	QGraphicsScene *glyph_scene;
	QGraphicsView *glyph_view;

	int number_of_attributes;
	int x_resolution, y_resolution;
	float radius;

	std::deque<QGraphicsLineItem*> background_lines;
	std::deque<QGraphicsLineItem*> line_set;
	std::deque<QGraphicsEllipseItem*> point_set;
};

#endif
