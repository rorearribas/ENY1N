#pragma once
#include "Engine/Render/Buffers/RenderBuffer.h"
#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Collisions/AABB.h"

#include "Libs/Math/Vector3.h"
#include "Libs/Math/Matrix4x4.h"
#include "Libs/Math/Transform.h"

#include <vector>

namespace render
{
  namespace gfx
  {
    class CPrimitive
    {
    public:
      CPrimitive() = default;
      ~CPrimitive() = default;

      CPrimitive(CPrimitive&& _rOther) noexcept;
      CPrimitive& operator=(CPrimitive&& _rOther) noexcept;
      CPrimitive(const CPrimitive& _rOther) = delete;
      CPrimitive& operator=(const CPrimitive& _rOther) = delete;

      void SetPos(const math::CVector3& _v3Pos);
      inline const math::CVector3& GetPos() const { return m_oTransform.GetPos(); }
      void SetRot(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRot() const { return m_oTransform.GetRot(); }
      void SetScl(const math::CVector3& _v3Scl);
      inline const math::CVector3& GetScl() const { return m_oTransform.GetScl(); }
      inline void SetColor(const math::CVector3& _v3Color) { m_v3Color = _v3Color; }
      inline const math::CVector3& GetColor() const { return m_v3Color; }

      void SetCullEnabled(bool _bCull);
      inline const bool IsCullEnabled() const { return m_bCullEnabled; }

      inline void SetVisible(bool _bVisible) { m_bVisible = _bVisible; }
      inline const bool IsVisible() const { return m_bVisible; }

      inline void SetRenderMode(render::ERenderMode _eRenderMode) { m_eRenderMode = _eRenderMode; }
      inline const ERenderMode& GetRenderMode() const { return m_eRenderMode; }

      inline const math::CMatrix4x4& GetMatrix() const { return m_oTransform.GetMatrix(); }
      inline const math::CTransform& GetTransform() const { return m_oTransform; }

      inline void SetLocalAABB(const collision::CAABB& _rLocalAABB) { m_oLocalAABB = _rLocalAABB; }
      inline const collision::CAABB& GetLocalAABB() const { return m_oLocalAABB; }
      inline void SetWorldAABB(const collision::CAABB& _rWorldAABB) { m_oWorldAABB = _rWorldAABB; }
      inline const collision::CAABB& GetWorldAABB() const { return m_oWorldAABB; }

      // Handlers
      inline const CBufferHandler& GetVtxBufferHandler() const { return m_oVtxBufferHandler; }
      inline void SetVtxBufferHandler(const CBufferHandler& _rBufferHandler) { m_oVtxBufferHandler = _rBufferHandler; }
      inline const CBufferHandler& GetIdxBufferHandler() const { return m_oIdxBufferHandler; }
      inline void SetIdxBufferHandler(const CBufferHandler& _rBufferHandler) { m_oIdxBufferHandler  = _rBufferHandler; }
      inline const uint32_t GetIndexCount() const { return m_oIdxBufferHandler.GetOffset(); }

    private:
      math::CVector3 m_v3Color = math::CVector3::One;

      math::CTransform m_oTransform = math::CTransform();
      collision::CAABB m_oWorldAABB = collision::CAABB();
      collision::CAABB m_oLocalAABB = collision::CAABB();

      ERenderMode m_eRenderMode = ERenderMode::SOLID;
      CBufferHandler m_oVtxBufferHandler = CBufferHandler();
      CBufferHandler m_oIdxBufferHandler = CBufferHandler();

      bool m_bCullEnabled = true;
      bool m_bVisible = true;
    };
  }
}
