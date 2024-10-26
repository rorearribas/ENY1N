#pragma once
#include <chrono>
#include <thread>

namespace global
{
  class CFixedTick 
  {
  public:
    CFixedTick(int _iTickTarget);

    void UpdateTick();
    void SetMaxFrameRate(int _iTickTarget);
    int GetMaxFrameRate();

  private:
    int m_iTickTarget;
    std::chrono::high_resolution_clock::time_point lastTickTime;
  };

}


