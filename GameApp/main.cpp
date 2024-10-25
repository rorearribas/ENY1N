#include <iostream>
#include "../Engine/Base/Engine.h"
#include "../Engine/Utils/Utils.h"

int main()
{
  engine::CEngine* pEngine = engine::CEngine::GetInstance();
  pEngine->InitEngine(1920, 1080);
}