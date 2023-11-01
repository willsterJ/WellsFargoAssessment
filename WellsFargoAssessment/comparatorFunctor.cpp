#include "comparatorFunctor.h"

void printMyStructVec(std::vector<MyStruct> vec){
    for (const auto& item : vec){
        printf("{%d, %d, %s} ", item.x1, item.get_x2(), item.get_x3().c_str());
    }
    printf("\n");
}