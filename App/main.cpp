#include <iostream>
#include <random>

#include "Engine/Engine.h"
#include "Libs/Math/Vector2.h"
#include "Libs/Math/Vector3.h"
#include "Libs/Macros/GlobalMacros.h"

#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Lights/DirectionalLight.h"
#include "Engine/Scenes/SceneManager.h"

#include "Libs/Time/TimeManager.h"
#include "Libs/ImGui/imgui_internal.h"

#include "Engine/Managers/ResourceManager.h"
#include "Engine/Managers/InputManager.h"

#include "Game/GameManager/GameManager.h"
#include "Game/Entity/Components/ModelComponent/ModelComponent.h"
#include "Game/Entity/Components/LightComponent/LightComponent.h"
#include "Game/Entity/Components/CollisionComponent/CollisionComponent.h"
#include "Game/Entity/Components/RigidbodyComponent/RigidbodyComponent.h"

#include "Engine/Collisions/CollisionManager.h"
#include "Engine/Collisions/BoxCollider.h"
#include "Engine/Physics/PhysicsManager.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/Math/Math.h"
#include "Reflection/TypeManager.h"

float GenerateFloat(float min, float max)
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dist(min, max);
  return dist(gen);
}

static bool bThrowRay = false;

#define WIDTH 2560
#define HEIGHT 1440

int main()
{
  global::mem::s_oMemoryTracker.PrintStats();

  // Init
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->Init(WIDTH, HEIGHT);
  pEngine->GetCamera()->SetPosition(math::CVector3(0.0f, 5.0f, -10.0f));

  global::mem::s_oMemoryTracker.PrintStats();

  // Time manager
  chrono::CTimeManager* pTimeManager = chrono::CTimeManager::CreateSingleton();
  pTimeManager->SetMaxFPS(144);

  // Create resource manager
  CResourceManager::CreateSingleton();

  // Game manager
  game::CGameManager* pGameManager = game::CGameManager::CreateSingleton();
  // Collisions manager
  collision::CCollisionManager* pCollManager = collision::CCollisionManager::CreateSingleton();
  // Physics manager
  physics::CPhysicsManager* pPhysicsManager = physics::CPhysicsManager::CreateSingleton();
  // Input manager
  input::CInputManager* pInputManager = input::CInputManager::CreateSingleton();

  // Create directional light
  game::CEntity* pDirectionalLight = pGameManager->CreateEntity("Directional Light");
  pDirectionalLight->RegisterComponent<game::CLightComponent>(render::lights::ELightType::DIRECTIONAL_LIGHT);

  std::vector<game::CEntity*> lstModels = {};
  for (uint32_t uIndex = 0; uIndex < 5; uIndex++)
  {
    // FBX Test
    game::CEntity* pModelEnt = pGameManager->CreateEntity("Model");
    pModelEnt->SetPosition(math::CVector3(GenerateFloat(-100.0f, 100.0f), GenerateFloat(10.0f, 40.0f), GenerateFloat(-20.0f, 20.0f)));
    game::CModelComponent* pModelTest = pModelEnt->RegisterComponent<game::CModelComponent>();
    pModelTest->LoadModel("models/spaceship/fbx/spaceship.fbx");
    pModelEnt->SetRotation(math::CVector3(90.0f, 0.0f, 0.0f));

    lstModels.emplace_back(pModelEnt);
  }

  // OBJ test
  game::CEntity* pModelEnt2 = pGameManager->CreateEntity("Model");
  pModelEnt2->SetPosition(math::CVector3(0.0f, 5.0f, 0.0f));
  game::CModelComponent* pModelTest2 = pModelEnt2->RegisterComponent<game::CModelComponent>();
  pModelTest2->LoadModel("models/airplane/11805_airplane_v2_L2.obj");
  pModelEnt2->SetRotation(math::CVector3(90.0f, 180.0f, 0.0f));
  pModelEnt2->SetScale(math::CVector3(0.01f, 0.01f, 0.01f));

  // Sphere collider
  for (uint32_t uIndex = 0; uIndex < 1; uIndex++)
  {
    game::CEntity* pCapsuleEntity = pGameManager->CreateEntity("Capsule");
    pCapsuleEntity->SetPosition(math::CVector3(0.0f, 10.0f, 0.0f));
    game::CModelComponent* pModelCompTest = pCapsuleEntity->RegisterComponent<game::CModelComponent>();
    pModelCompTest->CreatePrimitive(render::gfx::EPrimitiveType::E3D_CAPSULE, render::ERenderMode::WIREFRAME);
    pModelCompTest->SetColor(math::CVector3::Forward);
    pCapsuleEntity->RegisterComponent<game::CCollisionComponent>(collision::EColliderType::CAPSULE_COLLIDER);
    pCapsuleEntity->RegisterComponent<game::CRigidbodyComponent>();
  }

  game::CEntity* pPlaneEntity = pGameManager->CreateEntity("Plane");
  game::CModelComponent* pPlaneModel = pPlaneEntity->RegisterComponent<game::CModelComponent>();
  pPlaneModel->CreatePrimitive(render::gfx::EPrimitiveType::E3D_PLANE, render::ERenderMode::SOLID);
  pPlaneModel->SetColor(math::CVector3(0.5f, 0.5f, 0.5f));
  pPlaneEntity->SetScale(math::CVector3(200.0f, 1.0f, 200.0f));
  game::CCollisionComponent* pCollisionComponent = pPlaneEntity->RegisterComponent<game::CCollisionComponent>();
  pCollisionComponent->CreateCollider(collision::EColliderType::BOX_COLLIDER);
  collision::CBoxCollider* pBoxCollider = static_cast<collision::CBoxCollider*>(pCollisionComponent->GetCollider());
  pBoxCollider->SetSize(math::CVector3(200.0f, 0.0f, 200.0f));

  for (uint32_t uIndex = 0; uIndex < 3; uIndex++)
  {
    game::CEntity* pBoxTest = pGameManager->CreateEntity("Box");
    pBoxTest->SetPosition(math::CVector3(GenerateFloat(-70.0f, 70.0f), GenerateFloat(1.0f, 2.0f), GenerateFloat(-40.0f, 40.0f)));
    game::CModelComponent* pModelCompTest = pBoxTest->RegisterComponent<game::CModelComponent>();
    pModelCompTest->CreatePrimitive(render::gfx::EPrimitiveType::E3D_CUBE, render::ERenderMode::SOLID);
    pModelCompTest->SetColor(math::CVector3(1.0f, 1.0f, 1.0f));
    pBoxTest->RegisterComponent<game::CCollisionComponent>(collision::EColliderType::BOX_COLLIDER);
    pBoxTest->RegisterComponent<game::CRigidbodyComponent>();
  }

  render::CRender* const pRender = pEngine->GetRender();
  render::CCamera* const pCamera = pEngine->GetCamera();
  pRender->ShowRenderWindow(true);

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
      // Push begin draw
      pEngine->PushBeginDraw();

      // Calculate delta
      pTimeManager->BeginFrame();
      float fDeltaTime = pTimeManager->GetDeltaTime();
      float fFixedDelta = pTimeManager->GetFixedDelta();
      float fOffset = math::Clamp(fDeltaTime, 0.0f, pTimeManager->GetMaxFixedDelta());
      m_fFixedDeltaAccumulator += fOffset;

      // Update
      while (m_fFixedDeltaAccumulator >= fFixedDelta)
      {
        pCamera->Update(fFixedDelta);
        pCamera->DrawDebug();

        pPhysicsManager->Update(fFixedDelta);
        pCollManager->Update(fFixedDelta);
        pGameManager->Update(fFixedDelta);

        if (bThrowRay)
        {
          // Draw line
          math::CVector3 v3Pos = math::CVector3(0.0f, 10.0f, 0.0f);
          math::CVector3 v3End = v3Pos + (math::CVector3::Forward * 100.0f);
          pEngine->DrawLine(v3Pos, v3End, math::CVector3::Right);

          const float fMaxDistance(100);
          physics::CRay oRay(v3Pos, math::CVector3::Forward);
          oRay.DrawRay(fMaxDistance, math::CVector3::Right);

          // Throw ray
          std::vector<collision::SHitEvent> lstHits;
          if (pCollManager->RaycastAll(oRay, fMaxDistance, lstHits))
          {
            for (auto& HitEvent : lstHits)
            {
              pEngine->DrawSphere(HitEvent.ImpactPoint, 0.05f, 8, 8, math::CVector3::Up, render::ERenderMode::WIREFRAME);
            }
          }
        }

        pInputManager->Flush();
        m_fFixedDeltaAccumulator -= fFixedDelta;
      }

      // Draw
      pEngine->PushDraw();

      if (ImGui::Button("Show wireframe"))
      {
        pEngine->GetRender()->SetFillMode(D3D11_FILL_WIREFRAME);
      }
      if (ImGui::Button("Show lit"))
      {
        pEngine->GetRender()->SetFillMode(D3D11_FILL_SOLID);
      }

      ImGui::Begin("TEST - CULLING");
      if (ImGui::Button("Enabled"))
      {
        static bool bState = false;
        bState = !bState;

        for (auto& pEntity : lstModels)
        {
          auto* pComponent = pEntity->GetComponent<game::CModelComponent>();
          pComponent->SetCullingEnabled(bState);
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
      if (ImGui::Button("999"))
      {
        pTimeManager->SetMaxFPS(999);
      }
      ImGui::End();

      ImGui::Begin("TEST - RAYCAST");
      if (ImGui::Button("Enabled"))
      {
        bThrowRay = !bThrowRay;
      }
      ImGui::End();

      ImGui::Begin("Create Lights");
      if (ImGui::Button("Create point light"))
      {
        // Create point light
        game::CEntity* pPointLight = pGameManager->CreateEntity("Point Light");
        pPointLight->RegisterComponent<game::CLightComponent>(render::lights::ELightType::POINT_LIGHT);
      }
      if (ImGui::Button("Create spot light"))
      {
        // Create spot light
        game::CEntity* pSpotLight = pGameManager->CreateEntity("Spot Light");
        pSpotLight->RegisterComponent<game::CLightComponent>(render::lights::ELightType::SPOT_LIGHT);
      }
      ImGui::End();

      // End draw
      pEngine->PushEndDraw();
      pTimeManager->EndFrame();
    }
  }

  global::mem::s_oMemoryTracker.PrintStats();

  // Destroy
  pGameManager->DestroySingleton();
  pCollManager->DestroySingleton();
  pTimeManager->DestroySingleton();
  pInputManager->DestroySingleton();
  pEngine->DestroySingleton();
  CResourceManager::GetInstance()->DestroySingleton();

  global::mem::s_oMemoryTracker.PrintStats();
}