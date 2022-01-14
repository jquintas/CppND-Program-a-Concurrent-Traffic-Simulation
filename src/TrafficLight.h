#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <thread>
#include <future>
#include <random>
#include <chrono>
#include "TrafficObject.h"


using my_rd_device = std::random_device;
using my_distribution = std::uniform_int_distribution<int>;


// forward declarations to avoid include cycle
class Vehicle;

// @.@ DONE FP.3 Define a class „MessageQueue“ which has the public methods send and receive. 
// 
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Also, there should be an std::condition_variable as well as an std::mutex as private members. 
// Used the code from last lesson

template <class T>
class MessageQueue
{
public:
    // +.+ public method receive
    T receive();

    // +.+  Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type.
    void send(T &&msg);
    

private:
    // +.+ there should be an std::mutex private
    std::mutex _mutex;

    // +.+ there should be an std::condition_variable private
    std::condition_variable _cond;

    // +.+ hould define an std::dequeue called _queue
    std::deque<T> _queue;
};

// @.@ DONE: FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject. 
// The class shall have the public methods „void waitForGreen()“ and „void simulate()“ 
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that 
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“. 
// Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value. 

enum TrafficLightPhase {red, green};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();


    TrafficLightPhase _currentPhase;

    std::condition_variable _condition;
    std::mutex _mutex;
    
    // ?? Why? Could be a shared pointer 
    // @.@ DONEFP.4b -> create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.
    MessageQueue<TrafficLightPhase> _messages_queue;
};

#endif