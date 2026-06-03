#pragma once
#include "Game/Entity/Components/Component.h"

#include "Libs/Utils/FixedPool.h"
#include "Libs/Math/Vector3.h"

#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Graphics/RenderInstance.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Render/Graphics/Primitive.h"

namespace game { class CEntity; }
namespace game
{
  class CModelComponent : public CComponent
  {
  public:
    CModelComponent(CEntity* _pOwner) : CComponent(_pOwner) {}
    virtual ~CModelComponent();

    void LoadModel(const char* _sModelPath);
    void CreatePrimitive(render::EPrimitive _eType, render::ERenderMode _eRenderMode);

    void SetColor(const math::CVector3& _v3Color);
    void SetCullingEnabled(bool _bCull);

    void SetPos(const math::CVector3& _v3Position);
    math::CVector3 GetPosition() const;
    void SetRotation(const math::CVector3& _v3Rot);
    math::CVector3 GetRotation() const;
    void SetScl(const math::CVector3& _v3Scl);
    math::CVector3 GetScale() const;

    virtual void DrawDebug() override;

  protected:
    virtual void OnPositionChanged(const math::CVector3& _v3Pos) override;
    virtual void OnRotationChanged(const math::CVector3& _v3Rot) override;
    virtual void OnScaleChanged(const math::CVector3& _v3Scale) override;

  private:
    void Clean();

    // Items
    utils::CWeakPtr<render::gfx::CModel> m_wpModel;
    utils::CWeakPtr<render::gfx::CRenderInstance> m_wpModelInstance;
    utils::CWeakPtr<render::gfx::CPrimitive> m_wpPrimitive;
  };
}

