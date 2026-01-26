#pragma once
#include <chrono>
#include <windows.h>
#include "Libs/Utils/Singleton.h"

namespace chrono
{
  class CTimeManager : public utils::CSingleton<CTimeManager>
  {
  public:
    CTimeManager(int32_t _iTargetFramerate = 60);
    ~CTimeManager() {}

    void BeginFrame();
    void EndFrame();

    inline int32_t GetTargetFramerate() const { return m_iTargetFramerate; }
    void SetTargetFramerate(int32_t _iFramerate);

    inline float GetDeltaTime() const { return m_fDeltaTime; }
    inline float GetFixedDelta() const { return m_fFixedDelta; }
    float GetMaxFixedDelta() const;

  private:
    int32_t m_iTargetFramerate = 60;
    float m_fFixedDelta = 1.0f / 60.0f;
    float m_fDeltaTime = 0.0f;

  private:
    __int64 m_llBaseTime;
    __int64 m_llPausedTime;
    __int64 m_llStopTime;
    __int64 m_llTargetTick;

    LARGE_INTEGER m_llPrevTime;
    LARGE_INTEGER m_llCurrentTickCount;

    std::chrono::steady_clock::time_point m_oBeginFrame;
    std::chrono::steady_clock::time_point m_oEndFrame;
  };
}
