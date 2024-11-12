#include <iostream>
#include "Engine/Base/Engine.h"
#include "Libs/Maths/Vector2.h"
#include "Libs/Maths/Vector3.h"
#include "Engine/Render/Primitives/Primitive.h"
#include <random>
#include "Engine/Scenes/SceneManager.h"

#define WIDTH 1920
#define HEIGHT 1080

int main()
{
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->InitEngine(WIDTH, HEIGHT);

  pEngine->UpdateEngine();
  pEngine->DestroySingleton();
}