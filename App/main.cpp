#include <iostream>
#include <random>

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

float GenerateFloat(float min, float max) 
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dist(min, max);
  return dist(gen);
}

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

  game::CEntity* pPlaneEntity = pGameManager->CreateEntity("Plane");
  game::CModelComponent* pPlaneModel = pPlaneEntity->RegisterComponent<game::CModelComponent>();
  pPlaneModel->CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType::E3D_PLANE);
  pPlaneModel->SetPrimitiveColor(maths::CVector3(0.5f, 0.5f, 0.5f));
  pPlaneEntity->SetScale(maths::CVector3(200.0f, 0.0f, 200.0f));
  game::CCollisionComponent* pCollisionComponent = pPlaneEntity->RegisterComponent<game::CCollisionComponent>();
  pCollisionComponent->CreateCollider(collisions::EColliderType::BOX_COLLIDER);
  collisions::CBoxCollider* pBoxCollider = static_cast<collisions::CBoxCollider*>(pCollisionComponent->GetCollider());
  pBoxCollider->SetSize(maths::CVector3(200.0f, 0.0f, 200.0f));

  for (uint32_t uIndex = 0; uIndex < 2; uIndex++)
  {
    game::CEntity* pBoxTest = pGameManager->CreateEntity("Box");
    game::CModelComponent* pBoxTestModel = pBoxTest->RegisterComponent<game::CModelComponent>();
    pBoxTestModel->CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType::E3D_CUBE);
    game::CCollisionComponent* pCollisionComponentBoxTest = pBoxTest->RegisterComponent<game::CCollisionComponent>();
    pCollisionComponentBoxTest->CreateCollider(collisions::EColliderType::BOX_COLLIDER);
    pBoxTest->RegisterComponent<game::CRigidbodyComponent>();
  }

  std::vector<game::CEntity*> vctPhysics = {};
  for (uint32_t uIndex = 0; uIndex < 100; uIndex++)
  {
    game::CEntity* pSphereEntity = pGameManager->CreateEntity("Sphere");
    game::CModelComponent* pSphereModel2 = pSphereEntity->RegisterComponent<game::CModelComponent>();

    pSphereModel2->CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType::E3D_SPHERE);

    float fColorX = GenerateFloat(0.01f, 0.99f);
    float fColorY = GenerateFloat(0.01f, 0.99f);
    float fColorZ = GenerateFloat(0.01f, 0.99f);

    pSphereModel2->SetPrimitiveColor(maths::CVector3(fColorX, fColorY, fColorZ));
    game::CCollisionComponent* pCollisionComponent4 = pSphereEntity->RegisterComponent<game::CCollisionComponent>();
    pCollisionComponent4->CreateCollider(collisions::EColliderType::SPHERE_COLLIDER);
    pSphereEntity->RegisterComponent<game::CRigidbodyComponent>();

    float fRandomY = GenerateFloat(5.0f, 50.0f);
    float fRandomX = GenerateFloat(-10.0f, 10.0f);
    float fRandomZ = GenerateFloat(-0.5f, 0.5f);

    pSphereEntity->SetPosition(maths::CVector3(fRandomX, fRandomY, fRandomZ));
    vctPhysics.emplace_back(pSphereEntity);
  }

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

        pEngine->Update(fFixedDeltaTime); // Update camera.

        pPhysicsManager->Update(fFixedDeltaTime);
        pCollisionManager->Update(fFixedDeltaTime);

        pGameManager->Update(fFixedDeltaTime);
        pInputManager->Flush();

        m_fFixedDeltaAccumulator -= fFixedDeltaTime;
      }

      // Draw
      pEngine->PushDrawProcess();

      if (ImGui::Button("Show wireframe"))
      {
        pEngine->GetRender()->SetFillMode(D3D11_FILL_WIREFRAME);
      }
      if (ImGui::Button("Show lit"))
      {
        pEngine->GetRender()->SetFillMode(D3D11_FILL_SOLID);
      }

      ImGui::Begin("TEST_PHYSICS");
      if (ImGui::Button("Enabled"))
      {
        for (auto& pEntity : vctPhysics)
        {
          auto* pComponent = pEntity->GetComponent<game::CRigidbodyComponent>();
          physics::ERigidbodyType eRigidbodyType = pComponent->GetRigidbodyType();
          switch (eRigidbodyType)
          {
          case physics::KINEMATIC:
            eRigidbodyType = physics::DYNAMIC;
            break;
          case physics::DYNAMIC:
            eRigidbodyType = physics::KINEMATIC;
            break;
          }
          pComponent->SetRigidbodyType(eRigidbodyType);
        }
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
      if (ImGui::Button("300"))
      {
        pTimeManager->SetMaxFPS(300);
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