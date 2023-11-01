#ifndef WELLSFARGO_CHRONOUTILITY_H
#define WELLSFARGO_CHRONOUTILITY_H

#include <chrono>

typedef std::chrono::time_point< std::chrono::system_clock> timeType;
static timeType global_start;

// Utility class that handles std::chrono time calculations.
template<typename chronoType>
class chronoUtility {
public:
    inline static void beginTimer() {
        global_start = std::chrono::system_clock::now();
    }
    inline static long long getElapsedTime(timeType start, timeType end){
        return std::chrono::duration_cast<chronoType>(end - start).count();
    }

    inline static long long getElapsedTime(timeType end){
        return std::chrono::duration_cast<chronoType>(end - global_start).count();
    }
};

#endif //WELLSFARGO_CHRONOUTILITY_H
