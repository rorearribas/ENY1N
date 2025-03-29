#pragma once
#include "Game/ETT/Components/Component.h"
#include "Engine/Render/Lights/Light.h"
#include "Libs/Maths/Vector3.h"

namespace game { class CEntity; }

namespace game
{
  class CLightComponent : public CComponent
  {
  public:
    CLightComponent(render::lights::ELightType _eLightType);
    virtual ~CLightComponent();

    virtual void Update(float _fDeltaTime) override;
    virtual void DrawDebug() override;

    void SetPosition(const maths::CVector3& _v3Position);
    const maths::CVector3& GetPosition() const;
    void SetRotation(const maths::CVector3& _v3Rot);
    const maths::CVector3& GetRotation() const;

  protected:
    virtual void OnPositionChanged(const maths::CVector3& _v3Pos) override;
    virtual void OnRotationChanged(const maths::CVector3& _v3Rot) override;

  private:
    void Clean();
    void CreateLight(render::lights::ELightType _eLightType);

  private:
    render::lights::CLight* m_pLight = nullptr;
  };
}

