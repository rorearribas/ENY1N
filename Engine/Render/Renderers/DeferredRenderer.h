#pragma once
#include "Engine/Render/Resources/RenderTarget.h"
#include "Renderer.h"

namespace scene { class CRenderScene; }

namespace render
{
  class CDeferredRenderer : public IRenderer
  {
  public:
    CDeferredRenderer(CRender* _pRender) : IRenderer(_pRender) {}
    ~CDeferredRenderer() { Release(); }

    HRESULT Init(uint32_t _uWidth, uint32_t _uHeight);

    void PrepareFrame() override;
    void Draw(scene::CRenderScene& _rRenderScene) override;

    CRenderTarget& GetDiffuseRT() const { return *m_pDiffuseRT; }
    CRenderTarget& GetNormalRT() const { return *m_pNormalRT; }
    CRenderTarget& GetSpecularRT() const { return *m_pSpecularRT; }

    ID3D11DepthStencilView* GetDepthStencilView() const { return m_oDepthStencil.GetView(); }
    ID3D11ShaderResourceView* GetShaderResourceView() const { return m_oDepthStencilShader.GetView(); }
    ID3D11DepthStencilState* GetDepthStencilState() const { return m_pDepthStencilState; }

  private:
    HRESULT SetupRenderTargets(uint32_t _uWidth, uint32_t _uHeight);
    void Release();

  private:
    std::unique_ptr<CRenderTarget> m_pDiffuseRT;
    std::unique_ptr<CRenderTarget> m_pNormalRT;
    std::unique_ptr<CRenderTarget> m_pSpecularRT;

    ID3D11DepthStencilState* m_pDepthStencilState = nullptr;
    render::texture::TDepthStencil m_oDepthStencil;
    render::texture::TShaderResource m_oDepthStencilShader;
  };
}


