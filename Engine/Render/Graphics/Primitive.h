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
  namespace graphics
  {
    class CPrimitive
    {
    public:
      enum EPrimitiveType
      {
        E2D_SQUARE,
        E2D_TRIANGLE,
        E3D_PLANE,
        E3D_CUBE,
        E3D_SPHERE,
        CUSTOM,
        INVALID
      };

      struct SCustomPrimitive
      {
        std::vector<render::graphics::SVertexData> m_vctVertexData;
        std::vector<uint32_t> m_vctIndices;
      };

    public:
      CPrimitive(const SCustomPrimitive& _oVertexData, render::ERenderMode _eRenderMode = SOLID);
      CPrimitive(EPrimitiveType _ePrimitiveType, render::ERenderMode _eRenderMode = SOLID);
      ~CPrimitive();

      void DrawPrimitive();
      void SetRenderMode(render::ERenderMode _eRenderMode);

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
      HRESULT CreateBufferFromPrimitiveData(const std::vector<render::graphics::SVertexData>&, const std::vector<uint32_t>&);
      HRESULT CreateInputLayout();

      // Primitive data
      CConstantBuffer<SConstantMatrix> m_oConstantBuffer;
      CConstantBuffer<SConstantModelData> m_oConstantModelData;

      ID3D11Buffer* m_pVertexBuffer = nullptr;
      ID3D11Buffer* m_pIndexBuffer = nullptr;
      ID3D11InputLayout* m_pInputLayout = nullptr;

      // Data
      render::ERenderMode m_eRenderMode = SOLID;
      CPrimitive::EPrimitiveType m_ePrimitiveType = CPrimitive::EPrimitiveType::INVALID;

      math::CTransform m_oTransform = math::CTransform();
      math::CVector3 m_v3Color = math::CVector3::One;

      uint32_t m_uVertices = 0;
      uint32_t m_uIndices = 0;
    };
  }
}
