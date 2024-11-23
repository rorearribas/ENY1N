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

#define WIDTH 1920
#define HEIGHT 1080

int main()
{
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->InitEngine(WIDTH, HEIGHT);

  tick::CTimeManager* pTimeManager = tick::CTimeManager::CreateSingleton();
  input::CInputManager* pInputManager = input::CInputManager::CreateSingleton();

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
      pTimeManager->BeginFrame();

      // Calculate 
      float fDeltaTime = pTimeManager->GetDeltaTime();
      float fSeconds = std::clamp(fDeltaTime, 0.0f, pTimeManager->GetMaxFixedDelta());
      m_fFixedDeltaAccumulator += fSeconds;

      // Update
      while (m_fFixedDeltaAccumulator >= pTimeManager->GetFixedDelta())
      {
        pEngine->Update(pTimeManager->GetFixedDelta());
        pInputManager->Flush();

        m_fFixedDeltaAccumulator -= pTimeManager->GetFixedDelta();
      }

      // Begin draw
      pEngine->BeginDraw();

      // Draw
      pEngine->Draw();

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