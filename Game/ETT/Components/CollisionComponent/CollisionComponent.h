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
    CCollisionComponent() {}
    virtual ~CCollisionComponent() { Clean(); }

    virtual void Update(float _fDeltaTime) override;

    void CreateCollider(collisions::EColliderType _eColliderType);
    collisions::CCollider* const GetCollider() { return m_pCollider; }
    void SetPosition(const maths::CVector3& _v3Position);
    const maths::CVector3& GetPosition() const;

    virtual void OnPositionChanged(const maths::CVector3& _v3Pos) override;
    virtual void DrawDebug() override;

  private:
    void Clean();

  private:
    collisions::CCollider* m_pCollider = nullptr;
    render::graphics::CPrimitive* m_pPrimitive = nullptr;
  };
}