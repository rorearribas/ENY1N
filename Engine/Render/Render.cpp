#include "Render.h"
#include "Engine/Global/DX11GlobalInterface.h"
#include "Engine/Scenes/Scene.h"
#include "Libs/ImGui/imgui_impl_win32.h"
#include "Libs/ImGui/imgui_impl_dx11.h"
#include "../Base/Engine.h"

namespace render
{
  namespace internal_rendersystem
  {
    static const float s_fMinDepth(0.0f);
    static const float s_fMaxDepth(1.0f);
  }

  CRender::CRender(const UINT32& _uWidth, const UINT32& _uHeight)
  {
    // Create render window
    m_pRenderWindow = new render::CRenderWindow(_uWidth, _uHeight);
    // Create render camera
    m_pCamera = new render::CCamera();
  }
  // ------------------------------------
  CRender::~CRender()
  {
    if (global::dx11::s_pDX11Device) { global::dx11::s_pDX11Device->Release(); }
    if (global::dx11::s_pDX11DeviceContext) { global::dx11::s_pDX11DeviceContext->Release(); }
    if (global::dx11::s_pDX11SwapChain) { global::dx11::s_pDX11SwapChain->Release(); }
    if (global::dx11::s_pDX11RenderTargetView) { global::dx11::s_pDX11RenderTargetView->Release(); }
  }
  // ------------------------------------
  HRESULT CRender::Init()
  {
    // Create deviec
    HRESULT hr = CreateDevice();
    if (FAILED(hr)) return hr;

    // Init camera
    hr = InitCamera();
    if (FAILED(hr)) return hr;

    // Init ImGui
    if (!InitImGui()) return -1;

    // Set delegate
    global::dx11::s_oWindowResizeDelegate.Bind(&CRender::OnWindowResizeEvent, this);

    return S_OK;
  }
  // ------------------------------------
  HRESULT CRender::InitCamera()
  {
    HRESULT hr = constantBuffer.Initialize(global::dx11::s_pDX11Device, global::dx11::s_pDX11DeviceContext);
    if (FAILED(hr)) { return hr; }

    m_pCamera->SetPosition(0.0f, 0.0f, -2.0f);
    m_pCamera->SetProjectionValues(45.0f, static_cast<float>(m_pRenderWindow->GetWidth()) / static_cast<float>(m_pRenderWindow->GetHeight()), 0.1f, 1000.0f);

    return S_OK;
  }
  // ------------------------------------
  bool CRender::InitImGui()
  {
    if (!IMGUI_CHECKVERSION()) 
      return false;

    if (!ImGui::CreateContext()) 
      return false;

    if (!ImGui_ImplWin32_Init(m_pRenderWindow->GetHwnd())) 
      return false;

    if (!ImGui_ImplDX11_Init(global::dx11::s_pDX11Device, global::dx11::s_pDX11DeviceContext)) 
      return false;

    ImGui::StyleColorsDark();
    return true;
  }
  // ------------------------------------
  HRESULT CRender::CreateDevice()
  {
    DXGI_SWAP_CHAIN_DESC oSwapChainDescriptor = {};
    oSwapChainDescriptor.BufferCount = 1;
    oSwapChainDescriptor.BufferDesc.Width = m_pRenderWindow->GetWidth();
    oSwapChainDescriptor.BufferDesc.Height = m_pRenderWindow->GetHeight();
    oSwapChainDescriptor.OutputWindow = m_pRenderWindow->GetHwnd();
    oSwapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    oSwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    oSwapChainDescriptor.SampleDesc.Count = 1;
    oSwapChainDescriptor.SampleDesc.Quality = 0;
    oSwapChainDescriptor.Windowed = TRUE;

    D3D_FEATURE_LEVEL vctFeatureLevels[] =
    {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_3,
      D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1
    };
    D3D_FEATURE_LEVEL oFeatureLevel;
    UINT uNumFeatureLevels = ARRAYSIZE(vctFeatureLevels);

    // Device and swapchain
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, vctFeatureLevels,
    uNumFeatureLevels, D3D11_SDK_VERSION, &oSwapChainDescriptor, &global::dx11::s_pDX11SwapChain,
    &global::dx11::s_pDX11Device, &oFeatureLevel, &global::dx11::s_pDX11DeviceContext);
    if (FAILED(hr)) return hr;

    return S_OK;
  }
  // ------------------------------------
  void CRender::OnWindowResizeEvent(UINT32 _uX, UINT32 _uY)
  {
    // Delete current target view
    if (global::dx11::s_pDX11RenderTargetView)
    {
      global::dx11::s_pDX11RenderTargetView->Release();
      global::dx11::s_pDX11RenderTargetView = nullptr;
    }

    // Resize buffers
    HRESULT hr = global::dx11::s_pDX11SwapChain->ResizeBuffers(0, _uX, _uY, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    assert(!FAILED(hr));

    // Create new render target
    hr = CreateRenderTargetView();
    assert(!FAILED(hr));

    // Configure viewport
    ConfigureViewport(_uX, _uY);
  }
  // ------------------------------------
  HRESULT CRender::CreateRenderTargetView()
  {
    ID3D11Texture2D* pBackBuffer = nullptr;
    global::dx11::s_pDX11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&pBackBuffer));
    if (!pBackBuffer) return -1;

    HRESULT hr = global::dx11::s_pDX11Device->CreateRenderTargetView(pBackBuffer, nullptr, &global::dx11::s_pDX11RenderTargetView);
    if (FAILED(hr)) return hr;

    pBackBuffer->Release();
    global::dx11::s_pDX11DeviceContext->OMSetRenderTargets(1, &global::dx11::s_pDX11RenderTargetView, nullptr);

    return hr;
  }
  // ------------------------------------
  void CRender::ConfigureViewport(UINT32 _uX, UINT32 _uY)
  {
    if (global::dx11::s_pDX11DeviceContext && m_pRenderWindow)
    {
      D3D11_VIEWPORT oD3D11Viewport = {};
      oD3D11Viewport.Width = (FLOAT)(_uX);
      oD3D11Viewport.Height = (FLOAT)(_uY);
      oD3D11Viewport.MinDepth = internal_rendersystem::s_fMinDepth;
      oD3D11Viewport.MaxDepth = internal_rendersystem::s_fMaxDepth;

      global::dx11::s_pDX11DeviceContext->RSSetViewports(1, &oD3D11Viewport);
    }
  }
  // ------------------------------------
  void CRender::DrawScene(scene::CScene* _pScene)
  {
    // Clear the back buffer to cornflower blue for the new frame
    float background_color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    global::dx11::s_pDX11DeviceContext->ClearRenderTargetView(global::dx11::s_pDX11RenderTargetView, background_color);

    // Draw scene
    DrawImGui();

    // Draw ImGui
    _pScene->DrawScene();

    // Update camera
    m_pCamera->Update();

    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
    constantBuffer.data.matrix = world * m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix();
    constantBuffer.data.matrix = DirectX::XMMatrixTranspose(constantBuffer.data.matrix);

    if (!constantBuffer.ApplyChanges()) return;
    global::dx11::s_pDX11DeviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    // Swap the back and front buffers (show the frame we just drew)
    global::dx11::s_pDX11SwapChain->Present(m_bVerticalSync, 0);
  }
  // ------------------------------------
  void CRender::DrawImGui()
  {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Test
    ImGui::Begin("Handler");

    if (ImGui::Button("Create primitive"))
    {
      render::primitive::CPrimitive* pPrimitive = engine::CEngine::GetInstance()->CreatePrimitive(render::primitive::CPrimitive::RECTANGLE);
      pPrimitive->SetColor(maths::CVector3(1.0f, 1.0f, 0.0f));
    }
    if (ImGui::Button("Destroy primitive"))
    {
      engine::CEngine::GetInstance()->DestroyAllPrimimitives();
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  }
}