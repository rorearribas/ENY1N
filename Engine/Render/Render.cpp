#include "Render.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Scenes/Scene.h"
#include "Libs/ImGui/imgui_impl_win32.h"
#include "Libs/ImGui/imgui_impl_dx11.h"
#include "Engine/Base/Engine.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/ImGui/ImGuizmo.h"

// Shaders
#include "Engine/Shaders/Standard/ForwardPixelShader.h"
#include "Engine/Shaders/Standard/ForwardVertexShader.h"
#include "Engine/Shaders/Primitive/PixelShader.h"
#include "Engine/Shaders/Primitive/VertexShader.h"

namespace render
{
  namespace internal_render
  {
    static const float s_v4ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    static const float s_fMinDepth(0.0f);
    static const float s_fMaxDepth(1.0f);

    struct SRenderPipeline
    {
      // Swap char + render target
      IDXGISwapChain* SwapChain = nullptr;
      ID3D11RenderTargetView* RenderTargetView = nullptr;

      // Depth stencil
      ID3D11Texture2D* DepthStencilTexture = nullptr;
      ID3D11DepthStencilState* DepthStencilState = nullptr;
      ID3D11DepthStencilView* DepthStencilView = nullptr;

      // Rasterizer
      ID3D11RasterizerState* RasterizerState = nullptr;

      // Blending
      ID3D11BlendState* BlendState = nullptr;

      // Shaders for 3D Pipeline
      shader::CShader<shader::EShaderType::PIXEL_SHADER>* ForwardPS;
      shader::CShader<shader::EShaderType::VERTEX_SHADER>* ForwardVS;

      // Shaders for primitives
      shader::CShader<shader::EShaderType::PIXEL_SHADER>* PrimitivesPS;
      shader::CShader<shader::EShaderType::VERTEX_SHADER>* PrimitivesVS;
    };

    static SRenderPipeline s_oPipeline;
  }
  // ------------------------------------
  CRender::CRender(uint32_t _uX, uint32_t _uY)
  {
    // Create render window
    m_pRenderWindow = new render::CRenderWindow(_uX, _uY);

    // Init render
    HRESULT hResult = Init(_uX, _uY);
    UNUSED_VARIABLE(hResult);
    assert(!FAILED(hResult));
  }
  // ------------------------------------
  CRender::~CRender()
  {
    // Delete global resources
    global::dx11::SafeRelease(global::dx11::s_pDevice);
    global::dx11::SafeRelease(global::dx11::s_pDeviceContext);

    // Delete internal resources
    global::dx11::SafeRelease(internal_render::s_oPipeline.SwapChain);
    global::dx11::SafeRelease(internal_render::s_oPipeline.RenderTargetView);
    global::dx11::SafeRelease(internal_render::s_oPipeline.DepthStencilTexture);
    global::dx11::SafeRelease(internal_render::s_oPipeline.DepthStencilState);
    global::dx11::SafeRelease(internal_render::s_oPipeline.DepthStencilView);
    global::dx11::SafeRelease(internal_render::s_oPipeline.RasterizerState);
  }
  // ------------------------------------
  HRESULT CRender::Init(uint32_t _uX, uint32_t _uY)
  {
    // Create device
    HRESULT hResult = CreateDevice(_uX, _uY);
    if (FAILED(hResult)) return hResult;

    // Init ImGui
    if (!InitImGui()) return -1;

    // Setup basic pipeline
    hResult = InitBasicPipeline(_uX, _uY);
    if (FAILED(hResult)) return hResult;

    // Create shaders for 3D pipeline
    internal_render::s_oPipeline.ForwardPS = new shader::CShader<shader::EShaderType::PIXEL_SHADER>(g_ForwardPixelShader, ARRAYSIZE(g_ForwardPixelShader));
    internal_render::s_oPipeline.ForwardVS = new shader::CShader<shader::EShaderType::VERTEX_SHADER>(g_ForwardVertexShader, ARRAYSIZE(g_ForwardVertexShader));

    // Create shaders for primitives
    internal_render::s_oPipeline.PrimitivesPS = new shader::CShader<shader::EShaderType::PIXEL_SHADER>(g_PixelShader, ARRAYSIZE(g_PixelShader));
    internal_render::s_oPipeline.PrimitivesVS = new shader::CShader<shader::EShaderType::VERTEX_SHADER>(g_VertexShader, ARRAYSIZE(g_VertexShader));

    // Set delegate
    utils::CDelegate<void(uint32_t, uint32_t)> oResizeDelegate(&CRender::OnWindowResizeEvent, this);
    global::delegates::s_vctOnWindowResizeDelegates.push_back(oResizeDelegate);

    // Push shaders for standard 3D pipeline
    internal_render::s_oPipeline.ForwardPS->PushShader();
    internal_render::s_oPipeline.ForwardVS->PushShader();

    return S_OK;
  }
  // ------------------------------------
  HRESULT CRender::InitBasicPipeline(uint32_t _uX, uint32_t _uY)
  {
    // Configure viewport
    ConfigureViewport(_uX, _uY);

    // Create new render target
    HRESULT hResult = CreateRenderTargetView();
    assert(!FAILED(hResult));

    // Create a new depth stencil
    hResult = CreateDepthStencilView(_uX, _uY);
    assert(!FAILED(hResult));

    // Create rasterizer
    hResult = CreateRasterizerState();
    assert(!FAILED(hResult));

    hResult = CreateBlendState();
    assert(!FAILED(hResult));

    // Update scissor
    SetScissorRect(_uX, _uY);

    return hResult;
  }
  // ------------------------------------
  void CRender::SetScissorRect(uint32_t _uX, uint32_t _uY)
  {
    D3D11_RECT oRect = D3D11_RECT();
    oRect.left = 0;
    oRect.top = 0;
    oRect.right = static_cast<LONG>(_uX);
    oRect.bottom = static_cast<LONG>(_uY);
    global::dx11::s_pDeviceContext->RSSetScissorRects(1, &oRect);
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

    if (!ImGui_ImplDX11_Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext))
      return false;

    ImGui::StyleColorsDark();
    return true;
  }
  // ------------------------------------
  HRESULT CRender::CreateDevice(uint32_t _uX, uint32_t _uY)
  {
    DXGI_SWAP_CHAIN_DESC oSwapChainDescriptor = DXGI_SWAP_CHAIN_DESC();
    oSwapChainDescriptor.BufferCount = 1;
    oSwapChainDescriptor.BufferDesc.Width = _uX;
    oSwapChainDescriptor.BufferDesc.Height = _uY;
    oSwapChainDescriptor.OutputWindow = global::window::s_oHwnd;
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

    // Device and swap chain
    HRESULT hResult = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, vctFeatureLevels,
      uNumFeatureLevels, D3D11_SDK_VERSION, &oSwapChainDescriptor, &internal_render::s_oPipeline.SwapChain,
      &global::dx11::s_pDevice, &oFeatureLevel, &global::dx11::s_pDeviceContext);
    if (FAILED(hResult)) return hResult;

    return S_OK;
  }
  // ------------------------------------
  void CRender::OnWindowResizeEvent(uint32_t _uX, uint32_t _uY)
  {
    // Remove current target view
    global::dx11::SafeRelease(internal_render::s_oPipeline.RenderTargetView);

    // Resize buffers
    HRESULT hResult = internal_render::s_oPipeline.SwapChain->ResizeBuffers(0, _uX, _uY, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    assert(!FAILED(hResult));

    // Init basic pipeline
    hResult = InitBasicPipeline(_uX, _uY);
    assert(!FAILED(hResult));
  }
  // ------------------------------------
  HRESULT CRender::CreateRenderTargetView()
  {
    ID3D11Texture2D* pBackBuffer = nullptr;
    internal_render::s_oPipeline.SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (!pBackBuffer) return E_FAIL;

    HRESULT hResult = global::dx11::s_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &internal_render::s_oPipeline.RenderTargetView);
    if (FAILED(hResult)) return hResult;

    pBackBuffer->Release();
    return hResult;
  }
  // ------------------------------------
  HRESULT CRender::CreateDepthStencilView(uint32_t _uX, uint32_t _uY)
  {
    // Release resources
    global::dx11::SafeRelease(internal_render::s_oPipeline.DepthStencilTexture);
    global::dx11::SafeRelease(internal_render::s_oPipeline.DepthStencilState);
    global::dx11::SafeRelease(internal_render::s_oPipeline.DepthStencilView);

    // Create texture
    D3D11_TEXTURE2D_DESC oTextureDesc = D3D11_TEXTURE2D_DESC();
    oTextureDesc.Width = _uX;
    oTextureDesc.Height = _uY;
    oTextureDesc.MipLevels = 1;
    oTextureDesc.ArraySize = 1;
    oTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    oTextureDesc.SampleDesc.Count = 1;
    oTextureDesc.SampleDesc.Quality = 0;
    oTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    oTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    oTextureDesc.CPUAccessFlags = 0;
    oTextureDesc.MiscFlags = 0;
    HRESULT hResult = global::dx11::s_pDevice->CreateTexture2D(&oTextureDesc, nullptr, &internal_render::s_oPipeline.DepthStencilTexture);
    assert(!FAILED(hResult));

    // Depth test parameters
    D3D11_DEPTH_STENCIL_DESC oDepthStencilDesc = D3D11_DEPTH_STENCIL_DESC();
    oDepthStencilDesc.DepthEnable = true;
    oDepthStencilDesc.StencilEnable = true;
    oDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    oDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    oDepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    oDepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    // Front-face
    oDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    oDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    oDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    oDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Back-face
    oDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    oDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    oDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    oDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    hResult = global::dx11::s_pDevice->CreateDepthStencilState(&oDepthStencilDesc, &internal_render::s_oPipeline.DepthStencilState);

    D3D11_DEPTH_STENCIL_VIEW_DESC oDepthStencilViewDesc = D3D11_DEPTH_STENCIL_VIEW_DESC();
    oDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    oDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    oDepthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view
    return global::dx11::s_pDevice->CreateDepthStencilView
    (
      internal_render::s_oPipeline.DepthStencilTexture,
      &oDepthStencilViewDesc,
      &internal_render::s_oPipeline.DepthStencilView
    );
  }
  // ------------------------------------
  HRESULT CRender::CreateRasterizerState(D3D11_FILL_MODE _eFillMode)
  {
    global::dx11::SafeRelease(internal_render::s_oPipeline.RasterizerState);

    D3D11_RASTERIZER_DESC oRasterizerDesc = D3D11_RASTERIZER_DESC();
    oRasterizerDesc.FillMode = _eFillMode;
    oRasterizerDesc.CullMode = D3D11_CULL_BACK;
    oRasterizerDesc.FrontCounterClockwise = false;
    oRasterizerDesc.DepthBias = false;
    oRasterizerDesc.DepthBiasClamp = 0;
    oRasterizerDesc.SlopeScaledDepthBias = 0;
    oRasterizerDesc.DepthClipEnable = true;
    oRasterizerDesc.ScissorEnable = true;
    oRasterizerDesc.MultisampleEnable = false;
    oRasterizerDesc.AntialiasedLineEnable = false;

    return global::dx11::s_pDevice->CreateRasterizerState(&oRasterizerDesc, &internal_render::s_oPipeline.RasterizerState);
  }
  // ------------------------------------
  HRESULT CRender::CreateBlendState()
  {
    global::dx11::SafeRelease(internal_render::s_oPipeline.BlendState);

    D3D11_RENDER_TARGET_BLEND_DESC oRenderTargetBlendDesc = D3D11_RENDER_TARGET_BLEND_DESC();
    oRenderTargetBlendDesc.BlendEnable = false;
    oRenderTargetBlendDesc.SrcBlend = D3D11_BLEND_ONE;
    oRenderTargetBlendDesc.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    oRenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
    oRenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
    oRenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
    oRenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    oRenderTargetBlendDesc.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    D3D11_BLEND_DESC oBlendDesc;
    ZeroMemory(&oBlendDesc, sizeof(oBlendDesc));

    oBlendDesc.AlphaToCoverageEnable = false;
    oBlendDesc.RenderTarget[0] = oRenderTargetBlendDesc;

    return global::dx11::s_pDevice->CreateBlendState(&oBlendDesc, &internal_render::s_oPipeline.BlendState);
  }
  // ------------------------------------
  void CRender::ConfigureViewport(uint32_t _uX, uint32_t _uY)
  {
    if (global::dx11::s_pDeviceContext && m_pRenderWindow)
    {
      D3D11_VIEWPORT oViewport = D3D11_VIEWPORT();
      oViewport.Width = static_cast<float>(_uX);
      oViewport.Height = static_cast<float>(_uY);
      oViewport.MinDepth = internal_render::s_fMinDepth;
      oViewport.MaxDepth = internal_render::s_fMaxDepth;

      global::dx11::s_pDeviceContext->RSSetViewports(1, &oViewport);
    }
  }
  // ------------------------------------
  void CRender::BeginDraw() const
  {
    // Clear resources
    global::dx11::s_pDeviceContext->ClearRenderTargetView(internal_render::s_oPipeline.RenderTargetView, internal_render::s_v4ClearColor);
    global::dx11::s_pDeviceContext->ClearDepthStencilView(internal_render::s_oPipeline.DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Prepare ImGu
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Begin frame
    ImGuizmo::BeginFrame();
  }
  // ------------------------------------
  void CRender::Draw(scene::CScene* _pScene) const
  {
    // Update lights
    _pScene->ApplyLights();

    // Draw models
    _pScene->DrawModels();

    // Draw primitives
    _pScene->DrawPrimitives();
  }
  // ------------------------------------
  void CRender::EndDraw() const
  {
    // Update resources
    global::dx11::s_pDeviceContext->OMSetRenderTargets(1, &internal_render::s_oPipeline.RenderTargetView, internal_render::s_oPipeline.DepthStencilView);
    global::dx11::s_pDeviceContext->OMSetDepthStencilState(internal_render::s_oPipeline.DepthStencilState, 1);
    global::dx11::s_pDeviceContext->OMSetBlendState(internal_render::s_oPipeline.BlendState, nullptr, 0xFFFFFFFF);
    global::dx11::s_pDeviceContext->RSSetState(internal_render::s_oPipeline.RasterizerState);

    // End imgui draw
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Draw the current frame
    internal_render::s_oPipeline.SwapChain->Present(m_bVerticalSync, 0);
  }
}