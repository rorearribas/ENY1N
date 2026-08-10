#include "Decal.h"
#include "Libs/Math/Math.h"

namespace render
{
  namespace gfx
  {
    // ------------------------------------
    CDecal::CDecal(CDecal&& _rOther) noexcept : m_oDecalTexture(std::move(_rOther.m_oDecalTexture))
      , m_oTransform(_rOther.m_oTransform)
      , m_oLocalAABB(_rOther.m_oLocalAABB)
      , m_oWorldAABB(_rOther.m_oWorldAABB)
      , m_bCullEnabled(_rOther.m_bCullEnabled)
      , m_bVisible(_rOther.m_bVisible)
    {
    }
    // ------------------------------------
    render::gfx::CDecal& CDecal::operator=(CDecal&& _rOther) noexcept
    {
      if (this != &_rOther)
      {
        Release();

        m_oDecalTexture = std::move(_rOther.m_oDecalTexture);
        m_oTransform = _rOther.m_oTransform;
        m_oLocalAABB = _rOther.m_oLocalAABB;
        m_oWorldAABB = _rOther.m_oWorldAABB;
        m_bCullEnabled = _rOther.m_bCullEnabled;
        m_bVisible = _rOther.m_bVisible;
      }
      return *this;
    }
    // ------------------------------------
    void CDecal::Release()
    {
      m_oDecalTexture.Release();
    }
    // ------------------------------------
    void CDecal::SetPos(const math::CVector3& _v3Pos)
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
    void CDecal::SetRot(const math::CVector3& _v3Rot)
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
    void CDecal::SetBoundingBox(const collision::CAABB& _rBoundingBox)
    {
      // Set bounding box
      m_oLocalAABB = _rBoundingBox;

      // Update bounding box
      if (m_bCullEnabled)
      {
        collision::ComputeWorldAABB(m_oLocalAABB, m_oTransform, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CDecal::SetCullEnabled(bool _bCull)
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
  }
}

