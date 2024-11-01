#include <iostream>
#include "Engine/Base/Engine.h"
#include "Libs/Maths/Vector2.h"
#include "Libs/Maths/Vector3.h"
#include "Engine/Render/RenderItem/PrimitiveItem.h"
#include <random>
#include "Engine/Scenes/SceneManager.h"

#define WIDTH 1280
#define HEIGHT 720

int main()
{
  engine::CEngine* pEngine = engine::CEngine::CreateSingleton();
  pEngine->InitEngine(WIDTH, HEIGHT);
  const scene::CSceneManager* pSceneManager = pEngine->GetSceneManager();

  std::random_device rd;
  std::mt19937 gen(rd()); // Generador Mersenne Twister
  std::uniform_real_distribution<float> distr(0.1f, 0.5f); // Rango: 1.0f a 10.0f
  // Generar un número aleatorio de tipo float
  for (UINT32 uIndex = 0; uIndex < 100; uIndex++)
  {
    float fRandomFloat = distr(gen);
    std::vector<float> vctVertexData =
    {
      // Primer triángulo
      -0.1f + fRandomFloat,  0.1f + fRandomFloat,  0.0f,  // Esquina superior izquierda
      0.1f + fRandomFloat, -0.1f + fRandomFloat,  0.0f,  // Esquina inferior derecha
      -0.1f + fRandomFloat, -0.1f + fRandomFloat,  0.0f,  // Esquina inferior izquierda
      // Segundo triángulo
      -0.1f + fRandomFloat, 0.1f + fRandomFloat,  0.0f,  // Esquina superior izquierda
      0.1f + fRandomFloat, 0.1f + fRandomFloat,  0.0f,  // Esquina superior derecha
      0.1f + fRandomFloat, -0.1f + fRandomFloat,  0.0f   // Esquina inferior derecha
    };
    render::items::CPrimitiveItem* pPrimitiveItem = pEngine->CreatePrimitiveItem(vctVertexData);
  }

  pEngine->UpdateEngine();
  pEngine->DestroySingleton();
}