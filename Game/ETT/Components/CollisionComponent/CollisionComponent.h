#pragma once
#include "Game/ETT/Components/Component.h"
#include "Libs/Maths/Vector3.h"
#include "Engine/Physics/Collider.h"
#include "Engine/Render/Graphics/Primitive.h"

namespace game { class CEntity; }
namespace game
{
  class CCollisionComponent : public CComponent
  {
  public:
    CCollisionComponent() = default;
    CCollisionComponent(physics::EColliderType _eColliderType);
    virtual ~CCollisionComponent() { Clean(); }

    void CreateCollider(physics::EColliderType _eColliderType);
    virtual void Update(float _fDeltaTime) override;
    virtual void DrawDebug() override;

    void SetPosition(const maths::CVector3& _v3Position);
    const maths::CVector3& GetPosition() const;
    virtual void OnPositionChanged(const maths::CVector3& _v3Pos) override;

  private:
    void Clean();
    physics::CCollider* m_pCollider = nullptr;
    render::graphics::CPrimitive* m_pPrimitive = nullptr;
  };
}