#pragma once
#include "Game/ETT/Components/Component.h"
#include "Engine/Render/Lights/Light.h"
#include "Libs/Math/Vector3.h"

namespace game { class CEntity; }

namespace game
{
  class CLightComponent : public CComponent
  {
  public:
    CLightComponent(CEntity* _pOwner, render::lights::ELightType _eLightType);
    virtual ~CLightComponent();

    virtual void Update(float _fDeltaTime) override;
    virtual void DrawDebug() override;

    render::lights::CBaseLight* GetLight() const { return m_pLight; }
    void SetLightType(render::lights::ELightType _eLightType);
    render::lights::ELightType GetLightType() const;

    void SetPosition(const math::CVector3& _v3Position);
    const math::CVector3& GetPosition() const;
    void SetDirection(const math::CVector3& _v3Rot);
    const math::CVector3& GetDirection() const;

  protected:
    virtual void OnPositionChanged(const math::CVector3& _v3Pos) override;

  private:
    void Clean();
    void CreateLight(render::lights::ELightType _eLightType);

  private:
    render::lights::CBaseLight* m_pLight = nullptr;
  };
}

