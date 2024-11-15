#pragma once
#include <d3d11.h>
#include <vector>
#include "Libs/Maths/Vector3.h"
#include "Libs/Maths/Matrix4x4.h"
#include "Engine/Render/ConstantBuffer.h"

namespace render
{
  namespace primitive
  {
    class CPrimitive 
    {
    private:
      static const maths::CVector3 s_vDefaultColor;

    public:
      enum EPrimitiveType { RECTANGLE, TRIANGLE };
      struct SPrimitiveInfo
      {
        maths::CVector3 Position = maths::CVector3::Zero;
        maths::CVector3 Color = s_vDefaultColor;
      };

      CPrimitive(const EPrimitiveType& _ePrimitiveType);
      CPrimitive(const std::vector<SPrimitiveInfo>& _vctVertexData);
      ~CPrimitive();

      void SetPosition(const maths::CVector3& _v3Position) { m_vPos = _v3Position; }
      const maths::CVector3& GetPosition() const { return m_vPos; }

      void SetColor(const maths::CVector3& _v3Color);
      const maths::CVector3& GetColor() const { return m_v3Color; }

      ID3D11Buffer* GetBuffer() { return m_pBuffer; }
      ConstantBuffer<SConstantBuffer>& GetConstantBuffer() { return m_oConstantBuffer; }

      UINT GetIndexCount() { return m_uVertexCount; }
      ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; }

      ID3D11VertexShader* GetVertexShader() { return m_pVertexShader; }
      ID3D11PixelShader* GetPixelShader() { return m_pPixelShader; }

    private:
      HRESULT InitPrimitive();
      HRESULT CompileShaders();
      HRESULT InitShaders();
      HRESULT CreateInputLayout();
      HRESULT CreateBufferFromVertexData(const std::vector<SPrimitiveInfo>& _vctPrimitiveInfo);

      // Buffers
      ConstantBuffer<SConstantBuffer> m_oConstantBuffer;
      ID3D11Buffer* m_pBuffer = nullptr;
      // Vertex shader
      ID3DBlob* m_pVertexShaderBlob = nullptr;
      ID3D11VertexShader* m_pVertexShader = nullptr;
      // Pixel shader
      ID3DBlob* m_pPixelShaderBlob = nullptr;
      ID3D11PixelShader* m_pPixelShader = nullptr;
      // Input layout
      ID3D11InputLayout* m_pInputLayout = nullptr;

      // Info
      maths::CVector3 m_v3Color = s_vDefaultColor;
      maths::CVector3 m_vPos = maths::CVector3::Zero;
      UINT m_uVertexCount = 0;
    };
  }
}
