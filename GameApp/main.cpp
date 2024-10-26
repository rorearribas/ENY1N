#include <iostream>
#include "../Engine/Base/Engine.h"
#include "../Libs/Maths/Vector2.h"
#include "../Libs/Maths/Vector3.h"

#define WIDTH 1280
#define HEIGHT 720

int main()
{
  engine::CEngine* pEngine = engine::CEngine::GetInstance();
  pEngine->InitEngine(WIDTH, HEIGHT);
  pEngine->Update();
}