#include "RealTimeGlyph.h"
	// * * * * * * * 
// GlyphGraphicsView
	// * * * * * * * 

// * * * * * * * * * * * * * * * * *

RTGlyphGraphicsView::RTGlyphGraphicsView(QGraphicsScene* scene, RealTimeGlyph* glyph)
	: QGraphicsView(scene)
{
	parent_glyph = glyph;
}

// * * * * * * * * * * * * * * * * *

void RTGlyphGraphicsView::resizeEvent(QResizeEvent *event)
{	
	parent_glyph->RedrawLabelsOnResize(this->size().width(), this->size().height());
	QGraphicsView::resizeEvent(event);
}

void RTGlyphGraphicsView::CorrectResize(int x_res, int y_res)
{
    parent_glyph->RedrawLabelsOnResize(x_res, y_res);
    QGraphicsView::resizeEvent(0);
}

// * * * * * * * * * * * * * * * * *
	// * * * * * * * 
// GlyphAxisItem
	// * * * * * * * 

RTGlyphAxisItem::RTGlyphAxisItem(QGraphicsItem* parent, QGraphicsScene* scene, QLabel* glyph_axis_label, const std::string& attribute_name)
	: QGraphicsLineItem(parent, scene)
{
	axis_label = glyph_axis_label;
	axis_name = attribute_name;
	setAcceptHoverEvents(true);
}

// * * * * * * * * * * * * * * * * *

RTGlyphAxisItem::~RTGlyphAxisItem(void)
{}

// * * * * * * * * * * * * * * * * *

void RTGlyphAxisItem::SetAxisName(const std::string& attribute_name)
{
	axis_name = attribute_name;
}

// * * * * * * * * * * * * * * * * *

void RTGlyphAxisItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	axis_label->setText(axis_name.c_str());
}

// * * * * * * * * * * * * * * * * *

void RTGlyphAxisItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	axis_label->setText("");
}

// * * * * * * * * * * * * * * * * *

	// * * * * * * *
// RealTimeGlyph
	// * * * * * * *

// * * * * * * * * * * * * * * * * *

RealTimeGlyph::RealTimeGlyph(const int x_res, const int y_res, const int number_of_variables)
{
		//initializing data members
	glyph_scene = new QGraphicsScene();
	glyph_view = new RTGlyphGraphicsView(glyph_scene, this);
	x_resolution = x_res;
	y_resolution = y_res;
	radius = std::min(x_res, y_res)/2.0 * 0.85;
	number_of_attributes = number_of_variables;
		//setting axis label
	axis_label = new QLabel(glyph_view, 0);
	axis_label->setAlignment(Qt::AlignLeft);
	axis_label->setMargin(7);
	axis_label->setIndent(7);
	axis_label->setFont(QFont("Helvetica", 22));
	axis_label->setMinimumWidth(x_res);
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

RealTimeGlyph::~RealTimeGlyph(void)
{
	std::deque<RTGlyphAxisItem*>::iterator iter = background_lines.begin();
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
		RTGlyphAxisItem* temp_line = new RTGlyphAxisItem(0, glyph_scene, axis_label, "");
		temp_line->setLine((x_resolution/2), (y_resolution/2), x_draw, y_draw);
		temp_line->setPen(QPen(QColor(11,11,11,255),3));
		background_lines.push_back(temp_line);
	}
}

// * * * * * * * * * * * * * * * * *

void RealTimeGlyph::DrawPointSet(const QList<QVariant>& data)
{
	ClearGlyph();
	if((int)data.size() == number_of_attributes)
	{
		std::vector<std::pair<float, float> > draw_points;
		for(int i=0; i<number_of_attributes; ++i)
		{
                       if(data[i].toFloat() != -1)
			{
                                float x_draw = (x_resolution/2) + data[i].toFloat()*radius*std::cos(((double)i*2.0*RTGlyph::glyph_pi)/(double)number_of_attributes + (3.0*RTGlyph::glyph_pi/2.0));
                                float y_draw = (y_resolution/2) + data[i].toFloat()*radius*std::sin(((double)i*2.0*RTGlyph::glyph_pi)/(double)number_of_attributes + (3.0*RTGlyph::glyph_pi/2.0));
				draw_points.push_back(std::pair<float, float>(x_draw, y_draw));
			}
			else
			{
				float x_draw = (x_resolution/2) + 0*radius*std::cos(((double)i*2.0*RTGlyph::glyph_pi)/(double)number_of_attributes + (3.0*RTGlyph::glyph_pi/2.0));
				float y_draw = (y_resolution/2) + 0*radius*std::sin(((double)i*2.0*RTGlyph::glyph_pi)/(double)number_of_attributes + (3.0*RTGlyph::glyph_pi/2.0));
				draw_points.push_back(std::pair<float, float>(x_draw, y_draw));
			}
		}
		for(int i=0; i<number_of_attributes-1; ++i)
		{
			QGraphicsLineItem* temp_line = new QGraphicsLineItem(0, glyph_scene);
			temp_line->setLine(draw_points[i].first, draw_points[i].second, draw_points[i+1].first, draw_points[i+1].second);
                       if(data[i].toFloat() == -1 || data[i+1].toFloat() == -1)
			{
				QPen glyph_pen(QColor(40,95,150,255),4);
				glyph_pen.setStyle(Qt::DotLine);
				temp_line->setPen(glyph_pen);
			}
			else
				temp_line->setPen(QPen(QColor(40,95,150,255),4));
			line_set.push_back(temp_line);
		}
		QGraphicsLineItem* temp_line = new QGraphicsLineItem(0, glyph_scene);
		temp_line->setLine(draw_points[number_of_attributes-1].first, draw_points[number_of_attributes-1].second, draw_points[0].first, draw_points[0].second);
               if(data[number_of_attributes-1].toFloat() == -1 || data[0].toFloat() == -1)
		{
			QPen glyph_pen(QColor(40,95,150,255),4);
			glyph_pen.setStyle(Qt::DotLine);
			temp_line->setPen(glyph_pen);
		}
		else
			temp_line->setPen(QPen(QColor(40,95,150,255),4));
		line_set.push_back(temp_line);
		for(int i=0; i<number_of_attributes; ++i)
		{
                        if(data[i].toFloat() != -1)
			{
				QGraphicsEllipseItem* temp_point = new QGraphicsEllipseItem(0, glyph_scene);
				temp_point->setRect(draw_points[i].first-10, draw_points[i].second-10, 20, 20);
				temp_point->setBrush(QColor(40,95,150,255));
				temp_point->setPen(QPen(QColor(20, 75, 130),2));
				point_set.push_back(temp_point);
			}
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
	std::deque<RTGlyphAxisItem*>::iterator iter = background_lines.begin();
	while(iter != background_lines.end())
	{
		delete *iter;
		++iter;
	}
	background_lines.clear();
	number_of_attributes = number_of_variables;
	name_labels.clear();
	DrawGlyphBackground();
}

// * * * * * * * * * * * * * * * * *

void RealTimeGlyph::SetAxisLabels(const std::vector<std::string>& axis_names)
{
	if((int)axis_names.size() == number_of_attributes)
	{
		name_labels.clear();
		int name_count = 0;
		for(std::deque<RTGlyphAxisItem*>::iterator iter = background_lines.begin(); iter != background_lines.end(); ++iter)
		{
			(*iter)->SetAxisName(axis_names[name_count]);
			++name_count;
		}
		for(int i=0; i<number_of_attributes; ++i)
		{
			double x_draw = (x_resolution/2) + 1.1*radius*std::cos(((double)i*2.0*RTGlyph::glyph_pi)/(double)number_of_attributes + (3.0*RTGlyph::glyph_pi/2.0));
			double y_draw = (y_resolution/2) + 1.1*radius*std::sin(((double)i*2.0*RTGlyph::glyph_pi)/(double)number_of_attributes + (3.0*RTGlyph::glyph_pi/2.0));
			QLabel* temp_label = new QLabel(glyph_view, 0);
			temp_label->setFont(QFont("Helvetica", 18));
			temp_label->setText(axis_names[i].c_str());
			temp_label->setGeometry(x_draw-(temp_label->sizeHint().width()/2), y_draw-(temp_label->sizeHint().height()/2), temp_label->sizeHint().width(), temp_label->sizeHint().height());
			temp_label->setAlignment(Qt::AlignCenter);
			name_labels.push_back(temp_label);
		}
	}
}

// * * * * * * * * * * * * * * * * *

void RealTimeGlyph::ShowGlyph(void)
{
	glyph_view->show();
}

// * * * * * * * * * * * * * * * * *

RTGlyphGraphicsView* RealTimeGlyph::GetGlyphView(void)
{
	return glyph_view;
}

// * * * * * * * * * * * * * * * * *

void RealTimeGlyph::RedrawLabelsOnResize(const int x_res, const int y_res)
{
	std::deque<QLabel*>::iterator iter = name_labels.begin();
	for(int i=0; i<number_of_attributes; ++i)
	{
		double x_draw = (x_res/2) + 1.1*radius*std::cos(((double)i*2.0*RTGlyph::glyph_pi)/(double)number_of_attributes + (3.0*RTGlyph::glyph_pi/2.0));
		double y_draw = (y_res/2) + 1.1*radius*std::sin(((double)i*2.0*RTGlyph::glyph_pi)/(double)number_of_attributes + (3.0*RTGlyph::glyph_pi/2.0));
		(*iter)->setGeometry(x_draw-((*iter)->sizeHint().width()/2), y_draw-((*iter)->sizeHint().height()/2), (*iter)->sizeHint().width(), (*iter)->sizeHint().height());
		(*iter)->update();
		++iter;
	}
}

// * * * * * * * * * * * * * * * * *
