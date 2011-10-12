
/*!
This file is part of the application, and is 
 
  Copyright © 2011 David Sheets ALL RIGHTS RESERVED.
   
  The application is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  The application is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TEST_H
#define TEST_H

#include <QtCore/QObject>

class Test : public QObject
{
	Q_OBJECT

public:
	Test(QObject* parent = 0);
	~Test();

private:
	//! Method called by the testing framework to initialize all tests in this object.
	void init();

	//! Method called by the testing framework prior to calling each test function.
	void initTestCase();

	//! Method called by the testing framework after each test function is executed.
	void cleanup();
	
	//! Method called by the testing framework to cleanup the test.
	void cleanupTestCase();

};

#endif // TEST_H
