#include "BoundingBox.h"
#include "Engine/Engine.h"

namespace collision
{
  // ------------------------------------
  void CBoundingBox::DrawDebug()
  {
    // Draw cube
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    pEngine->DrawCube(GetCenter(), GetSize(), math::CVector3::Zero, math::CVector3::Right, render::ERenderMode::WIREFRAME);
  }
}