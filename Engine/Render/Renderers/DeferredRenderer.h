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

    void DrawOpaques(void) {}
    void ComputeGraphicsBuffer(void) {}

    void AttachRenderTargets(ID3D11DepthStencilView* _pDepthStencilView);
    void DetachRenderTargets();

    ID3D11ShaderResourceView* GetDepthTexture() const { return m_oDepthStencilResource.GetView(); }
    const CRenderTarget* GetDiffuseRT() const { return m_pDiffuseRT.get(); }
    const CRenderTarget* GetNormalRT() const { return m_pNormalRT.get(); }
    const CRenderTarget* GetSpecularRT() const { return m_pSpecularRT.get(); }

    void ClearRenderTargets(const float _v4ClearColor[4]);

  private:
    HRESULT SetupRenderTargets(uint32_t _uWidth, uint32_t _uHeight);
    void Release();

  private:
    std::unique_ptr<CRenderTarget> m_pDiffuseRT;
    std::unique_ptr<CRenderTarget> m_pNormalRT;
    std::unique_ptr<CRenderTarget> m_pSpecularRT;

    ID3D11DepthStencilState* m_pDepthStencilState = nullptr;
    render::texture::TDepthStencil m_oDepthStencilTexture;
    render::texture::TShaderResource m_oDepthStencilResource;
  };
}


