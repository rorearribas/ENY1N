#pragma once
#include "Libs/Maths/Vector3.h"

namespace game { class CEntity; }

// Component definition
namespace game
{
  class CComponent
  {
  public:
    using Super = CComponent;

    CComponent(CEntity* _pOwner) : m_pOwner(_pOwner) {}
    virtual ~CComponent() { m_pOwner = nullptr; }

    virtual void UpdateComponent(float /*_fDeltaTime*/) {}
    virtual void DrawDebug() {}

    virtual void OnPositionChanged(const maths::CVector3& /*_v3Pos*/) {}
    virtual void OnRotationChanged(const maths::CVector3& /*_v3Rot*/) {}
    virtual void OnScaleChanged(const maths::CVector3& /*_v3Scale*/) {}

  protected:
    CEntity* m_pOwner = nullptr;
  };
}

