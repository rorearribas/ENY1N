#pragma once
#include "Engine/Render/Resources/Material.h"
#include "Libs/Math/Vector3.h"
#include "Libs/Math/Vector2.h"
#include "Libs/Math/Transform.h"

#include <vector>

namespace render { namespace mat { class CMaterial; } }
namespace render
{
  namespace gfx
  {
    class CMesh
    {
    public:
      CMesh() = default;
      CMesh(const std::string& _sMeshName) : m_sMeshID(_sMeshName) {}
      ~CMesh();

      void Draw(uint32_t _uInstanceCount = 0);
      HRESULT CreateBuffer(const std::vector<uint32_t>& _lstIndices);

      inline render::mat::CMaterial* GetMaterial() const { return m_pMaterial.get(); }
      inline void SetMaterial(std::unique_ptr<render::mat::CMaterial> _pMaterial) { m_pMaterial = std::move(_pMaterial); }
      inline const std::string& GetID() const { return m_sMeshID; }

    private:
      void ClearBuffer();
      void ClearMaterial();

    private:
      // Mesh
      ID3D11Buffer* m_pIndexBuffer = nullptr;
      uint32_t m_uIndices = 0;

    private:
      // Data
      std::string m_sMeshID = std::string();
      std::unique_ptr<render::mat::CMaterial> m_pMaterial = nullptr;
    };
  }
}