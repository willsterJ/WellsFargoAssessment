#include <string>
#include <vector>
#include "comparatorFunctor.h"
#include "Timer.h"

void Problem1(int runTime){
    std::shared_ptr<EventQueue> queue(new EventQueue());
    Timer timer(queue);
    Worker worker(queue);
    timer.start();
    worker.start();

    // dictates how long this function will run
    std::this_thread::sleep_for(std::chrono::seconds(runTime));

    worker.stop();
    timer.stop();
}

void Problem2(){
    std::vector<MyStruct> v {MyStruct(5,1,"a"), MyStruct(1,1,"c"), MyStruct(2,2,"b")};
    printMyStructVec(v);
    std::sort(v.begin(), v.end(), comparer_builder<MyStruct>().by(&MyStruct::x1).by(&MyStruct::get_x2));
    printMyStructVec(v);
    std::sort(v.begin(), v.end(), comparer_builder<MyStruct>().by(&MyStruct::get_x3).by(&MyStruct::get_x2));
    printMyStructVec(v);
}

int main() {
    Problem1(5);
    Problem2();
}
