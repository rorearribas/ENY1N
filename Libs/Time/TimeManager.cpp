#include "TimeManager.h"

namespace chrono
{
  namespace internal_time_manager
  {
    const float s_fMaxFixedDelta = 1.0f / 15.f;
  }
  // ------------------------------------
  CTimeManager::CTimeManager(int32_t _iTargetFramerate) : m_fDeltaTime(-1.0)
    , m_llBaseTime(0)
    , m_llPausedTime(0)
  {
    QueryPerformanceCounter(&m_llPrevTime);
    SetTargetFramerate(_iTargetFramerate);
  }
  // ------------------------------------
  void CTimeManager::BeginFrame()
  {
    // Get current tick count
    QueryPerformanceCounter(&m_llCurrentTickCount);

    // Compute tick
    m_oBeginFrame = std::chrono::steady_clock::now();
    m_fDeltaTime = std::chrono::duration<float>(m_oBeginFrame - m_oEndFrame).count();
    m_oEndFrame = m_oBeginFrame;
  }
  // ------------------------------------
  void CTimeManager::EndFrame()
  {
    // Wait max fps
    while ((m_llCurrentTickCount.QuadPart - m_llPrevTime.QuadPart) < m_llTargetTick)
    {
      QueryPerformanceCounter(&m_llCurrentTickCount);
    }

    // Update
    m_llPrevTime = m_llCurrentTickCount;
  }
  // ------------------------------------
  void CTimeManager::SetTargetFramerate(int32_t _iTargetFramerate)
  {
    __int64 i64CountsPerSec = 0;
    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&i64CountsPerSec));
    m_llTargetTick = (i64CountsPerSec / _iTargetFramerate);

    m_fFixedDelta = static_cast<float>(1.0f / _iTargetFramerate);
    m_iTargetFramerate = _iTargetFramerate;
  }
  // ------------------------------------
  float CTimeManager::GetMaxFixedDelta() const
  {
    return internal_time_manager::s_fMaxFixedDelta;
  }
}
