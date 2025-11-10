#pragma once
#include "Engine/Render/Resources/Material.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"

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
      typedef std::vector<uint32_t> TIndicesList;

    public:
      CMesh(const std::string& _sMeshName) : m_sMeshID(_sMeshName) {}
      ~CMesh();

      void Draw(uint32_t _uInstanceCount = 0);
      HRESULT CreateBuffer(TIndicesList& _lstIndices);

      inline std::shared_ptr<render::mat::CMaterial> GetMaterial() const { return m_pMaterial; }
      inline void SetMaterial(std::shared_ptr<render::mat::CMaterial> _pMaterial) { m_pMaterial = _pMaterial; }

      inline const uint32_t& GetIndexCount() const { return static_cast<uint32_t>(m_lstIndices.size()); }
      inline const std::string& GetMeshID() const { return m_sMeshID; }

    private:
      void ClearBuffers();
      void ClearMaterial();

      // Info
      std::string m_sMeshID = std::string();

      // Materials
      std::shared_ptr<render::mat::CMaterial> m_pMaterial = nullptr;
      TIndicesList m_lstIndices = TIndicesList();

      // Buffer
      CConstantBuffer<STexturesData> m_oConstantBuffer;
      ID3D11Buffer* m_pIndexBuffer = nullptr;
    };
  }
}