#include "LightingRenderer.h"
#include "Engine/Render/Render.h"
#include "Engine/Scenes/RenderScene.h"
#include "Engine/Render/Window/RenderWindow.h"

namespace render
{
  namespace internal
  {
    static const wchar_t* s_sComputeLightingMrk(L"Lighting");
    static const wchar_t* s_sComputeShadowsMrk(L"ShadowMapping");
  }
  // ------------------------------------
  HRESULT CLightingRenderer::Init(uint32_t /*_uWidth*/, uint32_t /*_uHeight*/)
  {
    // Set shadow rasterizer config
    D3D11_RASTERIZER_DESC rShadowRasterizerCfg = D3D11_RASTERIZER_DESC();
    rShadowRasterizerCfg.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rShadowRasterizerCfg.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
    rShadowRasterizerCfg.DepthBias = 500;
    rShadowRasterizerCfg.DepthBiasClamp = 0.0f;
    rShadowRasterizerCfg.SlopeScaledDepthBias = 1.5f;
    rShadowRasterizerCfg.DepthClipEnable = true;
    rShadowRasterizerCfg.ScissorEnable = false;
    rShadowRasterizerCfg.MultisampleEnable = false;
    rShadowRasterizerCfg.AntialiasedLineEnable = false;

    // Create rasterizer
    return global::api::Device->CreateRasterizerState(&rShadowRasterizerCfg, &m_pShadowsRasterizer);
  }
  // ------------------------------------
  void CLightingRenderer::Execute(scene::CRenderScene& _rRenderScene)
  {
    // Get light manager -> apply lighting
    render::lights::CLightManager* pLightManager = _rRenderScene.GetLightManager();
    pLightManager->ApplyLighting();

    m_pRender->BeginMarker(internal::s_sComputeShadowsMrk);
    {
      // Compute shadow map
      const lights::CLightManager::TShadowMaps& lstShadowMaps = pLightManager->GetShadowMaps();
      utils::CWeakPtr<render::lights::CDirectionalLight> pDirLight = pLightManager->GetDirectionalLight();
      bool bCastShadows = pDirLight.IsValid() && pDirLight->CastShadows();
      if (bCastShadows && lstShadowMaps.GetSize() > 0)
      {
        // Set custom rasterizer for shadow mapping
        m_pRender->SetRasterizerState(m_pShadowsRasterizer);
        {
          // Clear depth stencil view
          utils::CWeakPtr<render::gfx::CShadowMap> wpShadowMap = lstShadowMaps[0];
          const texture::TDepthStencil& rShadowStencil = wpShadowMap->GetStencil();
          global::api::DeviceContext->ClearDepthStencilView(rShadowStencil.GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

          // Configure viewport
          uint32_t uWidth = 0, uHeight = 0;
          rShadowStencil.GetTextureSize(uWidth, uHeight);
          m_pRender->SetViewport(uWidth, uHeight);

          // Create view matrix from directional light
          const float fMaxDistance = 50.0f;
          math::CVector3 v3Dir = pLightManager->GetDirectionalLight()->GetDir();
          math::CVector3 v3SceneCenter = m_pRenderCamera->GetPos() + (m_pRenderCamera->GetDir() * fMaxDistance); // Max distance
          math::CVector3 v3ShadowPos = v3SceneCenter - (v3Dir * (fMaxDistance * 2.0f)); // Calculate shadow pos

          // Orthographic values ( testing )
          float fAspectRatio = static_cast<float>(uWidth) / static_cast<float>(uHeight);
          float fHeight = 100.0f;
          float fWidth = fHeight * fAspectRatio;

          const float fNear = m_pRenderCamera->GetNear();
          const float fFar = m_pRenderCamera->GetFar();

          math::CMatrix4x4 mView = math::CMatrix4x4::LookAt(v3ShadowPos, v3SceneCenter, render::CRender::s_v3WorldUp);
          math::CMatrix4x4 mOrthographicProj = math::CMatrix4x4::CreateOrtographicMatrix(fWidth, fHeight, fNear, fFar);

#ifdef _DEBUG
          assert(m_pRenderCamera);
#endif
          // Configure shadow camera
          m_pShadowCamera->SetProjectionMode(EProjectionMode::ORTOGRAPHIC);
          m_pShadowCamera->SetOrthographicSize(fHeight);
          m_pShadowCamera->SetProjectionMatrix(mOrthographicProj);
          m_pShadowCamera->SetViewMatrix(mView);

          m_pShadowCamera->SetPos(v3ShadowPos);
          m_pShadowCamera->SetDir(v3Dir);
          m_pShadowCamera->SetAspectRatio(fAspectRatio);

          m_pShadowCamera->SetNear(fNear);
          m_pShadowCamera->SetFar(fFar);

          // Build frustum planes
          m_pShadowCamera->BuildFrustumPlanes();

          // Calculate transforms for shadow mapping
          TCameraTransform rTransforms = TCameraTransform();
          {
            math::CMatrix4x4 mViewProjection = m_pShadowCamera->GetViewProjection();
            rTransforms.ViewProjection = mViewProjection;
            rTransforms.InvViewProjection = math::CMatrix4x4::Invert(mViewProjection);
          }

          // Write buffer
          bool bOk = internal::Pipeline.LightingViewBuffer.WriteBuffer(rTransforms);
          UNUSED_VAR(bOk);
#ifdef _DEBUG
          assert(bOk);
#endif // DEBUG
          internal::Pipeline.LightingViewBuffer.Bind<render::EShader::E_VERTEX>(internal::Pipeline.CameraTransformSlot);

          // Set render target
          m_pRender->SetRenderTargets(0u, nullptr, rShadowStencil.GetView());

          // Set depth stencil state
          ID3D11DepthStencilState* pCurrentStencilState = nullptr;
          uint32_t uCurrentRef = 0;
          global::api::DeviceContext->OMGetDepthStencilState(&pCurrentStencilState, &uCurrentRef);
          if (pCurrentStencilState != internal::Pipeline.DepthStencilState)
          {
            global::api::DeviceContext->OMSetDepthStencilState(internal::Pipeline.DepthStencilState, 1);
          }

          // Attach vertex shader for shadows (vertex shader)
          internal::Pipeline.DeferredShadowsVS.Attach();
          // Detach pixel shader for models
          internal::Pipeline.DeferredGBuffer.Detach();

          // Cache models
          _rRenderScene.CacheModels(*m_pShadowCamera);

          // Draw models only in z-prepass pass from the light view
          m_pRender->DrawModels(_rRenderScene);

          uint32_t uRenderWidth = 0, uRenderHeight = 0;
          m_pRender->GetRenderWindow()->GetWindowSize(uRenderWidth, uRenderHeight);
          m_pRender->SetViewport(uRenderWidth, uRenderHeight);
        }
        // Set invalid rasterizer
        m_pRender->SetRasterizerState(nullptr);
      }
    }
    m_pRender->EndMarker();

    m_pRender->BeginMarker(internal::s_sComputeLightingMrk);
    {
      // Set transform constant
      internal::Pipeline.CameraTransformBuffer.Bind<render::EShader::E_PIXEL>(internal::Pipeline.CameraTransformSlot);

      // Apply lighting
      render::lights::CLightManager* pLightManager = _rRenderScene.GetLightManager();
      pLightManager->ApplyLighting();

      utils::CWeakPtr<render::lights::CDirectionalLight> pDirLight = pLightManager->GetDirectionalLight();
      bool bCastShadows = pDirLight.IsValid() && pDirLight->CastShadows();
      const lights::CLightManager::TShadowMaps& lstShadowMaps = pLightManager->GetShadowMaps();

      // Shadow mapping texture
      ID3D11ShaderResourceView* pShadowTexture = nullptr;
      if (bCastShadows && lstShadowMaps.GetSize() > 0)
      {
        pShadowTexture = lstShadowMaps[0]->GetTexture().GetView();
        global::api::DeviceContext->PSSetSamplers(1u, 1u, &internal::Pipeline.ShadowSampler);
        internal::Pipeline.LightingViewBuffer.Bind<render::EShader::E_PIXEL>(internal::Pipeline.LightingViewSlot);
      }

      static constexpr uint32_t uTexturesSize(5);
      ID3D11ShaderResourceView* lstGBufferSRV[uTexturesSize] =
      {
        internal::Pipeline.DepthTexture.GetView(),
        m_pDeferredRenderer->GetDiffuseRT().GetShaderView(),
        m_pDeferredRenderer->GetNormalRT().GetShaderView(),
        m_pDeferredRenderer->GetNormalRT().GetShaderView(),
        pShadowTexture
      };

      // Bind buffers
      SetRenderTargets(1u, &internal::Pipeline.BackBuffer);
      global::api::DeviceContext->PSSetShaderResources(0, uTexturesSize, &lstGBufferSRV[0]);

      // Attach calculate lights shader (pixel shader)
      internal::Pipeline.DeferredLightsPS.Attach();

      // Draw quad to apply lighting
      m_pRender->DrawQuad();

      // Set invalid shaders
      ID3D11ShaderResourceView* lstEmptyTextures[uTexturesSize] = { nullptr, nullptr, nullptr, nullptr };
      global::api::DeviceContext->PSSetShaderResources(0, uTexturesSize, lstEmptyTextures);

      // Attach back buffer
      ID3D11DepthStencilView* pDepthStencilView = internal::Pipeline.DepthStencil.GetView();
      SetRenderTargets(1u, &internal::Pipeline.BackBuffer, pDepthStencilView);
    }
    m_pRender->EndMarker();
  }
}
