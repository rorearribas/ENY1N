#pragma once
#include "Libs/Maths/Vector3.h"

namespace collisions { class CCollider; }
namespace game { class CEntity; }

// Component definition
namespace game
{
  class CComponent
  {
  public:
    using Super = CComponent;

    CComponent() {}
    virtual ~CComponent() { m_pOwner = nullptr; }

    virtual void Update(float) {}
    virtual void DrawDebug() {}
    
    void SetOwner(CEntity* _pOwner) { m_pOwner = _pOwner; }
    CEntity* GetOwner() const { return m_pOwner; }

    virtual void OnPositionChanged(const maths::CVector3&) {}
    virtual void OnRotationChanged(const maths::CVector3&) {}
    virtual void OnScaleChanged(const maths::CVector3&) {}
  
    virtual void OnCollisionEnter(const collisions::CCollider*) {}
    virtual void OnCollisionStay(const collisions::CCollider*) {}
    virtual void OnCollisionExit(const collisions::CCollider*) {}

  protected:
    CEntity* m_pOwner = nullptr;
  };
}


