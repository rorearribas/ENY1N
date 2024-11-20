#pragma once
#include <chrono>
#include <windows.h>

namespace tick
{
  class CTickRate
  {
  public:
    CTickRate(int _iMaxFPS = 60);
    ~CTickRate() {}

    void BeginFrame();
    void EndFrame();

    float TotalTime() const;
    float DeltaTime() const;
    bool IsStopped() const;

    void SetMaxFPS(int maxFPS);
    int GetMaxFPS() { return m_iMaxFPS; }

    void Reset();
    void Start();
    void Stop();

  private:
    int m_iMaxFPS = 60;
    float m_fDeltaTime;
    bool m_bStopped;

    __int64 m_llBaseTime;
    __int64 m_llPausedTime;
    __int64 m_llStopTime;
    __int64 m_llTargetTick;

    LARGE_INTEGER m_llTicksPerFrame;
    LARGE_INTEGER m_llPrevTime;
    LARGE_INTEGER m_llCurrentTickCount;

    std::chrono::steady_clock::time_point m_oBeginFrame;
    std::chrono::steady_clock::time_point m_oEndFrame;

  };
}
