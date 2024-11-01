#pragma once
#include <d3d11.h>
#include <vector>

namespace render
{
  namespace items
  {
    class CPrimitiveItem {
    public:
      CPrimitiveItem(std::vector<float>& _vctVertexData);
      ~CPrimitiveItem();

      UINT GetIndexCount() { return (UINT)m_vctVertexData.size(); }
      ID3D11Buffer* GetBuffer() { return m_pBuffer; }
      ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; }

      ID3D11VertexShader* GetVertexShader() { return m_pVertexShader; }
      ID3D11PixelShader* GetPixelShader() { return m_pPixelShader; }

    private:
      HRESULT CompileShaders();
      HRESULT InitShaders(ID3D11Device* _pDevice);
      HRESULT CreateInputLayout(ID3D11Device* _pDevice);
      HRESULT CreateBufferFromVertexData(ID3D11Device* _pDevice, std::vector<float>& _vctVertexData);

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
      // Vertex data
      std::vector<float> m_vctVertexData = {};
    };
  }
}
