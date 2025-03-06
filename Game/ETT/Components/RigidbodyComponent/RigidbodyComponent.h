#pragma once
#include "Game/ETT/Components/Component.h"
#include "Libs/Maths/Vector3.h"
#include "Engine/Collisions/Collider.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Physics/Rigidbody.h"

namespace physics { class CRigidbody; }

namespace game { class CEntity; }
namespace game
{
  class CRigidbodyComponent : public CComponent
  {
  public:
    CRigidbodyComponent(physics::ERigidbodyType _eRigidbodyType = physics::ERigidbodyType::KINEMATIC);
    virtual ~CRigidbodyComponent() { Clean(); }

    virtual void Update(float _fDeltaTime) override;
    virtual void DrawDebug() override;

    void SetRigidbodyType(physics::ERigidbodyType _eRigidbodyType);
    const physics::ERigidbodyType& GetRigidbodyType() const { return m_pRigidbody->GetType(); }
    const float& GetMass() const { return m_pRigidbody->GetMass(); }
    void SetMass(float _fMass) { m_pRigidbody->SetMass(_fMass); }

  protected:
    void OnVelocityChanged(const maths::CVector3& _v3Velocity);

  private:
    void CreateRigidbody(physics::ERigidbodyType _eRigidbodyType);
    void Clean();

  private:
    physics::CRigidbody* m_pRigidbody = nullptr;
  };
}