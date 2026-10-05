#pragma once
#include <d3d11.h>
#include "Engine/Render/RenderContext/RenderDeviceDX11.h"
#include "Engine/Render/RenderContext/RenderCommandsDX11.h"
#include "Engine/Render/Resources/Texture2D.h"
#include "Engine/Render/RenderTypes.h"

namespace render
{
  struct TRenderTargetDesc
  {
    DXGI_FORMAT eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    uint32_t uWidth = 0u;
    uint32_t uHeight = 0u;
  };

  class CRenderTarget
  {
  public:
    CRenderTarget() = default;
    ~CRenderTarget() { Release(); }

    CRenderTarget(const CRenderTarget&) = delete;
    CRenderTarget& operator=(const CRenderTarget&) = delete;

    HRESULT Init(const CRenderDeviceDX11& _rRenderDevice, const TRenderTargetDesc& _rDesc);
    void SetClearColor(const CRenderCommandsDX11& _rCommands, const float _v4ClearColor[4]);
    void Release();

    inline ID3D11RenderTargetView* GetRenderTargetView() const { return m_oRTTexture.GetView(); }
    inline ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView; }

  private:
    texture::CTexture2D<render::EView::RENDER_TARGET> m_oRTTexture;
    ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
  };
}