#pragma once
#include "Libs/Math/Vector3.h"
#include "Libs/Math/Vector2.h"
#include "Engine/Render/Resources/Material.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"

#include <vector>
#include <functional>
#include "Libs/Math/Transform.h"
#include <d3d11.h>

namespace render
{
  namespace graphics
  {
    struct SVertexData
    {
      math::CVector3 Position = math::CVector3::Zero;
      math::CVector3 Normal = math::CVector3::Zero;
      math::CVector3 Color = math::CVector3::One;
      math::CVector2 TexCoord = math::CVector2::Zero;
      uint32_t MaterialId = 0;

      bool operator==(const SVertexData& _other) const
      {
        return Position == _other.Position
          && Normal == _other.Normal
          && TexCoord == _other.TexCoord
          && Color == _other.Color;
      }
      bool operator!=(const SVertexData& _other) const
      {
        return !(*this == _other);
      }
    };

    class CMesh
    {
    public:
      typedef std::map<uint32_t, render::material::CMaterial*> TMapMaterials;
      typedef std::vector<render::graphics::SVertexData> TVertexDataList;
      typedef std::vector<uint32_t> TIndexesList;

    public:
      CMesh(const std::string& _sMeshName) : m_sMeshId(_sMeshName) {}
      ~CMesh();

      void DrawMesh();
      HRESULT AssignIndexBuffer(TIndexesList& _vctIndices);

      void AddMaterial(render::material::CMaterial* _pMaterial, const uint32_t& _uMaterialIdx);
      void UpdateVertexColor(ID3D11Buffer* _pVertexBuffer);

      const TMapMaterials& GetMaterials() const { return m_dctMaterials; }
      const uint32_t& GetIndexCount() const { return static_cast<uint32_t>(m_vctIndices.size()); }
      const std::string& GetMeshId() const { return m_sMeshId; }

    private:
      void Clean();

      // Info
      std::string m_sMeshId = std::string();

      // Materials
      TMapMaterials m_dctMaterials = TMapMaterials();
      TIndexesList m_vctIndices = TIndexesList();

      // Buffers
      CConstantBuffer<SConstantTexture> m_oConstantTexture;
      ID3D11Buffer* m_pIndexBuffer = nullptr;
    };
  }
}

namespace std {
  template <>
  struct hash<render::graphics::SVertexData>
  {
    size_t operator()(const render::graphics::SVertexData& vertex) const
    {
      return ((hash<math::CVector3>()(vertex.Position) ^
        (hash<math::CVector3>()(vertex.Normal) << 1)) >> 1) ^
        (hash<math::CVector2>()(vertex.TexCoord) << 1) ^
        (hash<math::CVector3>()(vertex.Color) << 1) ^
        (hash<int>()(vertex.MaterialId) << 1);
    }
  };
}