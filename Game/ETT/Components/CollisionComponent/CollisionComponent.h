#pragma once
#include "Game/ETT/Components/Component.h"
#include "Libs/Math/Vector3.h"
#include "Engine/Collisions/Collider.h"
#include "Engine/Render/Graphics/Primitive.h"

namespace physics { class CRigidbody; }

namespace game { class CEntity; }
namespace game
{
  class CCollisionComponent : public CComponent
  {
  public:
    CCollisionComponent(CEntity* _pOwner) : CComponent(_pOwner) {}
    explicit CCollisionComponent(CEntity* _pOwner, collision::EColliderType _eColliderType);
    virtual ~CCollisionComponent() { Clean(); }

    void CreateCollider(collision::EColliderType _eColliderType);
    collision::CCollider* const GetCollider() { return m_pCollider; }

    void SetPosition(const math::CVector3& _v3Position);
    const math::CVector3& GetPosition() const;
    void SetRotation(const math::CVector3& _v3Rotation);
    const math::CVector3& GetRotation() const;

  protected:
    virtual void OnPositionChanged(const math::CVector3& _v3Pos) override;
    virtual void OnRotationChanged(const math::CVector3& _v3Rot) override;
    virtual void DrawDebug() override;

  private:
    void Clean();

  private:
    collision::CCollider* m_pCollider = nullptr;
    render::graphics::CPrimitive* m_pPrimitive = nullptr;
  };
}