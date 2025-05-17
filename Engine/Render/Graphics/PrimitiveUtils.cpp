#include "PrimitiveUtils.h"
#include "Libs/Math/Math.h"

namespace render
{
	namespace graphics
	{
    // Triangle Primitive
    const std::vector<render::graphics::SVertexData> CPrimitiveUtils::s_oTrianglePrimitive =
    {
      { math::CVector3(0, 0.5f, 0.0f), math::CVector3::One, math::CVector3::One },
      { math::CVector3(0.5f, -0.5f,  0.0f),  math::CVector3::One, math::CVector3::One },
      { math::CVector3(-0.5f, -0.5f,  0.0f), math::CVector3::One, math::CVector3::One },
    };

    // Cube Primitive
    const std::vector<render::graphics::SVertexData> CPrimitiveUtils::s_oCubePrimitive =
    {
      { math::CVector3(-0.5f, -0.5f, -0.5f), math::CVector3(1.0f, 0.0f, 0.0f), math::CVector3::One },
      { math::CVector3(-0.5f,  0.5f, -0.5f), math::CVector3(0.0f, 1.0f, 0.0f), math::CVector3::One },
      { math::CVector3(0.5f,  0.5f, -0.5f), math::CVector3(0.0f, 0.0f, 1.0f), math::CVector3::One },
      { math::CVector3(0.5f, -0.5f, -0.5f), math::CVector3(1.0f, 1.0f, 1.0f), math::CVector3::One },

      { math::CVector3(-0.5f, -0.5f,  0.5f), math::CVector3(1.0f, 0.0f, 0.0f), math::CVector3::One },
      { math::CVector3(-0.5f,  0.5f,  0.5f), math::CVector3(0.0f, 1.0f, 0.0f), math::CVector3::One },
      { math::CVector3(0.5f,  0.5f,  0.5f), math::CVector3(0.0f, 0.0f, 1.0f), math::CVector3::One},
      { math::CVector3(0.5f, -0.5f,  0.5f), math::CVector3(1.0f, 1.0f, 1.0f), math::CVector3::One}
    };

    // Plane Primitive
    const std::vector<render::graphics::SVertexData> CPrimitiveUtils::s_oPlanePrimitive =
    {
      { math::CVector3(-0.5f, 0.0f, -0.5f), math::CVector3(1.0f, 0.0f, 0.0f), math::CVector3::One },  // Bottom-left
      { math::CVector3(-0.5f, 0.0f,  0.5f), math::CVector3(0.0f, 1.0f, 0.0f), math::CVector3::One },  // Top-left
      { math::CVector3(0.5f, 0.0f,  0.5f), math::CVector3(0.0f, 0.0f, 1.0f), math::CVector3::One },  // Top-right
      { math::CVector3(0.5f, 0.0f, -0.5f), math::CVector3(1.0f, 1.0f, 1.0f), math::CVector3::One }   // Bottom-right
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
    const std::vector<uint32_t> CPrimitiveUtils::s_oTriangleWireframeIndices =
    {
      0, 1, // Line 1
      1, 2, // Line 2
      2, 0  // Line 3
    };

    // 3D Cube Indices
    const std::vector<uint32_t> CPrimitiveUtils::s_oCubeIndices =
    {
      0, 1, 2, 0, 2, 3, //FRONT
      4, 7, 6, 4, 6, 5, //BACK 
      3, 2, 6,3, 6, 7, //RIGHT SIDE
      4, 5, 1, 4, 1, 0, //LEFT SIDE
      1, 5, 6,  1, 6, 2, //TOP
      0, 3, 7, 0, 7, 4 //BOTTOM
    };
    const std::vector<uint32_t> CPrimitiveUtils::s_oCubeWireframeIndices =
    {
      0, 1, 1, 2, 2, 3, 3, 0, // FRONT FACE
      4, 5, 5, 6, 6, 7, 7, 4, // BACK FACE
      0, 4, 1, 5, 2, 6, 3, 7  // CONNECTING EDGES
    };

    // 3D Plane Indices
    const std::vector<uint32_t> CPrimitiveUtils::s_oPlaneIndices =
    {
      0, 1, 2, //FRONT
      0, 2, 3, //FRONT
    };
    const std::vector<uint32_t> CPrimitiveUtils::s_oPlaneWireframeIndices =
    {
      0, 1, // TOP EDGE
      1, 2, // RIGHT EDGE
      2, 3, // BOTTOM EDGE
      3, 0  // LEFT EDGE 
    };

    // 3D Line
    void CPrimitiveUtils::CreateLine(const math::CVector3& _v3Origin, const math::CVector3& _v3Dest, const math::CVector3& _v3Color, CPrimitive::SCustomPrimitive& _oVertexData_)
    { 
      // Clean
      _oVertexData_.m_vctVertexData.clear();
      _oVertexData_.m_vctIndices.clear();

      // Fill data 
      render::graphics::SVertexData oOrigin = render::graphics::SVertexData();
      oOrigin.Position = _v3Origin;
      oOrigin.Color = _v3Color;

      _oVertexData_.m_vctVertexData.emplace_back(oOrigin);
      _oVertexData_.m_vctIndices.emplace_back(0);

      // Fill data 
      render::graphics::SVertexData oDest = render::graphics::SVertexData();
      oDest.Position = _v3Dest;
      oDest.Color = _v3Color;

      _oVertexData_.m_vctVertexData.emplace_back(oDest);
      _oVertexData_.m_vctIndices.emplace_back(1);
    }

    // 3D Sphere
    void CPrimitiveUtils::CreateSphere(float _fRadius, int _iStacks, int _iSlices, std::vector<render::graphics::SVertexData>& _vctPrimitiveData)
    {
      for (int uX = 0; uX <= _iStacks; ++uX)
      {
        float fPhi = static_cast<float>(uX) / static_cast<float>(_iStacks) * static_cast<float>(math::s_fPI);
        for (int uJ = 0; uJ <= _iSlices; ++uJ)
        {
          // Conversion
          float fTheta = static_cast<float>(uJ) / static_cast<float>(_iSlices) * 2.0f * (float)math::s_fPI;
          float fX = _fRadius * sinf(fPhi) * cosf(fTheta);
          float fY = _fRadius * cosf(fPhi);
          float fZ = _fRadius * sinf(fPhi) * sinf(fTheta);

          // Add vertex
          render::graphics::SVertexData oVertexData = render::graphics::SVertexData();
          oVertexData.Position = math::CVector3(fX, fY, fZ);
          _vctPrimitiveData.push_back(oVertexData);
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

          vctIndices.push_back(iFirst);
          vctIndices.push_back(iSecond);
          vctIndices.push_back(iFirst + 1);

          vctIndices.push_back(iSecond);
          vctIndices.push_back(iSecond + 1);
          vctIndices.push_back(iFirst + 1);
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
            vctWireframeIndices.push_back(iCurrent);
            vctWireframeIndices.push_back(iNext);
          }
          if (iI < _iStacks)
          {
            vctWireframeIndices.push_back(iCurrent);
            vctWireframeIndices.push_back(iAbove);
          }
        }
      }
      return vctWireframeIndices;
    }
	}
}