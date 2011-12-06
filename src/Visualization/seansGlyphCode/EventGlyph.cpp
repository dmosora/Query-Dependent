#include "EventGlyph.h"

	// * * * * * * * 
// GlyphGraphicsView
	// * * * * * * * 

// * * * * * * * * * * * * * * * * *

GlyphGraphicsView::GlyphGraphicsView(QGraphicsScene* scene, EventGlyph* glyph)
	: QGraphicsView(scene)
{
	parent_glyph = glyph;
}

// * * * * * * * * * * * * * * * * *

void GlyphGraphicsView::resizeEvent(QResizeEvent *event)
{	
	parent_glyph->RedrawLabelsOnResize(this->size().width(), this->size().height());
	QGraphicsView::resizeEvent(event);
}

// * * * * * * * * * * * * * * * * *

	// * * * * * * * 
// GlyphAxisItem
	// * * * * * * * 

// * * * * * * * * * * * * * * * * *

GlyphAxisItem::GlyphAxisItem(QGraphicsItem* parent, QGraphicsScene* scene, QLabel* glyph_axis_label, const std::string& attribute_name)
	: QGraphicsLineItem(parent, scene)
{
	axis_label = glyph_axis_label;
	axis_name = attribute_name;
	setAcceptHoverEvents(true);
}

// * * * * * * * * * * * * * * * * *

GlyphAxisItem::~GlyphAxisItem(void)
{}

// * * * * * * * * * * * * * * * * *

void GlyphAxisItem::SetAxisName(const std::string& attribute_name)
{
	axis_name = attribute_name;
}

// * * * * * * * * * * * * * * * * *

void GlyphAxisItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	axis_label->setText(axis_name.c_str());
}

// * * * * * * * * * * * * * * * * *

void GlyphAxisItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	axis_label->setText("");
}

// * * * * * * * * * * * * * * * * *

	// * * * * * * *
// GlyphLineItem
	// * * * * * * *

// * * * * * * * * * * * * * * * * *

GlyphLineItem::GlyphLineItem(QGraphicsItem* parent, QGraphicsScene* scene, EventGlyph* glyph, const std::string& id, const bool warning)
	: QGraphicsLineItem(parent, scene)
{
	parent_glyph = glyph;
	set_id = id;
	warning_mode = warning;
	missing_value = false;
}

// * * * * * * * * * * * * * * * * *

GlyphLineItem::~GlyphLineItem(void)
{}

// * * * * * * * * * * * * * * * * *

bool GlyphLineItem::CheckWarning(void)
{
	return warning_mode;
}

// * * * * * * * * * * * * * * * * *

bool GlyphLineItem::CheckIfMissing(void)
{
	return missing_value;
}

// * * * * * * * * * * * * * * * * *

void GlyphLineItem::SetMissing(void)
{
	missing_value = true;
}

// * * * * * * * * * * * * * * * * *

void GlyphLineItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if(parent_glyph->GetActiveSetID() != set_id)
		parent_glyph->ActivateSet(set_id);
	else
		parent_glyph->DeactivateSet(set_id);
}		

// * * * * * * * * * * * * * * * * *

	// * * * * * * *
// GlyphPointItem
	// * * * * * * *

// * * * * * * * * * * * * * * * * *

GlyphPointItem::GlyphPointItem(QGraphicsItem* parent, QGraphicsScene* scene, EventGlyph* glyph, const std::string& id, const bool warning)
	: QGraphicsEllipseItem(parent, scene)
{
	parent_glyph = glyph;
	set_id = id;
	warning_mode = warning;
}

// * * * * * * * * * * * * * * * * *

GlyphPointItem::~GlyphPointItem(void)
{}

// * * * * * * * * * * * * * * * * *

bool GlyphPointItem::CheckWarning(void)
{
	return warning_mode;
}

// * * * * * * * * * * * * * * * * *

void GlyphPointItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if(parent_glyph->GetActiveSetID() != set_id)
		parent_glyph->ActivateSet(set_id);
	else
		parent_glyph->DeactivateSet(set_id);
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
		QPen glyph_pen(QColor(130,240,140,255),5);
		if(set_lines[i]->CheckIfMissing())
			glyph_pen.setStyle(Qt::DotLine);
		set_lines[i]->setPen(glyph_pen);
		set_lines[i]->setZValue(2);
	}

	for(int i=0; i<(int)set_points.size(); ++i)
	{
		//set_points[i]->setBrush(QColor(QColor(130,240,140,255)));
		set_points[i]->setPen(QPen(QColor(110,220,120,255),3));
		set_points[i]->setZValue(2);
	}
}

// * * * * * * * * * * * * * * * * *

void PointSet::Deactivate(void)
{
	active_state = false;
	
	for(int i=0; i<(int)set_lines.size(); ++i)
	{
		if(set_lines[i]->CheckWarning())
		{
			QPen glyph_pen(QColor(215,20,20,255),4);
			if(set_lines[i]->CheckIfMissing())
				glyph_pen.setStyle(Qt::DotLine);
			set_lines[i]->setPen(glyph_pen);
		}
		else
		{
			QPen glyph_pen(QColor(40,95,150,255),4);
			if(set_lines[i]->CheckIfMissing())
				glyph_pen.setStyle(Qt::DotLine);
			set_lines[i]->setPen(glyph_pen);
		}
		set_lines[i]->setZValue(-2);
	}

	for(int i=0; i<(int)set_points.size(); ++i)
	{
		if(set_points[i]->CheckWarning())
		{
			set_points[i]->setBrush(QColor(215,20,20,255));
			set_points[i]->setPen(QPen(QColor(180, 10, 10),2));
		}
		else
		{
			set_points[i]->setBrush(QColor(40,95,150,255));
			set_points[i]->setPen(QPen(QColor(20, 75, 130),2));
		}
		set_points[i]->setZValue(-2);
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
	//glyph_view = new QGraphicsView(glyph_scene);
	glyph_view = new GlyphGraphicsView(glyph_scene, this);

	//this->connect(glyph_view, SIGNAL(resize()), SLOT(resize()));
	//glyph_view->connect(this, SIGNAL(resize()), SLOT(resize()));

	x_resolution = x_res;
	y_resolution = y_res;
	radius = std::min(x_res, y_res)/2.0 * 0.85;
	number_of_attributes = number_of_variables;
	active_set_id = "";
		//setting axis label
	axis_label = new QLabel(glyph_view, 0);
	axis_label->setAlignment(Qt::AlignLeft);
	axis_label->setMargin(7);
	axis_label->setIndent(7);
	axis_label->setFont(QFont("Helvetica", 26));
	axis_label->setMinimumWidth(x_res/3);
		//setting set label
	set_label = new QLabel(glyph_view, 0);
	set_label->setAlignment(Qt::AlignRight);
	set_label->setMargin(7);
	set_label->setIndent(7);
	set_label->setFont(QFont("Helvetica", 26));
	set_label->setMinimumWidth(x_res);
	//set_label->setGeometry(x_res, 0, x_res/3, set_label->sizeHint().height());
		//drawing the background gradient
	QLinearGradient gradient(0, 0, x_resolution, y_resolution);
	gradient.setColorAt(0, QColor(100,100,100,255));
	gradient.setColorAt(1, QColor(200,200,200,255));
	glyph_view->setBackgroundBrush(gradient);
	glyph_view->setRenderHint(QPainter::Antialiasing);
		//setting scene object size
	//glyph_scene->setSceneRect(0, 0, x_res, y_res);
	//glyph_view->setFixedSize(x_res, y_res);
		//calling the background line drawing method
	DrawGlyphBackground();
}

// * * * * * * * * * * * * * * * * *

EventGlyph::~EventGlyph(void)
{
	std::deque<GlyphAxisItem*>::iterator iter = background_lines.begin();
	while(iter != background_lines.end())
	{
		delete *iter;
		++iter;
	}

	std::deque<QGraphicsLineItem*>::iterator min_iter = min_lines.begin();
	while(min_iter != min_lines.end())
	{
		delete *min_iter;
		++min_iter;
	}

	std::deque<QGraphicsLineItem*>::iterator max_iter = max_lines.begin();
	while(max_iter != max_lines.end())
	{
		delete *max_iter;
		++max_iter;
	}

	std::map<std::string, PointSet*>::iterator set_iter = point_sets.begin();
	while(set_iter != point_sets.end())
	{
		delete set_iter->second;
		++set_iter;
	}

	delete glyph_view;
	delete glyph_scene;
	//delete axis_label;
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::DrawGlyphBackground(void)
{
	for(int i=0; i<number_of_attributes; ++i)
	{
		double x_draw = (x_resolution/2) + radius*std::cos(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
		double y_draw = (y_resolution/2) + radius*std::sin(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
		GlyphAxisItem* temp_line = new GlyphAxisItem(0, glyph_scene, axis_label, "");
		temp_line->setLine((x_resolution/2), (y_resolution/2), x_draw, y_draw);
		temp_line->setPen(QPen(QColor(11,11,11,255),3));
		temp_line->setZValue(-3);
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
			if(data[i] != -1)
			{
				float x_draw = (x_resolution/2) + data[i]*radius*std::cos(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
				float y_draw = (y_resolution/2) + data[i]*radius*std::sin(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
				draw_points.push_back(std::pair<float, float>(x_draw, y_draw));
			}
			else
			{
				float x_draw = (x_resolution/2) + 0*radius*std::cos(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
				float y_draw = (y_resolution/2) + 0*radius*std::sin(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
				draw_points.push_back(std::pair<float, float>(x_draw, y_draw));
			}
		}
		bool warning;
		for(int i=0; i<number_of_attributes-1; ++i)
		{
			if(data[i] == -1 || data[i+1] == -1 || min_data.size() < number_of_attributes || max_data.size() < number_of_attributes)
				warning = false;
			else
				warning = (data[i] > max_data[i] || data[i] < min_data[i] || data[i+1] > max_data[i+1] || data[i+1] < min_data[i+1]);
			GlyphLineItem* temp_line = new GlyphLineItem(0, glyph_scene, this, id, warning);
			temp_line->setLine(draw_points[i].first, draw_points[i].second, draw_points[i+1].first, draw_points[i+1].second);
			if(temp_line->CheckWarning())
			{
			    QPen glyph_pen(QColor(215,20,20,255),4);
				if(data[i] == -1 || data[i+1] == -1)
				{
					glyph_pen.setStyle(Qt::DotLine);
					temp_line->SetMissing();
				}
				temp_line->setPen(glyph_pen);
			}
			else
			{
				QPen glyph_pen(QColor(40,95,150,255),4);
				if(data[i] == -1 || data[i+1] == -1)
				{
					glyph_pen.setStyle(Qt::DotLine);
					temp_line->SetMissing();
				}
				temp_line->setPen(glyph_pen);
			}
			new_set->PushLine(temp_line);			
		}
		if(data[number_of_attributes-1] == -1 || data[0] == -1 || min_data.size() < number_of_attributes || max_data.size() < number_of_attributes)
			warning = false;
		else
			warning = (data[number_of_attributes-1] > max_data[number_of_attributes-1] || data[number_of_attributes-1] < min_data[number_of_attributes-1] || data[0] > max_data[0] || data[0] < min_data[0]);
		GlyphLineItem* temp_line = new GlyphLineItem(0, glyph_scene, this, id, warning);
		temp_line->setLine(draw_points[number_of_attributes-1].first, draw_points[number_of_attributes-1].second, draw_points[0].first, draw_points[0].second);
		if(temp_line->CheckWarning())
		{
			QPen glyph_pen(QColor(215,20,20,255),4);
			if(data[number_of_attributes-1] == -1 || data[0] == -1)
			{
				temp_line->SetMissing();
				glyph_pen.setStyle(Qt::DotLine);
			}
			temp_line->setPen(glyph_pen);
		}
		else
		{
			QPen glyph_pen(QColor(40,95,150,255),4);
			if(data[number_of_attributes-1] == -1 || data[0] == -1)
			{
				glyph_pen.setStyle(Qt::DotLine);
				temp_line->SetMissing();
			}
			temp_line->setPen(glyph_pen);
		}
		new_set->PushLine(temp_line);
		for(int i=0; i<number_of_attributes; ++i)
		{
			if(data[i] != -1)
			{
				if(min_data.size() >= number_of_attributes || max_data.size() >= number_of_attributes)
					warning = (data[i] > max_data[i] || data[i] < min_data[i]);
				else
					warning = false;
				GlyphPointItem* temp_point = new GlyphPointItem(0, glyph_scene, this, id, warning);
				temp_point->setRect(draw_points[i].first-10, draw_points[i].second-10, 20, 20);
				if(warning)
				{
					temp_point->setBrush(QColor(215,20,20,255));
					temp_point->setPen(QPen(QColor(180, 10, 10,255),2));
				}
				else
				{
					temp_point->setBrush(QColor(40,95,150,255));
					temp_point->setPen(QPen(QColor(20, 75, 130,255),2));
				}
				new_set->PushPoint(temp_point);
			}
		}
		point_sets.insert(std::pair<std::string, PointSet*>(id, new_set));
	}
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::SetMinLines(const std::vector<float>& data)
{
	if((int)data.size() == number_of_attributes)
	{
		min_data.clear();
		min_data = data;
		std::vector<std::pair<float, float> > draw_points;
		for(int i=0; i<number_of_attributes; ++i)
		{
			float x_draw = (x_resolution/2) + data[i]*radius*std::cos(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
			float y_draw = (y_resolution/2) + data[i]*radius*std::sin(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
			draw_points.push_back(std::pair<float, float>(x_draw, y_draw));
		}
		for(int i=0; i<number_of_attributes-1; ++i)
		{
			QGraphicsLineItem* temp_line = new QGraphicsLineItem(0, glyph_scene);
			temp_line->setLine(draw_points[i].first, draw_points[i].second, draw_points[i+1].first, draw_points[i+1].second);
			QPen glyph_pen(QColor(110,60,190,200),5);
			glyph_pen.setStyle(Qt::DashLine);
			temp_line->setPen(glyph_pen);
			temp_line->setZValue(1.5);
			min_lines.push_back(temp_line);
		}
		QGraphicsLineItem* temp_line = new QGraphicsLineItem(0, glyph_scene);
		temp_line->setLine(draw_points[number_of_attributes-1].first, draw_points[number_of_attributes-1].second, draw_points[0].first, draw_points[0].second);
		QPen glyph_pen(QColor(110,60,190,200),5);
		glyph_pen.setStyle(Qt::DashLine);
		temp_line->setPen(glyph_pen);
		temp_line->setZValue(1.5);
		min_lines.push_back(temp_line);
	}
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::SetMaxLines(const std::vector<float>& data)
{
	if((int)data.size() == number_of_attributes)
	{
		max_data.clear();
		max_data = data;
		std::vector<std::pair<float, float> > draw_points;
		for(int i=0; i<number_of_attributes; ++i)
		{
			float x_draw = (x_resolution/2) + data[i]*radius*std::cos(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
			float y_draw = (y_resolution/2) + data[i]*radius*std::sin(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
			draw_points.push_back(std::pair<float, float>(x_draw, y_draw));
		}
		for(int i=0; i<number_of_attributes-1; ++i)
		{
			QGraphicsLineItem* temp_line = new QGraphicsLineItem(0, glyph_scene);
			temp_line->setLine(draw_points[i].first, draw_points[i].second, draw_points[i+1].first, draw_points[i+1].second);
			QPen glyph_pen(QColor(110,60,190,200),5);
			glyph_pen.setStyle(Qt::DashLine);
			temp_line->setPen(glyph_pen);
			temp_line->setZValue(1.5);
			max_lines.push_back(temp_line);
		}
		QGraphicsLineItem* temp_line = new QGraphicsLineItem(0, glyph_scene);
		temp_line->setLine(draw_points[number_of_attributes-1].first, draw_points[number_of_attributes-1].second, draw_points[0].first, draw_points[0].second);
		QPen glyph_pen(QColor(110,60,190,200),5);
		glyph_pen.setStyle(Qt::DashLine);
		temp_line->setPen(glyph_pen);
		temp_line->setZValue(1.5);
		max_lines.push_back(temp_line);
	}
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::ActivateSet(const std::string& id)
{
	std::map<std::string, PointSet*>::iterator set_iter = point_sets.find(GetActiveSetID());
	if(set_iter != point_sets.end())
		set_iter->second->Deactivate();
	set_iter = point_sets.find(id);
	if(set_iter != point_sets.end())
		set_iter->second->Activate();
	active_set_id = id;
	set_label->setText(id.c_str());
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::DeactivateSet(const std::string& id)
{
	std::map<std::string, PointSet*>::iterator set_iter = point_sets.find(id);
	if(set_iter != point_sets.end())
		set_iter->second->Deactivate();
	active_set_id = "";
	set_label->setText("");
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

void EventGlyph::ClearMinMaxLines(void)
{
	std::deque<QGraphicsLineItem*>::iterator min_iter = min_lines.begin();
	while(min_iter != min_lines.end())
	{
		delete *min_iter;
		++min_iter;
	}

	std::deque<QGraphicsLineItem*>::iterator max_iter = max_lines.begin();
	while(max_iter != max_lines.end())
	{
		delete *max_iter;
		++max_iter;
	}
	min_lines.clear();
	max_lines.clear();
	min_data.clear();
	max_data.clear();
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::SetAttributeNumber(const int number_of_variables)
{
	std::deque<GlyphAxisItem*>::iterator iter = background_lines.begin();
	while(iter != background_lines.end())
	{
		delete *iter;
		++iter;
	}
	background_lines.clear();
		// * * *
	for(std::deque<QLabel*>::iterator label_iter = name_labels.begin(); label_iter != name_labels.end(); ++label_iter)
	{
		delete (*label_iter);
	}
	name_labels.clear();
	number_of_attributes = number_of_variables;
	DrawGlyphBackground();
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::SetAxisLabels(const std::vector<std::string>& axis_names)
{
	if((int)axis_names.size() == number_of_attributes)
	{
		for(std::deque<QLabel*>::iterator label_iter = name_labels.begin(); label_iter != name_labels.end(); ++label_iter)
		{
			delete (*label_iter);
		}
		name_labels.clear();
		int name_count = 0;
		for(std::deque<GlyphAxisItem*>::iterator iter = background_lines.begin(); iter != background_lines.end(); ++iter)
		{
			(*iter)->SetAxisName(axis_names[name_count]);
			++name_count;
		}
		for(int i=0; i<number_of_attributes; ++i)
		{
			double x_draw = (x_resolution/2) + 1.1*radius*std::cos(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
			double y_draw = (y_resolution/2) + 1.1*radius*std::sin(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
			QLabel* temp_label = new QLabel(glyph_view, 0);
			temp_label->setFont(QFont("Helvetica", 20));
			temp_label->setText(axis_names[i].c_str());
			temp_label->setGeometry(x_draw-(temp_label->sizeHint().width()/2), y_draw-(temp_label->sizeHint().height()/2), temp_label->sizeHint().width(), temp_label->sizeHint().height());
			temp_label->setAlignment(Qt::AlignCenter);
			name_labels.push_back(temp_label);
		}
	}
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

QGraphicsView* EventGlyph::GetGlyphView(void)
{
	return glyph_view;
}

// * * * * * * * * * * * * * * * * *

void EventGlyph::RedrawLabelsOnResize(const int x_res, const int y_res)
{
	set_label->setMinimumWidth(x_res);
	std::deque<QLabel*>::iterator iter = name_labels.begin();
	for(int i=0; i<number_of_attributes; ++i)
	{
		double x_draw = (x_res/2) + 1.1*radius*std::cos(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
		double y_draw = (y_res/2) + 1.1*radius*std::sin(((double)i*2.0*EGlyph::glyph_pi)/(double)number_of_attributes + (3.0*EGlyph::glyph_pi/2.0));
		(*iter)->setGeometry(x_draw-((*iter)->sizeHint().width()/2), y_draw-((*iter)->sizeHint().height()/2), (*iter)->sizeHint().width(), (*iter)->sizeHint().height());
		(*iter)->update();
		++iter;
	}
}

// * * * * * * * * * * * * * * * * *
