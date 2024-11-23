#include <iostream>
#include "Engine/Base/Engine.h"
#include "Libs/Maths/Vector2.h"
#include "Libs/Maths/Vector3.h"
#include "Engine/Render/Primitives/Primitive.h"
#include <random>
#include "Engine/Scenes/SceneManager.h"
#include "Engine/Input/InputManager.h"
#include "Libs/Time/TimeManager.h"
#include "Libs/ImGui/imgui_internal.h"
#include "Game/GameManager/GameManager.h"
#include "Reflection/Types/Game/Entity.h"
#include "Game/ETT/Components/ModelComponent/ModelComponent.h"

#define WIDTH 1920
#define HEIGHT 1080

int main()
{
  game::CGameManager* pGameManager = game::CGameManager::CreateSingleton();
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->InitEngine(WIDTH, HEIGHT);

  input::CInputManager* pInputManager = input::CInputManager::CreateSingleton();
  tick::CTimeManager* pTimeManager = tick::CTimeManager::CreateSingleton();
  pTimeManager->SetMaxFPS(144);

  game::CEntity* pEntity = pGameManager->CreateEntity("Test");
  pEntity->RegisterComponent<game::CModelComponent>();
  game::CModelComponent* pComponent = pEntity->GetComponent<game::CModelComponent>();

  const render::CRender* pRender = pEngine->GetRender();
  const render::CRenderWindow* pRenderWindow = pRender->GetRenderWindow();
  pRenderWindow->SetEnabled(true);

  float m_fFixedDeltaAccumulator = 0.0f;
  MSG oMsg = { 0 };
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
      pEngine->BeginDraw();

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
      pEngine->DrawScene();

      const maths::CVector3 vRot(1.0f * pTimeManager->GetFixedDelta(), 0.0f, 0.0f);
      pEntity->AddRotation(vRot);

      if (ImGui::Button("Move actor"))
      {
        const maths::CVector3 v3Dir(5.0f, 0.0f, 0.0f);
        pEntity->MovePosition(v3Dir);
        pTimeManager->SetMaxFPS(999);
      }

      // Imgui utils
      if (ImGui::Button("Set max fps to unlimited"))
      {
        pTimeManager->SetMaxFPS(999);
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
      pEngine->EndDraw();
      pTimeManager->EndFrame();
    }
  }

  pEngine->DestroySingleton();
  pTimeManager->DestroySingleton();
  pInputManager->DestroySingleton();
}