#include <iostream>
#include "../Engine/Base/Engine.h"
#include "../Engine/Utils/Utils.h"

#define WIDTH 1280
#define HEIGHT 720

int main()
{
  engine::CEngine* pEngine = engine::CEngine::GetInstance();
  pEngine->InitEngine(WIDTH, HEIGHT);
  pEngine->Update();
}