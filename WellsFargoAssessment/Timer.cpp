#include "Timer.h"
using namespace std::chrono;

/* **************************** Event **************************** */

Event::Event(int ID, timeType creationTime, timeType executionTime)
    : m_ID(ID)
    , m_creationTime(creationTime)
    , m_executionTime(executionTime) {
}

/* **************************** Event **************************** */

/* ************************** EventQueue ************************** */

void EventQueue::add(const std::shared_ptr<Event>& event) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push_back(event);
}

std::shared_ptr<Event> EventQueue::peek() {
    // take a soft peek to see if queue is populated
    if (m_queue.empty()) {
        // acquire lock and check again in case a new Event was being added
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty())
            return nullptr;  // bummer, queue is actually empty
        return m_queue.front();
    }
    return m_queue.front();
}

std::shared_ptr<Event> EventQueue::pop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::shared_ptr<Event> event;
    try {
        if (m_queue.empty())
            return nullptr;
        event = m_queue.front();
        m_queue.pop_front();
    } catch(std::exception const &except){
        printf("Error: %s", except.what());
    }
    return event;
}

void EventQueue::sort() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.empty())
        return;
    std::sort(m_queue.begin(), m_queue.end(), [](const std::shared_ptr<Event>& e1, const std::shared_ptr<Event>& e2){
        auto duration = chronoUtility<milliseconds>::getElapsedTime(e1->m_executionTime, e2->m_executionTime);
        return duration > 0; });
}

/* ************************** EventQueue ************************** */

/* **************************** Timer **************************** */

Timer::Timer(const std::shared_ptr<EventQueue>& eventQueue)
    : m_eventQueue(eventQueue)
    , m_runThread(false) {
}

void Timer::start() {
    m_runThread = true;
    // spawn async thread to execute event generation
    m_eventProducerFuture = std::async(std::launch::async, [&](){
        produceEvents();
    });
}

void Timer::stop() {
    m_runThread = false;
    // recall thread by fetching future object - no return value since we're dealing with void
    m_eventProducerFuture.get();
}

void Timer::produceEvents() {
    int eventID(1);
    std::srand(0); // random seed for testing
    chronoUtility<milliseconds>::beginTimer();

    while (m_runThread){
        timeType now = std::chrono::system_clock::now();
        int randomVal = std::rand() % 501; // generate random number from range [0, 500]

        // create new event which has execution time = now + randomVal
        std::shared_ptr<Event> event(new Event(eventID++, now, now + std::chrono::milliseconds(randomVal)));
        m_eventQueue->add(event);

        // print event generation log
        auto creationTime_ms = chronoUtility<milliseconds>::getElapsedTime(event->m_creationTime);
        auto executionTime_ms = chronoUtility<milliseconds>::getElapsedTime(event->m_executionTime);
        printf("EVENT GENERATED : EventID=%d, CurrentTime=%lldms, ExecutionTime=%lldms\n",
               event->m_ID, creationTime_ms, executionTime_ms);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

/* **************************** Timer **************************** */

/* **************************** Worker **************************** */

Worker::Worker(const std::shared_ptr<EventQueue>& eventQueue)
    : m_eventQueue(eventQueue)
    , m_runThread(false) {
}

void Worker::start() {
    m_runThread = true;
    m_eventConsumerFuture = std::async(std::launch::async, [&](){ consumeEvents(); });
}

void Worker::stop() {
    m_runThread = false;
    m_eventConsumerFuture.get();
}

void Worker::consumeEvents() {
    // dummy job counter
    int jobCount(0);

    while(m_runThread) {
        // sort EventQueue by execution time
        m_eventQueue->sort();
        // peek earliest execution time event
        std::shared_ptr<Event> event = m_eventQueue->peek();
        if (event == nullptr)
            continue;

        // execute Event if currentTime >= Event.m_executionTime
        timeType now = std::chrono::system_clock::now();
        auto duration = chronoUtility<milliseconds>::getElapsedTime(event->m_executionTime, now);
        if (duration >= 0) {
            executeEvent(event);
            m_eventQueue->pop();
        }
        // assign jobs to worker in the meantime
        else {
            Job job;
            job.m_ID = jobCount++;
            job.m_function = [](){ /* job implementation, can be anything? */ };
            addJob(job);

            // sleep this thread to avoid generating a gazillion jobs.
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}

void Worker::executeEvent(const std::shared_ptr<Event>& event) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::future<void>> asyncThreads;
    asyncThreads.reserve(m_jobsVec.size());

    // asynchronous execute jobs
    for (const Job& job : m_jobsVec){
        asyncThreads.emplace_back(std::async(std::launch::async, [&](){
            runJob(event, job);
        }));
    }
    // collect thread results
    for (auto& thread : asyncThreads){
        thread.get();
    }
    m_jobsVec.clear();
}

void Worker::addJob(const Job &job) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_jobsVec.push_back(job);
}

void Worker::removeJob(int jobID) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_jobsVec.erase(std::remove_if(m_jobsVec.begin(), m_jobsVec.end(), [jobID](const Job& job) {
        return jobID == job.m_ID;
    }), m_jobsVec.end());
}

void Worker::runJob(const std::shared_ptr<Event>& event, const Job &job) {
    timeType now = std::chrono::system_clock::now();
    auto currentTime_ms = chronoUtility<milliseconds>::getElapsedTime(now);
    auto delayTime_ms = chronoUtility<milliseconds>::getElapsedTime(event->m_executionTime, now);
    printf("WORKER RUN JOB : JobID=%d, EventID=%d, CurrentTime=%lldms, TimeLag=%lldms\n",
           job.m_ID, event->m_ID, currentTime_ms, delayTime_ms);

    try {
        // run the job function
        job.m_function();
    }
    catch (...) {
        // catch exceptions
    }
}

/* **************************** Worker **************************** */
