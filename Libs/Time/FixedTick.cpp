#include "Fixedtick.h"

namespace global
{
  static int s_iMilliseconds(1000);

  CFixedTick::CFixedTick(int _iTickTarget)
  {
    m_iTickTarget = (s_iMilliseconds / _iTickTarget);
    lastTickTime = std::chrono::high_resolution_clock::now();
  }
  // ------------------------------------
  void CFixedTick::UpdateTick()
  {
    auto oCurrentTime = std::chrono::high_resolution_clock::now();
    auto oElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(oCurrentTime - lastTickTime).count();

    if (oElapsedTime < m_iTickTarget)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_iTickTarget - oElapsedTime));
    }
    lastTickTime = std::chrono::high_resolution_clock::now();
  }
  // ------------------------------------
  void CFixedTick::SetMaxFrameRate(int _iTickTarget)
  {
    m_iTickTarget = (s_iMilliseconds / _iTickTarget);
  }
  // ------------------------------------
  int CFixedTick::GetMaxFrameRate()
  {
    return s_iMilliseconds / m_iTickTarget;
  }

}