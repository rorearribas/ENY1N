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
    static std::vector<D3D11_INPUT_ELEMENT_DESC> s_vctInputElementDesc;

    struct SVertexData
    {
      // Vertex data
      math::CVector3 Position = math::CVector3::Zero;
      math::CVector3 Normal = math::CVector3::Zero;
      math::CVector3 Color = math::CVector3::One;
      math::CVector2 TexCoord = math::CVector2::Zero;

      bool operator==(const SVertexData& _other) const;
      bool operator!=(const SVertexData& _other) const;
    };

    class CMesh
    {
    public:
      typedef std::vector<uint32_t> TIndicesList;

    public:
      CMesh(const std::string& _sMeshName) : m_sMeshID(_sMeshName) {}
      ~CMesh();

      void Draw();
      HRESULT CreateBuffer(TIndicesList& _vctIndices);

      inline std::shared_ptr<render::mat::CMaterial> GetMaterial() const { return m_pMaterial; }
      inline void SetMaterial(std::shared_ptr<render::mat::CMaterial> _pMaterial) { m_pMaterial = _pMaterial; }

      inline const uint32_t& GetIndexCount() const { return static_cast<uint32_t>(m_vctIndices.size()); }
      inline const std::string& GetMeshID() const { return m_sMeshID; }

    private:
      void ClearBuffers();
      void ClearMaterial();

      // Info
      std::string m_sMeshID = std::string();

      // Materials
      std::shared_ptr<render::mat::CMaterial> m_pMaterial = nullptr;
      TIndicesList m_vctIndices = TIndicesList();

      // Buffer
      CConstantBuffer<STexturesData> m_oConstantBuffer;
      ID3D11Buffer* m_pIndexBuffer = nullptr;
    };
  }
}

namespace std
{
  template <>
  struct hash<render::gfx::SVertexData>
  {
    size_t operator()(const render::gfx::SVertexData& v) const
    {
      auto oHashFunc = [](float f) { return static_cast<int>(f * 1000); };
      size_t h1 = hash<int>()(oHashFunc(v.Position.x)) ^ hash<int>()(oHashFunc(v.Position.y)) ^ hash<int>()(oHashFunc(v.Position.z));
      size_t h2 = hash<int>()(oHashFunc(v.Normal.x)) ^ hash<int>()(oHashFunc(v.Normal.y)) ^ hash<int>()(oHashFunc(v.Normal.z));
      size_t h3 = hash<int>()(oHashFunc(v.TexCoord.x)) ^ hash<int>()(oHashFunc(v.TexCoord.y));
      return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
  };
}
