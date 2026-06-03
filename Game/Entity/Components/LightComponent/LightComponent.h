#pragma once
#include "Game/Entity/Components/Component.h"
#include "Engine/Render/Lighting/Light.h"
#include "Engine/Render/RenderTypes.h"
#include "Libs/Utils/FixedPool.h"
#include "Libs/Math/Vector3.h"

namespace game { class CEntity; }

namespace game
{
  class CLightComponent : public CComponent
  {
  public:
    CLightComponent(CEntity* _pOwner) : CComponent(_pOwner) {}
    virtual ~CLightComponent();

    inline void SetLightType(render::ELight _eLightType) { CreateLight(_eLightType); }
    inline render::ELight GetLightType() const { return m_pLight->GetLightType(); }
    void CreateLight(render::ELight _eLightType);

    inline void SetPos(const math::CVector3& _v3Position) { m_pLight->SetPos(_v3Position); }
    inline math::CVector3 GetPos() const { return m_pLight->GetPos(); }
    inline void SetDir(const math::CVector3& _v3Dir) { m_pLight->SetDir(_v3Dir); }
    inline math::CVector3 GetDir() const { return m_pLight->GetDir(); }

    render::lights::CLight* GetLight() const { return m_pLight.GetPtr(); }
    virtual void DrawDebug() override;

  protected:
    virtual void OnPositionChanged(const math::CVector3& _v3Pos) override;
    virtual void OnRotationChanged(const math::CVector3& _v3Rot) override;

  private:
    void Clean();

  private:
    utils::CWeakPtr<render::lights::CLight> m_pLight;
  };
}

