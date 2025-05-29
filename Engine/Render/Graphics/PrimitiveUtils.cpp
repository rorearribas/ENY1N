#include "PrimitiveUtils.h"
#include "Libs/Math/Math.h"

namespace render
{
  namespace graphics
  {
    // Triangle Primitive
    const std::vector<render::graphics::SVertexData> CPrimitiveUtils::s_oTrianglePrimitive =
    {
      { math::CVector3(0.0f, 0.5f, 0.0f), math::CVector3(0.0f, 0.0f, 1.0f), math::CVector3(1.0f, 1.0f, 1.0f) },
      { math::CVector3(0.5f, -0.5f,  0.0f), math::CVector3(0.0f, 0.0f, 1.0f), math::CVector3(1.0f, 1.0f, 1.0f) },
      { math::CVector3(-0.5f, -0.5f,  0.0f), math::CVector3(0.0f, 0.0f, 1.0f), math::CVector3(1.0f, 1.0f, 1.0f)},
    };

    // Cube Primitive
    const std::vector<render::graphics::SVertexData> CPrimitiveUtils::s_oCubePrimitive =
    {
      // FRONT
     { math::CVector3(-0.5f, -0.5f,  0.5f), math::CVector3(0.0f,  0.0f,  1.0f), math::CVector3(1,1,1) },
     { math::CVector3(-0.5f,  0.5f,  0.5f), math::CVector3(0.0f,  0.0f,  1.0f), math::CVector3(1,1,1) },
     { math::CVector3(0.5f,  0.5f,  0.5f), math::CVector3(0.0f,  0.0f,  1.0f), math::CVector3(1,1,1) },
     { math::CVector3(0.5f, -0.5f,  0.5f), math::CVector3(0.0f,  0.0f,  1.0f), math::CVector3(1,1,1) },

     // BACK
     { math::CVector3(0.5f, -0.5f, -0.5f), math::CVector3(0.0f,  0.0f, -1.0f), math::CVector3(1,1,1) },
     { math::CVector3(0.5f,  0.5f, -0.5f), math::CVector3(0.0f,  0.0f, -1.0f), math::CVector3(1,1,1) },
     { math::CVector3(-0.5f,  0.5f, -0.5f), math::CVector3(0.0f,  0.0f, -1.0f), math::CVector3(1,1,1) },
     { math::CVector3(-0.5f, -0.5f, -0.5f), math::CVector3(0.0f,  0.0f, -1.0f), math::CVector3(1,1,1) },

     // LEFT
     { math::CVector3(-0.5f, -0.5f, -0.5f), math::CVector3(-1.0f,  0.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(-0.5f,  0.5f, -0.5f), math::CVector3(-1.0f,  0.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(-0.5f,  0.5f,  0.5f), math::CVector3(-1.0f,  0.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(-0.5f, -0.5f,  0.5f), math::CVector3(-1.0f,  0.0f,  0.0f), math::CVector3(1,1,1) },

     // RIGHT
     { math::CVector3(0.5f, -0.5f,  0.5f), math::CVector3(1.0f,  0.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(0.5f,  0.5f,  0.5f), math::CVector3(1.0f,  0.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(0.5f,  0.5f, -0.5f), math::CVector3(1.0f,  0.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(0.5f, -0.5f, -0.5f), math::CVector3(1.0f,  0.0f,  0.0f), math::CVector3(1,1,1) },

     // UP
     { math::CVector3(-0.5f,  0.5f,  0.5f), math::CVector3(0.0f,  1.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(-0.5f,  0.5f, -0.5f), math::CVector3(0.0f,  1.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(0.5f,  0.5f, -0.5f), math::CVector3(0.0f,  1.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(0.5f,  0.5f,  0.5f), math::CVector3(0.0f,  1.0f,  0.0f), math::CVector3(1,1,1) },

     // BOTTOM
     { math::CVector3(-0.5f, -0.5f, -0.5f), math::CVector3(0.0f, -1.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(-0.5f, -0.5f,  0.5f), math::CVector3(0.0f, -1.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(0.5f, -0.5f,  0.5f), math::CVector3(0.0f, -1.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(0.5f, -0.5f, -0.5f), math::CVector3(0.0f, -1.0f,  0.0f), math::CVector3(1,1,1) },
    };

    // Plane Primitive
    const std::vector<render::graphics::SVertexData> CPrimitiveUtils::s_oPlanePrimitive =
    {
      { math::CVector3(-0.5f, 0.0f, -0.5f), math::CVector3(0.0f, 1.0f, 0.0f), math::CVector3(1.0f, 1.0f, 1.0f) },  // Bottom-left
      { math::CVector3(-0.5f, 0.0f,  0.5f), math::CVector3(0.0f, 1.0f, 0.0f), math::CVector3(1.0f, 1.0f, 1.0f) },  // Top-left
      { math::CVector3(0.5f, 0.0f,  0.5f), math::CVector3(0.0f, 1.0f, 0.0f), math::CVector3(1.0f, 1.0f, 1.0f) },  // Top-right
      { math::CVector3(0.5f, 0.0f, -0.5f), math::CVector3(0.0f, 1.0f, 0.0f), math::CVector3(1.0f, 1.0f, 1.0f) }   // Bottom-right
    };

    // 2D Square Indices
    const std::vector<uint32_t> CPrimitiveUtils::s_oSquareIndices =
    {
      0, 1, 2, // FRONT
      0, 2, 3, // FRONT
    };
    const std::vector<uint32_t> CPrimitiveUtils::s_oSquareWireframeIndices =
    {
      0, 1, 1, 2, 2, 3, 3, 0, // FRONT FACE
    };

    // 2D Triangle Indices
    const std::vector<uint32_t> CPrimitiveUtils::s_oTriangleIndices =
    {
      0, 1, 2, // FRONT
    };
    const std::vector<uint32_t> CPrimitiveUtils::s_oWireframeTriangleIndices =
    {
      0, 1, // Line 1
      1, 2, // Line 2
      2, 0  // Line 3
    };

    // 3D Cube Indices
    const std::vector<uint32_t> CPrimitiveUtils::s_oCubeIndices =
    {
      // FRONT
      0, 3, 1,
      1, 3, 2,

      // BACK
      4, 7, 5,
      5, 7, 6,

      // LEFT
      8, 11, 9,
      9, 11, 10,

      // RIGHT
      12, 15, 13,
      13, 15, 14,

      // UP
      16, 19, 17,
      17, 19, 18,

      // BOTTOM
      20, 23, 21,
      21, 23, 22
    };
    const std::vector<uint32_t> CPrimitiveUtils::s_oWireframeCubeIndices =
    {
      // FRONT FACE EDGES
      0, 1, 1, 2, 2, 3, 3, 0,

      // BACK FACE EDGES
      4, 5, 5, 6, 6, 7, 7, 4,

      // LEFT FACE EDGES
      8, 9, 9, 10, 10, 11, 11, 8,

      // RIGHT FACE EDGES
      12, 13, 13, 14, 14, 15, 15, 12,

      // TOP FACE EDGES
      16, 17, 17, 18, 18, 19, 19, 16,

      // BOTTOM FACE EDGES
      20, 21, 21, 22, 22, 23, 23, 20
    };

    // 3D Plane Indices
    const std::vector<uint32_t> CPrimitiveUtils::s_oPlaneIndices =
    {
      0, 1, 2, //FRONT
      0, 2, 3, //FRONT
    };
    const std::vector<uint32_t> CPrimitiveUtils::s_oWireframePlaneIndices =
    {
      0, 1, // TOP EDGE
      1, 2, // RIGHT EDGE
      2, 3, // BOTTOM EDGE
      3, 0  // LEFT EDGE 
    };

    // 3D Line
    void CPrimitiveUtils::CreateLine(const math::CVector3& _v3Origin, const math::CVector3& _v3Dest, CPrimitive::SCustomPrimitive& _oVertexData_)
    {
      // Clean
      _oVertexData_.m_vctVertexData.clear();
      _oVertexData_.m_vctIndices.clear();

      // Fill data 
      render::graphics::SVertexData oOrigin = render::graphics::SVertexData();
      oOrigin.Position = _v3Origin;

      _oVertexData_.m_vctVertexData.emplace_back(oOrigin);
      _oVertexData_.m_vctIndices.emplace_back(0);

      // Fill data 
      render::graphics::SVertexData oDest = render::graphics::SVertexData();
      oDest.Position = _v3Dest;

      _oVertexData_.m_vctVertexData.emplace_back(oDest);
      _oVertexData_.m_vctIndices.emplace_back(1);
    }

    // 3D Sphere
    void CPrimitiveUtils::CreateSphere
    (
      float _fRadius, int _iStacks, int _iSlices,
      std::vector<render::graphics::SVertexData>& _vctVertexData_
    )
    {
      // Generate vertex data
      for (int uX = 0; uX <= _iStacks; ++uX)
      {
        float fPhi = static_cast<float>(uX) / static_cast<float>(_iStacks) * static_cast<float>(math::s_fPI);
        for (int uJ = 0; uJ <= _iSlices; ++uJ)
        {
          // Conversion
          float fTheta = static_cast<float>(uJ) / static_cast<float>(_iSlices) * 2.0f * static_cast<float>(math::s_fPI);
          float fX = _fRadius * sinf(fPhi) * cosf(fTheta);
          float fY = _fRadius * cosf(fPhi);
          float fZ = _fRadius * sinf(fPhi) * sinf(fTheta);

          // Add vertex
          render::graphics::SVertexData oVertexData = render::graphics::SVertexData();
          math::CVector3 v3VertexPos(fX, fY, fZ);

          oVertexData.Position = v3VertexPos;
          oVertexData.Normal = math::CVector3::Normalize(v3VertexPos);
          oVertexData.TexCoord = math::CVector2((static_cast<float>(uJ) / _iSlices), 1.0f - (static_cast<float>(uX) / _iStacks));

          _vctVertexData_.emplace_back(oVertexData);
        }
      }
    }

    // Generate sphere indices
    std::vector<uint32_t> CPrimitiveUtils::GetSphereIndices(int _iStacks, int _iSlices)
    {
      std::vector<uint32_t> vctIndices = {};
      for (int i = 0; i < _iStacks; ++i)
      {
        for (int j = 0; j < _iSlices; ++j)
        {
          int iFirst = (i * (_iSlices + 1)) + j;
          int iSecond = iFirst + _iSlices + 1;

          vctIndices.emplace_back(iFirst);
          vctIndices.emplace_back(iSecond);
          vctIndices.emplace_back(iFirst + 1);

          vctIndices.emplace_back(iSecond);
          vctIndices.emplace_back(iSecond + 1);
          vctIndices.emplace_back(iFirst + 1);
        }
      }
      return vctIndices;
    }

    // Sphere wireframe indices
    std::vector<uint32_t> CPrimitiveUtils::GetWireframeSphereIndices(int _iStacks, int _iSlices)
    {
      std::vector<uint32_t> vctWireframeIndices = {};
      for (int iI = 0; iI < _iStacks; ++iI)
      {
        for (int iJ = 0; iJ < _iSlices; ++iJ)
        {
          int iCurrent = (iI * (_iSlices + 1)) + iJ;
          int iNext = iCurrent + 1;
          int iAbove = iCurrent + _iSlices + 1;

          if (iJ < _iSlices)
          {
            vctWireframeIndices.emplace_back(iCurrent);
            vctWireframeIndices.emplace_back(iNext);
          }
          if (iI < _iStacks)
          {
            vctWireframeIndices.emplace_back(iCurrent);
            vctWireframeIndices.emplace_back(iAbove);
          }
        }
      }
      return vctWireframeIndices;
    }

    // Compute normals
    void CPrimitiveUtils::ComputeNormals(std::vector<SVertexData>& _oVertexData, const std::vector<uint32_t>& _vctIndices)
    {
      // Reset normals
      for (auto& oVertex : _oVertexData)
      {
        oVertex.Normal = math::CVector3::Zero;
      }

      // Calculate normals
      uint32_t uSize = static_cast<uint32_t>(_vctIndices.size());
      for (uint32_t uIndex = 0; uIndex < _vctIndices.size(); uIndex += 3)
      {
        if (uIndex + 1 >= uSize || uIndex + 2 >= uSize)
        {
          break;
        }

        uint32_t i0 = _vctIndices[uIndex];
        uint32_t i1 = _vctIndices[uIndex + 1];
        uint32_t i2 = _vctIndices[uIndex + 2];

        math::CVector3& v0 = _oVertexData[i0].Position;
        math::CVector3& v1 = _oVertexData[i1].Position;
        math::CVector3& v2 = _oVertexData[i2].Position;

        math::CVector3 v3Edge01 = v1 - v0;
        math::CVector3 v3Edge02 = v2 - v0;
        math::CVector3 v3Normal = math::CVector3::Normalize(math::CVector3::Cross(v3Edge01, v3Edge02));

        _oVertexData[i0].Normal += v3Normal;
        _oVertexData[i1].Normal += v3Normal;
        _oVertexData[i2].Normal += v3Normal;
      }

      // Normalize
      for (auto& oVertex : _oVertexData)
      {
        oVertex.Normal = math::CVector3::Normalize(oVertex.Normal);
      }
    }

    void CPrimitiveUtils::ComputeBasicNormals(std::vector<SVertexData>& _oVertexData)
    {
      // Set normals
      for (auto& oVertex : _oVertexData)
      {
        oVertex.Normal = math::CVector3::Normalize(oVertex.Position);
      }
    }
  }
}