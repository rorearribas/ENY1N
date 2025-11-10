#include "RenderInstance.h"
#include "Model.h"

namespace render
{
  namespace gfx
  {
    void CRenderInstance::Draw()
    {
      // Not implemented yet!
      m_oBoundingBox.DrawDebug();
    }
    // ------------------------------------
    void CRenderInstance::SetPosition(const math::CVector3& _v3Pos)
    {
      // Set pos
      m_oTransform.SetPosition(_v3Pos);

      // Update bounding box
      if (m_bCullEnabled)
      {
        m_pModel->ComputeBoundingBox(m_oTransform.CreateTransform(), m_oBoundingBox);
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
        m_pModel->ComputeBoundingBox(m_oTransform.CreateTransform(), m_oBoundingBox);
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
        m_pModel->ComputeBoundingBox(m_oTransform.CreateTransform(), m_oBoundingBox);
      }
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
        m_pModel->ComputeBoundingBox(m_oTransform.CreateTransform(), m_oBoundingBox);
      }
    }
  }
}

