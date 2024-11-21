#include "TickRate.h"
#include <iostream>

namespace tick
{
  CTickRate::CTickRate(int _iMaxFPS) : m_fDeltaTime(-1.0), m_llBaseTime(0), m_llPausedTime(0), m_bStopped(false)
  {
    QueryPerformanceCounter(&m_llPrevTime);
    QueryPerformanceCounter(&m_llTicksPerFrame);
    SetMaxFPS(_iMaxFPS);
  }
  // ------------------------------------
  void CTickRate::SetMaxFPS(int _iMaxFPS)
  {
    __int64 lCountsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&lCountsPerSec);
    m_iMaxFPS = _iMaxFPS;
    m_llTargetTick = lCountsPerSec / m_iMaxFPS;
  }
  // ------------------------------------
  void CTickRate::Reset()
  {
    __int64 lCurrentTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&lCurrentTime);

    m_llBaseTime = lCurrentTime;
    m_llStopTime = 0;
    m_bStopped = false;
  }
  // ------------------------------------
  float CTickRate::DeltaTime() const
  {
    return static_cast<float>(m_fDeltaTime);
  }
  // ------------------------------------
  bool CTickRate::IsStopped() const
  {
    return m_bStopped;
  }
  // ------------------------------------
  void CTickRate::Start()
  {
    if (m_bStopped)
    {
      __int64 lStartTime;
      QueryPerformanceCounter((LARGE_INTEGER*)&lStartTime);

      // Ajustar tiempo pausado
      m_llPausedTime += (lStartTime - m_llStopTime);

      //m_llPrevTime = lStartTime;
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
  void CTickRate::BeginFrame()
  {
    if (m_bStopped)
    {
      m_fDeltaTime = 0.0;
      return;
    }

    // Get current tick count
    QueryPerformanceCounter(&m_llCurrentTickCount);
    uint64_t elapsedTicks = m_llCurrentTickCount.QuadPart - m_llPrevTime.QuadPart;

    // Get current frequency
    LARGE_INTEGER lFrequency;
    QueryPerformanceFrequency(&lFrequency);
    double elapsedTimeSeconds = static_cast<double>(elapsedTicks) / lFrequency.QuadPart;

    m_oBeginFrame = std::chrono::steady_clock::now();
    m_fDeltaTime = std::chrono::duration<float>(m_oBeginFrame - m_oEndFrame).count();
    m_oEndFrame = m_oBeginFrame;
  }
  // ------------------------------------
  void CTickRate::EndFrame()
  {
    if (m_bStopped)
      return;

    // Wait max FPS
    while ((m_llCurrentTickCount.QuadPart - m_llPrevTime.QuadPart) < m_llTargetTick)
    {
      QueryPerformanceCounter(&m_llCurrentTickCount);
    }

    // Update
    m_llPrevTime = m_llCurrentTickCount;
  }
}
