#include "ShadowRenderer.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/Window/RenderWindow.h"

namespace render
{
  namespace internal
  {
    static const wchar_t* s_sComputeShadowsMrk(L"Shadows");
  }
  // ------------------------------------
  HRESULT CShadowRenderer::Init(uint32_t /*_uWidth*/, uint32_t /*_uHeight*/)
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
    return m_pRender->CreateRasterizerState(rShadowRasterizerCfg, &m_pShadowsRasterizer);
  }
  // ------------------------------------
  void CShadowRenderer::Draw(scene::CRenderScene& _rRenderScene)
  {
    m_pRender->BeginMarker(internal::s_sComputeShadowsMrk);
    {
      // Compute shadow map
      render::lights::CLightManager* pLightManager = _rRenderScene.GetLightManager();
      const lights::CLightManager::TShadowMaps& lstShadowMaps = pLightManager->GetShadowMaps();
      utils::CWeakPtr<render::lights::CDirectionalLight> pDirLight = pLightManager->GetDirectionalLight();
      bool bCastShadows = pDirLight.IsValid() && pDirLight->CastShadows();
      if (bCastShadows && lstShadowMaps.GetSize() > 0)
      {
        // Set custom rasterizer for shadow mapping
        {
          // Clear depth stencil view
          utils::CWeakPtr<render::gfx::CShadowMap> wpShadowMap = lstShadowMaps[0];
          const texture::TDepthStencil& rShadowStencil = wpShadowMap->GetDepthStencil();
          m_pRender->ClearDepthStencil(rShadowStencil.GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);

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

          // Configure shadow camera
          render::CCamera rShadowCamera = render::CCamera();
          {
            rShadowCamera.SetProjectionMode(EProjectionMode::ORTOGRAPHIC);
            rShadowCamera.SetOrthographicSize(fHeight);
            rShadowCamera.SetProjectionMatrix(mOrthographicProj);
            rShadowCamera.SetViewMatrix(mView);

            rShadowCamera.SetPos(v3ShadowPos);
            rShadowCamera.SetDir(v3Dir);
            rShadowCamera.SetAspectRatio(fAspectRatio);

            rShadowCamera.SetNear(fNear);
            rShadowCamera.SetFar(fFar);

            // Build frustum planes
            rShadowCamera.BuildFrustumPlanes();
          }

          // Push shadow mapping pass
          m_pRender->PushShadowMappingPass(rShadowCamera);

          // Only write in stencil (z-prepass)
          m_pRender->SetRenderTargets(nullptr, 0u, rShadowStencil.GetView());

          // Set rasterizer
          m_pRender->SetRasterizerState(m_pShadowsRasterizer);

          // Cache models
          _rRenderScene.CacheModels(rShadowCamera);

          // Draw models in z-prepass pass
          m_pRender->DrawModels(_rRenderScene);

          // Restore viewport
          uint32_t uRenderWidth = 0, uRenderHeight = 0;
          const render::CRenderWindow& rRenderWindow = m_pRender->GetRenderWindow();
          rRenderWindow.GetWindowSize(uRenderWidth, uRenderHeight);
          m_pRender->SetViewport(uRenderWidth, uRenderHeight);

          // Set invalid render targets
          m_pRender->SetRenderTargets(nullptr, 0u, nullptr);
        }
        // Set invalid rasterizer
        m_pRender->SetRasterizerState(nullptr);
      }
    }
    m_pRender->EndMarker();
  }
}