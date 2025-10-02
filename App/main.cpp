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

#define WIDTH 1920
#define HEIGHT 1080

int main()
{
  // Create resource manager
  CResourceManager::CreateSingleton();

  // Init
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->Init(WIDTH, HEIGHT);
  pEngine->GetCamera()->SetPosition(math::CVector3(0.0f, 5.0f, -10.0f));

  // Collisions manager
  collision::CCollisionManager* pCollisionManager = collision::CCollisionManager::CreateSingleton();
  // Physics manager
  physics::CPhysicsManager* pPhysicsManager = physics::CPhysicsManager::CreateSingleton();
  // Input manager
  input::CInputManager* pInputManager = input::CInputManager::CreateSingleton();
  // Time manager
  tick::CTimeManager* pTimeManager = tick::CTimeManager::CreateSingleton();
  pTimeManager->SetMaxFPS(144);

  // Game manager
  game::CGameManager* pGameManager = game::CGameManager::CreateSingleton();

  // Create directional light
  game::CEntity* pDirectionalLight = pGameManager->CreateEntity("Directional Light");
  pDirectionalLight->RegisterComponent<game::CLightComponent>(render::lights::ELightType::DIRECTIONAL_LIGHT);

  // Create spot light
  game::CEntity* pSpotLight = pGameManager->CreateEntity("Spot light");
  pSpotLight->RegisterComponent<game::CLightComponent>(render::lights::ELightType::SPOT_LIGHT);
  pSpotLight->SetPosition(math::CVector3(0.0f, 10.0f, 0.0f));

  // Create point light
  game::CEntity* pPointLight = pGameManager->CreateEntity("Point Light");
  pPointLight->RegisterComponent<game::CLightComponent>(render::lights::ELightType::POINT_LIGHT);

  // Load test model
  game::CEntity* pModelEnt = pGameManager->CreateEntity("Model");
  pModelEnt->SetPosition(math::CVector3(0.0f, 10.0f, 0.0f));
  game::CModelComponent* pModelTest = pModelEnt->RegisterComponent<game::CModelComponent>();
  pModelTest->LoadModel("Test/uploads_files_2711294_manhattan.fbx", "");
  pModelTest->SetRotation(math::CVector3(90.0f, 0.0f, 0.0f));

  // Sphere collider
  for (uint32_t uIndex = 0; uIndex < 1; uIndex++)
  {
    game::CEntity* pCapsuleEntity = pGameManager->CreateEntity("Capsule");
    pCapsuleEntity->SetPosition(math::CVector3(0.0f, 10.0f, 0.0f));
    game::CModelComponent* pModelCompTest = pCapsuleEntity->RegisterComponent<game::CModelComponent>();
    pModelCompTest->CreatePrimitive(render::gfx::EPrimitiveType::E3D_CAPSULE, render::ERenderMode::WIREFRAME);
    pModelCompTest->SetPrimitiveColor(math::CVector3::Forward);
    pCapsuleEntity->RegisterComponent<game::CCollisionComponent>(collision::EColliderType::CAPSULE_COLLIDER);
    pCapsuleEntity->RegisterComponent<game::CRigidbodyComponent>();
  }

  game::CEntity* pPlaneEntity = pGameManager->CreateEntity("Plane");
  game::CModelComponent* pPlaneModel = pPlaneEntity->RegisterComponent<game::CModelComponent>();
  pPlaneModel->CreatePrimitive(render::gfx::EPrimitiveType::E3D_PLANE);
  pPlaneModel->SetPrimitiveColor(math::CVector3(1.0f, 1.0f, 1.0f));
  pPlaneEntity->SetScale(math::CVector3(200.0f, 0.0f, 200.0f));
  game::CCollisionComponent* pCollisionComponent = pPlaneEntity->RegisterComponent<game::CCollisionComponent>();
  pCollisionComponent->CreateCollider(collision::EColliderType::BOX_COLLIDER);
  collision::CBoxCollider* pBoxCollider = static_cast<collision::CBoxCollider*>(pCollisionComponent->GetCollider());
  pBoxCollider->SetSize(math::CVector3(200.0f, 0.0f, 200.0f));

  std::vector<game::CEntity*> vctPhysics = {};
  for (uint32_t uIndex = 0; uIndex < 1; uIndex++)
  {
    game::CEntity* pBoxTest = pGameManager->CreateEntity("Box");
    pBoxTest->SetPosition(math::CVector3(1.0f, 5.0f, 0.0f));
    game::CModelComponent* pModelCompTest = pBoxTest->RegisterComponent<game::CModelComponent>();
    pModelCompTest->CreatePrimitive(render::gfx::EPrimitiveType::E3D_CUBE);
    pModelCompTest->SetPrimitiveColor(math::CVector3::Up);
    pBoxTest->RegisterComponent<game::CCollisionComponent>(collision::EColliderType::BOX_COLLIDER);
    pBoxTest->RegisterComponent<game::CRigidbodyComponent>();
  }

  render::CRender* const pRender = pEngine->GetRender();
  render::CCamera* const pCamera = pEngine->GetCamera();
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
      float fOffset = math::Clamp(fDeltaTime, 0.0f, pTimeManager->GetMaxFixedDelta());
      m_fFixedDeltaAccumulator += fOffset;

      math::CPlane oPlane(math::CVector3(0.0f, 5.0f, 0.0f), math::CVector3(0.0f, 1.0f, 0.0f));
      pEngine->DrawPlane(oPlane, math::CVector3(10, 10, 10), math::CVector3::Right, render::ERenderMode::SOLID);

      // Update
      while (m_fFixedDeltaAccumulator >= pTimeManager->GetFixedDelta())
      {
        float fFixedDeltaTime = pTimeManager->GetFixedDelta();

        pCamera->Update(fFixedDeltaTime);
        pCamera->DrawDebug();

        pPhysicsManager->Update(fFixedDeltaTime);
        pCollisionManager->Update(fFixedDeltaTime);
        pGameManager->Update(fFixedDeltaTime);

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
          collision::CCollisionManager* pCollManager = collision::CCollisionManager::GetInstance();
          std::vector<collision::SHitEvent> vctHits;
          if (pCollManager->RaycastAll(oRay, fMaxDistance, vctHits))
          {
            for (auto& HitEvent : vctHits)
            {
              pEngine->DrawSphere(HitEvent.ImpactPoint, 0.05f, 8, 8, math::CVector3::Up);
            }
          }
        }

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

      ImGui::Begin("TEST - PHYSICS");
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

      ImGui::Begin("TEST - RAYCAST");
      if (ImGui::Button("Enabled"))
      {
        bThrowRay = !bThrowRay;
      }
      ImGui::End();

      // End draw
      pEngine->PushEndDraw();
      pTimeManager->EndFrame();
    }
  }

  // Destroy
  pGameManager->DestroySingleton();
  pCollisionManager->DestroySingleton();
  pTimeManager->DestroySingleton();
  pInputManager->DestroySingleton();
  pEngine->DestroySingleton();

  global::mem::s_oMemoryTracker.PrintStats();
}