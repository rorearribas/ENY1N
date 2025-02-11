#include <iostream>
#include "Engine/Base/Engine.h"
#include "Libs/Maths/Vector2.h"
#include "Libs/Maths/Vector3.h"
#include "Engine/Render/Graphics/Primitive.h"
#include <random>
#include "Engine/Scenes/SceneManager.h"
#include "Engine/Input/InputManager.h"
#include "Libs/Time/TimeManager.h"
#include "Libs/ImGui/imgui_internal.h"
#include "Game/GameManager/GameManager.h"
#include "Reflection/Types/Game/Entity.h"
#include "Game/ETT/Components/ModelComponent/ModelComponent.h"
#include "Engine/Managers/ResourceManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Game/ETT/Components/LightComponent/LightComponent.h"
#include "Engine/Render/Lights/DirectionalLight.h"

#define WIDTH 2560
#define HEIGHT 1440

int main()
{
  game::CGameManager* pGameManager = game::CGameManager::CreateSingleton();
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->InitEngine(WIDTH, HEIGHT);

  input::CInputManager* pInputManager = input::CInputManager::CreateSingleton();
  tick::CTimeManager* pTimeManager = tick::CTimeManager::CreateSingleton();
  pTimeManager->SetMaxFPS(144);

  game::CEntity* pDirectionalEntity = pGameManager->CreateEntity("Directional Light");
  pDirectionalEntity->RegisterComponent<game::CLightComponent>(render::lights::ELightType::DIRECTIONAL_LIGHT);

  game::CEntity* pPointLightEntity = pGameManager->CreateEntity("Point Light Test");
  pPointLightEntity->RegisterComponent<game::CLightComponent>(render::lights::ELightType::POINT_LIGHT);

  game::CEntity* pSpotLightEntity = pGameManager->CreateEntity("Spot light Test");
  pSpotLightEntity->RegisterComponent<game::CLightComponent>(render::lights::ELightType::SPOT_LIGHT);

  game::CEntity* pSkullEntity = pGameManager->CreateEntity("Skull");
  game::CModelComponent* pModelComponent = pSkullEntity->RegisterComponent<game::CModelComponent>();
  pModelComponent->LoadModel("..\\Assets//Models//Skull//12140_Skull_v3_L2.obj", "..\\Assets//Models//Skull");
  pSkullEntity->SetRotation(maths::CVector3(90.f, 0.0f, 180.0f));
  pSkullEntity->SetPosition(maths::CVector3(0.0f, 0.0f, 0.0f));

  const render::CRender* pRender = pEngine->GetRender();
  const render::CRenderWindow* pRenderWindow = pRender->GetRenderWindow();
  pRenderWindow->SetEnabled(true);

  float m_fFixedDeltaAccumulator = 0.0f;
  MSG oMsg = { 0 };

  bool bRotateActor = false;
  while (WM_QUIT != oMsg.message)
  {
    if (PeekMessage(&oMsg, nullptr, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&oMsg);
      DispatchMessage(&oMsg);
    }
    else
    {
      // Compute delta time
      pTimeManager->BeginFrame();
      // Begin draw
      pEngine->PushBeginDraw();

      // Calculate 
      float fDeltaTime = pTimeManager->GetDeltaTime();
      float fSeconds = std::clamp(fDeltaTime, 0.0f, pTimeManager->GetMaxFixedDelta());
      m_fFixedDeltaAccumulator += fSeconds;

      // Update
      while (m_fFixedDeltaAccumulator >= pTimeManager->GetFixedDelta())
      {
        pEngine->Update(pTimeManager->GetFixedDelta());
        pGameManager->Update(pTimeManager->GetFixedDelta());
        pInputManager->Flush();
        m_fFixedDeltaAccumulator -= pTimeManager->GetFixedDelta();
      }

      // Draw
      pEngine->PushDrawProcess();

      if (bRotateActor)
      {
        const maths::CVector3 vRot(0.0f, 0.0f, -45.0f * pTimeManager->GetFixedDelta());
        maths::CVector3 vCurrentRotation = pSkullEntity->GetRotation();
        pSkullEntity->SetRotation(vCurrentRotation + vRot);
      }

      ImGui::ShowDemoWindow();

      if (ImGui::Button("Move actor"))
      {
        maths::CVector3 v3Pos = pSkullEntity->GetPosition();
        const maths::CVector3 v3Dir(5.0f, 0.0f, 0.0f);
        pSkullEntity->SetPosition(v3Pos + v3Dir);
      }

      if (ImGui::Button("Rotate Actor"))
      {
        bRotateActor = !bRotateActor;

        if (!bRotateActor)
        {
          pSkullEntity->SetRotation(maths::CVector3::Zero);
        }
      }

      if (ImGui::Button("Low scale"))
      {
        pSkullEntity->SetScale(maths::CVector3(0.01f, 0.01f, 0.01f));
      }

      if (ImGui::Button("Normal scale"))
      {
        pSkullEntity->SetScale(maths::CVector3(1.0f, 1.0f, 1.0f));
      }

      if (ImGui::Button("High scale"))
      {
        pSkullEntity->SetScale(maths::CVector3(3.0f, 3.0f, 3.0f));
      }

      // Imgui utils
      if (ImGui::Button("Set max fps to unlimited"))
      {
        pTimeManager->SetMaxFPS(9999);
      }
      if (ImGui::Button("Set max fps to 144"))
      {
        pTimeManager->SetMaxFPS(144);
      }
      if (ImGui::Button("Set max fps to 60"))
      {
        pTimeManager->SetMaxFPS(60);
      }
      if (ImGui::Button("Set max fps to 30"))
      {
        pTimeManager->SetMaxFPS(30);
      }

      // End draw
      pEngine->PushEndDraw();
      pTimeManager->EndFrame();
    }
  }

  pGameManager->DestroySingleton();
  pTimeManager->DestroySingleton();
  pInputManager->DestroySingleton();
  pEngine->DestroySingleton();
}