#include "Primitive.h"
#include "Engine/Engine.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Shaders/Forward/SimpleVS.h"
#include "Engine/Render/Utils/PrimitiveUtils.h"

#include "Libs/Macros/GlobalMacros.h"
#include "Libs/Math/Math.h"

#include <cassert>
#include <iostream>

namespace render
{
  namespace gfx
  {
    CPrimitive::CPrimitive(CPrimitive&& _rOther) noexcept : m_v3Color(_rOther.m_v3Color)
    , m_oTransform(_rOther.m_oTransform)
    , m_oWorldAABB(_rOther.m_oWorldAABB)
    , m_oLocalAABB(_rOther.m_oLocalAABB)
    , m_eRenderMode(_rOther.m_eRenderMode)
    , m_oVtxBufferHandler(_rOther.m_oVtxBufferHandler)
    , m_oIdxBufferHandler(_rOther.m_oIdxBufferHandler)
    , m_bCullEnabled(_rOther.m_bCullEnabled)
    , m_bVisible(_rOther.m_bVisible)
    {}
    // ------------------------------------
    render::gfx::CPrimitive& CPrimitive::operator=(CPrimitive&& _rOther) noexcept
    {
      if (this != &_rOther)
      {
        m_v3Color = _rOther.m_v3Color;
        m_oTransform = _rOther.m_oTransform;
        m_oWorldAABB = _rOther.m_oWorldAABB;
        m_oLocalAABB = _rOther.m_oLocalAABB;
        m_eRenderMode = _rOther.m_eRenderMode;
        m_oVtxBufferHandler = _rOther.m_oVtxBufferHandler;
        m_oIdxBufferHandler = _rOther.m_oIdxBufferHandler;
        m_bCullEnabled = _rOther.m_bCullEnabled;
        m_bVisible = _rOther.m_bVisible;
      }
      return *this;
    }
    // ------------------------------------
    void CPrimitive::SetPos(const math::CVector3& _v3Pos)
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
    void CPrimitive::SetRot(const math::CVector3& _v3Rot)
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
    void CPrimitive::SetScl(const math::CVector3& _v3Scl)
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
    void CPrimitive::SetCullEnabled(bool _bCull)
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