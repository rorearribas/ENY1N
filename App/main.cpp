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
#include "Engine/Managers/InputManager.h"

#include "Game/GameManager/GameManager.h"
#include "Game/ETT/Components/ModelComponent/ModelComponent.h"
#include "Game/ETT/Components/LightComponent/LightComponent.h"
#include "Game/ETT/Components/CollisionComponent/CollisionComponent.h"

#include "Engine/Collisions/CollisionManager.h"
#include "Engine/Collisions/BoxCollider.h"
#include "Engine/Physics/PhysicsManager.h"
#include "Libs/ImGui/imgui.h"

#define WIDTH 2560
#define HEIGHT 1440

int main()
{
  // Engine
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->InitEngine(WIDTH, HEIGHT);

  // Game manager
  game::CGameManager* pGameManager = game::CGameManager::CreateSingleton();

  // Collisions manager
  collisions::CCollisionManager* pCollisionManager = collisions::CCollisionManager::CreateSingleton();
  // Physics manager
  physics::CPhysicsManager* pPhysicsManager = physics::CPhysicsManager::CreateSingleton();

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

  game::CEntity* pPlaneEntity = pGameManager->CreateEntity("PLANE");
  game::CModelComponent* pPlaneModel = pPlaneEntity->RegisterComponent<game::CModelComponent>();
  pPlaneModel->CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType::E3D_CUBE);
  pPlaneModel->SetPrimitiveColor(maths::CVector3(0.5f, 0.5f, 0.5f));
  pPlaneEntity->SetScale(maths::CVector3(5.0f, 0.050f, 5.0f));
  game::CCollisionComponent* pCollisionComponent = pPlaneEntity->RegisterComponent<game::CCollisionComponent>();
  pCollisionComponent->CreateCollider(collisions::EColliderType::BOX_COLLIDER);
  pCollisionComponent->SetPhysicsEnabled(false);
  collisions::CBoxCollider* pBoxCollider = static_cast<collisions::CBoxCollider*>(pCollisionComponent->GetCollider());
  pBoxCollider->SetSize(maths::CVector3(5.0f, 0.050f, 5.0f));

  game::CEntity* pCube02 = pGameManager->CreateEntity("Cube");
  game::CModelComponent* pModel02 = pCube02->RegisterComponent<game::CModelComponent>();
  pModel02->CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType::E3D_CUBE);
  pModel02->SetPrimitiveColor(maths::CVector3(0.0f, 0.0f, 1.0f));
  game::CCollisionComponent* pCollisionComponent2 = pCube02->RegisterComponent<game::CCollisionComponent>();
  pCollisionComponent2->CreateCollider(collisions::EColliderType::BOX_COLLIDER);
  pCube02->SetPosition(maths::CVector3(0.0f, 5.0f, 0.0f));

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
        float fFixedDeltaTime = pTimeManager->GetFixedDelta();

        pEngine->Update(fFixedDeltaTime);
        pGameManager->Update(fFixedDeltaTime);

        pCollisionManager->Update(fFixedDeltaTime);
        pPhysicsManager->Update(fFixedDeltaTime);

        pInputManager->Flush();
        m_fFixedDeltaAccumulator -= fFixedDeltaTime;
      }

      // Draw
      pEngine->PushDrawProcess();

      ImGui::Begin("TEST_PHYSICS");
      if (ImGui::Button("Enabled"))
      {
        pCollisionComponent2->SetPhysicsEnabled(pCollisionComponent2->IsPhysicsEnabled());
      }
      ImGui::End();

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