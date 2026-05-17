#pragma once
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
      CPrimitive(TCustomPrimitive& _rData, render::ERenderMode _eRenderMode = ERenderMode::SOLID);
      CPrimitive(EPrimitive _eType, render::ERenderMode _eRenderMode = ERenderMode::SOLID);
      ~CPrimitive();

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

      void SetRenderMode(render::ERenderMode _eRenderMode);
      inline const ERenderMode& GetRenderMode() const { return m_eRenderMode; }
      inline const EPrimitive& GetPrimitiveType() const { return m_ePrimitiveType; }

      inline const math::CMatrix4x4& GetMatrix() const { return m_oTransform.GetMatrix(); }
      inline const math::CTransform& GetTransform() const { return m_oTransform; }

      inline const collision::CAABB& GetWorldAABB() const { return m_oWorldAABB; }
      inline const collision::CAABB& GetLocalAABB() const { return m_oLocalAABB; }

      inline ID3D11Buffer* GetVertexBuffer() const { return m_pVertexBuffer; }
      inline ID3D11Buffer* GetIndexBuffer() const { return m_pIndexBuffer; }

      inline const uint32_t& GetIndices() const { return m_uIndices; }
      inline const uint32_t& GetVerticesCount() const { return m_uVertices; }

    private:
      void Clear();
      HRESULT CreatePrimitive(EPrimitive _ePrimitiveType, render::ERenderMode _eRenderMode);
      HRESULT CreateBuffer(const std::vector<math::CVector3>& _lstPrimitiveData, const std::vector<uint32_t>& _lstIndices);

    private:
      // Buffers
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      ID3D11Buffer* m_pIndexBuffer = nullptr;

      // Data
      ERenderMode m_eRenderMode = ERenderMode::SOLID;
      EPrimitive m_ePrimitiveType = EPrimitive::INVALID;

      math::CTransform m_oTransform = math::CTransform();
      collision::CAABB m_oWorldAABB = collision::CAABB();
      collision::CAABB m_oLocalAABB = collision::CAABB();

      math::CVector3 m_v3Color = math::CVector3::One;
      bool m_bCullEnabled = true;
      bool m_bVisible = true;

      uint32_t m_uVertices = 0;
      uint32_t m_uIndices = 0;
    };
  }
}
