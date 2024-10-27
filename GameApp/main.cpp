#include <iostream>
#include "Engine/Base/Engine.h"
#include "Libs/Maths/Vector2.h"
#include "Libs/Maths/Vector3.h"
#include "Engine/Render/RenderItem/PrimitiveItem.h"

#define WIDTH 1280
#define HEIGHT 720

int main()
{
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->InitEngine(WIDTH, HEIGHT);

  std::vector<float> vctVertexData =
  {
    // Primer triángulo
    -0.5f,  0.5f,  0.0f,  // Esquina superior izquierda
    0.5f, -0.5f,  0.0f,  // Esquina inferior derecha
    -0.5f, -0.5f,  0.0f,  // Esquina inferior izquierda
    // Segundo triángulo
    -0.5f,  0.5f,  0.0f,  // Esquina superior izquierda
    0.5f,  0.5f,  0.0f,  // Esquina superior derecha
    0.5f, -0.5f,  0.0f   // Esquina inferior derecha
  };
  render::items::CPrimitiveItem* pPrimitiveItem = pEngine->CreatePrimitiveItem(vctVertexData);


  pEngine->Update();
  pEngine->DestroySingleton();
}