#pragma once
#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Collisions/BoundingBox.h"

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
      CPrimitive(TCustomPrimitive& _oData, render::ERenderMode _eRenderMode = ERenderMode::SOLID);
      CPrimitive(EPrimitiveType _eType, render::ERenderMode _eRenderMode = ERenderMode::SOLID);
      ~CPrimitive();

      void Draw();

      void SetCullingEnabled(bool _bCull);
      inline const bool& IsCullingEnabled() const { return m_bCullingEnabled; }

      inline const collision::CBoundingBox& GetWorldBoudingBox() const { return m_oWorldAABB; }
      inline const collision::CBoundingBox& GetLocalBoundingBox() const { return m_oLocalAABB; }
      inline const EPrimitiveType& GetPrimitiveType() const { return m_ePrimitiveType; }

      void SetColor(const math::CVector3& _v3Color);
      inline const math::CVector3& GetColor() const { return m_v3Color; }
      void SetPosition(const math::CVector3& _v3Pos);
      inline const math::CVector3& GetPosition() const { return m_oTransform.GetTranslation(); }
      void SetRotation(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
      void SetScale(const math::CVector3& _v3Scl);
      inline const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }

      void SetRenderMode(ERenderMode _eRenderMode);
      inline const ERenderMode& GetRenderMode() const { return m_eRenderMode; }

      inline void SetVisible(bool _bVisible) { m_bVisible = _bVisible; }
      inline const bool& IsVisible() const { return m_bVisible; }

    private:
      HRESULT CreatePrimitive(EPrimitiveType _ePrimitiveType, render::ERenderMode _eRenderMode);
      HRESULT CreateBuffer(const std::vector<render::gfx::TPrimitiveData>& _lstPrimitiveData, const std::vector<uint32_t>& _lstIndices);
      void Clear();

    private:
      // Buffers
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      ID3D11Buffer* m_pIndexBuffer = nullptr;

    private:
      // Data
      ERenderMode m_eRenderMode = ERenderMode::SOLID;
      EPrimitiveType m_ePrimitiveType = EPrimitiveType::INVALID;

      math::CTransform m_oTransform = math::CTransform();
      collision::CBoundingBox m_oWorldAABB = collision::CBoundingBox();
      collision::CBoundingBox m_oLocalAABB = collision::CBoundingBox();

      math::CVector3 m_v3Color = math::CVector3::One;
      bool m_bCullingEnabled = true;
      bool m_bVisible = true;

      uint32_t m_uVertices = 0;
      uint32_t m_uIndices = 0;
    };
  }
}
