#pragma once
#include "Engine/Render/RenderTypes.h"
#include "Engine/Utils/Plane.h"
#include <vector>

namespace render
{
  namespace gfx
  {
    class CPrimitiveUtils
    {
    public:
      // Primitive data
      static const std::vector<math::CVector3> s_oTrianglePrimitive;
      static const std::vector<math::CVector3> s_oSquarePrimitive;
      static const std::vector<math::CVector3> s_oCubePrimitive;
      static const std::vector<math::CVector3> s_oPlanePrimitive;

      // 2D Square
      static const std::vector<uint32_t> s_oSquareIndices;
      static const std::vector<uint32_t> s_oSquareWireframeIndices;

      // 2D Triangle
      static const std::vector<uint32_t> s_oTriangleIndices;
      static const std::vector<uint32_t> s_oWireframeTriangleIndices;

      // 3D Cube
      static const std::vector<uint32_t> s_oCubeIndices;
      static const std::vector<uint32_t> s_oWireframeCubeIndices;

      // 3D Plane
      static const std::vector<uint32_t> s_oPlaneIndices;
      static const std::vector<uint32_t> s_oWireframePlaneIndices;

      // Create primitive
      static TPrimitiveData CreatePrimitive(render::EPrimitive _ePrimitiveType, render::ERenderMode _eRenderMode);

      // 2D Circle
      static TPrimitiveData CreateCircle(float _fRadius, uint32_t _uSegments, render::ERenderMode _eRenderMode);

      // 3D Line
      static TPrimitiveData CreateLine(const math::CVector3& _v3Origin, const math::CVector3& _v3Dest);

      // 3D Plane
      static TPrimitiveData CreatePlane(const math::CPlane& _oPlane, render::ERenderMode _eRenderMode);

      // 3D Capsule
      static TPrimitiveData CreateCapsule(float _fRadius, float _fHeight, uint32_t _uStacks, uint32_t _iulices, render::ERenderMode _eRenderMode);

      // 3D Sphere
      static void CreateSphere(float _fRadius, uint32_t _iStacks, uint32_t _uSlices, std::vector<math::CVector3>& _lstPrimitiveData_);

      static std::vector<uint32_t> GetSphereIndices(uint32_t _uSegments, uint32_t _uSlices);
      static std::vector<uint32_t> GetWireframeSphereIndices(uint32_t _uStacks, uint32_t _uSlices);

      static std::vector<uint32_t> GetCircleIndices(uint32_t _uSegments);
      static std::vector<uint32_t> GetWireframeCircleIndices(uint32_t _uSegments);
    };

  }
}


