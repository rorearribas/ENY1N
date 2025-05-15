#pragma once
#include "Game/ETT/Components/Component.h"
#include "Engine/Physics/Rigidbody.h"

namespace physics { class CRigidbody; }

namespace game { class CEntity; }
namespace game
{
  class CRigidbodyComponent : public CComponent
  {
  public:
    CRigidbodyComponent(CEntity* _pOwner, physics::ERigidbodyType _eRigidbodyType = physics::ERigidbodyType::KINEMATIC);
    virtual ~CRigidbodyComponent() { Clean(); }

    void SetRigidbodyType(physics::ERigidbodyType _eRigidbodyType);
    const physics::ERigidbodyType& GetRigidbodyType() const { return m_pRigidbody->GetRigidbodyType(); }

    const float& GetMass() const { return m_pRigidbody->GetMass(); }
    void SetMass(float _fMass) { m_pRigidbody->SetMass(_fMass); }

  protected:
    virtual void OnCollisionEnter(const collisions::SHitEvent&) override;
    virtual void OnCollisionStay(const collisions::SHitEvent&) override;
    virtual void OnCollisionExit(const collisions::SHitEvent&) override;
    virtual void DrawDebug() override;

  private:
    void Clean();
    void CreateRigidbody(physics::ERigidbodyType _eRigidbodyType);

    void OnApplyVelocity(const math::CVector3& _v3Velocity);
    void OnApplyRotation(const math::CVector3& _v3Rot);

  private:
    physics::CRigidbody* m_pRigidbody = nullptr;
  };
}