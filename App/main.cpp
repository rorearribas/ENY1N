#include <iostream>
#include "Engine/Base/Engine.h"
#include "Libs/Maths/Vector2.h"
#include "Libs/Maths/Vector3.h"
#include "Libs/Macros/GlobalMacros.h"

#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Lights/DirectionalLight.h"
#include "Engine/Scenes/SceneManager.h"

#include "Libs/Time/TimeManager.h"
#include "Libs/ImGui/imgui_internal.h"

#include "Engine/Managers/ResourceManager.h"
#include "Engine/Managers/CollisionManager.h"
#include "Engine/Managers/InputManager.h"

#include "Game/GameManager/GameManager.h"
#include "Game/ETT/Components/ModelComponent/ModelComponent.h"
#include "Game/ETT/Components/LightComponent/LightComponent.h"
#include "Game/ETT/Components/CollisionComponent/CollisionComponent.h"

#define WIDTH 2560
#define HEIGHT 1440

int main()
{
  // Engine
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->InitEngine(WIDTH, HEIGHT);

  // GameMananager
  game::CGameManager* pGameManager = game::CGameManager::CreateSingleton();
  // CollisionManager
  physics::CCollisionManager* pCollisionManager = physics::CCollisionManager::CreateSingleton();
  // Input manager
  input::CInputManager* pInputManager = input::CInputManager::CreateSingleton();
  // Time manager
  tick::CTimeManager* pTimeManager = tick::CTimeManager::CreateSingleton();
  pTimeManager->SetMaxFPS(144);

  // Entities
  game::CEntity* pDirectionalEntity = pGameManager->CreateEntity("Directional Light");
  pDirectionalEntity->RegisterComponent<game::CLightComponent>(render::lights::ELightType::DIRECTIONAL_LIGHT);
  game::CEntity* pPointLightEntity = pGameManager->CreateEntity("Point Light Test");
  pPointLightEntity->RegisterComponent<game::CLightComponent>(render::lights::ELightType::POINT_LIGHT);
  game::CEntity* pSpotLightEntity = pGameManager->CreateEntity("Spot light Test");
  pSpotLightEntity->RegisterComponent<game::CLightComponent>(render::lights::ELightType::SPOT_LIGHT);

  //game::CEntity* pRoomEntitty = pGameManager->CreateEntity("Room");
  //game::CModelComponent* pModelComponent = pRoomEntitty->RegisterComponent<game::CModelComponent>();
  //pModelComponent->LoadModel("..\\Assets//Models//Room//room.obj", "..\\Assets//Models//Room");

  game::CEntity* pCube01 = pGameManager->CreateEntity("Cube");
  game::CModelComponent* pModel01 = pCube01->RegisterComponent<game::CModelComponent>();
  pModel01->CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType::CUBE);
  game::CCollisionComponent* pCollisionComponent = pCube01->RegisterComponent<game::CCollisionComponent>();
  pCollisionComponent->CreateCollider(physics::EColliderType::BOX_COLLIDER);

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

      // End draw
      pEngine->PushEndDraw();
      pTimeManager->EndFrame();
    }
  }

  pGameManager->DestroySingleton();
  pCollisionManager->DestroySingleton();
  pTimeManager->DestroySingleton();
  pInputManager->DestroySingleton();
  pEngine->DestroySingleton();
}