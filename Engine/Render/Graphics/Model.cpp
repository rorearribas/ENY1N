#include "Model.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Managers/ResourceManager.h"
#include <d3dcompiler.h>
#include "Libs/Macros/GlobalMacros.h"
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
      m_vctMeshes.clear();
    }
    // ------------------------------------
    HRESULT CModel::InitModel(const char* _sModelPath, const char* _sBaseMltDir)
    {
      // Try to load model
      CResourceManager* pResourceManager = CResourceManager::GetInstance();
      m_vctMeshes = std::move(pResourceManager->LoadModel(_sModelPath, _sBaseMltDir));
      if (m_vctMeshes.empty()) return -1;

      return S_OK;
    }
    // ------------------------------------
    void CModel::DrawModel()
    {
      for (render::graphics::CMesh* pMesh :m_vctMeshes)
      {
        pMesh->DrawMesh(m_oModelTransform);
      }
    }
  }
}