#pragma once
#include "Game/ETT/Components/Component.h"
#include "Libs/Maths/Vector3.h"
#include "Game/ETT/Entity.h"

namespace render { namespace primitive { class CPrimitive; } }

namespace game
{
  class CModelComponent : public CComponent
  {
  public:
    CModelComponent(CEntity* _pEntity);

    virtual void Update(float _fDeltaTime) override;

    void SetPosition(const maths::CVector3& _v3Position);
    const maths::CVector3& GetPosition() const { return m_pOwner->GetPosition(); }

    void SetRotation(const maths::CVector3& _v3Rot);
    const maths::CVector3& GetRotation() const { return m_pOwner->GetRotation(); }

    // We need models!!!!
    void SetScale(const maths::CVector3& _v3Scale);
    const maths::CVector3& GetScale() const { return m_pOwner->GetScale(); }

  private:
    render::primitive::CPrimitive* m_pPrimitive = nullptr;
  };
}

