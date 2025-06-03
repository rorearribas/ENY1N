#include <iostream>
#include <random>

#include "Engine/Base/Engine.h"
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
#include "Game/ETT/Components/ModelComponent/ModelComponent.h"
#include "Game/ETT/Components/LightComponent/LightComponent.h"
#include "Game/ETT/Components/CollisionComponent/CollisionComponent.h"
#include "Game/ETT/Components/RigidbodyComponent/RigidbodyComponent.h"

#include "Engine/Collisions/CollisionManager.h"
#include "Engine/Collisions/BoxCollider.h"
#include "Engine/Physics/PhysicsManager.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/Math/Math.h"
#include "Engine/Managers/MemoryTracker.h"

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
  // Init
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->Init(WIDTH, HEIGHT);
  pEngine->GetCamera()->SetPosition(math::CVector3(0.0f, 0.0f, 10.0f));

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

  game::CEntity* pPlaneEntity = pGameManager->CreateEntity("Plane");
  game::CModelComponent* pPlaneModel = pPlaneEntity->RegisterComponent<game::CModelComponent>();
  pPlaneModel->CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType::E3D_PLANE);
  pPlaneModel->SetPrimitiveColor(math::CVector3(1.0f, 1.0f, 1.0f));
  pPlaneEntity->SetScale(math::CVector3(200.0f, 0.0f, 200.0f));
  game::CCollisionComponent* pCollisionComponent = pPlaneEntity->RegisterComponent<game::CCollisionComponent>();
  pCollisionComponent->CreateCollider(collision::EColliderType::BOX_COLLIDER);
  collision::CBoxCollider* pBoxCollider = static_cast<collision::CBoxCollider*>(pCollisionComponent->GetCollider());
  pBoxCollider->SetSize(math::CVector3(200.0f, 0.0f, 200.0f));

  std::vector<game::CEntity*> vctPhysics = {};
  for (uint32_t uIndex = 0; uIndex < 1; uIndex++)
  {
    game::CEntity* pSphereEntity = pGameManager->CreateEntity("Sphere");
    game::CModelComponent* pSphereModel2 = pSphereEntity->RegisterComponent<game::CModelComponent>();
    pSphereModel2->CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType::E3D_SPHERE);

    float fColorX = GenerateFloat(0.01f, 0.99f);
    float fColorY = GenerateFloat(0.01f, 0.99f);
    float fColorZ = GenerateFloat(0.01f, 0.99f);

    pSphereModel2->SetPrimitiveColor(math::CVector3(fColorX, fColorY, fColorZ));
    pSphereEntity->RegisterComponent<game::CCollisionComponent>(collision::EColliderType::SPHERE_COLLIDER);
    pSphereEntity->RegisterComponent<game::CRigidbodyComponent>();

    float fRandomY = GenerateFloat(5.0f, 10.f);
    float fRandomX = GenerateFloat(-10.0f, 10.0f);
    float fRandomZ = GenerateFloat(-0.5f, 0.5f);

    pSphereEntity->SetPosition(math::CVector3(fRandomX, fRandomY, fRandomZ));
    vctPhysics.emplace_back(pSphereEntity);
  }

  for (uint32_t uIndex = 0; uIndex < 40; uIndex++)
  {
    game::CEntity* pBoxTest = pGameManager->CreateEntity("Box");
    pBoxTest->SetPosition(math::CVector3(0.0f, 5.0f, 0.0f));
    game::CModelComponent* pModelCompTest = pBoxTest->RegisterComponent<game::CModelComponent>();
    pModelCompTest->CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType::E3D_CUBE);
    pModelCompTest->SetPrimitiveColor(math::CVector3::Up);
    pBoxTest->RegisterComponent<game::CCollisionComponent>(collision::EColliderType::BOX_COLLIDER);
    pBoxTest->RegisterComponent<game::CRigidbodyComponent>();

    float fRandomY = GenerateFloat(-200.0f, 200.f);
    float fRandomX = GenerateFloat(-100.0f, 100.0f);
    float fRandomZ = GenerateFloat(-10.5f, 10.5f);
    pBoxTest->SetPosition(math::CVector3(fRandomX, fRandomY, fRandomZ));
  }

  render::CRender* const pRender = pEngine->GetRender();
  render::CCamera* const pRenderCamera = pEngine->GetCamera();
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

      // Update
      while (m_fFixedDeltaAccumulator >= pTimeManager->GetFixedDelta())
      {
        float fFixedDeltaTime = pTimeManager->GetFixedDelta();

        pRenderCamera->Update(fFixedDeltaTime);

        pPhysicsManager->Update(fFixedDeltaTime);
        pCollisionManager->Update(fFixedDeltaTime);
        pGameManager->Update(fFixedDeltaTime);

        // Draw line
        math::CVector3 v3Pos = math::CVector3(0.0f, 10.0f, 0.0f);
        math::CVector3 v3End = v3Pos + (math::CVector3::Forward * 100.0f);
        pEngine->DrawLine(v3Pos, v3End, math::CVector3::Right);

        pEngine->DrawPlane(math::CPlane(math::CVector3(0.0f, 20.0f, 0.0f), math::CVector3::Right), 10.0f, math::CVector3::Forward);

        // Throw ray
        collision::CCollisionManager* pCollManager = collision::CCollisionManager::GetInstance();
        collision::SHitEvent oHitEvent = collision::SHitEvent();
        if (pCollManager->Raycast(physics::CRay(v3Pos, math::CVector3::Forward), oHitEvent, 100.0f))
        {
          std::cout << "dist: " << oHitEvent.Distance << std::endl;
          pEngine->DrawSphere(oHitEvent.ImpactPoint, 0.05f, 8, 8, math::CVector3::Right);
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

      ImGui::ShowDemoWindow();

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
}