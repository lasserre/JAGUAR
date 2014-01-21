#include "jptestcontroller.h"

JPTestController::JPTestController(QObject *parent) :
    QObject(parent)
  , test(new JPTest(this))
  , testRunThread(new QThread(this))
{
    this->test->moveToThread(testRunThread);
}

JPTestController::~JPTestController()
{
    delete this->test;
}
