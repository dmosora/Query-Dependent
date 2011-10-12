#include <QtCore/QCoreApplication>
#include <QtTest/QtTest>

#include "Test.h"

int main(int argc, char *argv[])
{
	// Creating an application may be needed for more in depth testing.
	// While it doesn't seem specifically required to run the tests, 
	// simply creating this object handles clean up of the QObjects and
	// the memory they use(i.e. it gets rid of memory leaks).
	QCoreApplication a(argc, argv);


	// Method returns zero if all test cases succeed. Otherwise, non-zero
	int retVal = 0;

	// Create all of the utilities tests.
	Test test(0);

	// Execute and track the return value.
	retVal += QTest::qExec(&test, argc, argv);

	// Return the results 
	return retVal;
}

