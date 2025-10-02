#pragma once
#include "Game/Entity/Components/Component.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Libs/Math/Vector3.h"

namespace game { class CEntity; }

namespace game
{
  class CModelComponent : public CComponent
  {
  public:
    CModelComponent(CEntity* _pOwner) : CComponent(_pOwner) {}
    virtual ~CModelComponent();

    virtual void Update(float _fDeltaTime) override;
    virtual void DrawDebug() override {};

    void LoadModel(const char* _sModelPath, const char* _sBaseMltDir);
    void CreatePrimitive(render::gfx::EPrimitiveType _ePrimitiveType, render::ERenderMode _eRenderMode = render::ERenderMode::SOLID);

    void SetPrimitiveRenderMode(render::ERenderMode _eRenderMode);
    void SetPrimitiveColor(const math::CVector3& _v3Color);

    void SetPosition(const math::CVector3& _v3Position);
    const math::CVector3& GetPosition() const;
    void SetRotation(const math::CVector3& _v3Rot);
    const math::CVector3& GetRotation() const;
    void SetScale(const math::CVector3& _v3Scale);
    const math::CVector3& GetScale() const;

  protected:
    virtual void OnPositionChanged(const math::CVector3& _v3Pos) override;
    virtual void OnRotationChanged(const math::CVector3& _v3Rot) override;
    virtual void OnScaleChanged(const math::CVector3& _v3Scale) override;

  private:
    void Clean();

  private:
    render::gfx::CPrimitive* m_pPrimitive = nullptr;
    render::gfx::CModel* m_pModel = nullptr;
  };
}

