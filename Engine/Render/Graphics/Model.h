#pragma once
#include "Engine/Render/Graphics/Mesh.h"
#include "Engine/Render/Graphics/RenderInstance.h"
#include "Engine/Render/Buffers/BufferTypes.h"
#include "Engine/Collisions/AABB.h"

#include "Libs/Utils/FixedPool.h"
#include "Libs/Math/Transform.h"
#include <map>

namespace render { class CRender; }

namespace render
{
  namespace gfx
  {
    typedef std::vector<std::unique_ptr<render::gfx::CMesh>> TMeshes;
    typedef utils::CFixedPool<render::gfx::CRenderInstance, s_uMaxDrawableInstances> TInstances;

    struct TModelData
    {
      std::map<uint32_t, std::vector<uint32_t>> Indices;
      std::vector<render::gfx::TVertexData> VertexData;
      std::vector<std::unique_ptr<render::gfx::CMesh>> Meshes;
      char AssetPath[128];
    };

    class CModel
    {
    public:
      CModel(TModelData& _rModelData);
      ~CModel();

      void SetPos(const math::CVector3& _v3Pos);
      inline const math::CVector3& GetPosition() const { return m_oTransform.GetPos(); }
      void SetRot(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRotation() const { return m_oTransform.GetRot(); }
      void SetScl(const math::CVector3& _v3Scl);
      inline const math::CVector3& GetScl() const { return m_oTransform.GetScl(); }

      inline const math::CMatrix4x4& GetMatrix() const { return m_oTransform.GetMatrix(); }
      inline const math::CTransform& GetTransform() const { return m_oTransform; }

      inline const collision::CAABB& GetWorldAABB() const { return m_oWorldAABB; }
      inline const collision::CAABB& GetLocalAABB() const { return m_oLocalAABB; }

      inline const CBufferHandler& GetVtxBufferHandler() const { return m_oVtxBufferHandler; }
      inline void SetVtxBufferHandler(const CBufferHandler& _rBufferHandler) { m_oVtxBufferHandler = _rBufferHandler; }

      inline void SetVisible(bool _bVisible) { m_bVisible = _bVisible; }
      inline const bool IsVisible() const { return m_bVisible; }

      void SetCullEnabled(bool _bCull);
      inline const bool IsCullEnabled() const { return m_bCullEnabled; }

      inline std::string GetAssetPath() const { return std::string(m_sAssetPath); }
      inline const TMeshes& GetMeshes() const { return m_lstMeshes; }
      inline TInstances& GetInstances() { return m_lstInstances; }
      inline const TInstances& GetInstances() const { return m_lstInstances; }

      CRenderInstance* CreateInstance();
      bool RemoveInstance(uint16_t _uInstanceID);

      inline const bool AllowInstancing() const { return m_lstInstances.GetSize() < m_lstInstances.GetMaxSize(); }
      inline const bool HasInstances() const { return GetInstances().GetSize() > 0; }

    private:
      void Clear();

    private:
      TMeshes m_lstMeshes = TMeshes();
      TInstances m_lstInstances = TInstances();
      char m_sAssetPath[128];

      // Transforms
      math::CTransform m_oTransform = math::CTransform();
      collision::CAABB m_oLocalAABB = collision::CAABB();
      collision::CAABB m_oWorldAABB = collision::CAABB();

      CBufferHandler m_oVtxBufferHandler = CBufferHandler();
      bool m_bCullEnabled = true;
      bool m_bVisible = true;
    };
  }
}