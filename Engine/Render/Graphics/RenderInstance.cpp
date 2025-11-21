#include "RenderInstance.h"
#include "Libs/Math/Math.h"
#include "Model.h"

namespace render
{
  namespace gfx
  {
    CRenderInstance::CRenderInstance(const render::gfx::CModel* _pParent, uint32_t _uId) 
    : m_pParent(_pParent)
    , m_uInstanceID(_uId)
    {
      assert(_uId != render::instance::s_uInvalidID);
    }
    // ------------------------------------
    void CRenderInstance::SetCullingEnabled(bool _bCull)
    {
      // Set state
      if (m_bCullEnabled != _bCull)
      {
        m_bCullEnabled = _bCull;
      }

      // Update bounding box
      if (m_bCullEnabled)
      {
        ComputeWorldAABB(m_pParent->GetLocalBoundingBox(), m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CRenderInstance::SetPosition(const math::CVector3& _v3Pos)
    {
      // Set pos
      m_oTransform.SetPosition(_v3Pos);

      // Update bounding box
      if (m_bCullEnabled)
      {
        ComputeWorldAABB(m_pParent->GetLocalBoundingBox(), m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CRenderInstance::SetRotation(const math::CVector3& _v3Rot)
    {
      // Set rot
      m_oTransform.SetRotation(_v3Rot);

      // Update bounding box
      if (m_bCullEnabled)
      {
        ComputeWorldAABB(m_pParent->GetLocalBoundingBox(), m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CRenderInstance::SetScale(const math::CVector3& _v3Scl)
    {
      // Set scale
      m_oTransform.SetScale(_v3Scl);

      // Update bounding box
      if (m_bCullEnabled)
      {
        ComputeWorldAABB(m_pParent->GetLocalBoundingBox(), m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CRenderInstance::ComputeWorldAABB(const collision::CBoundingBox& _rLocalAABB, collision::CBoundingBox& _rWorldAABB_)
    {
      math::CVector3 lstExtents[8];
      _rLocalAABB.GetExtents(lstExtents);

      // Compute world AABB
      math::CVector3 v3Min(FLT_MAX, FLT_MAX, FLT_MAX);
      math::CVector3 v3Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

      for (uint32_t uIndex = 0; uIndex < 8; uIndex++)
      {
        // Calculate extent pos
        math::CVector3 v3Extent = m_oTransform.GetMatrix() * lstExtents[uIndex];

        // Calculate Min
        v3Min.x = math::Min(v3Min.x, v3Extent.x);
        v3Min.y = math::Min(v3Min.y, v3Extent.y);
        v3Min.z = math::Min(v3Min.z, v3Extent.z);

        // Calculate Max
        v3Max.x = math::Max(v3Max.x, v3Extent.x);
        v3Max.y = math::Max(v3Max.y, v3Extent.y);
        v3Max.z = math::Max(v3Max.z, v3Extent.z);
      }

      // Set world bounding
      _rWorldAABB_ = collision::CBoundingBox(v3Min, v3Max);
    }
  }
}

