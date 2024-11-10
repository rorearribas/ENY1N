#pragma once

namespace camera
{
  struct SCameraSettings
  {
    float m_fFov = 90.f;
    float m_fNear = 100.0f;
    float m_fFar = 100000.0f;
  };

  class CCamera {
  public:
    CCamera() {}
    ~CCamera() {}

    void Update();

    void SetConfig(const SCameraSettings& _oCameraSettings) { m_oCameraSettings = _oCameraSettings; }
    const SCameraSettings& GetCameraSettings() const { return m_oCameraSettings; }

  private:
    SCameraSettings m_oCameraSettings = {};
  };
}


