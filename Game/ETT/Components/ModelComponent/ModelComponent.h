#pragma once
#include "Game/ETT/Components/Component.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Libs/Maths/Vector3.h"

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
    void CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType _ePrimitiveType, render::ERenderMode _eRenderMode = render::ERenderMode::SOLID);

    void SetPrimitiveRenderMode(render::ERenderMode _eRenderMode);
    void SetPrimitiveColor(const maths::CVector3& _v3Color);

    void SetPosition(const maths::CVector3& _v3Position);
    const maths::CVector3& GetPosition() const;
    void SetRotation(const maths::CVector3& _v3Rot);
    const maths::CVector3& GetRotation() const;
    void SetScale(const maths::CVector3& _v3Scale);
    const maths::CVector3& GetScale() const;

  protected:
    virtual void OnPositionChanged(const maths::CVector3& _v3Pos) override;
    virtual void OnRotationChanged(const maths::CVector3& _v3Rot) override;
    virtual void OnScaleChanged(const maths::CVector3& _v3Scale) override;

  private:
    void Clean();

  private:
    render::graphics::CPrimitive* m_pPrimitive = nullptr;
    render::graphics::CModel* m_pModel = nullptr;
  };
}

