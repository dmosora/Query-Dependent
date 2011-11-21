#include "EventGlyph.h"

	// * * * * * * *
// GlyphLineItem
	// * * * * * * *

// * * * * * * * * * * * * * * * * *

GlyphLineItem::GlyphLineItem(QGraphicsItem* parent, QGraphicsScene* scene, EventGlyph* glyph, const std::string& id)
	: QGraphicsLineItem(parent, scene)
{
	parent_glyph = glyph;
	set_id = id;
}

// * * * * * * * * * * * * * * * * *

GlyphLineItem::~GlyphLineItem(void)
{}

// * * * * * * * * * * * * * * * * *

void GlyphLineItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	parent_glyph->ActivateSet(set_id);
}

// * * * * * * * * * * * * * * * * *

	// * * * * * * *
// GlyphPointItem
	// * * * * * * *

// * * * * * * * * * * * * * * * * *

GlyphPointItem::GlyphPointItem(QGraphicsItem* parent, QGraphicsScene* scene, EventGlyph* glyph, const std::string& id)
	: QGraphicsEllipseItem(parent, scene)
{
	parent_glyph = glyph;
	set_id = id;
}

// * * * * * * * * * * * * * * * * *

GlyphPointItem::~GlyphPointItem(void)
{}

// * * * * * * * * * * * * * * * * *

void GlyphPointItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	parent_glyph->ActivateSet(set_id);
}

// * * * * * * * * * * * * * * * * *

	// * * * * * * *
// PointSet
	// * * * * * * *

// * * * * * * * * * * * * * * * * *

PointSet::PointSet(const std::string& id)
{
	set_id = id;
	active_state = false;
}

// * * * * * * * * * * * * * * * * *

PointSet::~PointSet(void)
{
	for(int i=0; i<(int)set_lines.size(); ++i)
	{
		delete set_lines[i];
	}

	for(int i=0; i<(int)set_points.size(); ++i)
	{
		delete set_points[i];
	}
}

// * * * * * * * * * * * * * * * * *

void PointSet::SetState(const bool active)
{
	active_state = active;
}

// * * * * * * * * * * * * * * * * *

bool PointSet::GetState(void)
{
	return active_state;
}

// * * * * * * * * * * * * * * * * *

std::string PointSet::GetID(void)
{
	return set_id;
}

// * * * * * * * * * * * * * * * * *

void PointSet::PushLine(GlyphLineItem* line)
{
	set_lines.push_back(line);
}

// * * * * * * * * * * * * * * * * *

void PointSet::PushPoint(GlyphPointItem* point)
{
	set_points.push_back(point);
}

// * * * * * * * * * * * * * * * * *

void PointSet::Activate(void)
{
	active_state = true;

	for(int i=0; i<(int)set_lines.size(); ++i)
	{
		set_lines[i]->setPen(QPen(QColor(130,240,140,255),5));
	}

	for(int i=0; i<(int)set_points.size(); ++i)
	{
		set_points[i]->setBrush(QColor(QColor(130,240,140,255)));
		set_points[i]->setPen(QPen(QColor(110,220,120,255),3));
	}
}

// * * * * * * * * * * * * * * * * *

void PointSet::Deactivate(void)
{
	active_state = false;
	
	for(int i=0; i<(int)set_lines.size(); ++i)
	{
		set_lines[i]->setPen(QPen(QColor(40,95,150,255),4));
	}

	for(int i=0; i<(int)set_points.size(); ++i)
	{
		set_points[i]->setBrush(QColor(40,95,150,255));
		set_points[i]->setPen(QPen(QColor(20, 75, 130),2));
	}
}

// * * * * * * * * * * * * * * * * *

	// * * * * * * *
// EventGlyph
	// * * * * * * *

// * * * * * * * * * * * * * * * * *

EventGlyph::EventGlyph(const int x_res, const int y_res, const int number_of_variables)
{
		//initializing data members
	glyph_scene = new QGraphicsScene();
	glyph_view = new QGraphicsView(glyph_scene);
	x_resolution = x_res;
	y_resolution = y_res;
	radius = std::min(x_res, y_res)/2.0 * 0.95;
	number_of_attributes = number_of_variables;
	active_set_id = "";
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

EventGlyph::~EventGlyph(void)
{
	std::deque<QGraphicsLineItem*>::iterator iter = background_lines.begin();
	while(iter != background_lines.end())
	{
		delete *iter;
		++iter;
	}

	std::map<std::string, PointSet*>::iterator set_iter = point_sets.begin();
	while(set_iter != point_sets.end())
	{
		delete set_iter->second;
		++set_iter;
	}

	delete glyph_view;
	delete glyph_scene;
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::DrawGlyphBackground(void)
{
	for(int i=0; i<number_of_attributes; ++i)
	{
		double x_draw = (x_resolution/2) + radius*std::cos(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
		double y_draw = (y_resolution/2) + radius*std::sin(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
		QGraphicsLineItem* temp_line = new QGraphicsLineItem(0, glyph_scene);
		temp_line->setLine((x_resolution/2), (y_resolution/2), x_draw, y_draw);
		temp_line->setPen(QPen(QColor(11,11,11,255),3));
		background_lines.push_back(temp_line);
	}
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::CreatePointSet(const std::vector<float>& data, const std::string& id)
{
	if((int)data.size() == number_of_attributes)
	{
		PointSet* new_set = new PointSet(id);
		std::vector<std::pair<float, float> > draw_points;
		for(int i=0; i<number_of_attributes; ++i)
		{
			float x_draw = (x_resolution/2) + data[i]*radius*std::cos(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
			float y_draw = (y_resolution/2) + data[i]*radius*std::sin(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
			draw_points.push_back(std::pair<float, float>(x_draw, y_draw));
		}
		for(int i=0; i<number_of_attributes-1; ++i)
		{
			GlyphLineItem* temp_line = new GlyphLineItem(0, glyph_scene, this, id);
			temp_line->setLine(draw_points[i].first, draw_points[i].second, draw_points[i+1].first, draw_points[i+1].second);
			temp_line->setPen(QPen(QColor(40,95,150,255),4));
			new_set->PushLine(temp_line);
		}
		GlyphLineItem* temp_line = new GlyphLineItem(0, glyph_scene, this, id);
		temp_line->setLine(draw_points[number_of_attributes-1].first, draw_points[number_of_attributes-1].second, draw_points[0].first, draw_points[0].second);
		temp_line->setPen(QPen(QColor(40,95,150,255),4));
		new_set->PushLine(temp_line);
		for(int i=0; i<number_of_attributes; ++i)
		{
			GlyphPointItem* temp_point = new GlyphPointItem(0, glyph_scene, this, id);
			temp_point->setRect(draw_points[i].first-10, draw_points[i].second-10, 20, 20);
			temp_point->setBrush(QColor(40,95,150,255));
			temp_point->setPen(QPen(QColor(20, 75, 130),2));
			new_set->PushPoint(temp_point);
		}
		point_sets.insert(std::pair<std::string, PointSet*>(id, new_set));
	}
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::ActivateSet(const std::string& id)
{
	std::map<std::string, PointSet*>::iterator set_iter = point_sets.begin();
	while(set_iter != point_sets.end())
	{
		if(set_iter == point_sets.find(id))
		{
			set_iter->second->Activate();
			active_set_id = id;
		}
		else
			set_iter->second->Deactivate();
		++set_iter;
	}
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::ClearSets(void)
{
	std::map<std::string, PointSet*>::iterator set_iter = point_sets.begin();
	while(set_iter != point_sets.end())
	{
		delete set_iter->second;
		++set_iter;
	}
	point_sets.clear();
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::SetAttributeNumber(const int number_of_variables)
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

std::string EventGlyph::GetActiveSetID(void)
{
	return active_set_id;
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::ShowGlyph(void)
{
	glyph_view->show();
}

// * * * * * * * * * * * * * * * * *
