#include <iostream>
#include <chrono>
#include <thread>

class Timer {
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint lastUpdateTime;
    double elapsedTime; // in seconds
    double duration;     // in seconds

public:
    Timer(double durationInSeconds) : elapsedTime(0.0), duration(durationInSeconds) {
        Start();
    }

    void Start() {
        lastUpdateTime = Clock::now();
        elapsedTime = 0.0;
    }

    bool IsExpired() const {
        return elapsedTime >= duration;
    }

    void Update() {
        TimePoint now = Clock::now();
        std::chrono::duration<double> delta = now - lastUpdateTime;
        elapsedTime += delta.count();
        lastUpdateTime = now;
    }

    double ElapsedTime() const {
        return elapsedTime;
    }
};