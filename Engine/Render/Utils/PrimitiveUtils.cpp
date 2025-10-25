#include "PrimitiveUtils.h"
#include "Libs/Math/Math.h"
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace gfx
  {
    // Triangle Primitive
    const std::vector<render::gfx::SVertexData> CPrimitiveUtils::s_oTrianglePrimitive =
    {
      { math::CVector3(0.0f, 0.5f, 0.0f), math::CVector3(0.0f, 0.0f, 1.0f) },
      { math::CVector3(0.5f, -0.5f,  0.0f), math::CVector3(0.0f, 0.0f, 1.0f) },
      { math::CVector3(-0.5f, -0.5f,  0.0f), math::CVector3(0.0f, 0.0f, 1.0f) },
    };

    // Cube Primitive
    const std::vector<render::gfx::SVertexData> CPrimitiveUtils::s_oCubePrimitive =
    {
      // FRONT
     { math::CVector3(-0.5f, -0.5f,  0.5f), math::CVector3(0.0f,  0.0f,  1.0f) },
     { math::CVector3(-0.5f,  0.5f,  0.5f), math::CVector3(0.0f,  0.0f,  1.0f) },
     { math::CVector3(0.5f,  0.5f,  0.5f), math::CVector3(0.0f,  0.0f,  1.0f) },
     { math::CVector3(0.5f, -0.5f,  0.5f), math::CVector3(0.0f,  0.0f,  1.0f) },

     // BACK
     { math::CVector3(0.5f, -0.5f, -0.5f), math::CVector3(0.0f,  0.0f, -1.0f) },
     { math::CVector3(0.5f,  0.5f, -0.5f), math::CVector3(0.0f,  0.0f, -1.0f) },
     { math::CVector3(-0.5f,  0.5f, -0.5f), math::CVector3(0.0f,  0.0f, -1.0f) },
     { math::CVector3(-0.5f, -0.5f, -0.5f), math::CVector3(0.0f,  0.0f, -1.0f) },

     // LEFT
     { math::CVector3(-0.5f, -0.5f, -0.5f), math::CVector3(-1.0f,  0.0f,  0.0f) },
     { math::CVector3(-0.5f,  0.5f, -0.5f), math::CVector3(-1.0f,  0.0f,  0.0f) },
     { math::CVector3(-0.5f,  0.5f,  0.5f), math::CVector3(-1.0f,  0.0f,  0.0f) },
     { math::CVector3(-0.5f, -0.5f,  0.5f), math::CVector3(-1.0f,  0.0f,  0.0f) },

     // RIGHT
     { math::CVector3(0.5f, -0.5f,  0.5f), math::CVector3(1.0f,  0.0f,  0.0f) },
     { math::CVector3(0.5f,  0.5f,  0.5f), math::CVector3(1.0f,  0.0f,  0.0f) },
     { math::CVector3(0.5f,  0.5f, -0.5f), math::CVector3(1.0f,  0.0f,  0.0f) },
     { math::CVector3(0.5f, -0.5f, -0.5f), math::CVector3(1.0f,  0.0f,  0.0f) },

     // UP
     { math::CVector3(-0.5f,  0.5f,  0.5f), math::CVector3(0.0f,  1.0f,  0.0f) },
     { math::CVector3(-0.5f,  0.5f, -0.5f), math::CVector3(0.0f,  1.0f,  0.0f) },
     { math::CVector3(0.5f,  0.5f, -0.5f), math::CVector3(0.0f,  1.0f,  0.0f) },
     { math::CVector3(0.5f,  0.5f,  0.5f), math::CVector3(0.0f,  1.0f,  0.0f) },

     // BOTTOM
     { math::CVector3(-0.5f, -0.5f, -0.5f), math::CVector3(0.0f, -1.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(-0.5f, -0.5f,  0.5f), math::CVector3(0.0f, -1.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(0.5f, -0.5f,  0.5f), math::CVector3(0.0f, -1.0f,  0.0f), math::CVector3(1,1,1) },
     { math::CVector3(0.5f, -0.5f, -0.5f), math::CVector3(0.0f, -1.0f,  0.0f), math::CVector3(1,1,1) },
    };

    // Plane Primitive
    static const math::CVector3 s_oPlaneNormal(0.0f, 1.0f, 0.0f);
    const std::vector<render::gfx::SVertexData> CPrimitiveUtils::s_oPlanePrimitive =
    {
      { math::CVector3(-0.5f, 0.0f, -0.5f), s_oPlaneNormal },  // Bottom-left
      { math::CVector3(-0.5f, 0.0f,  0.5f), s_oPlaneNormal },  // Top-left
      { math::CVector3(0.5f, 0.0f,  0.5f), s_oPlaneNormal },  // Top-right
      { math::CVector3(0.5f, 0.0f, -0.5f), s_oPlaneNormal }   // Bottom-right
    };

    // 2D Square Indices
    const std::vector<uint32_t> CPrimitiveUtils::s_oSquareIndices =
    {
      0, 1, 2,
      0, 2, 3
    };
    const std::vector<uint32_t> CPrimitiveUtils::s_oSquareWireframeIndices =
    {
      0, 1, // Line 1
      1, 2, // Line 2
      2, 3, // Line 3
      3, 0  // Line 4
    };

    // 2D Triangle Indices
    const std::vector<uint32_t> CPrimitiveUtils::s_oTriangleIndices =
    {
      0, 1, 2
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

    // ------------------------------------
    gfx::SCustomPrimitive CPrimitiveUtils::Create2DCircle
    (
      float /*_fStandardRadius*/,
      int /*_iSubvH*/,
      int /*_iSubvV*/,
      render::ERenderMode /*_eRenderMode*/
    )
    {
      return gfx::SCustomPrimitive();
    }

    // ------------------------------------
    void CPrimitiveUtils::CreateSphere
    (
      float _fRadius, int _iStacks, int _iSlices,
      std::vector<render::gfx::SVertexData>& _vctVertexData_
    )
    {
      // Clear
      _vctVertexData_.clear();

      // Generate vertex data
      for (int uX = 0; uX <= _iStacks; ++uX)
      {
        float fPhi = static_cast<float>(uX) / static_cast<float>(_iStacks) * static_cast<float>(math::s_fPI);
        for (int uJ = 0; uJ <= _iSlices; ++uJ)
        {
          // Conversion
          float fTheta = static_cast<float>(uJ) / static_cast<float>(_iSlices) * static_cast<float>(math::s_fPI * 2.0f);
          float fX = _fRadius * sinf(fPhi) * cosf(fTheta);
          float fY = _fRadius * cosf(fPhi);
          float fZ = _fRadius * sinf(fPhi) * sinf(fTheta);

          // Add vertex
          render::gfx::SVertexData oVertexData = render::gfx::SVertexData();
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

    // ------------------------------------
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

    // ------------------------------------
    gfx::SCustomPrimitive CPrimitiveUtils::CreateCapsule
    (
      float _fRadius,
      float _fHeight,
      int _iSubvH,
      int _iSubvV,
      render::ERenderMode _eRenderMode
    )
    {
      // Create primitve
      gfx::SCustomPrimitive oCustomPrimitive = gfx::SCustomPrimitive();

      // Calculate values
      float fHalfHeight = _fHeight * 0.5f;
      float fDiff = fHalfHeight - _fRadius;

      // Semi sphere lambda
      auto oComputeSemiSphereLamb = [&](bool _bInverse = false)
      {
        for (int uX = 0; uX <= _iSubvH; ++uX)
        {
          float fPhi = static_cast<float>(uX) / static_cast<float>(_iSubvH) * static_cast<float>(math::s_fHalfPI);
          for (int uJ = 0; uJ <= _iSubvV; ++uJ)
          {
            // Conversion
            float fTheta = (static_cast<float>(uJ) / static_cast<float>(_iSubvV)) * (static_cast<float>(math::s_fPI * 2.0f));
            float fX = _fRadius * sinf(fPhi) * cosf(fTheta);
            float fY = _fRadius * cosf(fPhi);
            float fZ = _fRadius * sinf(fPhi) * sinf(fTheta);

            if (_bInverse)
            {
              fY = -fY;
            }

            // Add vertex
            render::gfx::SVertexData oVertexData = render::gfx::SVertexData();
            oVertexData.Position = math::CVector3(fX, fY, fZ);
            oCustomPrimitive.m_vctVertexData.emplace_back(oVertexData);
          }
        }
      };

      // Generate indices lambda in wireframe mode
      auto oGenerateWireIndicesFunc = [&](int _iStacks, int _iSlices, int _iStartIdx)
      {
        for (int uX = 0; uX < _iStacks; ++uX)
        {
          for (int uJ = 0; uJ < _iSlices; ++uJ)
          {
            int iCurrent = _iStartIdx + (uX * (_iSlices + 1)) + uJ;
            int iNext = iCurrent + 1;
            int iAbove = iCurrent + (_iSlices + 1);

            oCustomPrimitive.m_vctIndices.emplace_back(iCurrent);
            oCustomPrimitive.m_vctIndices.emplace_back(iNext);

            oCustomPrimitive.m_vctIndices.emplace_back(iCurrent);
            oCustomPrimitive.m_vctIndices.emplace_back(iAbove);
          }
        }
      };

      // Generate indices lambda in lit mode
      auto oGenerateIndicesFunc = [&](int _iStacks, int _iSlices, int _iStartIdx, bool _bInverseCCW = false)
      {
        for (int uX = 0; uX < _iStacks; ++uX)
        {
          for (int uJ = 0; uJ < _iSlices; ++uJ)
          {
            int iFirst = _iStartIdx + (uX * (_iSlices + 1)) + uJ;
            int iSecond = iFirst + _iSlices + 1;

            oCustomPrimitive.m_vctIndices.emplace_back(iFirst);
            oCustomPrimitive.m_vctIndices.emplace_back(_bInverseCCW ? iSecond : (iFirst + 1));
            oCustomPrimitive.m_vctIndices.emplace_back(_bInverseCCW ? (iFirst + 1) : iSecond);

            oCustomPrimitive.m_vctIndices.emplace_back(iSecond);
            oCustomPrimitive.m_vctIndices.emplace_back(_bInverseCCW ? (iSecond + 1) : (iFirst + 1));
            oCustomPrimitive.m_vctIndices.emplace_back(_bInverseCCW ? (iFirst + 1) : (iSecond + 1));
          }
        }
      };

      // Top semi-sphere
      int iCacheStartIdx = static_cast<int>(oCustomPrimitive.m_vctVertexData.size());
      oComputeSemiSphereLamb();
      for (int iIdx = iCacheStartIdx; iIdx < static_cast<int>(oCustomPrimitive.m_vctVertexData.size()); ++iIdx)
      {
        oCustomPrimitive.m_vctVertexData[iIdx].Position.y += (fDiff >= 0 ? fDiff : 0.0f);
      }
      // Compute top semi-sphere indices
      _eRenderMode == render::ERenderMode::SOLID ? oGenerateIndicesFunc(_iSubvH, _iSubvV, iCacheStartIdx) :
        oGenerateWireIndicesFunc(_iSubvH, _iSubvV, iCacheStartIdx);

      // Body
      if (fDiff >= 0.0f)
      {
        const int s_iMaxBodySubdv = _eRenderMode == render::ERenderMode::SOLID ? _iSubvH : 1;
        iCacheStartIdx = static_cast<int>(oCustomPrimitive.m_vctVertexData.size());
        for (int iX = 0; iX <= s_iMaxBodySubdv; ++iX)
        {
          float fY = math::Lerp(-fHalfHeight + _fRadius, fHalfHeight - _fRadius, static_cast<float>(iX) / s_iMaxBodySubdv);
          for (int iJ = 0; iJ <= _iSubvV; ++iJ)
          {
            float fTheta = (static_cast<float>(iJ) / _iSubvV) * (static_cast<float>(math::s_fPI * 2.0f));
            float fX = _fRadius * cosf(fTheta);
            float fZ = _fRadius * sinf(fTheta);

            render::gfx::SVertexData oVertexData = {};
            oVertexData.Position = math::CVector3(fX, fY, fZ);
            oCustomPrimitive.m_vctVertexData.emplace_back(oVertexData);
          }
        }
        // Compute body indices
        _eRenderMode == render::ERenderMode::SOLID ? oGenerateIndicesFunc(s_iMaxBodySubdv, _iSubvV, iCacheStartIdx, true) :
          oGenerateWireIndicesFunc(s_iMaxBodySubdv, _iSubvV, iCacheStartIdx);
      }

      // Bottom semi-sphere
      iCacheStartIdx = static_cast<int>(oCustomPrimitive.m_vctVertexData.size());
      oComputeSemiSphereLamb(true);
      for (int iIdx = iCacheStartIdx; iIdx < static_cast<int>(oCustomPrimitive.m_vctVertexData.size()); ++iIdx)
      {
        oCustomPrimitive.m_vctVertexData[iIdx].Position.y -= (fDiff >= 0 ? fDiff : 0.0f);
      }

      // Compute bottom semi-sphere indices
      _eRenderMode == render::ERenderMode::SOLID ? oGenerateIndicesFunc(_iSubvH, _iSubvV, iCacheStartIdx, true) :
        oGenerateWireIndicesFunc(_iSubvH, _iSubvV, iCacheStartIdx);

      return oCustomPrimitive;
    }

    // ------------------------------------
    gfx::SCustomPrimitive CPrimitiveUtils::CreatePlane(const math::CPlane& _oPlane, render::ERenderMode _eRenderMode)
    {
      // Create primitive
      gfx::SCustomPrimitive oCustomPrimitive;
      oCustomPrimitive.m_vctVertexData = s_oPlanePrimitive;
      oCustomPrimitive.m_vctIndices = _eRenderMode == render::ERenderMode::SOLID ? s_oPlaneIndices : s_oWireframePlaneIndices;

      // Invert indices
      const math::CVector3& v3Normal = _oPlane.GetNormal();
      if (v3Normal.Dot(s_oPlaneNormal) < 0.0f)
      {
        size_t iSize = _eRenderMode == render::ERenderMode::SOLID ? 3 : 2;
        for (size_t i = 0; i < s_oPlaneIndices.size(); i += iSize)
        {
          std::swap(oCustomPrimitive.m_vctIndices[i], oCustomPrimitive.m_vctIndices[i + 2]);
        }
      }

      // Calculate rotation
      math::CMatrix4x4 mRot = math::CMatrix4x4::Identity;
      float fDot = math::CVector3::Dot(v3Normal, s_oPlaneNormal);
      if (std::abs(fDot) > (1.0f - math::s_fEpsilon3))
      {
        // Rotate 180 degrees
        if (fDot < 0.0f)
        {
          mRot = math::CMatrix4x4::Rotation(math::CVector3(0.0f, -180.0f, 0.0f));
        }
      }
      else
      {
        math::CVector3 v3Dir = math::CVector3::Cross(v3Normal, s_oPlaneNormal);
        float fAngle = math::CVector3::AngleBetween(s_oPlaneNormal, v3Normal);
        mRot = math::CMatrix4x4::RotationAxis(v3Dir, fAngle);
      }

      // Rotate vertices
      for (auto& oVertexData : oCustomPrimitive.m_vctVertexData)
      {
        oVertexData.Position = mRot * oVertexData.Position;
        oVertexData.Normal = v3Normal;
      }

      return oCustomPrimitive;
    }

    // ------------------------------------
    gfx::SCustomPrimitive CPrimitiveUtils::CreateLine(const math::CVector3& _v3Origin, const math::CVector3& _v3Dest)
    {
      // Create primitive
      gfx::SCustomPrimitive oPrimitive = gfx::SCustomPrimitive();

      // Fill data 
      render::gfx::SVertexData oVertexData = render::gfx::SVertexData();
      oVertexData.Position = _v3Origin;

      uint32_t uIndex = 0;
      oPrimitive.m_vctVertexData.emplace_back(oVertexData);
      oPrimitive.m_vctIndices.emplace_back(uIndex++);

      // Fill data 
      oVertexData.Position = _v3Dest;
      oPrimitive.m_vctVertexData.emplace_back(oVertexData);
      oPrimitive.m_vctIndices.emplace_back(uIndex);

      return oPrimitive;
    }

    // ------------------------------------
    void CPrimitiveUtils::ComputeNormals(std::vector<SVertexData>& _oVertexData, const std::vector<uint32_t>& _vctIndices)
    {
      // Reset normals
      for (auto& oVertex : _oVertexData)
      {
        oVertex.Normal = math::CVector3::Zero;
      }

      // Calculate normals
      long lSize = static_cast<long>(_vctIndices.size());
      for (long lIdx = 0; lIdx < lSize; lIdx += 3)
      {
        if (lIdx + 1 >= lSize || lIdx + 2 >= lSize)
        {
          break;
        }

        int i0 = _vctIndices[lIdx];
        int i1 = _vctIndices[lIdx + 1];
        int i2 = _vctIndices[lIdx + 2];

        const math::CVector3& v0 = _oVertexData[i0].Position;
        const math::CVector3& v1 = _oVertexData[i1].Position;
        const math::CVector3& v2 = _oVertexData[i2].Position;

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

    // ------------------------------------
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