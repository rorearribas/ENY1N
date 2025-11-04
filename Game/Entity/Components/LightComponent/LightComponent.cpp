#include "LightComponent.h"
#include "Engine/Engine.h"
#include "Game/Entity/Entity.h"
#include "Engine/Render/Lights/PointLight.h"
#include "Engine/Render/Lights/SpotLight.h"
#include "Engine/Render/Lights/DirectionalLight.h"
#include <cassert>
#include "Libs/ImGui/imgui.h"
#include "Libs/Math/Math.h"

namespace game
{
  // ------------------------------------
  CLightComponent::CLightComponent(CEntity* _pOwner, render::lights::ELightType _eLightType) : CComponent(_pOwner)
  {
    CreateLight(_eLightType);
  }
  // ------------------------------------
  CLightComponent::~CLightComponent()
  {
    Clean();
  }
  // ------------------------------------
  void CLightComponent::CreateLight(render::lights::ELightType _eLightType)
  {
    // Clean
    Clean();

    engine::CEngine* pEngine = engine::CEngine::GetInstance();
#ifdef _DEBUG
    assert(pEngine);
#endif

    switch (_eLightType)
    {
    case render::lights::DIRECTIONAL_LIGHT:
      m_pLight = pEngine->CreateDirectionalLight();
      break;
    case render::lights::POINT_LIGHT:
      m_pLight = pEngine->CreatePointLight();
      break;
    case render::lights::SPOT_LIGHT:
      m_pLight = pEngine->CreateSpotLight();
      break;
    default:
      break;
    }
  }
  // ------------------------------------
  void CLightComponent::SetLightType(render::lights::ELightType _eLightType)
  {
    CreateLight(_eLightType);
  }
  // ------------------------------------
  render::lights::ELightType CLightComponent::GetLightType() const
  {
    return m_pLight ? m_pLight->GetLightType() : render::lights::ELightType::INVALID;
  }
  // ------------------------------------
  void CLightComponent::DrawDebug()
  {
    ImGui::Spacing();

    CEntity* pEntity = GetOwner();
    std::string sOwnerName = pEntity->GetName();

    switch (m_pLight->GetLightType())
    {
    case render::lights::ELightType::DIRECTIONAL_LIGHT:
    {
      render::lights::CDirectionalLight* pDirectional = static_cast<render::lights::CDirectionalLight*>(m_pLight);
      float dir[3] = { pDirectional->GetDirection().x, pDirectional->GetDirection().y, pDirectional->GetDirection().z };
      float color[3] = { pDirectional->GetColor().x, pDirectional->GetColor().y, pDirectional->GetColor().z };
      float fIntensity = pDirectional->GetIntensity();

      ImGui::Text("DIRECTIONAL LIGHT");
      ImGui::InputFloat3("Direction", dir);
      ImGui::InputFloat3("Color", color);
      ImGui::InputFloat("Intensity", &fIntensity);

      pDirectional->SetDirection(math::CVector3(dir[0], dir[1], dir[2]));
      pDirectional->SetColor(math::CVector3(color[0], color[1], color[2]));
      pDirectional->SetIntensity(fIntensity);
    }
    break;
    case render::lights::ELightType::POINT_LIGHT:
    {
      render::lights::CPointLight* pPointLight = static_cast<render::lights::CPointLight*>(m_pLight);
      float lstColor[3] = { pPointLight->GetColor().x, pPointLight->GetColor().y, pPointLight->GetColor().z };
      float fIntensity = pPointLight->GetIntensity();
      float fRange = pPointLight->GetRange();

      ImGui::Text("POINT LIGHT");
      ImGui::InputFloat3("Color", lstColor);
      ImGui::InputFloat("Range", &fRange);
      ImGui::InputFloat("Intensity", &fIntensity);

      math::CVector3 v3Color(lstColor[0], lstColor[1], lstColor[2]);
      pPointLight->SetColor(v3Color);
      pPointLight->SetRange(fRange);
      pPointLight->SetIntensity(fIntensity);

      // Draw sphere
      engine::CEngine::GetInstance()->DrawSphere(pPointLight->GetPosition(), fRange, 12, 12, v3Color, render::ERenderMode::WIREFRAME);
    }
    break;
    case render::lights::ELightType::SPOT_LIGHT:
    {
      render::lights::CSpotLight* pSpotLight = static_cast<render::lights::CSpotLight*>(m_pLight);
      float lstDir[3] = { pSpotLight->GetDirection().x, pSpotLight->GetDirection().y, pSpotLight->GetDirection().z };
      float lstColor[3] = { pSpotLight->GetColor().x, pSpotLight->GetColor().y, pSpotLight->GetColor().z };
      float fIntensity = pSpotLight->GetIntensity();
      float fRange = pSpotLight->GetRange();

      ImGui::Text("SPOT LIGHT");
      ImGui::InputFloat3("Direction", lstDir);
      ImGui::InputFloat3("Color", lstColor);
      ImGui::InputFloat("Range", &fRange);
      ImGui::InputFloat("Intensity", &fIntensity);

      math::CVector3 v3Dir(lstDir[0], lstDir[1], lstDir[2]);
      math::CVector3 v3Color(lstColor[0], lstColor[1], lstColor[2]);

      pSpotLight->SetDirection(v3Dir);
      pSpotLight->SetColor(v3Color);
      pSpotLight->SetIntensity(fIntensity);
      pSpotLight->SetRange(fRange);

      // Draw line
      engine::CEngine::GetInstance()->DrawLine(pSpotLight->GetPosition(), pSpotLight->GetPosition() + (v3Dir * fRange), v3Color);
    }
    break;
    default:
      break;
    }
  }
  // ------------------------------------
  void CLightComponent::OnPositionChanged(const math::CVector3& _v3Pos)
  {
    SetPosition(_v3Pos);
  }
  // ------------------------------------
  void CLightComponent::Clean()
  {
    if (m_pLight)
    {
      engine::CEngine* pEngine = engine::CEngine::GetInstance();
      pEngine->DestroyLight(m_pLight);
    }
  }
  // ------------------------------------
  void CLightComponent::SetPosition(const math::CVector3& _v3Position)
  {
    if (m_pLight)
    {
      m_pLight->SetPosition(_v3Position);
    }
  }
  // ------------------------------------
  const math::CVector3& CLightComponent::GetPosition() const
  {
    return m_pLight ? m_pLight->GetPosition() : m_pOwner->GetPosition();
  }
  // ------------------------------------
  void CLightComponent::SetDirection(const math::CVector3& _v3Dir)
  {
    m_pLight->SetDirection(_v3Dir);
  }
  // ------------------------------------
  const math::CVector3& CLightComponent::GetDirection() const
  {
    return m_pLight->GetDirection();
  }
  // ------------------------------------
}