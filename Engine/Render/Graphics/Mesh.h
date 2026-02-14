#pragma once
#include "Engine/Render/Resources/Material.h"
#include "Libs/Math/Vector3.h"
#include "Libs/Math/Vector2.h"
#include "Libs/Math/Transform.h"

#include <vector>

namespace render { class CRender; }
namespace render { namespace mat { class CMaterial; } }

namespace render
{
  namespace gfx
  {
    class CMesh
    {
    public:
      CMesh(const TIndices& _lstIndices);
      ~CMesh();

      void Draw(uint32_t _uInstanceCount, uint32_t _uStartOffset);

      inline render::mat::CMaterial* GetMaterial() const { return m_pMaterial.get(); }
      inline void SetMaterial(std::unique_ptr<render::mat::CMaterial> _pMaterial) { m_pMaterial = std::move(_pMaterial); }

      inline ID3D11Buffer* GetBuffer() const { return m_pIndexBuffer; }
      inline const uint32_t GetIndexCount() const { return m_uIndices; }

    private:
      HRESULT CreateBuffer(const TIndices& _lstIndices);
      void ClearBuffer();
      void ClearMaterial();

    private:
      // Buffer
      ID3D11Buffer* m_pIndexBuffer = nullptr;
      uint32_t m_uIndices = 0;

    private:
      std::unique_ptr<render::mat::CMaterial> m_pMaterial = nullptr;
    };
  }
}