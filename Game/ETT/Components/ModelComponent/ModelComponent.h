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
    CModelComponent(CEntity* _pEntity) : CComponent(_pEntity) {}
    virtual ~CModelComponent();

    virtual void UpdateComponent(float _fDeltaTime) override;
    virtual void DrawDebug() override {};

    void LoadModel(const char* _sModelPath);
    void CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType _ePrimitiveType);

    void SetPosition(const maths::CVector3& _v3Position);
    const maths::CVector3& GetPosition() const;
    void SetRotation(const maths::CVector3& _v3Rot);
    const maths::CVector3& GetRotation() const;
    void SetScale(const maths::CVector3& _v3Scale);
    const maths::CVector3& GetScale() const;

    virtual void OnPositionChanged(const maths::CVector3& _v3Pos) override;
    virtual void OnRotationChanged(const maths::CVector3& _v3Rot) override;
    virtual void OnScaleChanged(const maths::CVector3& _v3Scale) override;

  private:
    render::graphics::CPrimitive* m_pPrimitive = nullptr;
    render::graphics::CModel* m_pModel = nullptr;
  };
}
