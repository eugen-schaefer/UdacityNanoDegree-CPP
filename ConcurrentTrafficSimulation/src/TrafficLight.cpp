#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <random>

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait()
    // to wait for and receive new messages and pull them from the queue using move semantics.
    // The received object should then be returned by the receive function.

    std::unique_lock<std::mutex> lk(_queue_mtx);
    _cond_var.wait(lk, [this] { return !_queue.empty(); });

    T queue_element = std::move(_queue.back());
    _queue.pop_back();

    return queue_element;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex>
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.

    std::lock_guard<std::mutex> lk(_queue_mtx);
    _queue.push_front(std::move(msg));
    _cond_var.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    _type = ObjectType::objectTrafficLight;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop
    // runs and repeatedly calls the receive function on the message queue.
    // Once it receives TrafficLightPhase::green, the method returns.

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        TrafficLightPhase msg = _queue.receive();
        if (msg == TrafficLightPhase::green)
        {
            break;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called.
    // To do this, use the thread queue in the base class.
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles
    // and toggles the current phase of the traffic light between red and green and sends an update method
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds.
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.

    // duration of a single simulation cycle in seconds
    // Generate a random number between 4 and 6 (https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(4.0, 6.0);
    double cycleDuration{dis(gen)};

    std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    // init stop watch
    lastUpdate = std::chrono::system_clock::now();
    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lastUpdate).count();

        if (timeSinceLastUpdate >= cycleDuration)
        {

            // toggle the traffic light
            _currentPhase = (TrafficLightPhase::red == _currentPhase) ? _currentPhase = TrafficLightPhase::green : _currentPhase = TrafficLightPhase::red;

            // reset stop watch for next cycle
            lastUpdate = std::chrono::system_clock::now();

            // assign a new random value to cycleDuration
            cycleDuration = dis(gen);

            // send an update method to the message queue using move semantics
            _queue.send(std::move(_currentPhase));
        }
    }
}
