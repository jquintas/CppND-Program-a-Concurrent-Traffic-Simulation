#include <algorithm>
#include <iostream>
#include <chrono>
#include "TrafficObject.h"

// init static variable
int TrafficObject::_idCnt = 0;
// @.@ DONE: L3.2 -> Add a static mutex to the base class TrafficObject (called _mtxCout) and properly instantiate it in the source file. 
// This mutex will be used in the next task to protect standard-out. 
std::mutex TrafficObject::_mtx;

void TrafficObject::setPosition(double x, double y)
{
    _posX = x;
    _posY = y;
}

void TrafficObject::getPosition(double &x, double &y)
{
    x = _posX;
    y = _posY;
}

TrafficObject::TrafficObject()
{
    _type = ObjectType::noObject;
    _id = _idCnt++;
}

TrafficObject::~TrafficObject()
{
    // set up thread barrier before this object is destroyed
    // Task L1.1 : In the base class TrafficObject, set up a thread barrier in its destructor that ensures that all the thread 
    // objects in the member vector _threads are joined.
    std::for_each(threads.begin(), threads.end(), [](std::thread &t) {
        t.join();
    });
}
