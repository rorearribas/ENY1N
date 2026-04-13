#include <iostream>
#include <random>

#include "Engine/Engine.h"
#include "Libs/Math/Vector2.h"
#include "Libs/Math/Vector3.h"
#include "Libs/Macros/GlobalMacros.h"

#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Lighting/DirectionalLight.h"
#include "Engine/Scenes/SceneManager.h"

#include "Libs/Time/TimeManager.h"
#include "Libs/ImGui/imgui_internal.h"

#include "Engine/Managers/ResourceManager.h"
#include "Engine/Managers/InputManager.h"
#include "Engine/Collisions/CollisionManager.h"
#include "Engine/Collisions/BoxCollider.h"
#include "Engine/Physics/PhysicsManager.h"
#include "Engine/Render/RenderTypes.h"

#include "Game/GameManager/GameManager.h"
#include "Game/Entity/Components/ModelComponent/ModelComponent.h"
#include "Game/Entity/Components/LightComponent/LightComponent.h"
#include "Game/Entity/Components/CollisionComponent/CollisionComponent.h"
#include "Game/Entity/Components/RigidbodyComponent/RigidbodyComponent.h"

#include "Libs/ImGui/imgui.h"
#include "Libs/Math/Math.h"

#include "Reflection/TypeManager.h"
#include "Reflection/ReflectionMacros.h"
#include "Reflection/Base/TypeResolver.h"

static std::random_device rd;
std::mt19937 s_oGenerator(rd());

float GenerateFloat(float min, float max)
{
  std::uniform_real_distribution<float> dist(min, max);
  return dist(s_oGenerator);
}

const std::string& GenerateString(const std::vector<std::string>& lstStrings)
{
  if (lstStrings.empty())
  {
    static const std::string s_sEmpty = "";
    return s_sEmpty;
  }
  std::uniform_int_distribution<size_t> oDistribution(0, lstStrings.size() - 1);
  size_t uRandomIndex = oDistribution(s_oGenerator);
  return lstStrings[uRandomIndex];
}

static bool bThrowRay = false;

#define WIDTH 1920
#define HEIGHT 1080

int main()
{
  global::mem::s_oMemoryTracker.PrintStats();

  // Init
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->Init(WIDTH, HEIGHT);
  pEngine->GetCamera()->SetPos(math::CVector3(0.0f, 10.0f, -10.0f));

  global::mem::s_oMemoryTracker.PrintStats();

  // Time manager
  chrono::CTimeManager* pTimeManager = chrono::CTimeManager::CreateSingleton();
  pTimeManager->SetTargetFramerate(144);

  // Create resource manager
  CResourceManager::CreateSingleton();

  // Game manager
  game::CGameManager* pGameManager = game::CGameManager::CreateSingleton();
  // Collisions manager
  collision::CCollisionManager* pCollisionManager = collision::CCollisionManager::CreateSingleton();
  // Physics manager
  physics::CPhysicsManager* pPhysicsManager = physics::CPhysicsManager::CreateSingleton();
  // Input manager
  input::CInputManager* pInputManager = input::CInputManager::CreateSingleton();

  // Create directional light
  game::CEntity* pDirectionalLight = pGameManager->CreateEntity("Directional Light");
  pDirectionalLight->RegisterComponent<game::CLightComponent>(render::ELight::DIRECTIONAL_LIGHT);

  game::CEntity* pSpotLight = pGameManager->CreateEntity("SpotLight");
  game::CLightComponent* pLightComp = pSpotLight->RegisterComponent<game::CLightComponent>(render::ELight::POINT_LIGHT);
  static_cast<render::lights::CPointLight*>(pLightComp->GetLight())->SetRange(100.0f);
  static_cast<render::lights::CPointLight*>(pLightComp->GetLight())->SetIntensity(0.1f);
  static_cast<render::lights::CPointLight*>(pLightComp->GetLight())->SetColor(math::CVector3(0.0f, 1.0f, 0.0f));

  float fOffsetZ = 0.0f;
  for (uint32_t uIndex = 0; uIndex < 5; uIndex++)
  {
    game::CEntity* pModelEnt = pGameManager->CreateEntity("Model");
    pModelEnt->SetPos(math::CVector3(0.0f, 10.0f, fOffsetZ));
    game::CModelComponent* pModelTest = pModelEnt->RegisterComponent<game::CModelComponent>();
    pModelTest->LoadModel("models/spaceship/spaceship.fbx");
    fOffsetZ += 10;
  }

  // Floor
  game::CEntity* pFloor = pGameManager->CreateEntity("Floor");
  pFloor->SetPos(math::CVector3(0.0f, 0.0f, 0.0f));
  game::CModelComponent* pModelTest2 = pFloor->RegisterComponent<game::CModelComponent>();
  pModelTest2->LoadModel("models/floor/floor.fbx");
  pFloor->SetScl(math::CVector3(50.0f, 50.0f, 50.0f));

  // Create primitives
  const uint32_t uSize = 3;
  render::EPrimitive ePrimitiveTypes[uSize] =
  {
    render::EPrimitive::E3D_CUBE,
    render::EPrimitive::E3D_SPHERE,
    render::EPrimitive::E3D_CAPSULE
  };
  collision::EColliderType eColliderTypes[uSize] =
  {
    collision::EColliderType::BOX_COLLIDER,
    collision::EColliderType::SPHERE_COLLIDER,
    collision::EColliderType::CAPSULE_COLLIDER
  };

  for (uint32_t uIndex = 0; uIndex < uSize; uIndex++)
  {
    game::CEntity* pBoxTest = pGameManager->CreateEntity("Primitive");
    pBoxTest->SetPos(math::CVector3(GenerateFloat(-10.0f, 10.0f), GenerateFloat(5.0f, 10.0f), GenerateFloat(-10.0f, 10.0f)));
    game::CModelComponent* pModelCompTest = pBoxTest->RegisterComponent<game::CModelComponent>();
    pModelCompTest->CreatePrimitive(ePrimitiveTypes[uIndex], render::ERenderMode::SOLID);
    pModelCompTest->SetColor(math::CVector3(0.75f, 0.0f, 0.0f));
    pBoxTest->RegisterComponent<game::CCollisionComponent>(eColliderTypes[uIndex]);
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
      pEngine->PrepareFrame();

      // Calculate delta
      pTimeManager->BeginFrame();
      float fDeltaTime = pTimeManager->GetDeltaTime();
      float fFixedDelta = pTimeManager->GetFixedDelta();
      float fOffset = math::Clamp(fDeltaTime, 0.0f, pTimeManager->GetMaxFixedDelta());
      m_fFixedDeltaAccumulator += fOffset;

      if (bThrowRay)
      {
        // Draw line
        math::CVector3 v3Pos = math::CVector3(0.0f, 10.0f, 0.0f);
        const float fMaxDistance(100);

        physics::CRay oRay(v3Pos, math::CVector3::Forward);
        oRay.DrawRay(fMaxDistance, math::CVector3::Right);

        // Throw ray
        std::vector<collision::THitEvent> lstHits;
        if (pCollisionManager->RaycastAll(oRay, fMaxDistance, lstHits))
        {
          for (auto& HitEvent : lstHits)
          {
            pEngine->DrawSphere(HitEvent.ImpactPoint, 0.05f, 8, 8, math::CVector3::Up, render::ERenderMode::WIREFRAME);
          }
        }
      }

      // Update
      while (m_fFixedDeltaAccumulator >= fFixedDelta)
      {
        pCamera->Update(fFixedDelta);
        pCamera->DrawDebug();

        pPhysicsManager->Update(fFixedDelta);
        pCollisionManager->Update(fFixedDelta);
        pGameManager->Update(fFixedDelta);

        pInputManager->Flush();
        m_fFixedDeltaAccumulator -= fFixedDelta;
      }

      ImGui::Begin("Testing");
      if (ImGui::Button("Enabled Raycast"))
      {
        bThrowRay = !bThrowRay;
      }
      if (ImGui::Button("Add random models (500 units)"))
      {
        for (uint32_t uIndex = 0; uIndex < 500; uIndex++)
        {
          game::CEntity* pModelEnt = pGameManager->CreateEntity("Model");
          pModelEnt->SetPos(math::CVector3(GenerateFloat(-100.0f, 100.0f), GenerateFloat(10.0f, 100.0f), GenerateFloat(-100.0f, 100.0f)));

          //const std::string& sModel = GenerateString(vAvailableModels);
          game::CModelComponent* pModelTest = pModelEnt->RegisterComponent<game::CModelComponent>();
          pModelTest->LoadModel("models/spaceship/spaceship.fbx");
          pModelEnt->SetRot(math::CVector3(90.0f, 0.0f, 0.0f));
        }
      }
      ImGui::End();

      ImGui::Begin("Create Lights");
      if (ImGui::Button("Create point light"))
      {
        // Create point light
        game::CEntity* pTemp = pGameManager->CreateEntity("Point Light");
        pTemp->RegisterComponent<game::CLightComponent>(render::ELight::POINT_LIGHT);
      }
      if (ImGui::Button("Create spot light"))
      {
        // Create spot light
        game::CEntity* pTemp = pGameManager->CreateEntity("Spot Light");
        pTemp->RegisterComponent<game::CLightComponent>(render::ELight::SPOT_LIGHT);
      }
      ImGui::End();

      // Draw
      pEngine->Draw();

      // End frame
      pTimeManager->EndFrame();
    }
  }

  global::mem::s_oMemoryTracker.PrintStats();

  // Destroy
  pGameManager->DestroySingleton();
  pCollisionManager->DestroySingleton();
  pTimeManager->DestroySingleton();
  pInputManager->DestroySingleton();
  pEngine->DestroySingleton();
  CResourceManager::GetInstance()->DestroySingleton();

  global::mem::s_oMemoryTracker.PrintStats();
}