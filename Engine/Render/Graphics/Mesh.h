#pragma once
#include "Libs/Maths/Vector3.h"
#include "Libs/Maths/Vector2.h"
#include "Engine/Render/Resources/Material.h"
#include <vector>

namespace render
{
  namespace graphics
  {
    struct SVertexData
    {
      maths::CVector3 Position = maths::CVector3::Zero;
      maths::CVector3 Normal = maths::CVector3::Zero;
      maths::CVector2 TexCoord = maths::CVector2::Zero;

      bool operator==(const SVertexData& _other)
      {
        return Position == _other.Position && Normal == _other.Normal && TexCoord == _other.TexCoord;
      }
      bool operator!=(const SVertexData& _other)
      {
        return Position != _other.Position && Normal != _other.Normal && TexCoord != _other.TexCoord;
      }
    };

    class CMesh
    {
    public:
      typedef std::vector<material::CMaterial> TMaterialList;
      typedef std::vector<render::graphics::SVertexData> TVertexDataList;
      typedef std::vector<uint32_t> TIndexesList;

    public:
      CMesh(const std::string& _sMeshName) : m_sMeshName(_sMeshName) {}
      ~CMesh();

      void PushVertexData(std::vector<SVertexData>& _vctVertexData) { m_vctVertexData = std::move(_vctVertexData); }
      void PushIndexes(std::vector<uint32_t>& _vctIndexes) { m_vctIndexes = std::move(_vctIndexes); }

      const TVertexDataList& GetVertexData() { return m_vctVertexData; }
      const TIndexesList& GetIndexes() { return m_vctIndexes; }

    private:
      std::string m_sMeshName = std::string();

      TVertexDataList m_vctVertexData = {};
      TIndexesList m_vctIndexes = {};
    };
  }
}
