#pragma once
#include "Libs/Maths/Vector3.h"
#include "Libs/Maths/Vector2.h"
#include "Engine/Render/Resources/Material.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"

#include <vector>
#include <functional>
#include "Libs/Maths/Transform.h"
#include <d3d11.h>

namespace render
{
  namespace graphics
  {
    struct SVertexData
    {
      maths::CVector3 Position = maths::CVector3::Zero;
      maths::CVector3 Normal = maths::CVector3::Zero;
      maths::CVector2 TexCoord = maths::CVector2::Zero;
      maths::CVector3 Color = maths::CVector3::One;
      int MaterialId = 0;

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
      CMesh(const std::string& _sMeshName) : m_sMeshName(_sMeshName) {}
      ~CMesh();

      void DrawMesh();
      HRESULT CreateMesh(TIndexesList& _vctIndexes);

      const std::string& GetMeshName() const { return m_sMeshName; }
      const TMapMaterials& GetMaterials() const { return m_dctMaterials; }
      const uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_vctIndexes.size()); }

      void AddMaterial(render::material::CMaterial* _pMaterial, const uint32_t& _uMaterialIdx);
      void ApplyMaterials(ID3D11Buffer* _pVertexBuffer);

    private:
      // Info
      std::string m_sMeshName = std::string();

      // Buffers
      ConstantBuffer<SConstantTexture> m_oConstantTexture;
      ID3D11Buffer* m_pIndexBuffer = nullptr;

      // Materials
      TMapMaterials m_dctMaterials = {};
      TIndexesList m_vctIndexes = {};
    };
  }
}

namespace std {
  template <>
  struct hash<render::graphics::SVertexData> 
  {
    size_t operator()(const render::graphics::SVertexData& vertex) const 
    {
      return ((hash<maths::CVector3>()(vertex.Position) ^
        (hash<maths::CVector3>()(vertex.Normal) << 1)) >> 1) ^
        (hash<maths::CVector2>()(vertex.TexCoord) << 1) ^
        (hash<maths::CVector3>()(vertex.Color) << 1) ^
        (hash<int>()(vertex.MaterialId) << 1);
    }
  };
}