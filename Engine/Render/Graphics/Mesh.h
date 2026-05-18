#pragma once
#include "Engine/Render/Resources/Material.h"

namespace render { class CRender; }
namespace render { namespace mat { class CMaterial; } }

namespace render
{
  namespace gfx
  {
    class CMesh
    {
    public:
      CMesh(std::vector<uint32_t>& _lstIndices);
      ~CMesh() {}

      inline std::vector<uint32_t>& GetIndices() { return m_lstIndices; }
      inline void SetIdxCount(uint32_t _uIndexCount) { m_uIndexCount = _uIndexCount; }
      inline uint32_t GetIdxCount() const { return m_uIndexCount; }
      inline void SetIdxOffset(uint32_t _uIndexOffset) { m_uIndexOffset = _uIndexOffset; }
      inline uint32_t GetIdxOffset() const { return m_uIndexOffset; }

      inline render::mat::CMaterial* GetMaterial() const { return m_pMaterial.get(); }
      inline void SetMaterial(std::unique_ptr<render::mat::CMaterial> _pMaterial) { m_pMaterial = std::move(_pMaterial); }

    private:
      std::unique_ptr<render::mat::CMaterial> m_pMaterial = nullptr;

      std::vector<uint32_t> m_lstIndices = std::vector<uint32_t>();
      uint32_t m_uIndexCount = 0;
      uint32_t m_uIndexOffset = 0;
    };
  }
}