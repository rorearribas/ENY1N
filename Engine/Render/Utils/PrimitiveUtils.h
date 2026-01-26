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
      static const std::vector<render::gfx::TPrimitiveData> s_oTrianglePrimitive;
      static const std::vector<render::gfx::TPrimitiveData> s_oSquarePrimitive;
      static const std::vector<render::gfx::TPrimitiveData> s_oCubePrimitive;
      static const std::vector<render::gfx::TPrimitiveData> s_oPlanePrimitive;

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

      // 2D Circle
      static gfx::TCustomPrimitive CreateCircle(float _fRadius, uint32_t _uSegments, render::ERenderMode _eRenderMode);

      // 3D Line
      static gfx::TCustomPrimitive CreateLine(const math::CVector3& _v3Origin, const math::CVector3& _v3Dest);

      // 3D Plane
      static gfx::TCustomPrimitive CreatePlane(const math::CPlane& _oPlane, render::ERenderMode _eRenderMode);

      // 3D Capsule
      static gfx::TCustomPrimitive CreateCapsule(float _fRadius, float _fHeight, uint32_t _uStacks, uint32_t _iulices, render::ERenderMode _eRenderMode);

      // 3D Sphere
      static void CreateSphere(float _fRadius, uint32_t _iStacks, uint32_t _uSlices, std::vector<render::gfx::TPrimitiveData>& _lstPrimitiveData_);

      static std::vector<uint32_t> GetSphereIndices(uint32_t _uSegments, uint32_t _uSlices);
      static std::vector<uint32_t> GetWireframeSphereIndices(uint32_t _uStacks, uint32_t _uSlices);

      static std::vector<uint32_t> GetCircleIndices(uint32_t _uSegments);
      static std::vector<uint32_t> GetWireframeCircleIndices(uint32_t _uSegments);
    };

  }
}


