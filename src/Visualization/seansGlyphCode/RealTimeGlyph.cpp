#include "RealTimeGlyph.h"

	// * * * * * * *
// RealTimeGlyph
	// * * * * * * *

// * * * * * * * * * * * * * * * * *

RealTimeGlyph::RealTimeGlyph(const int x_res, const int y_res, const int number_of_variables)
{
		//initializing data members
	glyph_scene = new QGraphicsScene();
	glyph_view = new QGraphicsView(glyph_scene);
	x_resolution = x_res;
	y_resolution = y_res;
	radius = std::min(x_res, y_res)/2.0 * 0.95;
	number_of_attributes = number_of_variables;
		//drawing the background gradient
	QLinearGradient gradient(0, 0, x_resolution, y_resolution);
	gradient.setColorAt(0, QColor(100,100,100,255));
	gradient.setColorAt(1, QColor(200,200,200,255));
	glyph_view->setBackgroundBrush(gradient);
	glyph_view->setRenderHint(QPainter::Antialiasing);
		//setting scene object size
	//glyph_scene->setSceneRect(0, 0, x_res, y_res);
	glyph_view->setFixedSize(x_res, y_res);
		//calling the background line drawing method
	DrawGlyphBackground();
}

// * * * * * * * * * * * * * * * * *

RealTimeGlyph::~RealTimeGlyph(void)
{
	std::deque<QGraphicsLineItem*>::iterator iter = background_lines.begin();
	while(iter != background_lines.end())
	{
		delete *iter;
		++iter;
	}

	std::deque<QGraphicsLineItem*>::iterator line_iter = line_set.begin();
	while(line_iter != line_set.end())
	{
		delete *line_iter;
		++line_iter;
	}

	std::deque<QGraphicsEllipseItem*>::iterator point_iter = point_set.begin();
	while(point_iter != point_set.end())
	{
		delete *point_iter;
		++point_iter;
	}

	delete glyph_view;
	delete glyph_scene;
}

// * * * * * * * * * * * * * * * * *

void RealTimeGlyph::DrawGlyphBackground(void)
{
	for(int i=0; i<number_of_attributes; ++i)
	{
		double x_draw = (x_resolution/2) + radius*std::cos(((double)i*2.0*RTGlyph::glyph_pi)/(double)number_of_attributes + (3.0*RTGlyph::glyph_pi/2.0));
		double y_draw = (y_resolution/2) + radius*std::sin(((double)i*2.0*RTGlyph::glyph_pi)/(double)number_of_attributes + (3.0*RTGlyph::glyph_pi/2.0));
		QGraphicsLineItem* temp_line = new QGraphicsLineItem(0, glyph_scene);
		temp_line->setLine((x_resolution/2), (y_resolution/2), x_draw, y_draw);
		temp_line->setPen(QPen(QColor(11,11,11,255),3));
		background_lines.push_back(temp_line);
	}
}

// * * * * * * * * * * * * * * * * *

void RealTimeGlyph::DrawPointSet(const std::vector<float>& data)
{
	ClearGlyph();
	if((int)data.size() == number_of_attributes)
	{
		std::vector<std::pair<float, float> > draw_points;
		for(int i=0; i<number_of_attributes; ++i)
		{
			float x_draw = (x_resolution/2) + data[i]*radius*std::cos(((double)i*2.0*RTGlyph::glyph_pi)/(double)number_of_attributes + (3.0*RTGlyph::glyph_pi/2.0));
			float y_draw = (y_resolution/2) + data[i]*radius*std::sin(((double)i*2.0*RTGlyph::glyph_pi)/(double)number_of_attributes + (3.0*RTGlyph::glyph_pi/2.0));
			draw_points.push_back(std::pair<float, float>(x_draw, y_draw));
		}
		for(int i=0; i<number_of_attributes-1; ++i)
		{
			QGraphicsLineItem* temp_line = new QGraphicsLineItem(0, glyph_scene);
			temp_line->setLine(draw_points[i].first, draw_points[i].second, draw_points[i+1].first, draw_points[i+1].second);
			temp_line->setPen(QPen(QColor(40,95,150,255),4));
			line_set.push_back(temp_line);
		}
		QGraphicsLineItem* temp_line = new QGraphicsLineItem(0, glyph_scene);
		temp_line->setLine(draw_points[number_of_attributes-1].first, draw_points[number_of_attributes-1].second, draw_points[0].first, draw_points[0].second);
		temp_line->setPen(QPen(QColor(40,95,150,255),4));
		line_set.push_back(temp_line);
		for(int i=0; i<number_of_attributes; ++i)
		{
			QGraphicsEllipseItem* temp_point = new QGraphicsEllipseItem(0, glyph_scene);
			temp_point->setRect(draw_points[i].first-10, draw_points[i].second-10, 20, 20);
			temp_point->setBrush(QColor(40,95,150,255));
			temp_point->setPen(QPen(QColor(20, 75, 130),2));
			point_set.push_back(temp_point);
		}
	}
}

// * * * * * * * * * * * * * * * * *

void RealTimeGlyph::ClearGlyph(void)
{
	std::deque<QGraphicsLineItem*>::iterator line_iter = line_set.begin();
	while(line_iter != line_set.end())
	{
		delete *line_iter;
		++line_iter;
	}

	std::deque<QGraphicsEllipseItem*>::iterator point_iter = point_set.begin();
	while(point_iter != point_set.end())
	{
		delete *point_iter;
		++point_iter;
	}

	line_set.clear();
	point_set.clear();
}

// * * * * * * * * * * * * * * * * *

void RealTimeGlyph::SetAttributeNumber(const int number_of_variables)
{
	std::deque<QGraphicsLineItem*>::iterator iter = background_lines.begin();
	while(iter != background_lines.end())
	{
		delete *iter;
		++iter;
	}
	background_lines.clear();
	number_of_attributes = number_of_variables;
	DrawGlyphBackground();
}

// * * * * * * * * * * * * * * * * *

void RealTimeGlyph::ShowGlyph(void)
{
	glyph_view->show();
}

// * * * * * * * * * * * * * * * * *
