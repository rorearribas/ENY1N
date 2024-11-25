#pragma once
#include <d3d11.h>
#include <vector>
#include "Libs/Maths/Vector3.h"
#include "Libs/Maths/Matrix4x4.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Libs/Maths/Transform.h"

namespace render
{
  namespace graphics
  {
    class CPrimitive {
    public:
      enum EPrimitiveType { SQUARE, TRIANGLE, CUBE };

      struct SPrimitiveInfo
      {
        maths::CVector3 Position = maths::CVector3::Zero;
        maths::CVector3 Color = maths::CVector3::One;
      };
    public:

      CPrimitive(const EPrimitiveType& _ePrimitiveType);
      CPrimitive(const std::vector<SPrimitiveInfo>& _vctVertexData);
      ~CPrimitive();

      void DrawPrimitive();

      void SetPosition(const maths::CVector3& _v3Position) { m_oPrimitiveTransform.SetPosition(_v3Position); }
      const maths::CVector3& GetPosition() const { return m_oPrimitiveTransform.GetPosition(); }
      void SetRotation(const maths::CVector3& _v3Rot) { m_oPrimitiveTransform.SetRotation(_v3Rot); }
      const maths::CVector3& GetRotation() const { return m_oPrimitiveTransform.GetRotation(); }
      void SetScale(const maths::CVector3& _v3Scale) { m_oPrimitiveTransform.SetScale(_v3Scale); }
      const maths::CVector3& GetScale() const { return m_oPrimitiveTransform.GetScale(); }
      void SetColor(const maths::CVector3& _v3Color);
      const maths::CVector3& GetColor() const { return m_v3CurrentColor; }

    private:
      HRESULT InitPrimitive();
      HRESULT CompileShaders();
      HRESULT InitShaders();
      HRESULT CreateInputLayout();
      HRESULT CreateBufferFromVertexData(const std::vector<CPrimitive::SPrimitiveInfo>& _vctPrimitiveInfo, const std::vector<UINT>& _vctIndexes = {});

      // Buffers
      ConstantBuffer<SConstantBuffer> m_oConstantBuffer;
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      ID3D11Buffer* m_pIndexBuffer = nullptr;
      // Vertex shader
      ID3DBlob* m_pVertexShaderBlob = nullptr;
      ID3D11VertexShader* m_pVertexShader = nullptr;
      // Pixel shader
      ID3DBlob* m_pPixelShaderBlob = nullptr;
      ID3D11PixelShader* m_pPixelShader = nullptr;
      // Input layout
      ID3D11InputLayout* m_pInputLayout = nullptr;

      // Data
      maths::CTransform m_oPrimitiveTransform;
      maths::CVector3 m_v3CurrentColor = maths::CVector3::One;
      UINT m_uVertexCount = 0;
    };
  }
}
