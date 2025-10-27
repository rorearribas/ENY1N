#pragma once
#include "Libs/Math/Transform.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Collisions/BoundingBox.h"
#include "Mesh.h"

namespace render
{
  namespace gfx
  {
    class CModel
    {
    public:
      struct SModelData
      {
        std::vector<std::shared_ptr<render::gfx::CMesh>> Meshes;
        std::vector<render::gfx::SVertexData> Vertices;
      };

    public:
      CModel(const char* _sModelPath);
      ~CModel();

      void Draw();

      inline void SetPosition(const math::CVector3& _v3Pos) { m_oTransform.SetPosition(_v3Pos); CalculateBoundingBox(); }
      inline const math::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
      inline void SetRotation(const math::CVector3& _v3Rot) { m_oTransform.SetRotation(_v3Rot); CalculateBoundingBox(); }
      inline const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
      inline void SetScale(const math::CVector3& _v3Scl) { m_oTransform.SetScale(_v3Scl); CalculateBoundingBox(); }
      inline const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }

      inline void SetBoundingBox(const collision::CBoundingBox& _oBoundingBox) { m_oBoundingBox = _oBoundingBox; }
      inline const collision::CBoundingBox& GetBoundingBox() const { return m_oBoundingBox; }

    private:
      HRESULT InitModel(const char* _sModelPath);
      HRESULT CalculateBoundingBox();
      void Clear();

    private:
      // Model data
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      SModelData m_oModelData = SModelData();

      math::CTransform m_oTransform = math::CTransform();
      collision::CBoundingBox m_oBoundingBox;
    };
  }
}

