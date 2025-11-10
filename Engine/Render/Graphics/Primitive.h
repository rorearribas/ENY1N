#pragma once
#include "Engine/Render/RenderTypes.h"
#include "Engine/Collisions/BoundingBox.h"

#include "Libs/Math/Vector3.h"
#include "Libs/Math/Matrix4x4.h"
#include "Libs/Math/Transform.h"

#include <vector>
#include "Mesh.h"

namespace render
{
  namespace gfx
  {
    struct SCustomPrimitive
    {
      std::vector<render::gfx::SVertexData> m_lstVertexData;
      std::vector<uint32_t> m_lstIndices;
    };

    class CPrimitive
    {
    public:
      CPrimitive(SCustomPrimitive& _oData, render::ERenderMode _eRenderMode = ERenderMode::SOLID);
      CPrimitive(EPrimitiveType _eType, render::ERenderMode _eRenderMode = ERenderMode::SOLID);
      ~CPrimitive();

      void Draw();

      void SetRenderMode(ERenderMode _eRenderMode);
      inline const ERenderMode& GetRenderMode() const { return m_eRenderMode; }

      inline void SetVisible(bool _bVisible) { m_bVisible = _bVisible; }
      inline const bool& IsVisible() const { return m_bVisible; }

      void SetCullingEnabled(bool _bCull);
      inline const bool& IsCullingEnabled() const { return m_bCullingEnabled; }

      void SetPosition(const math::CVector3& _v3Pos);
      inline const math::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
      void SetRotation(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
      void SetScale(const math::CVector3& _v3Scl);
      inline const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }
      void SetColor(const math::CVector3& _v3Color);
      inline const math::CVector3& GetColor() const { return m_v3Color; }

      inline const EPrimitiveType& GetPrimitiveType() const { return m_ePrimitiveType; }
      inline const collision::CBoundingBox& GetBoundingBox() const { return m_oBoundingBox; }

    private:
      HRESULT CreatePrimitive(EPrimitiveType _ePrimitiveType, render::ERenderMode _eRenderMode);
      HRESULT CreateBuffer(const std::vector<render::gfx::SVertexData>& _lstVertexData, const std::vector<uint32_t>& _lstIndices);
      HRESULT CalculateBoundingBox();
      void Clear();

    private:
      // Primitive data
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      ID3D11Buffer* m_pIndexBuffer = nullptr;

      // Data
      ERenderMode m_eRenderMode = ERenderMode::SOLID;
      EPrimitiveType m_ePrimitiveType = EPrimitiveType::INVALID;

      math::CTransform m_oTransform = math::CTransform();
      collision::CBoundingBox m_oBoundingBox = collision::CBoundingBox();

      math::CVector3 m_v3Color = math::CVector3::One;
      bool m_bCullingEnabled = true;
      bool m_bVisible = true;

      uint32_t m_uVertices = 0;
      uint32_t m_uIndices = 0;

    };
  }
}
