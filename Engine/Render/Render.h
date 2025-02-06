#pragma once
#include "Window/RenderWindow.h"
#include "Engine/Camera/Camera.h"
#include "Shader.h"

namespace scene { class CScene; }

namespace render
{
  class CRender
  {
  private:
    struct SRenderingResources
    {
      // Swap and main render target
      IDXGISwapChain* m_pSwapChain = nullptr;
      ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
      // Depth stencil
      ID3D11Texture2D* m_pDepthStencilTexture = nullptr;
      ID3D11DepthStencilState* m_pDepthStencilState = nullptr;
      ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
      // Rasterizer
      ID3D11RasterizerState* m_pRasterizerState = nullptr;
      // Blend
      ID3D11BlendState* m_pBlendState = nullptr;
      // Shaders for 3D Pipeline
      shader::CShader<shader::EShaderType::PIXEL_SHADER>* m_pForwardPS;
      shader::CShader<shader::EShaderType::VERTEX_SHADER>* m_pForwardVS;
      // Shaders for primitives
      shader::CShader<shader::EShaderType::PIXEL_SHADER>* m_pPrimitivesPS;
      shader::CShader<shader::EShaderType::VERTEX_SHADER>* m_pPrimitivesVS;
    };

  public:
    CRender(uint32_t _uX, uint32_t _uY);
    ~CRender();

    void BeginDraw();
    void Draw(scene::CScene* _pScene);
    void EndDraw();

    render::CRenderWindow* GetRenderWindow() const { return m_pRenderWindow; }
    void SetVSync(bool _bEnabled) { m_bVerticalSync = _bEnabled; }
    bool IsVSyncEnabled() { return m_bVerticalSync; }

  private:
    void OnWindowResizeEvent(uint32_t _uX, uint32_t _uY);

    HRESULT Init(uint32_t _uX, uint32_t _uY);
    HRESULT CreateDevice(uint32_t _uX, uint32_t _uY);
    HRESULT InitBasicPipeline(uint32_t _uX, uint32_t _uY);

    HRESULT CreateRenderTargetView();
    HRESULT CreateDepthStencilView(uint32_t _uX, uint32_t _uY);
    HRESULT CreateRasterizerState(D3D11_FILL_MODE _eFillMode = D3D11_FILL_SOLID);
    HRESULT CreateBlendState();

    void ConfigureViewport(uint32_t _uX, uint32_t _uY);
    void SetScissorRect(uint32_t _uX, uint32_t _uY);
    bool InitImGui();

    // Data
    render::CRenderWindow* m_pRenderWindow = nullptr;
    SRenderingResources m_oRenderingResources = {};
    bool m_bVerticalSync = false;
  };
}


