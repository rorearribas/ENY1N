#pragma once
#include <vector>
#include "Primitive.h"

namespace render
{
  namespace graphics
  {
    class CPrimitiveUtils
    {
    public:
      // Primitive data
      static const std::vector<render::graphics::SVertexData> s_oTrianglePrimitive;
      static const std::vector<render::graphics::SVertexData> s_oCubePrimitive;
      static const std::vector<render::graphics::SVertexData> s_oPlanePrimitive;

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
      static CPrimitive::SCustomPrimitive Create2DCircle(float _fRadius, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode);

      // 3D Line
      static CPrimitive::SCustomPrimitive CreateLine(const math::CVector3& _v3Origin, const math::CVector3& _v3Dest);

      // 3D Plane
      static CPrimitive::SCustomPrimitive CreatePlane(const math::CPlane& _oPlane, render::ERenderMode _eRenderMode);

      // 3D Capsule
      static CPrimitive::SCustomPrimitive CreateCapsule(float _fRadius, float _fHeight, int _iStacks, int _iSlices, render::ERenderMode _eRenderMode);

      // 3D Sphere
      static void CreateSphere(float _fRadius, int _iStacks, int _iSlices, std::vector<render::graphics::SVertexData>& _vctVertexData_);
      static std::vector<uint32_t> GetSphereIndices(int _iStacks, int _iSlices);
      static std::vector<uint32_t> GetWireframeSphereIndices(int _iStacks, int _iSlices);

      // Normals
      static void ComputeNormals(std::vector<SVertexData>& _oVertexData, const std::vector<uint32_t>& _vctIndices);
      static void ComputeBasicNormals(std::vector<SVertexData>& _oVertexData);
    };

  }
}


