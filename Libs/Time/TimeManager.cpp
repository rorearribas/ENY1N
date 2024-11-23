#include "TimeManager.h"

namespace tick
{
  namespace internal_tickratemanager
  {
    const float s_fMaxFixedDelta = 1.0f / 10.f;
  }
  // ------------------------------------
  CTimeManager::CTimeManager(int _iMaxFPS) : m_fDeltaTime(-1.0), m_llBaseTime(0), m_llPausedTime(0), m_bStopped(false)
  {
    QueryPerformanceCounter(&m_llPrevTime);
    QueryPerformanceCounter(&m_llTicksPerFrame);
    SetMaxFPS(_iMaxFPS);
  }
  // ------------------------------------
  void CTimeManager::SetMaxFPS(int _iMaxFPS)
  {
    __int64 lCountsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&lCountsPerSec);
    m_llTargetTick = lCountsPerSec / _iMaxFPS;

    m_fFixedDelta = static_cast<float>(1.0f / _iMaxFPS);
    m_iMaxFPS = _iMaxFPS;
  }
  // ------------------------------------
  void CTimeManager::Reset()
  {
    __int64 lCurrentTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&lCurrentTime);

    m_llBaseTime = lCurrentTime;
    m_llStopTime = 0;
    m_bStopped = false;
  }
  // ------------------------------------
  float CTimeManager::GetDeltaTime() const
  {
    return m_fDeltaTime;
  }
  float CTimeManager::GetFixedDelta() const
  {
    return m_fFixedDelta;
  }
  // ------------------------------------
  float CTimeManager::GetMaxFixedDelta() const
  {
    return internal_tickratemanager::s_fMaxFixedDelta;
  }
  // ------------------------------------
  bool CTimeManager::IsStopped() const
  {
    return m_bStopped;
  }
  // ------------------------------------
  void CTimeManager::Start()
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
  void CTimeManager::Stop()
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
  void CTimeManager::BeginFrame()
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
  void CTimeManager::EndFrame()
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
