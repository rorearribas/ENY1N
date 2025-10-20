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

      void SetRenderMode(render::ERenderMode _eRenderMode);
      void IgnoreGlobalLighting(bool _bIgnore); // This is trash!!

      void SetPosition(const math::CVector3& _v3Position) { m_oTransform.SetPosition(_v3Position); }
      const math::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
      void SetRotation(const math::CVector3& _v3Rot) { m_oTransform.SetRotation(_v3Rot); }
      const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
      void SetScale(const math::CVector3& _v3Scale) { m_oTransform.SetScale(_v3Scale); }
      const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }
      void SetColor(const math::CVector3& _v3Color);
      const math::CVector3& GetColor() const { return m_v3Color; }

    private:
      HRESULT CreatePrimitive(EPrimitiveType _ePrimitiveType, render::ERenderMode);
      HRESULT CreateBufferFromData(const std::vector<render::gfx::SVertexData>&, const std::vector<uint32_t>&);
      HRESULT CreateInputLayout();

      // Primitive data
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      ID3D11Buffer* m_pIndexBuffer = nullptr;
      ID3D11InputLayout* m_pInputLayout = nullptr;

      // Data
      render::ERenderMode m_eRenderMode = render::ERenderMode::SOLID;
      gfx::EPrimitiveType m_eType = gfx::EPrimitiveType::INVALID;

      math::CTransform m_oTransform = math::CTransform();
      math::CVector3 m_v3Color = math::CVector3::One;
      bool m_bIgnoreGlobalLighting = false;

      uint32_t m_uVertices = 0;
      uint32_t m_uIndices = 0;
    };
  }
}
