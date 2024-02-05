// Taken from https://forums.unrealengine.com/t/multithreading-and-performance-in-unreal/1216417
// created by Ayliroe - https://dev.epicgames.com/community/profile/Br5XL/Ayliroe

#pragma once
#include <chrono>
using namespace std::chrono;

class Timer {
  time_point<high_resolution_clock> t1;

public:
  void Start() { t1 = high_resolution_clock::now(); }

  float Stop() {
    auto t2 = high_resolution_clock::now();
    return duration_cast<microseconds>(t2 - t1).count();
}
};