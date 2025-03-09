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
#include "Game/ETT/Components/RigidbodyComponent/RigidbodyComponent.h"
#include "Libs/Maths/Maths.h"

#define WIDTH 2560
#define HEIGHT 1440

int main()
{
  // Engine
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->InitEngine(WIDTH, HEIGHT);
  // Collisions manager
  collisions::CCollisionManager* pCollisionManager = collisions::CCollisionManager::CreateSingleton();
  // Physics manager
  physics::CPhysicsManager* pPhysicsManager = physics::CPhysicsManager::CreateSingleton();
  // Input manager
  input::CInputManager* pInputManager = input::CInputManager::CreateSingleton();
  // Time manager
  tick::CTimeManager* pTimeManager = tick::CTimeManager::CreateSingleton();
  pTimeManager->SetMaxFPS(144);

  // Game manager
  game::CGameManager* pGameManager = game::CGameManager::CreateSingleton();

  game::CEntity* pPlaneEntity = pGameManager->CreateEntity("PLANE");
  game::CModelComponent* pPlaneModel = pPlaneEntity->RegisterComponent<game::CModelComponent>();
  pPlaneModel->CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType::E3D_PLANE);
  pPlaneModel->SetPrimitiveColor(maths::CVector3(0.5f, 0.5f, 0.5f));
  pPlaneEntity->SetScale(maths::CVector3(5.0f, 5.0f, 5.0f));
  game::CCollisionComponent* pCollisionComponent = pPlaneEntity->RegisterComponent<game::CCollisionComponent>();
  pCollisionComponent->CreateCollider(collisions::EColliderType::BOX_COLLIDER);
  collisions::CBoxCollider* pBoxCollider = static_cast<collisions::CBoxCollider*>(pCollisionComponent->GetCollider());
  pBoxCollider->SetSize(maths::CVector3(5.0f, 0.0f, 5.0f));

  game::CEntity* pEntityCube = pGameManager->CreateEntity("Cube");
  game::CModelComponent* pCubeModel = pEntityCube->RegisterComponent<game::CModelComponent>();
  pCubeModel->CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType::E3D_CUBE);
  pCubeModel->SetPrimitiveColor(maths::CVector3(1.0f, 0.0f, 0.0f));
  game::CCollisionComponent* pCollisionComponen2 = pEntityCube->RegisterComponent<game::CCollisionComponent>();
  pCollisionComponen2->CreateCollider(collisions::EColliderType::BOX_COLLIDER);
  game::CRigidbodyComponent* pRigidbodyTest = pEntityCube->RegisterComponent<game::CRigidbodyComponent>();
  pEntityCube->SetPosition(maths::CVector3(0.0f, 8.0f, 0.0f));

  const render::CRender* pRender = pEngine->GetRender();
  pRender->GetRenderWindow()->SetEnabled(true);

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
      // Begin draw
      pTimeManager->BeginFrame();
      pEngine->PushBeginDraw();

      // Calculate 
      float fDeltaTime = pTimeManager->GetDeltaTime();
      float fOffset = maths::clamp<float>(fDeltaTime, 0.0f, pTimeManager->GetMaxFixedDelta());
      m_fFixedDeltaAccumulator += fOffset;

      // Update
      while (m_fFixedDeltaAccumulator >= pTimeManager->GetFixedDelta())
      {
        float fFixedDeltaTime = pTimeManager->GetFixedDelta();

        pEngine->Update(fFixedDeltaTime); // This is for update the camera.

        pPhysicsManager->Update(fFixedDeltaTime);
        pCollisionManager->Update(fFixedDeltaTime);

        pGameManager->Update(fFixedDeltaTime);
        pInputManager->Flush();

        m_fFixedDeltaAccumulator -= fFixedDeltaTime;
      }

      // Draw
      pEngine->PushDrawProcess();

      ImGui::Begin("TEST_PHYSICS");
      if (ImGui::Button("Enabled"))
      {
        physics::ERigidbodyType eRigidbodyType = pRigidbodyTest->GetRigidbodyType();
        switch (eRigidbodyType)
        {
        case physics::KINEMATIC:
          eRigidbodyType = physics::DYNAMIC;
          break;
        case physics::DYNAMIC:
          eRigidbodyType = physics::KINEMATIC;
          break;
        }
        pRigidbodyTest->SetRigidbodyType(eRigidbodyType);
      }

      if (ImGui::Button("30"))
      {
        pTimeManager->SetMaxFPS(30);
      }
      if (ImGui::Button("60"))
      {
        pTimeManager->SetMaxFPS(60);
      }
      if (ImGui::Button("144"))
      {
        pTimeManager->SetMaxFPS(144);
      }
      if (ImGui::Button("1000"))
      {
        pTimeManager->SetMaxFPS(1000);
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