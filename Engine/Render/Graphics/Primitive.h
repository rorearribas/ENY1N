#pragma once
#include "Libs/Maths/Vector3.h"
#include "Libs/Maths/Matrix4x4.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Libs/Maths/Transform.h"
#include "Engine/Render/Render.h"
#include <d3d11.h>
#include <vector>

namespace render
{
  namespace graphics
  {
    class CPrimitive
    {
    public:
      enum EPrimitiveType { E2D_SQUARE, E2D_TRIANGLE, E3D_PLANE, E3D_CUBE, E3D_SPHERE, INVALID };

      struct SPrimitiveData
      {
        maths::CVector3 Position = maths::CVector3::Zero;
        maths::CVector3 Color = maths::CVector3::One;
      };

    public:
      CPrimitive(EPrimitiveType _ePrimitiveType, render::ERenderMode _eRenderMode = SOLID);
      ~CPrimitive();

      void DrawPrimitive();
      void SetRenderMode(render::ERenderMode _eRenderMode);

      void SetPosition(const maths::CVector3& _v3Position) { m_oPrimitiveTransform.SetPosition(_v3Position); }
      const maths::CVector3& GetPosition() const { return m_oPrimitiveTransform.GetPosition(); }
      void SetRotation(const maths::CVector3& _v3Rot) { m_oPrimitiveTransform.SetRotation(_v3Rot); }
      const maths::CVector3& GetRotation() const { return m_oPrimitiveTransform.GetRotation(); }
      void SetScale(const maths::CVector3& _v3Scale) { m_oPrimitiveTransform.SetScale(_v3Scale); }
      const maths::CVector3& GetScale() const { return m_oPrimitiveTransform.GetScale(); }
      void SetColor(const maths::CVector3& _v3Color);
      const maths::CVector3& GetColor() const { return m_v3CurrentColor; }

    private:
      HRESULT CreatePrimitive(EPrimitiveType _ePrimitiveType, render::ERenderMode);
      HRESULT CreateBufferFromVertexData(const std::vector<CPrimitive::SPrimitiveData>&, const std::vector<UINT>&);
      HRESULT CreateInputLayout();

      // Primitive data
      CConstantBuffer<SConstantMatrix> m_oConstantBuffer;
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      ID3D11Buffer* m_pIndexBuffer = nullptr;
      ID3D11InputLayout* m_pInputLayout = nullptr;

      // Data
      render::ERenderMode m_eRenderMode = SOLID;
      EPrimitiveType m_ePrimitiveType = EPrimitiveType::INVALID;

      maths::CTransform m_oPrimitiveTransform;
      maths::CVector3 m_v3CurrentColor = maths::CVector3::One;

      uint32_t m_uVertices = 0;
      uint32_t m_uIndices = 0;
    };
  }
}
