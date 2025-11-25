#pragma once
#include "Game/Entity/Components/Component.h"
#include "Engine/Render/Graphics/RenderInstance.h"
#include "Engine/Render/RenderTypes.h"
#include "Libs/Math/Vector3.h"
#include <Libs/Utils/UniquePtrList.h>

namespace render { namespace gfx { class CRenderInstance; } }
namespace render { namespace gfx { class CPrimitive; } }
namespace render { namespace gfx { class CModel; } }

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

    void SetCullingEnabled(bool _bCull);
    void SetRenderMode(render::ERenderMode _eRenderMode);
    void SetColor(const math::CVector3& _v3Color);

    void SetPosition(const math::CVector3& _v3Position);
    math::CVector3 GetPosition() const;
    void SetRotation(const math::CVector3& _v3Rot);
    math::CVector3 GetRotation() const;
    void SetScale(const math::CVector3& _v3Scl);
    math::CVector3 GetScale() const;

  protected:
    virtual void OnPositionChanged(const math::CVector3& _v3Pos) override;
    virtual void OnRotationChanged(const math::CVector3& _v3Rot) override;
    virtual void OnScaleChanged(const math::CVector3& _v3Scale) override;

  private:
    void Clean();

  private:
    // Primitive
    render::gfx::CPrimitive* m_pPrimitive = nullptr;

    // Model
    utils::CWeakPtr<render::gfx::CModel> m_wpModel;
    uint32_t m_uInstanceID = render::instance::s_uInvalidID;
  };
}

