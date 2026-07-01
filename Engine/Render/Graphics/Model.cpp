#include "Model.h"
#include "Engine/Shaders/Forward/SimpleVS.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Managers/ResourceManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/Math/Math.h"
#include <cassert>

namespace render
{
  namespace gfx
  {
    // ------------------------------------
    CModel::CModel(TModelData& _rModelData)
    {
      m_lstMeshes = std::move(_rModelData.Meshes);
      m_uMeshesCount = _rModelData.MeshesCount;

      strncpy_s(m_sAssetPath, _rModelData.AssetPath, sizeof(m_sAssetPath) - 1);
      collision::ComputeLocalAABB(_rModelData.VertexData, m_oLocalAABB);
    }
    // ------------------------------------
    CModel::CModel(CModel&& _rOther) noexcept : m_lstMeshes(std::move(_rOther.m_lstMeshes))
      , m_lstInstances(std::move(_rOther.m_lstInstances)) 
      , m_uMeshesCount(_rOther.m_uMeshesCount)
      , m_oTransform(_rOther.m_oTransform)
      , m_oLocalAABB(_rOther.m_oLocalAABB)
      , m_oWorldAABB(_rOther.m_oWorldAABB)
      , m_oVtxBufferHandler(_rOther.m_oVtxBufferHandler)
      , m_bCullEnabled(_rOther.m_bCullEnabled)
      , m_bVisible(_rOther.m_bVisible)  
    {
      memcpy(m_sAssetPath, _rOther.m_sAssetPath, strlen(_rOther.m_sAssetPath));
    }
    // ------------------------------------
    render::gfx::CModel& CModel::operator=(CModel&& _rOther) noexcept
    {
      if (this != &_rOther)
      {
        Clear();
        m_lstMeshes = std::move(_rOther.m_lstMeshes);
        m_lstInstances = std::move(_rOther.m_lstInstances);

        m_uMeshesCount = _rOther.m_uMeshesCount;
        m_oTransform = _rOther.m_oTransform;
        m_oLocalAABB = _rOther.m_oLocalAABB;
        m_oWorldAABB = _rOther.m_oWorldAABB;
        m_oVtxBufferHandler = _rOther.m_oVtxBufferHandler;
        m_bCullEnabled = _rOther.m_bCullEnabled;
        m_bVisible = _rOther.m_bVisible;

        strncpy_s(m_sAssetPath, _rOther.m_sAssetPath, sizeof(m_sAssetPath) - 1);
      }
      return *this;
    }
    // ------------------------------------
    CModel::~CModel()
    {
      Clear();
    }
    // ------------------------------------
    void CModel::SetPos(const math::CVector3& _v3Pos)
    {
      // Set pos
      m_oTransform.SetPos(_v3Pos);

      // Update bounding box
      if (m_bCullEnabled)
      {
        collision::ComputeWorldAABB(m_oLocalAABB, m_oTransform, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CModel::SetRot(const math::CVector3& _v3Rot)
    {
      // Set rot
      m_oTransform.SetRot(_v3Rot);

      // Update bounding box
      if (m_bCullEnabled)
      {
        collision::ComputeWorldAABB(m_oLocalAABB, m_oTransform, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CModel::SetScl(const math::CVector3& _v3Scl)
    {
      // Set scale
      m_oTransform.SetScl(_v3Scl);

      // Update bounding box
      if (m_bCullEnabled)
      {
        collision::ComputeWorldAABB(m_oLocalAABB, m_oTransform, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CModel::SetCullEnabled(bool _bCull)
    {
      // Set state
      if (m_bCullEnabled != _bCull)
      {
        m_bCullEnabled = _bCull;
      }

      // Update bounding box
      if (m_bCullEnabled)
      {
        collision::ComputeWorldAABB(m_oLocalAABB, m_oTransform, m_oWorldAABB);
      }
    }
    // ------------------------------------
    const render::gfx::TMeshes& CModel::GetMeshes(uint16_t& _uCount_) const
    {
      _uCount_ = m_uMeshesCount;
      return m_lstMeshes;
    }
    // ------------------------------------
    render::gfx::TMeshes& CModel::GetMeshes(uint16_t& _uCount_)
    {
      _uCount_ = m_uMeshesCount;
      return m_lstMeshes;
    }
    // ------------------------------------
    utils::CWeakPtr<render::gfx::CRenderInstance> CModel::CreateInstance()
    {
      if (!AllowInstancing())
      {
        WARNING_LOG("You have reached maximum instances in this model!");
        return utils::CWeakPtr<render::gfx::CRenderInstance>();
      }

      // Create instance
      uint16_t uInstanceID = static_cast<uint16_t>(m_lstInstances.GetSize());
      return m_lstInstances.Create(this, uInstanceID);
    }
    // ------------------------------------
    bool CModel::RemoveInstance(utils::CWeakPtr<render::gfx::CRenderInstance> _pInstance_)
    {
      bool bOk = m_lstInstances.Remove(_pInstance_);
      UNUSED_VAR(bOk);
#ifdef _DEBUG
      assert(bOk);
#endif
      // Sort ids
      uint16_t uSize = static_cast<uint16_t>(m_lstInstances.GetSize());
      for (uint16_t uIndex = 0; uIndex < uSize; uIndex++)
      {
        utils::CWeakPtr<render::gfx::CRenderInstance> pInstance = m_lstInstances[uIndex];
        pInstance->SetInstanceID(uIndex);
      }
      return bOk;
    }
    // ------------------------------------
    void CModel::Clear()
    {
      // Clear instances
      m_lstInstances.Clear();
    }
  }
}