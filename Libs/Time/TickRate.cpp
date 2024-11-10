#include "TickRate.h"
#include <windows.h>

namespace tick
{
  CTickRate::CTickRate(__int64 _iMaxFPS) : m_dSecondsPerCount(0.0), m_dDeltaTime(-1.0), m_llBaseTime(0),
    m_llPausedTime(0), m_llPrevTime(0), m_llCurrTime(0), m_bStopped(false)
  {
    __int64 lCountsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&lCountsPerSec);
    m_dSecondsPerCount = 1.0 / (double)lCountsPerSec;
    SetMaxFPS(_iMaxFPS);
  }
  // ------------------------------------
  void CTickRate::SetMaxFPS(__int64 _iMaxFPS)
  {
    __int64 lCountsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&lCountsPerSec);
    m_llTicksPerFrame = lCountsPerSec / _iMaxFPS;
  }
  // ------------------------------------
  void CTickRate::Reset()
  {
    __int64 lCurrentTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&lCurrentTime);

    m_llBaseTime = lCurrentTime;
    m_llPrevTime = lCurrentTime;
    m_llStopTime = 0;
    m_bStopped = false;
  }
  // ------------------------------------
  float CTickRate::DeltaTime() const
  {
    return (float)m_dDeltaTime;
  }
  // ------------------------------------
  bool CTickRate::IsStopped() const
  {
    return m_bStopped;
  }
  // ------------------------------------
  void CTickRate::Start()
  {
    __int64 lStartTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&lStartTime);

    if (m_bStopped)
    {
      m_llPausedTime += (lStartTime - m_llStopTime);

      m_llPrevTime = lStartTime;
      m_llStopTime = 0;
      m_bStopped = false;
    }
  }
  // ------------------------------------
  void CTickRate::Stop()
  {
    if (!m_bStopped)
    {
      __int64 lCurrentTime;
      QueryPerformanceCounter((LARGE_INTEGER*)&lCurrentTime);

      m_llStopTime = lCurrentTime;
      m_bStopped = true;
    }
  }
  // ------------------------------------
  void CTickRate::UpdateTick()
  {
    if (m_bStopped)
    {
      m_dDeltaTime = 0.0;
      return;
    }

    LONGLONG lCurrentTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&lCurrentTime);
    m_llCurrTime = lCurrentTime;

    // Get delta time
    m_dDeltaTime = (m_llCurrTime - m_llPrevTime) * m_dSecondsPerCount;

    // Wait
    while ((m_llCurrTime - m_llPrevTime) < m_llTicksPerFrame)
    {
      QueryPerformanceCounter((LARGE_INTEGER*)&lCurrentTime);
      m_llCurrTime = lCurrentTime;
    }

    // Update previous time
    m_llPrevTime = m_llCurrTime;

    if (m_dDeltaTime < 0.0)
    {
      m_dDeltaTime = 0.0;
    }
  }
  // ------------------------------------
  float CTickRate::TotalTime() const
  {
    if (m_bStopped)
    {
      return (float)(((m_llStopTime - m_llPausedTime) - m_llBaseTime) * m_dSecondsPerCount);
    }
    else
    {
      return (float)(((m_llCurrTime - m_llPausedTime) - m_llBaseTime) * m_dSecondsPerCount);
    }
  }
}
