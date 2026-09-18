#include "ImGuiRenderer.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Render/Render.h"
#include "Engine/Scenes/RenderScene.h"

#include "Libs/ImGui/imgui_impl_win32.h"
#include "Libs/ImGui/imgui_impl_dx11.h"
#include "Libs/ImGui/ImGuizmo.h"

namespace render
{
  namespace internal
  {
    static const wchar_t* s_sImGuiMarker(L"ImGui");
  }
  // ------------------------------------
  HRESULT CImGuiRenderer::Init(const HWND& _hWnd)
  {
    if (!IMGUI_CHECKVERSION())
    {
      return E_FAIL;
    }

    // Setup ImGui
    if (!ImGui::CreateContext())
    {
      return E_FAIL;
    }
    if (!ImGui_ImplWin32_Init(_hWnd))
    {
      return E_FAIL;
    }
    if (!ImGui_ImplDX11_Init(global::api::Device, global::api::DeviceContext))
    {
      return E_FAIL;
    }

    // Set ImGui
    ImGui::StyleColorsDark();

    return S_OK;
  }
  // ------------------------------------
  void CImGuiRenderer::PrepareFrame()
  {
    // Prepare ImGui
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // ImGuizmo
    ImGuizmo::BeginFrame();
  }
  // ------------------------------------
  void CImGuiRenderer::Execute(scene::CRenderScene& /*_rRenderScene*/)
  {
    // Render ImGui
    m_pRender->BeginMarker(internal::s_sImGuiMarker);
    {
      ImGui::Render();
      ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
    m_pRender->EndMarker();
  }
}

