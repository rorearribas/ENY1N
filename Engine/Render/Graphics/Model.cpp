#include "Model.h"
#include "Engine/Shaders/Standard/ForwardVertexShader.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Managers/ResourceManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Engine/Base/Engine.h"
#include <cassert>

namespace render
{
  namespace graphics
  {
    // ------------------------------------
    CModel::CModel(const char* _sModelPath, const char* _sBaseMltDir)
    {
      HRESULT hResult = InitModel(_sModelPath, _sBaseMltDir);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));
    }
    // ------------------------------------
    CModel::~CModel()
    {
      global::dx11::SafeRelease(m_pVertexBuffer);
      global::dx11::SafeRelease(m_pInputLayout);
      m_oModelData.m_vctMeshes.clear();
      m_oModelData.m_vctVertexData.clear();
    }
    // ------------------------------------
    HRESULT CModel::CreateInputLayout()
    {
      return global::dx11::s_pDevice->CreateInputLayout
      (
        render::graphics::SVertexData::s_vctInputElementDesc.data(),
        static_cast<uint32_t>(render::graphics::SVertexData::s_vctInputElementDesc.size()),
        g_ForwardVertexShader,
        sizeof(g_ForwardVertexShader),
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
      D3D11_BUFFER_DESC oVertexBufferDescriptor = D3D11_BUFFER_DESC();
      oVertexBufferDescriptor.ByteWidth = static_cast<uint32_t>((sizeof(render::graphics::SVertexData) * m_oModelData.m_vctVertexData.size()));
      oVertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
      oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      D3D11_SUBRESOURCE_DATA oSubresourceData = D3D11_SUBRESOURCE_DATA();
      oSubresourceData.pSysMem = m_oModelData.m_vctVertexData.data();
      HRESULT hResult = global::dx11::s_pDevice->CreateBuffer(&oVertexBufferDescriptor, &oSubresourceData, &m_pVertexBuffer);
      if (FAILED(hResult)) 
      {
        return hResult;
      }

      // Update vertex color
      for (render::graphics::CMesh* pMesh : m_oModelData.m_vctMeshes)
      {
        pMesh->UpdateVertexColor(m_pVertexBuffer);
      }

      // Create input layout
      hResult = CreateInputLayout();
      if (FAILED(hResult))
      {
        return hResult;
      }

      return hResult;
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
      m_oConstantBuffer.GetData().Matrix = m_oModelTransform.ComputeModelMatrix();
      bool bOk = m_oConstantBuffer.WriteBuffer();
      UNUSED_VAR(bOk);
      assert(bOk);

      // Apply constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
      global::dx11::s_pDeviceContext->VSSetConstantBuffers(1, 1, &pConstantBuffer);

      // Draw meshes
      for (render::graphics::CMesh* pMesh : m_oModelData.m_vctMeshes)
      {
        pMesh->DrawMesh();
      }
    }
    // ------------------------------------
    void CModel::UseGlobalLighting(bool _bEnabled)
    {
      // Draw meshes
      for (render::graphics::CMesh* pMesh : m_oModelData.m_vctMeshes)
      {
        pMesh->UseGlobalLighting(_bEnabled);
      }
    }
  }
}