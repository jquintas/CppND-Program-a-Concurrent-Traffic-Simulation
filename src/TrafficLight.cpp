#include <iostream>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // @.@ DONE: FP.5a -> The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    
    // perform queue modification under the lock
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_queue.empty(); }); // pass unique lock to condition variable

    // remove last vector element from queue
    T msg = std::move(_queue.back());
    _queue.pop_back();

    return msg; // will not be copied due to return value optimization (RVO) in C++
}


template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // @.@ DONE: FP.4a -> The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.

    // perform vector modification under the lock
    std::lock_guard<std::mutex> uLock(_mutex);

    // add vector to queue
    //std::cout << "   Message " << msg << " has been sent to the queue" << std::endl;
    _queue.emplace_back(std::move(msg));
    _cond.notify_one(); // notify client after pushing new Vehicle into vector
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // @.@ DONE: FP.5b -> add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    
    while (true){
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto ligth_phase = _messages_queue.receive();
        // +.+ Once it receives TrafficLightPhase::green, the method returns.
        if (ligth_phase == TrafficLightPhase::green)
            return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // @.@ DONE: FP.2b -> Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // @.@ DONE: FP.2a -> Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 

    // Generate random cycle duration between four and six seconds -> from https://stackoverflow.com/a/59019862 and
    // https://stackoverflow.com/a/19666713
    my_rd_device rd;
    std::mt19937 mt(rd());
    my_distribution distribution_four_to_six {4000, 6000}; // miliseconds
    int random_value_toggle_duration = distribution_four_to_six(mt);
    std::cout << "random:" << random_value_toggle_duration << '\n';

    // traffic light toggles time
    auto time_light_toggled = std::chrono::system_clock::now();

    while(true){

        // +.+ while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // +.+ time duration since last traffic light toggle
        auto duration_since_toggle = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time_light_toggled).count();
        //int duration_since_toggle = since_toggle.count();
        
        //std::cout << "dtoggle: " << duration_since_toggle<< '\n';
        // +.+ check if duration is equal or bigger to random_value_toggle_duration
        //std::cout << "random: " << random_value_toggle_duration << '\n';
        if (duration_since_toggle >= random_value_toggle_duration){
            // +.+ ternary operator to toggle phase 
            _currentPhase = ((_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red);
            std::cout << "here" << '\n';
            // +.+ update method to the message queue using move semantics.
            auto future_toggle_light = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, &_messages_queue, std::move(_currentPhase));

            // +.+ wait until the future is ready
            future_toggle_light.wait();

            // +.+ Time traffic light toggled
            time_light_toggled = std::chrono::system_clock::now();

            // +.+ Update random value 
            random_value_toggle_duration = distribution_four_to_six(mt);
            std::cout << "random: " << random_value_toggle_duration << '\n';
        }

    }

}

