#pragma once
#include <string>

namespace lights
{
  class CLight
  {
  public:
    enum ELightType { DIRECTIONAL, SPOT, POINT, COUNT };
  
  public:
    CLight(const std::string& _sName) : m_sLightName(_sName) {}
    ~CLight() {}
  
    virtual void Update() {};

  private:
    std::string m_sLightName;
  };
}

