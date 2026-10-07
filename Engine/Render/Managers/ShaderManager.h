#pragma once
#include "Engine/Shaders/Shader.h"
#include <array>

namespace render
{
  struct TShaderProgramData
  {
    // Vertex shader is mandatory
    const unsigned char* pVertexShader = nullptr;
    size_t tVertexShaderSize = 0u;

    // Pixel shader is optional, for example, for the quad program
    const unsigned char* pPixelShader = nullptr;
    size_t tPixelShaderSize = 0u;
  };

  struct TShaderProgram
  {
    shader::CShader<EShader::E_VERTEX> Vertex;
    shader::CShader<EShader::E_PIXEL> Pixel;

    inline uintptr_t ProgramID() const noexcept
    {
      uintptr_t h1 = reinterpret_cast<uintptr_t>(Vertex.GetShader());
      uintptr_t h2 = reinterpret_cast<uintptr_t>(Pixel.GetShader());

      // boost::hash_combine for 64 bits
      if constexpr (sizeof(uintptr_t) >= 8)
      {
        return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
      }
      else // boost::hash_combine for 32 bits
      {
        return h1 ^ (h2 + 0x9e3779b9U + (h1 << 6) + (h1 >> 2));
      }
    }
  };

  class CShaderManager
  {
  public:
    typedef std::unordered_map<uintptr_t, TShaderProgram> TPrograms;

    CShaderManager() = default;
    ~CShaderManager() = default;

    HRESULT RegisterProgram(const CRenderDeviceDX11& _rRenderDevice, const TShaderProgramData& _rProgramData, uintptr_t& _uProgramID);
    void BindProgram(const CRenderCommandsDX11& _rCommands, uintptr_t _uProgramID) const;

    const TShaderProgram& FindProgram(uintptr_t _uProgramID) const;
    inline const TPrograms& GetPrograms() const { return m_lstPrograms; }

  private:
    HRESULT InitProgram(const CRenderDeviceDX11& _rRenderDevice, TShaderProgram& _rShaderProgram, const TShaderProgramData& _rProgramData);
    TPrograms m_lstPrograms;
  };
}

namespace std
{
  template <>
  struct hash<render::TShaderProgram>
  {
    size_t operator()(const render::TShaderProgram& _rShaderProgram) const
    {
      return _rShaderProgram.ProgramID();
    }
  };
}