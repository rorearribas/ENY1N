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
      const uint32_t& GetIndexCount() const { return m_uIndexCount; }
      const TMapMaterials& GetMaterials() const { return m_dctMaterials; }

      void AddMaterial(render::material::CMaterial* _pMaterial, const uint32_t& _uMaterialIdx);
      void SetMaterialIndexes(const std::vector<int>& _vctIndexes) { m_vctMaterialsIdx = _vctIndexes; }
      void ApplyMaterials(ID3D11Buffer* _pVertexBuffer);

    private:
      // Info
      std::string m_sMeshName = std::string();

      // Buffers
      ID3D11Buffer* m_pIndexBuffer = nullptr;
      uint32_t m_uIndexCount = 0;

      // Materials
      TMapMaterials m_dctMaterials = {};
      std::vector<int> m_vctMaterialsIdx = {};
      TIndexesList m_vctIndexes = {};
    };
  }
}

namespace std
{
  template<>
  struct hash<render::graphics::SVertexData>
  {
    std::size_t operator()(const render::graphics::SVertexData& v) const
    {
      std::size_t h1 = std::hash<maths::CVector3>()(v.Position);
      std::size_t h2 = std::hash<maths::CVector3>()(v.Normal);
      std::size_t h3 = std::hash<maths::CVector2>()(v.TexCoord);
      return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
  };
}