#pragma once
#include <d3d11.h>
#include "Engine/Render/Resources/Material.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"

#include "Libs/Math/Vector3.h"
#include "Libs/Math/Vector2.h"
#include "Libs/Math/Transform.h"

#include <vector>
#include <unordered_map>

namespace render
{
  namespace gfx
  {
    struct SVertexData
    {
      // Input element desc
      static std::vector<D3D11_INPUT_ELEMENT_DESC> s_vctInputElementDesc;

      // Vertex data
      math::CVector3 Position = math::CVector3::Zero;
      math::CVector3 Normal = math::CVector3::Zero;
      math::CVector3 Color = math::CVector3::One;
      math::CVector2 UV = math::CVector2::Zero;

      // Material ID
      uint32_t MaterialId = 0;

      bool operator==(const SVertexData& _other) const;
      bool operator!=(const SVertexData& _other) const;
    };

    class CMesh
    {
    public:
      typedef std::unordered_map<uint32_t, render::mat::CMaterial*> TMapMaterials;
      typedef std::vector<uint32_t> TIndexesList;

    public:
      CMesh(const std::string& _sMeshName) : m_sMeshID(_sMeshName) {}
      ~CMesh();

      void DrawMesh();
      HRESULT CreateBuffer(TIndexesList& _vctIndices);

      void UseGlobalLighting(bool _bEnabled);
      void AddMaterial(render::mat::CMaterial* _pMaterial, const uint32_t& _uMaterialIdx);
      void UpdateVertexColor(ID3D11Buffer* _pVertexBuffer);

      const TMapMaterials& GetMaterials() const { return m_dctMaterials; }
      const uint32_t& GetIndexCount() const { return static_cast<uint32_t>(m_vctIndices.size()); }
      const std::string& GetMeshID() const { return m_sMeshID; }

    private:
      void Clean();

      // Info
      std::string m_sMeshID = std::string();
      bool m_bUseGlobalLightning = true;

      // Materials
      TMapMaterials m_dctMaterials = TMapMaterials();
      TIndexesList m_vctIndices = TIndexesList();

      // Buffers
      CConstantBuffer<SConstantModelData> m_oConstantModelData;
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
      auto roundFloat = [](float f) { return static_cast<int>(f * 1000); };
      size_t h1 = hash<int>()(roundFloat(v.Position.X)) ^ hash<int>()(roundFloat(v.Position.Y)) ^ hash<int>()(roundFloat(v.Position.Z));
      size_t h2 = hash<int>()(roundFloat(v.Normal.X)) ^ hash<int>()(roundFloat(v.Normal.Y)) ^ hash<int>()(roundFloat(v.Normal.Z));
      size_t h3 = hash<int>()(roundFloat(v.UV.X)) ^ hash<int>()(roundFloat(v.UV.Y));
      size_t h4 = hash<uint32_t>()(v.MaterialId);
      return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
  };
}
