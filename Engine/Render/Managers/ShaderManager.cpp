#include "ShaderManager.h"

namespace render
{
  HRESULT CShaderManager::RegisterProgram(const CRenderDeviceDX11& _rRenderDevice, const TShaderProgramData& _rProgramData, uintptr_t& _uProgramID)
  {
    // Create shader program
    TShaderProgram _rShaderProgram = TShaderProgram();
    HRESULT hResult = InitProgram(_rRenderDevice, _rShaderProgram, _rProgramData);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Emplace the program into the list
    uintptr_t uProgramID = _rShaderProgram.ProgramID();
    auto it = m_lstPrograms.find(uProgramID);
    if (it == m_lstPrograms.end())
    {
      m_lstPrograms[uProgramID] = std::move(_rShaderProgram);
    }

    _uProgramID = uProgramID;
    return S_OK;
  }
  // ------------------------------------
  void CShaderManager::BindProgram(const CRenderCommandsDX11& _rCommands, uintptr_t _uProgramID) const
  {
    auto it = m_lstPrograms.find(_uProgramID);
    if (it == m_lstPrograms.end())
    {
      return;
    }

    // Attach shaders
    const TShaderProgram* pProgram = &it->second;
    _rCommands->VSSetShader(pProgram->Vertex.IsValid() ? pProgram->Vertex.GetShader() : nullptr, nullptr, 0u);
    _rCommands->PSSetShader(pProgram->Pixel.IsValid() ? pProgram->Pixel.GetShader() : nullptr, nullptr, 0u);
  }
  // ------------------------------------
  const render::TShaderProgram& CShaderManager::FindProgram(uintptr_t _uProgramID) const
  {
    auto it = m_lstPrograms.find(_uProgramID);
    if (it == m_lstPrograms.end())
    {
      throw std::runtime_error("Shader program not found!");
    }
    return it->second;
  }
  // ------------------------------------
  HRESULT CShaderManager::InitProgram(const CRenderDeviceDX11& _rRenderDevice, TShaderProgram& _rShaderProgram, const TShaderProgramData& _rProgramData)
  {
    HRESULT hResult = E_FAIL;
    const unsigned char* pVertexShader = _rProgramData.pVertexShader;
    if (pVertexShader != nullptr && _rProgramData.tVertexShaderSize > 0u)
    {
      hResult = _rShaderProgram.Vertex.Init(_rRenderDevice, pVertexShader, _rProgramData.tVertexShaderSize);
    }
    if (FAILED(hResult))
    {
      return hResult;
    }

    const unsigned char* pPixelShader = _rProgramData.pPixelShader;
    if (pPixelShader != nullptr && _rProgramData.tPixelShaderSize > 0u)
    {
      hResult = _rShaderProgram.Pixel.Init(_rRenderDevice, pPixelShader, _rProgramData.tPixelShaderSize);
    }
    return hResult;
  }
}
