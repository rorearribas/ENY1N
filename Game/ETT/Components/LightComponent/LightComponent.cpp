#include "LightComponent.h"
#include "Engine/Base/Engine.h"
#include "Game/ETT/Entity.h"
#include "Engine/Render/Lights/PointLight.h"
#include "Engine/Render/Lights/SpotLight.h"
#include "Engine/Render/Lights/DirectionalLight.h"
#include <cassert>
#include "Libs/ImGui/imgui.h"

namespace game
{
  // ------------------------------------
  CLightComponent::CLightComponent(render::lights::ELightType _eLightType) : CComponent()
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
  void CLightComponent::DrawDebug()
  {
    ImGui::Spacing();

    std::string sOwnerName = GetOwner() ? GetOwner()->GetName() : std::string();

    switch (m_pLight->GetLightType())
    {
      case render::lights::ELightType::DIRECTIONAL_LIGHT:
      {
        // Generate unique ids
        std::string sLightDir = "Direction" + std::string("##" + sOwnerName);
        std::string sColor = "Color" + std::string("##" + sOwnerName);
        std::string sIntensity = "Intensity" + std::string("##" + sOwnerName);

        render::lights::CDirectionalLight* pDirectional = static_cast<render::lights::CDirectionalLight*>(m_pLight);
        float dir[3] = { pDirectional->GetDirection().X, pDirectional->GetDirection().Y, pDirectional->GetDirection().Z };
        float color[3] = { pDirectional->GetColor().X, pDirectional->GetColor().Y, pDirectional->GetColor().Z };
        float fIntensity = pDirectional->GetIntensity();

        ImGui::InputFloat3(sLightDir.c_str(), dir);
        ImGui::InputFloat3(sColor.c_str(), color);
        ImGui::InputFloat(sIntensity.c_str(), &fIntensity);

        pDirectional->SetDirection(maths::CVector3(dir[0], dir[1], dir[2]));
        pDirectional->SetColor(maths::CVector3(color[0], color[1], color[2]));
        pDirectional->SetIntensity(fIntensity);
      }
      break;
      case render::lights::ELightType::POINT_LIGHT:
      {
        // Generate unique ids
        std::string sColor = "Color" + std::string("##" + sOwnerName);
        std::string sRange = "Range" + std::string("##" + sOwnerName);
        std::string sIntensity = "Intensity" + std::string("##" + sOwnerName);

        render::lights::CPointLight* pPointLight = static_cast<render::lights::CPointLight*>(m_pLight);
        float color[3] = { pPointLight->GetColor().X, pPointLight->GetColor().Y, pPointLight->GetColor().Z };
        float fIntensity = pPointLight->GetIntensity();
        float fRange = pPointLight->GetRange();

        ImGui::InputFloat3(sColor.c_str(), color);
        ImGui::InputFloat(sRange.c_str(), &fRange);
        ImGui::InputFloat(sIntensity.c_str(), &fIntensity);

        pPointLight->SetColor(maths::CVector3(color[0], color[1], color[2]));
        pPointLight->SetRange(fRange);
        pPointLight->SetIntensity(fIntensity);
      }
      break;
      case render::lights::ELightType::SPOT_LIGHT:
      {
        // Generate unique ids
        std::string sLightDir = "Direction" + std::string("##" + sOwnerName);
        std::string sColor = "Color" + std::string("##" + sOwnerName);
        std::string sIntensity = "Intensity" + std::string("##" + sOwnerName);
        std::string sCutOffAngle = "CutOffAngle" + std::string("##" + sOwnerName);
        std::string sRange = "Range" + std::string("##" + sOwnerName);

        render::lights::CSpotLight* pSpotLight = static_cast<render::lights::CSpotLight*>(m_pLight);
        float dir[3] = { pSpotLight->GetDirection().X, pSpotLight->GetDirection().Y, pSpotLight->GetDirection().Z };
        float color[3] = { pSpotLight->GetColor().X, pSpotLight->GetColor().Y, pSpotLight->GetColor().Z };
        float fIntensity = pSpotLight->GetIntensity();
        float fCutOffAngle = pSpotLight->GetCutOffAngle();
        float fRange = pSpotLight->GetRange();

        ImGui::InputFloat3(sLightDir.c_str(), dir);
        ImGui::InputFloat3(sColor.c_str(), color);
        ImGui::InputFloat(sIntensity.c_str(), &fIntensity);
        ImGui::InputFloat(sCutOffAngle.c_str(), &fCutOffAngle);
        ImGui::InputFloat(sRange.c_str(), &fRange);

        pSpotLight->SetDirection(maths::CVector3(dir[0], dir[1], dir[2]));
        pSpotLight->SetColor(maths::CVector3(color[0], color[1], color[2]));
        pSpotLight->SetIntensity(fIntensity);
        pSpotLight->SetCutOffAngle(fCutOffAngle);
        pSpotLight->SetRange(fRange);
      }
      break;
    default:
      break;
    } 
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
    /*if (m_pLight)
    {
      delete m_pLight;
      m_pLight = nullptr;
    }*/
  }
  // ------------------------------------
  void CLightComponent::SetPosition(const maths::CVector3& /*_v3Position*/)
  {
    //m_pLight->SetPostion(_v3Position);
  }
  // ------------------------------------
  const maths::CVector3& CLightComponent::GetPosition() const
  {
    return m_pOwner->GetPosition();
  }
  // ------------------------------------
  void CLightComponent::SetRotation(const maths::CVector3& /*_v3Rot*/)
  {
    //m_pLight->SetRotation(_v3Rot);
  }
  // ------------------------------------
  const maths::CVector3& CLightComponent::GetRotation() const
  {
    return m_pOwner->GetRotation();
  }
  // ------------------------------------
}