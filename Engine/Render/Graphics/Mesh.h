#pragma once
#include "Engine/Render/Resources/Material.h"
#include "Engine/Render/Buffers/RenderBuffer.h"

namespace render { class CRender; }
namespace render { namespace mat { class CMaterial; } }

namespace render
{
  namespace gfx
  {
    class CMesh
    {
    public:
      CMesh() = default;
      ~CMesh() { m_pMaterial.reset(); };

      CMesh(CMesh&& _rOther) noexcept;
      CMesh& operator=(CMesh&& _rOther) noexcept;
      CMesh(const CMesh& _rOther) = delete;
      CMesh& operator=(const CMesh& _rOther) = delete;

      // Material
      inline render::mat::CMaterial* GetMaterial() const { return m_pMaterial.get(); }
      inline void SetMaterial(std::unique_ptr<render::mat::CMaterial> _pMaterial) { m_pMaterial = std::move(_pMaterial); }

      // Handlers
      inline void SetIdxBufferHandler(const CBufferHandler& _rBufferHandler) { m_oIdxBufferHandler = _rBufferHandler; }
      inline const CBufferHandler& GetIdxBufferHandler() const { return m_oIdxBufferHandler; }
      inline const uint32_t GetIndexCount() const { return m_oIdxBufferHandler.GetOffset(); }

    private:
      std::unique_ptr<render::mat::CMaterial> m_pMaterial;
      CBufferHandler m_oIdxBufferHandler = CBufferHandler();
    };
  }
}