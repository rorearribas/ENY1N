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
      static const std::vector<uint32_t> s_oTriangleWireframeIndices;

      // 3D Cube
      static const std::vector<uint32_t> s_oCubeIndices;

      // 3D Plane
      static const std::vector<uint32_t> s_oPlaneIndices;
      static const std::vector<uint32_t> s_oPlaneWireframeIndices;

      // 3D Line
      static void CreateLine(const math::CVector3& _v3Origin, const math::CVector3& _v3Dest, CPrimitive::SCustomPrimitive& _oVertexData_);

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


