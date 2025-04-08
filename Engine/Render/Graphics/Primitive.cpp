#include "Primitive.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Shaders/Primitive/VertexShader.h"
#include <cassert>
#include <iostream>
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/Maths/Maths.h"

namespace render
{
  namespace graphics
  {
    namespace internal_primitive
    {
      // Triangle Primitive
      static const std::vector<CPrimitive::SPrimitiveData> s_oTrianglePrimitive =
      {
        { maths::CVector3(0, 0.5f, 0.0f),   maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f,  0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(-0.5f, -0.5f,  0.0f), maths::CVector3(1.0f, 1.0f, 1.0f) },
      };
      // Square Primitive
      static const std::vector<CPrimitive::SPrimitiveData> s_oSquarePrimitive =
      {
        { maths::CVector3(-0.5f, -0.5f, -0.5f), maths::CVector3(1.0f, 0.0f, 0.0f) },
        { maths::CVector3(-0.5f,  0.5f, -0.5f), maths::CVector3(0.0f, 1.0f, 0.0f) },
        { maths::CVector3(0.5f,  0.5f, -0.5f), maths::CVector3(0.0f, 0.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f, -0.5f), maths::CVector3(1.0f, 1.0f, 1.0f) },

        { maths::CVector3(-0.5f, -0.5f,  0.5f), maths::CVector3(1.0f, 0.0f, 0.0f) },
        { maths::CVector3(-0.5f,  0.5f,  0.5f), maths::CVector3(0.0f, 1.0f, 0.0f) },
        { maths::CVector3(0.5f,  0.5f,  0.5f), maths::CVector3(0.0f, 0.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f,  0.5f), maths::CVector3(1.0f, 1.0f, 1.0f) }
      };
      // Plane Primitive
      static const std::vector<CPrimitive::SPrimitiveData> s_oPlanePrimitive =
      {
        { maths::CVector3(-0.5f, 0.0f, -0.5f), maths::CVector3(1.0f, 0.0f, 0.0f) },  // Bottom-left
        { maths::CVector3(-0.5f, 0.0f,  0.5f), maths::CVector3(0.0f, 1.0f, 0.0f) },  // Top-left
        { maths::CVector3(0.5f, 0.0f,  0.5f), maths::CVector3(0.0f, 0.0f, 1.0f) },  // Top-right
        { maths::CVector3(0.5f, 0.0f, -0.5f), maths::CVector3(1.0f, 1.0f, 1.0f) }   // Bottom-right
      };

      // 2D Square Indices
      static const std::vector<uint32_t> s_oSquareIndices =
      {
        0, 1, 2, // FRONT
        0, 2, 3, // FRONT
      };
      static const std::vector<uint32_t> s_oSquareWireframeIndices =
      {
        0, 1, 1, 2, 2, 3, 3, 0, // FRONT FACE
      };

      // 2D Triangle Indices
      static const std::vector<uint32_t> s_oTriangleIndices =
      {
        0, 1, 2, // FRONT
      };
      static const std::vector<uint32_t> s_oTriangleWireframeIndices =
      {
        0, 1, // Line 1
        1, 2, // Line 2
        2, 0  // Line 3
      };

      // 3D Cube Indices
      static const std::vector<uint32_t> s_oCubeIndices =
      {
        0, 1, 2, 0, 2, 3, //FRONT
        4, 7, 6, 4, 6, 5, //BACK 
        3, 2, 6,3, 6, 7, //RIGHT SIDE
        4, 5, 1, 4, 1, 0,  //LEFT SIDE
        1, 5, 6,  1, 6, 2, //TOP
        0, 3, 7, 0, 7, 4 //BOTTOM
      };
      static const std::vector<uint32_t> s_oCubeWireframeIndices =
      {
        0, 1, 1, 2, 2, 3, 3, 0, // FRONT FACE
        4, 5, 5, 6, 6, 7, 7, 4, // BACK FACE
        0, 4, 1, 5, 2, 6, 3, 7  // CONNECTING EDGES
      };

      // 3D Plane Indices
      static const std::vector<uint32_t> s_oPlaneIndices =
      {
        0, 1, 2, //FRONT
        0, 2, 3, //FRONT
      };
      static const std::vector<uint32_t> s_oPlaneWireframeIndices =
      {
        0, 1, // TOP EDGE
        1, 2, // RIGHT EDGE
        2, 3, // BOTTOM EDGE
        3, 0  // LEFT EDGE 
      };

      // 3D Sphere
      static void GenerateSphere(float _fRadius, int _iStacks, int _iSlices, std::vector<CPrimitive::SPrimitiveData>& _vctVertices_)
      {
        for (int uX = 0; uX <= _iStacks; ++uX)
        {
          float fPhi = static_cast<float>(uX) / static_cast<float>(_iStacks) * static_cast<float>(maths::s_fPI);
          for (int uJ = 0; uJ <= _iSlices; ++uJ)
          {
            // Conversion
            float fTheta = static_cast<float>(uJ) / static_cast<float>(_iSlices) * 2.0f * (float)maths::s_fPI;
            float fX = _fRadius * sinf(fPhi) * cosf(fTheta);
            float fY = _fRadius * cosf(fPhi);
            float fZ = _fRadius * sinf(fPhi) * sinf(fTheta);
            // Add vertex
            maths::CVector3 vVertex(fX, fY, fZ);
            _vctVertices_.push_back({ vVertex });
          }
        }
      }
      static std::vector<uint32_t> GenerateSphereIndices(int _iStacks, int _iSlices)
      {
        std::vector<uint32_t> vctIndices;

        // Connect vertices
        for (int i = 0; i < _iStacks; ++i)
        {
          for (int j = 0; j < _iSlices; ++j)
          {
            int first = (i * (_iSlices + 1)) + j;
            int second = first + _iSlices + 1;

            vctIndices.push_back(first);
            vctIndices.push_back(second);
            vctIndices.push_back(first + 1);

            vctIndices.push_back(second);
            vctIndices.push_back(second + 1);
            vctIndices.push_back(first + 1);
          }
        }

        return vctIndices;
      }
      static std::vector<uint32_t> GenerateSphereWireframeIndices(int _iStacks, int _iSlices)
      {
        std::vector<uint32_t> vctWireframeIndices;

        for (int i = 0; i < _iStacks; ++i)
        {
          for (int j = 0; j < _iSlices; ++j)
          {
            int current = (i * (_iSlices + 1)) + j;
            int next = current + 1;
            int above = current + _iSlices + 1;

            // Conectar con el siguiente en la misma fila
            if (j < _iSlices)
            {
              vctWireframeIndices.push_back(current);
              vctWireframeIndices.push_back(next);
            }

            // Conectar con el de arriba
            if (i < _iStacks)
            {
              vctWireframeIndices.push_back(current);
              vctWireframeIndices.push_back(above);
            }
          }
        }

        return vctWireframeIndices;
      }
    }

    // ------------------------------------
    CPrimitive::CPrimitive(EPrimitiveType _ePrimitiveType, ERenderMode _eRenderMode)
    {
      HRESULT hResult = CreateInputLayout();
      assert(!FAILED(hResult));

      // Create buffer from presets
      hResult = CreatePrimitive(_ePrimitiveType, _eRenderMode);
      assert(!FAILED(hResult));

      // Set values
      m_eRenderMode = _eRenderMode;
      m_ePrimitiveType = _ePrimitiveType;
    }
    // ------------------------------------
    CPrimitive::~CPrimitive()
    {
      m_oConstantBuffer.CleanBuffer();
      global::dx11::SafeRelease(m_pVertexBuffer);
      global::dx11::SafeRelease(m_pInputLayout);
    }
    // ------------------------------------
    HRESULT CPrimitive::CreatePrimitive(EPrimitiveType _ePrimitiveType, render::ERenderMode _eRenderMode)
    {
      switch (_ePrimitiveType)
      {
      // 2D Implementation
      case EPrimitiveType::E2D_SQUARE:
      {
        return CreateBufferFromVertexData
        (
          internal_primitive::s_oSquarePrimitive,
          _eRenderMode == SOLID ? internal_primitive::s_oSquareIndices : internal_primitive::s_oSquareWireframeIndices
        );
      }
      break;
      case EPrimitiveType::E2D_TRIANGLE:
      {
        return CreateBufferFromVertexData
        (
          internal_primitive::s_oTrianglePrimitive,
          _eRenderMode == SOLID ? internal_primitive::s_oTriangleIndices : internal_primitive::s_oTriangleWireframeIndices
        );
      }
      break;
      // 3D Implementation
      case EPrimitiveType::E3D_PLANE:
      {
        return CreateBufferFromVertexData
        (
          internal_primitive::s_oPlanePrimitive,
          _eRenderMode == SOLID ? internal_primitive::s_oPlaneIndices : internal_primitive::s_oPlaneWireframeIndices
        );
      }
      case EPrimitiveType::E3D_CUBE:
      {
        return CreateBufferFromVertexData
        (
          internal_primitive::s_oSquarePrimitive,
          _eRenderMode == SOLID ? internal_primitive::s_oCubeIndices : internal_primitive::s_oCubeWireframeIndices
        );
      }
      case EPrimitiveType::E3D_SPHERE:
      {
        const float fTargetRadius = 0.5f;
        const int iStacks = 12;
        const int iSlices = 12;
        std::vector<CPrimitive::SPrimitiveData> vctVertices = {};
        internal_primitive::GenerateSphere(fTargetRadius, iStacks, iSlices, vctVertices);

        return CreateBufferFromVertexData
        (
          vctVertices,
          _eRenderMode == SOLID ? internal_primitive::GenerateSphereIndices(iStacks, iSlices) : 
          internal_primitive::GenerateSphereWireframeIndices(iStacks, iSlices)
        );
      }
      break;
      }

      return S_FALSE;
    }
    // ------------------------------------
    HRESULT CPrimitive::CreateInputLayout()
    {
      D3D11_INPUT_ELEMENT_DESC oInputElementDesc[] =
      {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SPrimitiveData, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SPrimitiveData, Color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
      };

      return global::dx11::s_pDevice->CreateInputLayout
      (
        oInputElementDesc,
        ARRAYSIZE(oInputElementDesc),
        g_VertexShader,
        sizeof(g_VertexShader),
        &m_pInputLayout
      );
    }
    // ------------------------------------
    void CPrimitive::SetRenderMode(render::ERenderMode _eRenderMode)
    {
      if (m_eRenderMode != _eRenderMode)
      {
        m_eRenderMode = _eRenderMode;
        CreatePrimitive(m_ePrimitiveType, m_eRenderMode);
      }
    }
    // ------------------------------------
    void CPrimitive::SetColor(const maths::CVector3& _v3Color)
    {
      // Map
      D3D11_MAPPED_SUBRESOURCE oMappedSubresource;
      HRESULT hResult = global::dx11::s_pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &oMappedSubresource);
      UNUSED_VARIABLE(hResult);
      assert(!FAILED(hResult));

      CPrimitive::SPrimitiveData* pPrimitiveInfo = (CPrimitive::SPrimitiveData*)(oMappedSubresource.pData);
      assert(pPrimitiveInfo);

      // Update color
      for (uint32_t uIndex = 0; uIndex < m_uVertices; ++uIndex)
      {
        pPrimitiveInfo[uIndex].Color = _v3Color;
      }

      // Unmap
      global::dx11::s_pDeviceContext->Unmap(m_pVertexBuffer, 0);

      // Save color
      m_v3CurrentColor = _v3Color;
    }
    // ------------------------------------
    HRESULT CPrimitive::CreateBufferFromVertexData(const std::vector<CPrimitive::SPrimitiveData>& _vctPrimitiveInfo, const std::vector<UINT>& _vctIndexes)
    {
      if (_vctPrimitiveInfo.empty()) return -1;

      // Clean buffer
      m_oConstantBuffer.CleanBuffer();

      // Create constant buffer
      m_oConstantBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);

      // Create vertex buffer
      D3D11_BUFFER_DESC oVertexBufferDescriptor = {};
      oVertexBufferDescriptor.ByteWidth = static_cast<uint32_t>(sizeof(SPrimitiveData) * _vctPrimitiveInfo.size());
      oVertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
      oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      D3D11_SUBRESOURCE_DATA oSubresourceData = { 0 };
      oSubresourceData.pSysMem = _vctPrimitiveInfo.data();
      m_uVertices = static_cast<uint32_t>(_vctPrimitiveInfo.size());

      HRESULT hResult = global::dx11::s_pDevice->CreateBuffer
      (
        &oVertexBufferDescriptor,
        &oSubresourceData,
        &m_pVertexBuffer
      );
      if (_vctIndexes.empty()) return hResult;

      // Config index buffer
      D3D11_BUFFER_DESC oIndexBufferDesc = {};
      oIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      oIndexBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(uint32_t) * _vctIndexes.size()));
      oIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      oIndexBufferDesc.CPUAccessFlags = 0;
      m_uIndices = static_cast<uint32_t>(_vctIndexes.size());

      D3D11_SUBRESOURCE_DATA oSubresourceIndexesData = {};
      oSubresourceIndexesData.pSysMem = _vctIndexes.data();

      return global::dx11::s_pDevice->CreateBuffer(&oIndexBufferDesc, &oSubresourceIndexesData, &m_pIndexBuffer);
    }
    // ------------------------------------
    void CPrimitive::DrawPrimitive()
    {
      // Set general data
      UINT uVertexStride = sizeof(render::graphics::CPrimitive::SPrimitiveData);
      UINT uVertexOffset = 0;
      global::dx11::s_pDeviceContext->IASetInputLayout(m_pInputLayout);
      global::dx11::s_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStride, &uVertexOffset);

      // Set topology
      D3D_PRIMITIVE_TOPOLOGY ePrimitiveTopology = m_eRenderMode == ERenderMode::SOLID ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
      global::dx11::s_pDeviceContext->IASetPrimitiveTopology(ePrimitiveTopology);

      // Set model matrix
      m_oConstantBuffer.GetData().mMatrix = m_oPrimitiveTransform.ComputeModelMatrix();
      m_oConstantBuffer.UpdateBuffer();

      // Apply constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
      global::dx11::s_pDeviceContext->VSSetConstantBuffers(1, 1, &pConstantBuffer);

      // Draw
      global::dx11::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
      global::dx11::s_pDeviceContext->DrawIndexed(m_uIndices, 0, 0);
    }
  }
}