#include "Model.h"
#include "Engine/Shaders/Forward/SimpleVS.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Managers/ResourceManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include <cassert>

namespace render
{
  namespace gfx
  {
    // ------------------------------------
    CModel::CModel(const char* _sModelPath)
    {
      HRESULT hResult = InitModel(_sModelPath);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));
    }
    // ------------------------------------
    CModel::~CModel()
    {
      Clear();
    }
    // ------------------------------------
    void CModel::Draw()
    {
      // Set vertex buffer
      uint32_t uVertexStride = sizeof(render::gfx::SVertexData);
      uint32_t uVertexOffset = 0;
      global::dx::s_pDeviceContext->IASetInputLayout(m_pInputLayout);
      global::dx::s_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStride, &uVertexOffset);
      global::dx::s_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // Set model matrix
      math::CMatrix4x4 mModel = m_oTransform.CreateTransform();
      engine::CEngine* pEngine = engine::CEngine::GetInstance();
      pEngine->GetRender()->SetModelMatrix(mModel);

      // Draw meshes
      for (auto& pMesh : m_oModelData.Meshes)
      {
        pMesh->Draw();
      }
    }
    // ------------------------------------
    HRESULT CModel::CreateInputLayout()
    {
      return global::dx::s_pDevice->CreateInputLayout
      (
        SVertexData::s_vctInputElementDesc.data(),
        static_cast<uint32_t>(SVertexData::s_vctInputElementDesc.size()),
        g_SimpleVS,
        sizeof(g_SimpleVS),
        &m_pInputLayout
      );
    }
    // ------------------------------------
    HRESULT CModel::InitModel(const char* _sModelPath)
    {
      // Flush model data
      Clear();

      // Load model
      CResourceManager* pResourceManager = CResourceManager::GetInstance();
      m_oModelData = pResourceManager->LoadModel(_sModelPath); // I should change this!
      if (m_oModelData.Meshes.empty()) 
      {
        return E_FAIL;
      }

      // We create here the vertex buffer
      D3D11_BUFFER_DESC oVertexBufferDescriptor = D3D11_BUFFER_DESC();
      oVertexBufferDescriptor.ByteWidth = static_cast<uint32_t>((sizeof(render::gfx::SVertexData) * m_oModelData.Vertices.size()));
      oVertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
      oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      D3D11_SUBRESOURCE_DATA oSubresourceData = D3D11_SUBRESOURCE_DATA();
      oSubresourceData.pSysMem = m_oModelData.Vertices.data();
      HRESULT hResult = global::dx::s_pDevice->CreateBuffer(&oVertexBufferDescriptor, &oSubresourceData, &m_pVertexBuffer);
      if (FAILED(hResult))
      {
        return hResult;
      }

      // Create input layout
      return CreateInputLayout();
    }
    // ------------------------------------
    void CModel::Clear()
    {
      // Clear DX components
      global::dx::SafeRelease(m_pVertexBuffer);
      global::dx::SafeRelease(m_pInputLayout);

      // Clear meshes
      for (auto& pMesh : m_oModelData.Meshes)
      {
        pMesh.reset();
      }

      m_oModelData.Meshes.clear();
      m_oModelData.Vertices.clear();
    }
  }
}