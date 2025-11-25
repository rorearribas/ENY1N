#pragma once
#include "Game/Entity/Components/Component.h"
#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Lights/Light.h"
#include "Libs/Math/Vector3.h"

namespace game { class CEntity; }

namespace game
{
  class CLightComponent : public CComponent
  {
  public:
    CLightComponent(CEntity* _pOwner, render::ELight _eLightType);
    virtual ~CLightComponent();

    inline render::lights::CLight* GetLight() const { return m_pLight; }
    void SetLightType(render::ELight _eLightType);
    render::ELight GetLightType() const;

    void SetPosition(const math::CVector3& _v3Position);
    math::CVector3 GetPosition() const;
    void SetDirection(const math::CVector3& _v3Rot);
    math::CVector3 GetDirection() const;

    virtual void DrawDebug() override;

  protected:
    virtual void OnPositionChanged(const math::CVector3& _v3Pos) override;

  private:
    void Clean();
    void CreateLight(render::ELight _eLightType);

  private:
    render::lights::CLight* m_pLight = nullptr;
  };
}

