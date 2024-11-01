#include <iostream>
#include "Engine/Base/Engine.h"
#include "Libs/Maths/Vector2.h"
#include "Libs/Maths/Vector3.h"
#include "Engine/Render/Primitives/Primitive.h"
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

  //std::vector<render::primitive::CPrimitive::SPrimitiveInfo> vctVertexData =
  //{
  //  // Primer triángulo
  //  { maths::CVector3(-0.1f,  0.1f,  0.0f), maths::CVector3(1.0f,  1.0f,  1.0f)}, // Esquina superior izquierda
  //  { maths::CVector3(0.1f, -0.1f,  0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f)}, // Esquina superior izquierda
  //  { maths::CVector3(-0.1f, -0.1f,  0.0f), maths::CVector3(1.0f, 1.0f, 1.0f)}, // Esquina superior izquierda
  //  // Segundo triángulo
  //  { maths::CVector3(-0.1f, 0.1f,  0.0f), maths::CVector3(1.0f,  1.0f,  1.0f)}, // Esquina superior izquierda
  //  { maths::CVector3(0.1f, 0.1f,  0.0f),  maths::CVector3(1.0f,  1.0f,  1.0f)}, // Esquina superior izquierda
  //  { maths::CVector3(0.1f, -0.1f,  0.0f), maths::CVector3(1.0f,  1.0f,  1.0f)}, // Esquina superior izquierda
  //};
  //render::primitive::CPrimitive* pPrimitive = pEngine->CreatePrimitiveItem(vctVertexData);
  //pPrimitive->SetColor(maths::CVector3(1.0f, 0.0f, 0.0f));

  render::primitive::CPrimitive* pPrimitive2 = pEngine->CreatePrimitiveItem(render::primitive::CPrimitive::EPrimitiveType::TRIANGLE);
  pPrimitive2->SetColor(maths::CVector3(0.0f, 1.0f, 0.0f));

  /*render::primitive::CPrimitive* pPrimitive3 = pEngine->CreatePrimitiveItem(render::primitive::CPrimitive::EPrimitiveType::RECTANGLE);
  pPrimitive3->SetColor(maths::CVector3(0.0f, 0.0f, 1.0f));*/

  pEngine->UpdateEngine();
  pEngine->DestroySingleton();
}