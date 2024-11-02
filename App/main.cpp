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

  std::random_device rd;
  std::mt19937 gen(rd()); // Generador Mersenne Twister
  std::uniform_real_distribution<float> distr(0.1f, 0.5f); // Rango: 1.0f a 10.0f
  // Generar un número aleatorio de tipo float

  std::vector<render::primitive::CPrimitive::SPrimitiveInfo> vctVertexData =
  {
    // Primer triángulo
    { maths::CVector3(-0.1f,  0.1f,  0.0f)}, // Esquina superior izquierda
    { maths::CVector3(0.1f, -0.1f,  0.0f)}, // Esquina superior izquierda
    { maths::CVector3(-0.1f, -0.1f,  0.0f)}, // Esquina superior izquierda
    // Segundo triángulo
    { maths::CVector3(-0.1f, 0.1f,  0.0f)}, // Esquina superior izquierda
    { maths::CVector3(0.1f, 0.1f,  0.0f)}, // Esquina superior izquierda
    { maths::CVector3(0.1f, -0.1f,  0.0f)}, // Esquina superior izquierda
  };
  render::primitive::CPrimitive* pPrimitive = pEngine->CreatePrimitive(vctVertexData);
  pPrimitive->SetColor(maths::CVector3(1.0f, 0.0f, 0.0f));

  pEngine->UpdateEngine();
  pEngine->DestroySingleton();
}