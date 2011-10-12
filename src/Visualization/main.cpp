#include "Visualization.h"
#include <QtGui/QApplication>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    //CThread1 oThread1;
    //MyObject myObject;
    //QObject::connect
    //   ( & oThread1, SIGNAL( MySignal(int) )
    //   , & myObject, SLOT( MySlot(int) ) );
    //oThread1.start();



	Visualization w;
	w.show();
	return a.exec();
}
