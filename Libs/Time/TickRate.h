#pragma once

namespace tick
{
  class CTickRate
  {
  public:
    CTickRate(__int64 _iMaxFPS = 60);
    ~CTickRate() {}

    void UpdateTick();
    float TotalTime() const;
    float DeltaTime() const;
    bool IsStopped() const;

    void SetMaxFPS(__int64 maxFPS);
    void Reset();
    void Start();
    void Stop();

  private:
    double m_dSecondsPerCount;
    double m_dDeltaTime;
    bool m_bStopped;

    __int64 m_llBaseTime;
    __int64 m_llPausedTime;
    __int64 m_llStopTime;
    __int64 m_llPrevTime;
    __int64 m_llCurrTime;
    __int64 m_llTicksPerFrame;
  };
}
