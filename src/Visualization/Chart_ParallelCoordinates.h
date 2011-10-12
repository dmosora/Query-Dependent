// ============================================================================
// Written by David Sheets
// ============================================================================

#ifndef _CHART_PARALLELCOORDINATES_H_
#define _CHART_PARALLELCOORDINATES_H_

#include <QWidget>

namespace Chart
{
   class ParallelCoordinates : public QWidget
   {
      Q_OBJECT

      public:

         ParallelCoordinates(QWidget *parent = 0);

         virtual ~ParallelCoordinates();


         void draw();


      protected:
         void paintEvent(QPaintEvent* event);
   };
};

#endif // _CHART_PARALLELCOORDINATES_H_
