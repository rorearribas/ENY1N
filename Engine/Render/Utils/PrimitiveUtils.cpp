#include "PrimitiveUtils.h"
#include "Libs/Math/Math.h"
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace gfx
  {
    // Triangle Primitive
    const std::vector<render::gfx::TPrimitiveData> CPrimitiveUtils::s_oTrianglePrimitive =
    {
      { math::CVector3(0.0f, 0.5f, 0.0f), math::CVector3(0.0f, 0.0f, 1.0f) },
      { math::CVector3(0.5f, -0.5f,  0.0f), math::CVector3(0.0f, 0.0f, 1.0f) },
      { math::CVector3(-0.5f, -0.5f,  0.0f), math::CVector3(0.0f, 0.0f, 1.0f) },
    };

    // Cube Primitive
    const std::vector<render::gfx::TPrimitiveData> CPrimitiveUtils::s_oCubePrimitive =
    {
      // FRONT
      { math::CVector3(-0.5f, -0.5f, 0.5f),  math::CVector3(0.0f, 0.0f,  1.0f) },
      { math::CVector3(-0.5f, 0.5f, 0.5f),   math::CVector3(0.0f, 0.0f,  1.0f) },
      { math::CVector3(0.5f,  0.5f, 0.5f),   math::CVector3(0.0f, 0.0f,  1.0f) },
      { math::CVector3(0.5f, -0.5f, 0.5f),   math::CVector3(0.0f, 0.0f,  1.0f) },

      // BACK
      { math::CVector3(0.5f, -0.5f,  -0.5f), math::CVector3(0.0f, 0.0f, -1.0f) },
      { math::CVector3(0.5f,  0.5f,  -0.5f), math::CVector3(0.0f, 0.0f, -1.0f) },
      { math::CVector3(-0.5f, 0.5f, -0.5f),  math::CVector3(0.0f, 0.0f, -1.0f) },
      { math::CVector3(-0.5f, -0.5f, -0.5f), math::CVector3(0.0f, 0.0f, -1.0f) },

      // LEFT
      { math::CVector3(-0.5f, -0.5f, -0.5f), math::CVector3(-1.0f, 0.0f, 0.0f) },
      { math::CVector3(-0.5f, 0.5f, -0.5f),  math::CVector3(-1.0f, 0.0f, 0.0f) },
      { math::CVector3(-0.5f, 0.5f, 0.5f),   math::CVector3(-1.0f, 0.0f, 0.0f) },
      { math::CVector3(-0.5f, -0.5f, 0.5f),  math::CVector3(-1.0f, 0.0f, 0.0f) },

      // RIGHT
      { math::CVector3(0.5f, -0.5f,  0.5f),  math::CVector3(1.0f, 0.0f, 0.0f)  },
      { math::CVector3(0.5f,  0.5f,  0.5f),  math::CVector3(1.0f, 0.0f, 0.0f)  },
      { math::CVector3(0.5f,  0.5f, -0.5f),  math::CVector3(1.0f, 0.0f, 0.0f)  },
      { math::CVector3(0.5f, -0.5f, -0.5f),  math::CVector3(1.0f, 0.0f, 0.0f)  },

      // UP
      { math::CVector3(-0.5f, 0.5f,  0.5f),  math::CVector3(0.0f, 1.0f, 0.0f)  },
      { math::CVector3(-0.5f, 0.5f, -0.5f),  math::CVector3(0.0f, 1.0f, 0.0f)  },
      { math::CVector3(0.5f,  0.5f, -0.5f),  math::CVector3(0.0f, 1.0f, 0.0f)  },
      { math::CVector3(0.5f,  0.5f,  0.5f),  math::CVector3(0.0f, 1.0f, 0.0f)  },

      // BOTTOM
      { math::CVector3(-0.5f, -0.5f, -0.5f), math::CVector3(0.0f, -1.0f, 0.0f) },
      { math::CVector3(-0.5f, -0.5f, 0.5f),  math::CVector3(0.0f, -1.0f, 0.0f) },
      { math::CVector3(0.5f, -0.5f, 0.5f),   math::CVector3(0.0f, -1.0f, 0.0f) },
      { math::CVector3(0.5f, -0.5f, -0.5f),  math::CVector3(0.0f, -1.0f, 0.0f) },
    };

    // Plane Primitive
    static const math::CVector3 s_oPlaneNormal(0.0f, 1.0f, 0.0f);
    const std::vector<render::gfx::TPrimitiveData> CPrimitiveUtils::s_oPlanePrimitive =
    {
      { math::CVector3(-0.5f, 0.0f, -0.5f),  s_oPlaneNormal },  // Bottom-left
      { math::CVector3(-0.5f, 0.0f,  0.5f),  s_oPlaneNormal },  // Top-left
      { math::CVector3(0.5f,  0.0f,  0.5f),  s_oPlaneNormal },  // Top-right
      { math::CVector3(0.5f,  0.0f, -0.5f),  s_oPlaneNormal }   // Bottom-right
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
    gfx::TCustomPrimitive CPrimitiveUtils::Create2DCircle
    (
      float /*_fStandardRadius*/,
      int /*_iSubvH*/,
      int /*_iSubvV*/,
      render::ERenderMode /*_eRenderMode*/
    )
    {
      return gfx::TCustomPrimitive();
    }

    // ------------------------------------
    void CPrimitiveUtils::CreateSphere
    (
      float _fRadius, int _iStacks, int _iSlices,
      std::vector<render::gfx::TPrimitiveData>& _lstPrimitiveData_
    )
    {
      // Clear
      _lstPrimitiveData_.clear();

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
          render::gfx::TPrimitiveData rPrimitiveData = render::gfx::TPrimitiveData();
          math::CVector3 v3VertexPos(fX, fY, fZ);

          rPrimitiveData.VertexPos = v3VertexPos;
          _lstPrimitiveData_.emplace_back(rPrimitiveData);
        }
      }
    }

    // Generate sphere indices
    std::vector<uint32_t> CPrimitiveUtils::GetSphereIndices(int _iStacks, int _iSlices)
    {
      std::vector<uint32_t> lstIndices = {};
      for (int i = 0; i < _iStacks; ++i)
      {
        for (int j = 0; j < _iSlices; ++j)
        {
          int iFirst = (i * (_iSlices + 1)) + j;
          int iSecond = iFirst + _iSlices + 1;

          lstIndices.emplace_back(iFirst);
          lstIndices.emplace_back(iSecond);
          lstIndices.emplace_back(iFirst + 1);

          lstIndices.emplace_back(iSecond);
          lstIndices.emplace_back(iSecond + 1);
          lstIndices.emplace_back(iFirst + 1);
        }
      }
      return lstIndices;
    }

    // ------------------------------------
    std::vector<uint32_t> CPrimitiveUtils::GetWireframeSphereIndices(int _iStacks, int _iSlices)
    {
      std::vector<uint32_t> lstWireframeIndices = {};
      for (int iI = 0; iI < _iStacks; ++iI)
      {
        for (int iJ = 0; iJ < _iSlices; ++iJ)
        {
          int iCurrent = (iI * (_iSlices + 1)) + iJ;
          int iNext = iCurrent + 1;
          int iAbove = iCurrent + _iSlices + 1;

          if (iJ < _iSlices)
          {
            lstWireframeIndices.emplace_back(iCurrent);
            lstWireframeIndices.emplace_back(iNext);
          }
          if (iI < _iStacks)
          {
            lstWireframeIndices.emplace_back(iCurrent);
            lstWireframeIndices.emplace_back(iAbove);
          }
        }
      }
      return lstWireframeIndices;
    }

    // ------------------------------------
    gfx::TCustomPrimitive CPrimitiveUtils::CreateCapsule
    (
      float _fRadius,
      float _fHeight,
      int _iSubvH,
      int _iSubvV,
      render::ERenderMode _eRenderMode
    )
    {
      // Create primitve
      gfx::TCustomPrimitive oCustomPrimitive = gfx::TCustomPrimitive();

      // Calculate values
      float fHalfHeight = _fHeight * 0.5f;
      float fDiff = fHalfHeight - _fRadius;

      // Semi sphere lambda
      auto oCalcSemiSphereFunc = [&](bool _bInverse = false)
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
            render::gfx::TPrimitiveData rPrimitiveData = render::gfx::TPrimitiveData();
            rPrimitiveData.VertexPos = math::CVector3(fX, fY, fZ);
            oCustomPrimitive.PrimitiveData.emplace_back(rPrimitiveData);
          }
        }
      };

      // Generate indices lambda in wireframe mode
      auto oCalcWireIndicesFunc = [&](int _iStacks, int _iSlices, int _iStartIdx)
      {
        for (int uX = 0; uX < _iStacks; ++uX)
        {
          for (int uJ = 0; uJ < _iSlices; ++uJ)
          {
            int iCurrent = _iStartIdx + (uX * (_iSlices + 1)) + uJ;
            int iNext = iCurrent + 1;
            int iAbove = iCurrent + (_iSlices + 1);

            oCustomPrimitive.Indices.emplace_back(iCurrent);
            oCustomPrimitive.Indices.emplace_back(iNext);

            oCustomPrimitive.Indices.emplace_back(iCurrent);
            oCustomPrimitive.Indices.emplace_back(iAbove);
          }
        }
      };

      // Generate indices lambda in lit mode
      auto oCalcIndicesFunc = [&](int _iStacks, int _iSlices, int _iStartIdx, bool _bInverseCCW = false)
      {
        for (int uX = 0; uX < _iStacks; ++uX)
        {
          for (int uJ = 0; uJ < _iSlices; ++uJ)
          {
            int iFirst = _iStartIdx + (uX * (_iSlices + 1)) + uJ;
            int iSecond = iFirst + _iSlices + 1;

            oCustomPrimitive.Indices.emplace_back(iFirst);
            oCustomPrimitive.Indices.emplace_back(_bInverseCCW ? iSecond : (iFirst + 1));
            oCustomPrimitive.Indices.emplace_back(_bInverseCCW ? (iFirst + 1) : iSecond);

            oCustomPrimitive.Indices.emplace_back(iSecond);
            oCustomPrimitive.Indices.emplace_back(_bInverseCCW ? (iSecond + 1) : (iFirst + 1));
            oCustomPrimitive.Indices.emplace_back(_bInverseCCW ? (iFirst + 1) : (iSecond + 1));
          }
        }
      };

      // Top semi-sphere
      int iCacheStartIdx = static_cast<int>(oCustomPrimitive.PrimitiveData.size());
      oCalcSemiSphereFunc();
      for (int iIdx = iCacheStartIdx; iIdx < static_cast<int>(oCustomPrimitive.PrimitiveData.size()); ++iIdx)
      {
        oCustomPrimitive.PrimitiveData[iIdx].VertexPos.y += (fDiff >= 0 ? fDiff : 0.0f);
      }
      // Compute top semi-sphere indices
      _eRenderMode == render::ERenderMode::SOLID ? oCalcIndicesFunc(_iSubvH, _iSubvV, iCacheStartIdx) :
        oCalcWireIndicesFunc(_iSubvH, _iSubvV, iCacheStartIdx);

      // Body
      if (fDiff >= 0.0f)
      {
        const int s_iMaxBodySubdv = _eRenderMode == render::ERenderMode::SOLID ? _iSubvH : 1;
        iCacheStartIdx = static_cast<int>(oCustomPrimitive.PrimitiveData.size());
        for (int iX = 0; iX <= s_iMaxBodySubdv; ++iX)
        {
          float fY = math::Lerp(-fHalfHeight + _fRadius, fHalfHeight - _fRadius, static_cast<float>(iX) / s_iMaxBodySubdv);
          for (int iJ = 0; iJ <= _iSubvV; ++iJ)
          {
            float fTheta = (static_cast<float>(iJ) / _iSubvV) * (static_cast<float>(math::s_fPI * 2.0f));
            float fX = _fRadius * cosf(fTheta);
            float fZ = _fRadius * sinf(fTheta);

            render::gfx::TPrimitiveData rPrimitiveData = render::gfx::TPrimitiveData();
            rPrimitiveData.VertexPos = math::CVector3(fX, fY, fZ);
            oCustomPrimitive.PrimitiveData.emplace_back(rPrimitiveData);
          }
        }
        // Compute body indices
        _eRenderMode == render::ERenderMode::SOLID ? oCalcIndicesFunc(s_iMaxBodySubdv, _iSubvV, iCacheStartIdx, true) :
          oCalcWireIndicesFunc(s_iMaxBodySubdv, _iSubvV, iCacheStartIdx);
      }

      // Bottom semi-sphere
      iCacheStartIdx = static_cast<int>(oCustomPrimitive.PrimitiveData.size());
      oCalcSemiSphereFunc(true);
      for (int iIdx = iCacheStartIdx; iIdx < static_cast<int>(oCustomPrimitive.PrimitiveData.size()); ++iIdx)
      {
        oCustomPrimitive.PrimitiveData[iIdx].VertexPos.y -= (fDiff >= 0 ? fDiff : 0.0f);
      }

      // Compute bottom semi-sphere indices
      _eRenderMode == render::ERenderMode::SOLID ? oCalcIndicesFunc(_iSubvH, _iSubvV, iCacheStartIdx, true) :
        oCalcWireIndicesFunc(_iSubvH, _iSubvV, iCacheStartIdx);

      return oCustomPrimitive;
    }

    // ------------------------------------
    gfx::TCustomPrimitive CPrimitiveUtils::CreatePlane(const math::CPlane& _oPlane, render::ERenderMode _eRenderMode)
    {
      // Create primitive
      gfx::TCustomPrimitive oCustomPrimitive;
      oCustomPrimitive.PrimitiveData = s_oPlanePrimitive;
      oCustomPrimitive.Indices = _eRenderMode == render::ERenderMode::SOLID ? s_oPlaneIndices : s_oWireframePlaneIndices;

      // Invert indices
      const math::CVector3& v3Normal = _oPlane.GetNormal();
      if (v3Normal.Dot(s_oPlaneNormal) < math::s_fEpsilon3)
      {
        size_t iSize = _eRenderMode == render::ERenderMode::SOLID ? 3 : 2;
        for (size_t i = 0; i < s_oPlaneIndices.size(); i += iSize)
        {
          std::swap(oCustomPrimitive.Indices[i], oCustomPrimitive.Indices[i + 2]);
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

      // Apply vertex pos
      for (auto& oVertexData : oCustomPrimitive.PrimitiveData)
      {
        oVertexData.VertexPos = mRot * oVertexData.VertexPos;
      }

      return oCustomPrimitive;
    }

    // ------------------------------------
    gfx::TCustomPrimitive CPrimitiveUtils::CreateLine(const math::CVector3& _v3Origin, const math::CVector3& _v3Dest)
    {
      // Create primitive
      gfx::TCustomPrimitive oPrimitive = gfx::TCustomPrimitive();

      // Fill data 
      render::gfx::TPrimitiveData rPrimitiveData = render::gfx::TPrimitiveData();
      rPrimitiveData.VertexPos = _v3Origin;

      uint32_t uIndex = 0;
      oPrimitive.PrimitiveData.emplace_back(rPrimitiveData);
      oPrimitive.Indices.emplace_back(uIndex++);

      // Fill data 
      rPrimitiveData.VertexPos = _v3Dest;
      oPrimitive.PrimitiveData.emplace_back(rPrimitiveData);
      oPrimitive.Indices.emplace_back(uIndex);

      return oPrimitive;
    }
  }
}