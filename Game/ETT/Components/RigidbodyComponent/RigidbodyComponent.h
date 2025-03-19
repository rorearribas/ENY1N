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
    const physics::ERigidbodyType& GetRigidbodyType() const { return m_pRigidbody->GetRigidbodyType(); }
    const float& GetMass() const { return m_pRigidbody->GetMass(); }
    void SetMass(float _fMass) { m_pRigidbody->SetMass(_fMass); }
    const float& GetDrag() const { return m_pRigidbody->GetDrag();}
    void SetDrag(float _fDrag){ return m_pRigidbody->SetDrag(_fDrag);}

  protected:
    virtual void OnCollisionEnter(const collisions::CCollider*, const collisions::SHitEvent& _oHitEvent) override;
    virtual void OnCollisionStay(const collisions::CCollider*, const collisions::SHitEvent& _oHitEvent) override;
    virtual void OnCollisionExit(const collisions::CCollider*, const collisions::SHitEvent& _oHitEvent) override;

  private:
    void OnVelocityChanged(const maths::CVector3& _v3Velocity);
    void OnAddedRotation(const maths::CVector3& _v3Rot);

    void CreateRigidbody(physics::ERigidbodyType _eRigidbodyType);
    void Clean();

  private:
    physics::CRigidbody* m_pRigidbody = nullptr;
  };
}