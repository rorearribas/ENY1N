#pragma once
#include <d3d11.h>
#include <vector>
#include "Libs/Maths/Vector3.h"

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
        maths::CVector3 m_vPosition = maths::CVector3::Zero;
        maths::CVector3 m_vColor = s_vDefaultColor;
      };

      CPrimitive(const EPrimitiveType& _ePrimitiveType);
      CPrimitive(const std::vector<SPrimitiveInfo>& _vctVertexData);
      ~CPrimitive();

      void SetColor(const maths::CVector3& _v3Color);
      const maths::CVector3& GetColor() const { return m_v3Color; }

      UINT GetIndexCount() { return m_uVertexCount; }
      ID3D11Buffer* GetBuffer() { return m_pBuffer; }
      ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; }

      ID3D11VertexShader* GetVertexShader() { return m_pVertexShader; }
      ID3D11PixelShader* GetPixelShader() { return m_pPixelShader; }

    private:
      HRESULT InitPrimitive();
      HRESULT CompileShaders();
      HRESULT InitShaders(ID3D11Device* _pDevice);
      HRESULT CreateInputLayout(ID3D11Device* _pDevice);
      HRESULT CreateBufferFromVertexData(ID3D11Device* _pDevice, const std::vector<SPrimitiveInfo>& _vctPrimitiveInfo);

      // Vertex buffer
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
      UINT m_uVertexCount = 0;
    };
  }
}