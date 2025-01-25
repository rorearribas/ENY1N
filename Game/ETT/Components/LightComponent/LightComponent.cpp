#include "LightComponent.h"
#include "Engine/Base/Engine.h"
#include "Game/ETT/Entity.h"
#include "Engine/Render/Lights/PointLight.h"
#include "Engine/Render/Lights/SpotLight.h"
#include "Engine/Render/Lights/DirectionalLight.h"
#include <cassert>

namespace game
{
  // ------------------------------------
  CLightComponent::~CLightComponent()
  {
    Clean();
  }
  // ------------------------------------
  void CLightComponent::CreateLight(render::lights::ELightType _eLightType)
  {
    Clean();

    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    assert(pEngine);

    switch (_eLightType)
    {
    case render::lights::DIRECTIONAL_LIGHT:
      m_pLight = pEngine->CreateDirectionalLight();
      break;
    case render::lights::POINT_LIGHT:
      m_pLight = new render::lights::CPointLight(); // Not implemented yet
      break;
    case render::lights::SPOT_LIGHT:
      m_pLight = new render::lights::CSpotLight(); // Not implemented yet
      break;
    default:
      break;
    }
  }
  // ------------------------------------
  void CLightComponent::Update(float _fDeltaTime)
  {
    Super::Update(_fDeltaTime);
  }
  // ------------------------------------
  void CLightComponent::OnPositionChanged(const maths::CVector3& _v3Pos)
  {
    SetPosition(_v3Pos);
  }
  // ------------------------------------
  void CLightComponent::OnRotationChanged(const maths::CVector3& _v3Rot)
  {
    SetRotation(_v3Rot);
  }
  // ------------------------------------
  void CLightComponent::Clean()
  {
    // @TODO Delete from engine
    if (m_pLight)
    {
      delete m_pLight;
      m_pLight = nullptr;
    }
  }
  // ------------------------------------
  void CLightComponent::SetPosition(const maths::CVector3& _v3Position)
  {
    m_pLight->SetPostion(_v3Position);
  }
  // ------------------------------------
  const maths::CVector3& CLightComponent::GetPosition() const
  {
    return m_pOwner->GetPosition();
  }
  // ------------------------------------
  void CLightComponent::SetRotation(const maths::CVector3& _v3Rot)
  {
    m_pLight->SetRotation(_v3Rot);
  }
  // ------------------------------------
  const maths::CVector3& CLightComponent::GetRotation() const
  {
    return m_pOwner->GetRotation();
  }
  // ------------------------------------
}