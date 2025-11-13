#pragma once
#include "Libs/Math/Vector3.h"
#include "Engine/Collisions/Collider.h"

namespace collision { class CCollider; }
namespace game { class CEntity; }

// Component definition
namespace game
{
  class CComponent
  {
  public:
    using Super = CComponent;

    explicit CComponent(CEntity* _pOwner) { m_pOwner = _pOwner; }
    virtual ~CComponent() { m_pOwner = nullptr; }

    virtual void Update(float) {}
    CEntity* GetOwner() const { return m_pOwner; }

    virtual void OnPositionChanged(const math::CVector3&) {}
    virtual void OnRotationChanged(const math::CVector3&) {}
    virtual void OnScaleChanged(const math::CVector3&) {}

    virtual void OnCollisionEnter(const collision::THitEvent&) {}
    virtual void OnCollisionStay(const collision::THitEvent&) {}
    virtual void OnCollisionExit(const collision::THitEvent&) {}

    virtual void DrawDebug() {}

  protected:
    CEntity* m_pOwner = nullptr;
  };
}


