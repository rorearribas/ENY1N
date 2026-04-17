#pragma once
#include "Game/Entity/Components/Component.h"
#include "Engine/Render/Lighting/Light.h"
#include "Engine/Render/RenderTypes.h"
#include "Libs/Math/Vector3.h"

namespace game { class CEntity; }

namespace game
{
  class CLightComponent : public CComponent
  {
  public:
    CLightComponent(CEntity* _pOwner, render::ELight _eLightType);
    virtual ~CLightComponent();

    inline void SetLightType(render::ELight _eLightType) { CreateLight(_eLightType); }
    inline render::ELight GetLightType() const { return m_pLight->GetLightType(); }

    inline void SetPos(const math::CVector3& _v3Position) { m_pLight->SetPos(_v3Position); }
    inline math::CVector3 GetPos() const { return m_pLight->GetPos(); }
    inline void SetDir(const math::CVector3& _v3Dir) { m_pLight->SetDir(_v3Dir); }
    inline math::CVector3 GetDir() const { return m_pLight->GetDir(); }

    inline render::lights::CLight* GetLight() const { return m_pLight; }
    virtual void DrawDebug() override;

  protected:
    virtual void OnPositionChanged(const math::CVector3& _v3Pos) override;
    virtual void OnRotationChanged(const math::CVector3& _v3Rot) override;

  private:
    void Clean();
    void CreateLight(render::ELight _eLightType);

  private:
    render::lights::CLight* m_pLight = nullptr;
  };
}

