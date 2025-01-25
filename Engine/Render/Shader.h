#pragma once
#include <d3d11.h>
#include <type_traits>

namespace render
{
  namespace shader
  {
    struct SVertexShader
    {
      ID3D11VertexShader* m_pVertexShader = nullptr;
      HRESULT CreateShader();
      void Release();
    };

    struct SPixelShader
    {
      ID3D11PixelShader* m_pPixelShader = nullptr;
      HRESULT CreateShader();
      void Release();
    };

    template<typename T, typename = std::enable_if_t<std::is_same_v<T, render::shader::SVertexShader> ||
      std::is_same_v<T, render::shader::SPixelShader>>>
    class CShader
    {
    public:
      CShader() { m_oShaderData.CreateShader(); }
      ~CShader() { m_oShaderData.Release(); }

      const T& GetData() { return m_oShaderData; }

    private:
      T m_oShaderData;
    };
  }
}

