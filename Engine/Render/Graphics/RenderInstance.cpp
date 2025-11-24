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
#ifdef _DEBUG
      assert(_uId != render::instance::s_uInvalidID);
#endif // DEBUG
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
        collision::ComputeWorldAABB(m_pParent->GetLocalAABB(), m_oTransform, m_oWorldAABB);
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
        collision::ComputeWorldAABB(m_pParent->GetLocalAABB(), m_oTransform, m_oWorldAABB);
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
        collision::ComputeWorldAABB(m_pParent->GetLocalAABB(), m_oTransform, m_oWorldAABB);
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
        collision::ComputeWorldAABB(m_pParent->GetLocalAABB(), m_oTransform, m_oWorldAABB);
      }
    }
  }
}

