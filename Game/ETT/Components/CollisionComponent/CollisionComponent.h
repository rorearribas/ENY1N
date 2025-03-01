#pragma once
#include "Game/ETT/Components/Component.h"
#include "Libs/Maths/Vector3.h"
#include "Engine/Collisions/Collider.h"
#include "Engine/Render/Graphics/Primitive.h"

namespace physics { class CRigidbody; }

namespace game { class CEntity; }
namespace game
{
  class CCollisionComponent : public CComponent
  {
  public:
    CCollisionComponent() = default;
    CCollisionComponent(collisions::EColliderType _eColliderType);
    virtual ~CCollisionComponent() { Clean(); }

    virtual void Update(float _fDeltaTime) override;
    virtual void DrawDebug() override;

    void CreateCollider(collisions::EColliderType _eColliderType);
    collisions::CCollider* const GetCollider() { return m_pCollider; }

    void SetPhysicsEnabled(bool _bStatus);
    const bool& IsPhysicsEnabled() const { return m_bEnablePhysics; }

    void SetPosition(const maths::CVector3& _v3Position);
    const maths::CVector3& GetPosition() const;
    virtual void OnPositionChanged(const maths::CVector3& _v3Pos) override;

  private:
    void Clean();
    bool m_bEnablePhysics = true;

    collisions::CCollider* m_pCollider = nullptr;
    physics::CRigidbody* m_pRigidbody = nullptr;
    render::graphics::CPrimitive* m_pPrimitive = nullptr;
  };
}