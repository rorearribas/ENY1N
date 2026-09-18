#pragma once
#include "Engine/Scenes/RenderScene.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Shaders/Shader.h"

namespace render { class CRenderWindow; }

namespace render { namespace gfx { class CModel; } }
namespace render { namespace gfx { class CPrimitive; } }
namespace render { namespace mat { class CMaterial; } }

namespace render { class CDeferredRenderer; }
namespace render { class CForwardRenderer; }
namespace render { class CLightingRenderer; }
namespace render { class CImGuiRenderer; }

namespace render
{
  class CRender
  {
  public:
    static const math::CVector3 s_v3WorldUp;

  public:
    CRender(uint32_t _uWidth, uint32_t _uHeight);
    ~CRender();

    void PrepareFrame();
    void Draw(scene::CRenderScene& _rScene);

	public:
    inline const render::CRenderWindow* GetRenderWindow() const { return m_pRenderWindow.get(); }
    inline void SetRenderCamera(render::CCamera* _pCamera) { m_pRenderCamera = _pCamera; }
    inline void SetShadowCamera(render::CCamera* _pCamera) { m_pShadowCamera = _pCamera; }

    void ShowRenderWindow(bool _bStatus);
    void SetFillMode(D3D11_FILL_MODE _eFillMode);
    void PushMaterial(const render::mat::CMaterial* _pMaterial);

    void BeginMarker(const wchar_t* _sMarker) const;
    void EndMarker() const;

    inline void SetVSync(bool _bEnabled) { m_bVerticalSync = _bEnabled; }
    inline bool IsVSyncEnabled() const { return m_bVerticalSync; }

    void SetRenderTargets(uint32_t _uSize, ID3D11RenderTargetView** _pRenderTargets, ID3D11DepthStencilView* _pStencilView = nullptr);
    void ClearRenderTargets(ID3D11RenderTargetView** _pRenderTargets, const float _v4ClearColor[4]);

    void SetClearColor(const float _v4ClearColor[4]);
    void ClearDepthStencil(ID3D11DepthStencilView* _pDepthStencilView, uint32_t uFlags, float _fDepth = 1.0f, uint8_t _uStencil = 0u);

    HRESULT CreateBlendState(D3D11_BLEND_DESC& _rDesc, ID3D11BlendState** _ppBlendState);
    void SetBlendState(ID3D11BlendState* _pBlendState, const float _v4BlendFactor[4] = nullptr, uint32_t _uSampleMask = 0xFFFFFFFF);

    HRESULT CreateRasterizerState(D3D11_RASTERIZER_DESC& _rDesc, ID3D11RasterizerState** _ppRasterizerState);
    void SetRasterizerState(ID3D11RasterizerState* _pRasterizerState);
    void SetInputLayout(ID3D11InputLayout* _pInputLayout);

    void SetDepthStencilState(ID3D11DepthStencilState* _pDepthStencilState, uint32_t _uStencilRef = 0);
    HRESULT CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC& _rDesc, ID3D11DepthStencilState** _ppDepthStencilState);

    void SetViewport(uint32_t _uWidth, uint32_t _uHeight);
    void SetScissorRect(uint32_t _uWidth, uint32_t _uHeight);

    void DrawPrimitives(scene::CRenderScene& _rRenderScene);
    void DrawModels(scene::CRenderScene& _rRenderScene);

  protected:
    void OnWindowResizeEvent(uint32_t _uWidth, uint32_t _uHeight);

  private:
    HRESULT Init(uint32_t _uWidth, uint32_t _uHeight);
    HRESULT CreateDevice(uint32_t _uWidth, uint32_t _uHeight);
    HRESULT InitBasicPipeline(uint32_t _uWidth, uint32_t _uHeight);

  private:
    HRESULT SetupRenderers(uint32_t _uWidth, uint32_t _uHeight);
    HRESULT CreateBackBuffer();

  private:
    HRESULT SetupPrecompiledShaders();
    HRESULT SetupConstantBuffers();
    HRESULT SetupRenderBuffers();
    HRESULT SetupBlendState();
    HRESULT SetupRasterizers();
    HRESULT SetupSamplers();
    HRESULT SetupLayouts();

  private:
    D3D_PRIMITIVE_TOPOLOGY GetTopology(render::ERenderMode _eRenderMode);

  private:
    // Deferred
    void ComputeGBuffer(scene::CRenderScene& _rRenderScene);
    void ComputeShadowMapping(scene::CRenderScene& _rRenderScene);
    void ComputeLightingPass(scene::CRenderScene& _rRenderScene);

  private:
    void DrawQuad();
    void DrawModel(const render::gfx::CModel* _pModel, const scene::TCachedModel& _rCachedModel);
    void DrawPrimitive(const render::gfx::CPrimitive* _pPrimitive);

  private:
    std::unique_ptr<render::CRenderWindow> m_pRenderWindow = nullptr;
    bool m_bVerticalSync = false;

    render::CCamera* m_pRenderCamera = nullptr;
    render::CCamera* m_pShadowCamera = nullptr;

    std::unique_ptr<CDeferredRenderer> m_pDeferredRenderer = nullptr;
    std::unique_ptr<CForwardRenderer> m_pForwardRenderer = nullptr;
    std::unique_ptr<CLightingRenderer> m_pLightingRenderer = nullptr;
    std::unique_ptr<CImGuiRenderer> m_pImGuiRenderer = nullptr;
  };
}


