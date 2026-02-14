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
  CLightComponent::CLightComponent(CEntity* _pOwner, render::ELight _eLightType) : CComponent(_pOwner)
  {
    CreateLight(_eLightType);
  }
  // ------------------------------------
  CLightComponent::~CLightComponent()
  {
    Clean();
  }
  // ------------------------------------
  void CLightComponent::CreateLight(render::ELight _eLightType)
  {
    // Clean
    Clean();

    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    switch (_eLightType)
    {
      case render::ELight::DIRECTIONAL_LIGHT: { m_pLight = pEngine->CreateDirectionalLight(); } break;
      case render::ELight::POINT_LIGHT: { m_pLight = pEngine->CreatePointLight(); } break;
      case render::ELight::SPOT_LIGHT: { m_pLight = pEngine->CreateSpotLight(); } break;
      default: break;
    }
  }
  // ------------------------------------
  void CLightComponent::SetLightType(render::ELight _eLightType)
  {
    CreateLight(_eLightType);
  }
  // ------------------------------------
  render::ELight CLightComponent::GetLightType() const
  {
    return m_pLight ? m_pLight->GetLightType() : render::ELight::INVALID;
  }
  // ------------------------------------
  void CLightComponent::DrawDebug()
  {
    ImGui::Spacing();

    CEntity* pEntity = GetOwner();
    std::string sOwnerName = pEntity->GetName();

    switch (m_pLight->GetLightType())
    {
      case render::ELight::DIRECTIONAL_LIGHT:
      {
        render::lights::CDirectionalLight* pDirectional = static_cast<render::lights::CDirectionalLight*>(m_pLight);
        float fColor[3] = { pDirectional->GetColor().x, pDirectional->GetColor().y, pDirectional->GetColor().z };
        float fIntensity = pDirectional->GetIntensity();

        ImGui::Text("DIRECTIONAL LIGHT");
        ImGui::InputFloat3("Color", fColor);
        ImGui::InputFloat("Intensity", &fIntensity);

        // Create rotation matrix
        math::CMatrix4x4 mRot = math::CMatrix4x4::CreateRotation(pEntity->GetRot());
        math::CVector3 v3Dir = mRot * math::CVector3::Forward;

        float fDir[3] = { v3Dir.x, v3Dir.y, v3Dir.z };
        ImGui::InputFloat3("Direction", fDir);
        pDirectional->SetDir(math::CVector3(fDir[0], fDir[1], fDir[2]));

        pDirectional->SetColor(math::CVector3(fColor[0], fColor[1], fColor[2]));
        pDirectional->SetIntensity(fIntensity);
      }
      break;
      case render::ELight::POINT_LIGHT:
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

        // Draw debug
        engine::CEngine::GetInstance()->DrawSphere(pPointLight->GetPos(), fRange, 12, 12, v3Color, render::ERenderMode::WIREFRAME);
      }
      break;
      case render::ELight::SPOT_LIGHT:
      {
        render::lights::CSpotLight* pSpotLight = static_cast<render::lights::CSpotLight*>(m_pLight);
        float lstColor[3] = { pSpotLight->GetColor().x, pSpotLight->GetColor().y, pSpotLight->GetColor().z };
        float fIntensity = pSpotLight->GetIntensity();
        float fRange = pSpotLight->GetRange();

        ImGui::Text("SPOT LIGHT");

        // Create rotation matrix
        math::CMatrix4x4 mRotMatrix = math::CMatrix4x4::CreateRotation(pEntity->GetRot());
        math::CVector3 v3Dir = mRotMatrix * math::CVector3::Forward;
        float fDir[3] = { v3Dir.x, v3Dir.y, v3Dir.z };
        ImGui::InputFloat3("Direction", fDir);
        v3Dir = math::CVector3(fDir[0], fDir[1], fDir[2]);
        pSpotLight->SetDir(v3Dir);

        math::CVector3 v3Color(lstColor[0], lstColor[1], lstColor[2]);
        ImGui::InputFloat3("Color", lstColor);
        v3Color = math::CVector3(lstColor[0], lstColor[1], lstColor[2]);
        pSpotLight->SetColor(v3Color);

        ImGui::InputFloat("Range", &fRange);
        pSpotLight->SetRange(fRange);

        ImGui::InputFloat("Intensity", &fIntensity);
        pSpotLight->SetIntensity(fIntensity);

        // Draw debug
        engine::CEngine::GetInstance()->DrawCube(pSpotLight->GetPos(), pEntity->GetRot(), math::CVector3::One, v3Color, render::ERenderMode::WIREFRAME);
        engine::CEngine::GetInstance()->DrawLine(pSpotLight->GetPos(), pSpotLight->GetPos() + (v3Dir * fRange), v3Color);
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
      m_pLight->SetPos(_v3Position);
    }
  }
  // ------------------------------------
  math::CVector3 CLightComponent::GetPosition() const
  {
    return m_pLight->GetPos();
  }
  // ------------------------------------
  void CLightComponent::SetDirection(const math::CVector3& _v3Dir)
  {
    m_pLight->SetDir(_v3Dir);
  }
  // ------------------------------------
  math::CVector3 CLightComponent::GetDirection() const
  {
    return m_pLight->GetDir();
  }
  // ------------------------------------
}