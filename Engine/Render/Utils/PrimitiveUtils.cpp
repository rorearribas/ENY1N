#include "PrimitiveUtils.h"
#include "Libs/Math/Math.h"
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace gfx
  {
    // Triangle Primitive
    const std::vector<math::CVector3> CPrimitiveUtils::s_oTrianglePrimitive =
    {
      { math::CVector3(0.0f, 0.5f, 0.0f),    },
      { math::CVector3(0.5f, -0.5f,  0.0f),  },
      { math::CVector3(-0.5f, -0.5f,  0.0f), },
    };

    // Square primitive
    const std::vector<math::CVector3> CPrimitiveUtils::s_oSquarePrimitive =
    {
      { math::CVector3(-0.5f, -0.5f, 0.5f) },
      { math::CVector3(-0.5f, 0.5f, 0.5f)  },
      { math::CVector3(0.5f,  0.5f, 0.5f)  },
      { math::CVector3(0.5f, -0.5f, 0.5f)  }
    };

    // Cube Primitive
    const std::vector<math::CVector3> CPrimitiveUtils::s_oCubePrimitive =
    {
      // FRONT
      { math::CVector3(-0.5f, -0.5f, 0.5f),  },
      { math::CVector3(-0.5f, 0.5f, 0.5f),   },
      { math::CVector3(0.5f,  0.5f, 0.5f),   },
      { math::CVector3(0.5f, -0.5f, 0.5f),   },

      // BACK
      { math::CVector3(0.5f, -0.5f,  -0.5f), },
      { math::CVector3(0.5f,  0.5f,  -0.5f), },
      { math::CVector3(-0.5f, 0.5f, -0.5f),  },
      { math::CVector3(-0.5f, -0.5f, -0.5f), },

      // LEFT
      { math::CVector3(-0.5f, -0.5f, -0.5f), },
      { math::CVector3(-0.5f, 0.5f, -0.5f),  },
      { math::CVector3(-0.5f, 0.5f, 0.5f),   },
      { math::CVector3(-0.5f, -0.5f, 0.5f),  },

      // RIGHT
      { math::CVector3(0.5f, -0.5f,  0.5f),  },
      { math::CVector3(0.5f,  0.5f,  0.5f),  },
      { math::CVector3(0.5f,  0.5f, -0.5f),  },
      { math::CVector3(0.5f, -0.5f, -0.5f),  },

      // UP
      { math::CVector3(-0.5f, 0.5f,  0.5f),  },
      { math::CVector3(-0.5f, 0.5f, -0.5f),  },
      { math::CVector3(0.5f,  0.5f, -0.5f),  },
      { math::CVector3(0.5f,  0.5f,  0.5f),  },

      // BOTTOM
      { math::CVector3(-0.5f, -0.5f, -0.5f), },
      { math::CVector3(-0.5f, -0.5f, 0.5f),  },
      { math::CVector3(0.5f, -0.5f, 0.5f),   },
      { math::CVector3(0.5f, -0.5f, -0.5f),  },
    };

    // Plane Primitive
    static const math::CVector3 s_oPlaneNormal(0.0f, 1.0f, 0.0f);
    const std::vector<math::CVector3> CPrimitiveUtils::s_oPlanePrimitive =
    {
      { math::CVector3(-0.5f, 0.0f, -0.5f) }, // Bottom-left
      { math::CVector3(-0.5f, 0.0f,  0.5f) }, // Top-left
      { math::CVector3(0.5f,  0.0f,  0.5f) }, // Top-right
      { math::CVector3(0.5f,  0.0f, -0.5f) }  // Bottom-right
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
    TCustomPrimitive CPrimitiveUtils::CreateCircle
    (
      float _fRadius,
      uint32_t _uSegments,
      render::ERenderMode _eRenderMode
    )
    {
      // Create primitive
      TCustomPrimitive rCustomData = TCustomPrimitive();

      if (_eRenderMode == ERenderMode::SOLID)
      {
        // Middle vertex
        rCustomData.Vertices.emplace_back(math::CVector3::Zero);
      }

      for (uint32_t uX = 0; uX <= _uSegments; ++uX)
      {
        float fTheta = static_cast<float>(uX) / static_cast<float>(_uSegments) * (-math::s_fPI * 2.0f);
        float fX = _fRadius * cosf(fTheta);
        float fY = _fRadius * sinf(fTheta);
        rCustomData.Vertices.emplace_back(math::CVector3(fX, fY, 0.0f));
      }

      switch (_eRenderMode)
      {
        case render::ERenderMode::SOLID: rCustomData.Indices = GetCircleIndices(_uSegments); break;
        case render::ERenderMode::WIREFRAME: rCustomData.Indices = GetWireframeCircleIndices(_uSegments); break;
        default: break;
      }

      return rCustomData;
    }

    // ------------------------------------
    TCustomPrimitive CPrimitiveUtils::CreateLine(const math::CVector3& _v3Origin, const math::CVector3& _v3Dest)
    {
      // Create primitive
      TCustomPrimitive rCustomPrimitive = TCustomPrimitive();

      // Fill data 
      uint32_t uIndex = 0;
      rCustomPrimitive.Vertices.emplace_back(_v3Origin);
      rCustomPrimitive.Indices.emplace_back(uIndex++);

      // Fill data 
      rCustomPrimitive.Vertices.emplace_back(_v3Dest);
      rCustomPrimitive.Indices.emplace_back(uIndex);

      return rCustomPrimitive;
    }
    // ------------------------------------
    TCustomPrimitive CPrimitiveUtils::CreatePlane(const math::CPlane& _oPlane, render::ERenderMode _eRenderMode)
    {
      // Create primitive
      TCustomPrimitive rCustomPrimitive = TCustomPrimitive();
      rCustomPrimitive.Vertices = s_oPlanePrimitive;
      rCustomPrimitive.Indices = _eRenderMode == render::ERenderMode::SOLID ? s_oPlaneIndices : s_oWireframePlaneIndices;

      // Invert indices
      const math::CVector3& v3Normal = _oPlane.GetNormal();
      if (v3Normal.Dot(s_oPlaneNormal) < math::s_fEpsilon3)
      {
        size_t iSize = _eRenderMode == render::ERenderMode::SOLID ? 3 : 2;
        for (size_t i = 0; i < s_oPlaneIndices.size(); i += iSize)
        {
          std::swap(rCustomPrimitive.Indices[i], rCustomPrimitive.Indices[i + 2]);
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
          mRot = math::CMatrix4x4::CreateRotation(math::CVector3(0.0f, -180.0f, 0.0f));
        }
      }
      else
      {
        math::CVector3 v3Dir = math::CVector3::Cross(v3Normal, s_oPlaneNormal);
        float fAngle = math::CVector3::AngleBetween(s_oPlaneNormal, v3Normal);
        mRot = math::CMatrix4x4::RotationAxis(v3Dir, fAngle);
      }

      // Apply vertex pos
      for (math::CVector3& v3Vertex : rCustomPrimitive.Vertices)
      {
        v3Vertex = mRot * v3Vertex;
      }

      return rCustomPrimitive;
    }

    // ------------------------------------
    TCustomPrimitive CPrimitiveUtils::CreateCapsule
    (
      float _fRadius,
      float _fHeight,
      uint32_t _uSubvH,
      uint32_t _uSubvV,
      render::ERenderMode _eRenderMode
    )
    {
      // Create primitve
      TCustomPrimitive rCustomPrimitive = TCustomPrimitive();

      // Calculate values
      float fHalfHeight = _fHeight * 0.5f;
      float fDiff = fHalfHeight - _fRadius;

      // Semi sphere lambda
      auto oCalcSemiSphereFunc = [&](bool _bInverse = false)
      {
        for (uint32_t uX = 0; uX <= _uSubvH; ++uX)
        {
          float fPhi = static_cast<float>(uX) / static_cast<float>(_uSubvH) * static_cast<float>(math::s_fHalfPI);
          for (uint32_t uJ = 0; uJ <= _uSubvV; ++uJ)
          {
            // Conversion
            float fTheta = (static_cast<float>(uJ) / static_cast<float>(_uSubvV)) * (static_cast<float>(math::s_fPI * 2.0f));
            float fX = _fRadius * sinf(fPhi) * cosf(fTheta);
            float fY = _fRadius * cosf(fPhi);
            float fZ = _fRadius * sinf(fPhi) * sinf(fTheta);

            if (_bInverse)
            {
              fY = -fY;
            }

            // Add vertex
            rCustomPrimitive.Vertices.emplace_back(math::CVector3(fX, fY, fZ));
          }
        }
      };

      // Generate indices lambda in wireframe mode
      auto oCalcWireIndicesFunc = [&](uint32_t _uStacks, uint32_t _uSlices, int _iBeginIdx)
      {
        for (uint32_t uX = 0; uX < _uStacks; ++uX)
        {
          for (uint32_t uJ = 0; uJ < _uSlices; ++uJ)
          {
            uint32_t uCurrent = _iBeginIdx + (uX * (_uSlices + 1)) + uJ;
            uint32_t uNext = uCurrent + 1;
            uint32_t uAbove = uCurrent + (_uSlices + 1);

            rCustomPrimitive.Indices.emplace_back(uCurrent);
            rCustomPrimitive.Indices.emplace_back(uNext);

            rCustomPrimitive.Indices.emplace_back(uCurrent);
            rCustomPrimitive.Indices.emplace_back(uAbove);
          }
        }
      };

      // Generate indices lambda in lit mode
      auto oCalcIndicesFunc = [&](uint32_t _uStacks, uint32_t _uSlices, uint32_t _uBeginIdx, bool _bInverseCCW = false)
      {
        for (uint32_t uX = 0; uX < _uStacks; ++uX)
        {
          for (uint32_t uJ = 0; uJ < _uSlices; ++uJ)
          {
            uint32_t uFirst = _uBeginIdx + (uX * (_uSlices + 1)) + uJ;
            uint32_t uSecond = uFirst + _uSlices + 1;

            rCustomPrimitive.Indices.emplace_back(uFirst);
            rCustomPrimitive.Indices.emplace_back(_bInverseCCW ? uSecond : (uFirst + 1));
            rCustomPrimitive.Indices.emplace_back(_bInverseCCW ? (uFirst + 1) : uSecond);

            rCustomPrimitive.Indices.emplace_back(uSecond);
            rCustomPrimitive.Indices.emplace_back(_bInverseCCW ? (uSecond + 1) : (uFirst + 1));
            rCustomPrimitive.Indices.emplace_back(_bInverseCCW ? (uFirst + 1) : (uSecond + 1));
          }
        }
      };

      // Top semi-sphere
      uint32_t uBeginIdx = static_cast<uint32_t>(rCustomPrimitive.Vertices.size());
      oCalcSemiSphereFunc();
      for (uint32_t uIdx = uBeginIdx; uIdx < static_cast<uint32_t>(rCustomPrimitive.Vertices.size()); ++uIdx)
      {
        rCustomPrimitive.Vertices[uIdx].y += (fDiff >= 0 ? fDiff : 0.0f);
      }
      // Compute top semi-sphere indices
      _eRenderMode == render::ERenderMode::SOLID ? oCalcIndicesFunc(_uSubvH, _uSubvV, uBeginIdx) :
        oCalcWireIndicesFunc(_uSubvH, _uSubvV, uBeginIdx);

      // Body
      if (fDiff >= 0.0f)
      {
        const uint32_t s_uMaxSubdv = _eRenderMode == render::ERenderMode::SOLID ? _uSubvH : 1;
        uBeginIdx = static_cast<uint32_t>(rCustomPrimitive.Vertices.size());
        for (uint32_t uX = 0; uX <= s_uMaxSubdv; ++uX)
        {
          float fY = math::Lerp((-fHalfHeight + _fRadius), (fHalfHeight - _fRadius), static_cast<float>(uX) / s_uMaxSubdv);
          for (uint32_t uJ = 0; uJ <= _uSubvV; ++uJ)
          {
            float fTheta = (static_cast<float>(uJ) / _uSubvV) * (static_cast<float>(math::s_fPI * 2.0f));
            float fX = _fRadius * cosf(fTheta);
            float fZ = _fRadius * sinf(fTheta);
            rCustomPrimitive.Vertices.emplace_back(math::CVector3(fX, fY, fZ));
          }
        }
        // Compute body indices
        _eRenderMode == render::ERenderMode::SOLID ? oCalcIndicesFunc(s_uMaxSubdv, _uSubvV, uBeginIdx, true) :
          oCalcWireIndicesFunc(s_uMaxSubdv, _uSubvV, uBeginIdx);
      }

      // Bottom semi-sphere
      uBeginIdx = static_cast<int>(rCustomPrimitive.Vertices.size());
      oCalcSemiSphereFunc(true);
      for (int iIdx = uBeginIdx; iIdx < static_cast<int>(rCustomPrimitive.Vertices.size()); ++iIdx)
      {
        rCustomPrimitive.Vertices[iIdx].y -= (fDiff >= 0 ? fDiff : 0.0f);
      }

      // Compute bottom semi-sphere indices
      _eRenderMode == render::ERenderMode::SOLID ? oCalcIndicesFunc(_uSubvH, _uSubvV, uBeginIdx, true) :
        oCalcWireIndicesFunc(_uSubvH, _uSubvV, uBeginIdx);

      return rCustomPrimitive;
    }

    // ------------------------------------
    void CPrimitiveUtils::CreateSphere
    (
      float _fRadius, uint32_t _uStacks, uint32_t _uSlices,
      std::vector<math::CVector3>& _lstPrimitiveData_
    )
    {
      // Clear
      _lstPrimitiveData_.clear();

      // Generate vertex data
      for (uint32_t uX = 0; uX <= _uStacks; ++uX)
      {
        float fPhi = static_cast<float>(uX) / static_cast<float>(_uStacks) * static_cast<float>(math::s_fPI);
        for (uint32_t uJ = 0; uJ <= _uSlices; ++uJ)
        {
          // Conversion
          float fTheta = static_cast<float>(uJ) / static_cast<float>(_uSlices) * static_cast<float>(math::s_fPI * 2.0f);
          float fX = _fRadius * sinf(fPhi) * cosf(fTheta);
          float fY = _fRadius * cosf(fPhi);
          float fZ = _fRadius * sinf(fPhi) * sinf(fTheta);

          // Add vertex
          _lstPrimitiveData_.emplace_back(math::CVector3(fX, fY, fZ));
        }
      }
    }

    // ------------------------------------
    std::vector<uint32_t> CPrimitiveUtils::GetSphereIndices(uint32_t _uStacks, uint32_t _uSlices)
    {
      std::vector<uint32_t> lstIndices = std::vector<uint32_t>();
      for (uint32_t uI = 0; uI < _uStacks; ++uI)
      {
        for (uint32_t uJ = 0; uJ < _uSlices; ++uJ)
        {
          uint32_t uFirst = (uI * (_uSlices + 1)) + uJ;
          uint32_t uSecond = uFirst + _uSlices + 1;

          lstIndices.emplace_back(uFirst);
          lstIndices.emplace_back(uSecond);
          lstIndices.emplace_back(uFirst + 1);

          lstIndices.emplace_back(uSecond);
          lstIndices.emplace_back(uSecond + 1);
          lstIndices.emplace_back(uFirst + 1);
        }
      }
      return lstIndices;
    }

    // ------------------------------------
    std::vector<uint32_t> CPrimitiveUtils::GetWireframeSphereIndices(uint32_t _uStacks, uint32_t _uSlices)
    {
      std::vector<uint32_t> lstWireframeIndices = std::vector<uint32_t>();
      for (uint32_t uI = 0; uI < _uStacks; ++uI)
      {
        for (uint32_t uJ = 0; uJ < _uSlices; ++uJ)
        {
          uint32_t uCurrent = (uI * (_uSlices + 1)) + uJ;
          uint32_t uNext = uCurrent + 1;
          uint32_t uAbove = uCurrent + _uSlices + 1;

          if (uJ < _uSlices)
          {
            lstWireframeIndices.emplace_back(uCurrent);
            lstWireframeIndices.emplace_back(uNext);
          }
          if (uI < _uStacks)
          {
            lstWireframeIndices.emplace_back(uCurrent);
            lstWireframeIndices.emplace_back(uAbove);
          }
        }
      }
      return lstWireframeIndices;
    }

    // ------------------------------------
    std::vector<uint32_t> CPrimitiveUtils::GetCircleIndices(uint32_t _uSegments)
    {
      std::vector<uint32_t> lstIndices = std::vector<uint32_t>();
      for (uint32_t uX = 1; uX <= _uSegments; ++uX)
      {
        lstIndices.push_back(0);
        lstIndices.push_back(uX);
        lstIndices.push_back(uX + 1);
      }
      return lstIndices;
    }

    // ------------------------------------
    std::vector<uint32_t> CPrimitiveUtils::GetWireframeCircleIndices(uint32_t _uSegments)
    {
      std::vector<uint32_t> lstIndices = std::vector<uint32_t>();
      for (uint32_t uX = 0; uX < _uSegments; ++uX)
      {
        lstIndices.push_back(uX);
        lstIndices.push_back(uX + 1);
      }
      return lstIndices;
    }
  }
}