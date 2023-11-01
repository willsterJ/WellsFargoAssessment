#ifndef WELLSFARGO_TIMER_H
#define WELLSFARGO_TIMER_H

#include <vector>
#include <future>
#include <deque>
#include "chronoUtility.h"

// Job data struct containing to-be-executed function
struct Job {
    int m_ID;
    std::function<void()> m_function;
};

// Event data struct
struct Event{
    int m_ID;
    timeType m_creationTime;
    timeType m_executionTime;
    Event(int ID, timeType creationTime, timeType executionTime);
};

// class that encapsulates producer/consumer queue container. To be shared and accessed by Timer and Worker classes.
class EventQueue{
private:
    std::deque<std::shared_ptr<Event>> m_queue;
    std::mutex m_mutex;
public:
    // add event to EventQueue
    void add(std::shared_ptr<Event> event);
    // peeks at front of EventQueue
    std::shared_ptr<Event> peek();
    // pops and returns Event from EventQueue following FIFO
    std::shared_ptr<Event> pop();
    // sorts EventQueue by Event execution time
    void sort();
};

// Timer class responsible for generating Events
class Timer {
private:
    std::shared_ptr<EventQueue> m_eventQueue;
    std::future<void> m_eventProducerFuture;
    bool m_runThread;
public:
    Timer(std::shared_ptr<EventQueue>& eventQueue);
    // starts Timer process
    void start();
    // stops Timer process
    void stop();
private:
    // event generation function that spawns a new event every 100ms, then randomly assigns execution time (T0 + randTime)
    void produceEvents();
};

class Worker {
private:
    std::shared_ptr<EventQueue> m_eventQueue;
    std::future<void> m_eventConsumerFuture;
    bool m_runThread;
    std::vector<Job> m_jobsVec;
    std::mutex m_mutex;
public:
    Worker(std::shared_ptr<EventQueue>& eventQueue);
    // start worker
    void start();
    // stops worker
    void stop();
private:
    // listens from EventQueue for events. Will only consume Event if Event.m_executionTime is greater than current time
    void consumeEvents();
    // handle event trigger for running jobs
    void executeEvent(const std::shared_ptr<Event>& event);
    // add job to worker's queue
    void addJob(const Job& job);
    // remove specific job from queue
    void removeJob(int id);
    // run job triggered by event
    void runJob(const std::shared_ptr<Event>& event, const Job& job);
};


#endif //WELLSFARGO_TIMER_H
