#pragma once
#include "Libs/Math/Vector3.h"
#include "Libs/Math/Matrix4x4.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Libs/Math/Transform.h"
#include "Engine/Render/Render.h"
#include <d3d11.h>
#include <vector>
#include "Mesh.h"

namespace render
{
  namespace gfx
  {
    enum class EPrimitiveType
    {
      E2D_SQUARE,
      E2D_CIRCLE,
      E2D_TRIANGLE,
      E3D_PLANE,
      E3D_CUBE,
      E3D_SPHERE,
      E3D_CAPSULE,
      CUSTOM,
      INVALID
    };

    struct SCustomPrimitive
    {
      std::vector<render::gfx::SVertexData> m_vctVertexData;
      std::vector<uint32_t> m_vctIndices;
    };

    class CPrimitive
    {
    public:
      CPrimitive(SCustomPrimitive& _oData, render::ERenderMode _eRenderMode = ERenderMode::SOLID);
      CPrimitive(EPrimitiveType _eType, render::ERenderMode _eRenderMode = ERenderMode::SOLID);
      ~CPrimitive();

      void Draw();

      void SetPosition(const math::CVector3& _v3Pos);
      inline const math::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
      void SetRotation(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
      void SetScale(const math::CVector3& _v3Scl);
      inline const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }

      void SetColor(const math::CVector3& _v3Color);
      inline const math::CVector3& GetColor() const { return m_v3Color; }

      void SetRenderMode(ERenderMode _eRenderMode);
      inline const ERenderMode& GetRenderMode() const { return m_eRenderMode; }
      inline const EPrimitiveType& GetPrimitiveType() const { return m_ePrimitiveType; }

      inline void SetIgnoreLighting(bool _bIgnore) { m_bIgnoreLighting = _bIgnore; }
      void SetCullingEnabled(bool _bCull);
      inline const bool& IsCullingEnabled() const { return m_bCullingEnabled; }

      inline void SetBoundingBox(const collision::CBoundingBox& _oBoundingBox) { m_oBoundingBox = _oBoundingBox; }
      inline const collision::CBoundingBox& GetBoundingBox() const { return m_oBoundingBox; }

    private:
      HRESULT CreatePrimitive(EPrimitiveType _ePrimitiveType, render::ERenderMode _eRenderMode);
      HRESULT CreateBuffer(const std::vector<render::gfx::SVertexData>& _vctVertexData, const std::vector<uint32_t>& _vctIndices);
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
      collision::CBoundingBox m_oBoundingBox;

      math::CVector3 m_v3Color = math::CVector3::One;
      bool m_bIgnoreLighting = false;
      bool m_bCullingEnabled = true;

      uint32_t m_uVertices = 0;
      uint32_t m_uIndices = 0;

    };
  }
}
