#pragma once
#include "Engine/Render/Graphics/Mesh.h"
#include "Engine/Render/Graphics/RenderInstance.h"
#include "Engine/Render/ConstantBuffer/BufferTypes.h"
#include "Engine/Collisions/AABB.h"

#include "Libs/Utils/FixedPool.h"
#include "Libs/Math/Transform.h"

namespace render
{
  namespace gfx
  {
    typedef std::vector<std::unique_ptr<render::gfx::CMesh>> TMeshes;
    typedef utils::CFixedPool<render::gfx::CRenderInstance, s_uMaxInstancesPerModel> TInstances;

    class CModel
    {
    public:
      struct TModelData
      {
        std::vector<std::unique_ptr<render::gfx::CMesh>> Meshes;
        std::vector<render::gfx::TVertexData> Vertices;
        char AssetPath[128];
      };

    public:
      CModel(TModelData& _rModelData);
      ~CModel();

      void Draw();
      void DrawInstances(const TDrawableInstances& _lstDrawableInstances, uint16_t _uInstanceCount);

      void SetPos(const math::CVector3& _v3Pos);
      inline const math::CVector3& GetPosition() const { return m_oTransform.GetPos(); }
      void SetRot(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRotation() const { return m_oTransform.GetRot(); }
      void SetScl(const math::CVector3& _v3Scl);
      inline const math::CVector3& GetScl() const { return m_oTransform.GetScl(); }

      void SetCullEnabled(bool _bCull);
      inline const bool IsCullEnabled() const { return m_bCullEnabled; }

      inline void SetVisible(bool _bVisible) { m_bVisible = _bVisible; }
      inline const bool IsVisible() const { return m_bVisible; }

      CRenderInstance* CreateInstance();
      bool RemoveInstance(uint16_t _uInstanceID);

      inline TInstances& GetInstances() { return m_lstInstances; }
      inline const TInstances& GetInstances() const { return m_lstInstances; }

      inline const collision::CAABB& GetWorldAABB() const { return m_oWorldAABB; }
      inline const collision::CAABB& GetLocalAABB() const { return m_oLocalAABB; }

      inline std::string GetAssetPath() const { return std::string(m_sAssetPath); }
      inline const bool AllowInstancing() const { return m_lstInstances.GetSize() < m_lstInstances.GetMaxSize(); }
      inline const bool HasInstances() const { return GetInstances().GetSize() > 0; }

    private:
      HRESULT InitModel(TModelData& _rModelData);
      void Clear();

    private:
      // Buffers
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      ID3D11Buffer* m_pInstanceBuffer = nullptr;
      char m_sAssetPath[128];

    private:
      // Model
      TMeshes m_lstMeshes = TMeshes();
      TInstances m_lstInstances = TInstances();

      // Transforms
      math::CTransform m_oTransform = math::CTransform();
      collision::CAABB m_oLocalAABB = collision::CAABB();
      collision::CAABB m_oWorldAABB = collision::CAABB();

      bool m_bCullEnabled = true;
      bool m_bVisible = true;
    };
  }
}