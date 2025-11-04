#pragma once
#include "Engine/Render/Graphics/Mesh.h"
#include "Engine/Render/Graphics/RenderInstance.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Collisions/BoundingBox.h"

#include "Libs/Utils/FixedPool.h"
#include "Libs/Math/Transform.h"

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
      static uint32_t constexpr s_uMaxInstances = 128u;
      typedef utils::CFixedPool<render::gfx::CRenderInstance, s_uMaxInstances> TInstances;

    public:
      CModel(const char* _sModelPath);
      ~CModel();

      void Draw();

      inline void SetVisible(bool _bVisible) { m_bVisible = _bVisible; }
      inline const bool& IsVisible() const { return m_bVisible; }
      inline const collision::CBoundingBox& GetBoundingBox() const { return m_oBoundingBox; }

      const bool HasInstances() const { return m_lstInstances.GetCurrentSize() > 0; }
      const TInstances& GetInstances() const { return m_lstInstances; }
      TInstances& GetInstances() { return m_lstInstances; }

      void SetCullingEnabled(bool _bCull);
      inline const bool& IsCullingEnabled() const { return m_bCullEnabled; }

      void SetPosition(const math::CVector3& _v3Pos);
      inline const math::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
      void SetRotation(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
      void SetScale(const math::CVector3& _v3Scl);
      inline const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }

    private:
      HRESULT InitModel(const char* _sModelPath);
      HRESULT CalculateBoundingBox();
      void Clear();

    private:
      // Model data
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      SModelData m_oModelData = SModelData();
      TInstances m_lstInstances = TInstances();

    private:
      math::CTransform m_oTransform = math::CTransform();
      collision::CBoundingBox m_oBoundingBox = collision::CBoundingBox();

      bool m_bCullEnabled = true;
      bool m_bVisible = true;
    };
  }
}

