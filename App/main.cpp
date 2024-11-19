#include <iostream>
#include "Engine/Base/Engine.h"
#include "Libs/Maths/Vector2.h"
#include "Libs/Maths/Vector3.h"
#include "Engine/Render/Primitives/Primitive.h"
#include <random>
#include "Engine/Scenes/SceneManager.h"
#include "Engine/Input/InputManager.h"

#define WIDTH 1920
#define HEIGHT 1080

int main()
{
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->InitEngine(WIDTH, HEIGHT);
  input::CInputManager::CreateSingleton();

  const render::CRender* pRender = pEngine->GetRender();
  const render::CRenderWindow* pRenderWindow = pRender->GetRenderWindow();
  pRenderWindow->SetEnabled(true);

  pEngine->UpdateEngine();
  pEngine->DestroySingleton();
}