#pragma once
#include <chrono>
#include <windows.h>
#include "Libs/Utils/Singleton.h"

namespace tick
{
  class CTimeManager : public utils::CSingleton<CTimeManager>
  {
  public:
    CTimeManager(int _iMaxFPS = 60);
    ~CTimeManager() {}

    void BeginFrame();
    void EndFrame();
    bool IsStopped() const;

    float GetFixedDelta() const;
    float GetMaxFixedDelta() const;
    float GetDeltaTime() const;

    void SetMaxFPS(int _iMaxFPS);
    int GetMaxFPS() const { return m_iMaxFPS; }

    void Reset();
    void Start();
    void Stop();

  private:
    int m_iMaxFPS = 60;
    bool m_bStopped = false;

    float m_fFixedDelta = 1.0f / 60.0f;
    float m_fDeltaTime = 0.0f;

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
