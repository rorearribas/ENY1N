#include "Model.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Managers/ResourceManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Engine/Base/Engine.h"
#include "Engine/Shaders/Model/DeferredVertexShader.h"
#include <cassert>

namespace render
{
  namespace graphics
  {
    // ------------------------------------
    CModel::CModel(const char* _sModelPath, const char* _sBaseMltDir)
    {
      HRESULT hr = InitModel(_sModelPath, _sBaseMltDir);
      UNUSED_VARIABLE(hr);
      assert(!FAILED(hr));
    }
    // ------------------------------------
    CModel::~CModel()
    {
      m_oModelData.m_vctMeshes.clear();
      m_oModelData.m_vctVertexData.clear();

      global::dx11::SafeRelease(m_pVertexBuffer);
      global::dx11::SafeRelease(m_pInputLayout);
    }
    // ------------------------------------
    HRESULT CModel::CreateInputLayout()
    {
      D3D11_INPUT_ELEMENT_DESC oInputElementDesc[] =
      {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(render::graphics::SVertexData, TexCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
      };

      return global::dx11::s_pDevice->CreateInputLayout
      (
        oInputElementDesc,
        ARRAYSIZE(oInputElementDesc),
        g_DeferredVertexShader,
        sizeof(g_DeferredVertexShader),
        &m_pInputLayout
      );
    }
    // ------------------------------------
    HRESULT CModel::InitModel(const char* _sModelPath, const char* _sBaseMltDir)
    {
      // Try to load model
      CResourceManager* pResourceManager = CResourceManager::GetInstance();
      m_oModelData = std::move(pResourceManager->LoadModel(_sModelPath, _sBaseMltDir));
      if (m_oModelData.m_vctMeshes.empty()) return E_FAIL;

      // Init constant buffer
      m_oConstantBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);

      // We create here the vertex buffer
      D3D11_BUFFER_DESC oVertexBufferDescriptor = {};
      oVertexBufferDescriptor.ByteWidth = static_cast<uint32_t>((sizeof(render::graphics::SVertexData) * m_oModelData.m_vctVertexData.size()));
      oVertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
      oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      D3D11_SUBRESOURCE_DATA oSubresourceData = D3D11_SUBRESOURCE_DATA();
      oSubresourceData.pSysMem = m_oModelData.m_vctVertexData.data();
      HRESULT hr = global::dx11::s_pDevice->CreateBuffer
      (
        &oVertexBufferDescriptor,
        &oSubresourceData,
        &m_pVertexBuffer
      );
      if (FAILED(hr)) return hr;

      // Update vertex color
      for (render::graphics::CMesh* pMesh : m_oModelData.m_vctMeshes)
      {
        pMesh->UpdateVertexColor(m_pVertexBuffer);
      }

      // Create input layout
      hr = CreateInputLayout();
      if (FAILED(hr)) return hr;

      return hr;
    }
    // ------------------------------------
    void CModel::DrawModel()
    {
      // Set vertex buffer
      UINT uVertexStride = sizeof(render::graphics::SVertexData);
      UINT uVertexOffset = 0;
      global::dx11::s_pDeviceContext->IASetInputLayout(m_pInputLayout);
      global::dx11::s_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStride, &uVertexOffset);
      global::dx11::s_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // Set model matrix
      m_oConstantBuffer.GetData().mMatrix = m_oModelTransform.ComputeModelMatrix();
      m_oConstantBuffer.UpdateBuffer();

      // Apply constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
      global::dx11::s_pDeviceContext->VSSetConstantBuffers(1, 1, &pConstantBuffer);

      // Draw meshes
      for (render::graphics::CMesh* pMesh : m_oModelData.m_vctMeshes)
      {
        pMesh->DrawMesh();
      }
    }
  }
}