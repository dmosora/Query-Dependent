#include <QtGui/QApplication>
#include <QtGui/QGraphicsScene>
#include "EventGlyph.h"
#include "RealTimeGlyph.h"
#include <string>
#include <vector>

void split_str(const std::string& str, std::vector<float>& vec, const char& split_char)
{
	std::string temp_str;
	int i = 0;
	int j = 0;
	while(i < str.length())
	{
		j=i;
		while( (j<str.length()) && str[j] != '\n' && str[j] != split_char)
		{
			++j;
		}
		temp_str = str.substr(i, j-i);
		if(temp_str.length() > 0)
			vec.push_back(atof(temp_str.c_str()));
		if(i==j)
			++i;
		else
			i=j;
	}
}

void split_str2(const std::string& str, std::vector<std::string>& vec, const char& split_char)
{
	std::string temp_str;
	int i = 0;
	int j = 0;
	while(i < str.length())
	{
		j=i;
		while( (j<str.length()) && str[j] != '\n' && str[j] != split_char)
		{
			++j;
		}
		temp_str = str.substr(i, j-i);
		if(temp_str.length() > 0)
			vec.push_back(temp_str);
		if(i==j)
			++i;
		else
			i=j;
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	EventGlyph event_glyph(900, 750, 171);
	event_glyph.SetAttributeNumber(8);

	RealTimeGlyph realtime_glyph(600,600,171);
	realtime_glyph.SetAttributeNumber(8);

	std::string set1 = ".9 .5 .9 .5 .9 .5 .9 .5";
	std::string set2 = ".5 .75 .6 -1 .5 .75 -1 .9";
	std::string set3 = ".4 .3 .1 -1 .1 .3 .1 .3";
	std::string minset = ".2 .2 .2 .2 .2 .2 .2 .2";
	std::string maxset = ".8 .8 .8 .8 .8 .8 .8 .8";
	std::string axisnames = "axis1 axis2 axis3 axis4 axis5 axis6 axis7 axis8";

	std::vector<float> data1;
	std::vector<float> data2;
	std::vector<float> data3;
	std::vector<float> mindata;
	std::vector<float> maxdata;
	std::vector<std::string> axisname_vec;

	split_str(set1, data1, ' ');
	split_str(set2, data2, ' ');
	split_str(set3, data3, ' ');
	split_str(minset, mindata, ' ');
	split_str(maxset, maxdata, ' ');
	split_str2(axisnames, axisname_vec, ' ');

	event_glyph.SetAxisLabels(axisname_vec);
	event_glyph.SetAxisLabels(axisname_vec);
	event_glyph.SetMinLines(mindata);
	event_glyph.SetMaxLines(maxdata);
	event_glyph.ClearMinMaxLines();
	event_glyph.SetMinLines(mindata);
	event_glyph.SetMaxLines(maxdata);
	event_glyph.CreatePointSet(data1, "set1");
	event_glyph.CreatePointSet(data2, "set2");
	event_glyph.CreatePointSet(data3, "set3");

	realtime_glyph.SetAxisLabels(axisname_vec);
	realtime_glyph.DrawPointSet(data1);
	realtime_glyph.DrawPointSet(data3);
	realtime_glyph.DrawPointSet(data2);

	event_glyph.ShowGlyph();
	realtime_glyph.ShowGlyph();

	return a.exec();
}
