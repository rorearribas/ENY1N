#include "RenderSystem.h"
#include <d3dcompiler.h>

namespace render
{
  CRenderSystem::CRenderSystem(const UINT32& _uWidth, const UINT32& _uHeight)
  {
    // Create render window
    m_pRenderWindow = new CRenderWindow();
    m_pRenderWindow->CreateRenderWindow(_uWidth, _uHeight);
  }
  // ------------------------------------
  CRenderSystem::~CRenderSystem()
  {
    if (m_pDevice) { m_pDevice->Release(); }
    if (m_pDeviceContext) { m_pDeviceContext->Release(); }
    if (m_pDXGISwapChain) { m_pDXGISwapChain->Release(); }
    if (m_pRenderTargetView) { m_pRenderTargetView->Release(); }
  }
  // ------------------------------------
  HRESULT CRenderSystem::InitDevice()
  {
    DXGI_SWAP_CHAIN_DESC oSwapChainDescriptor = {};
    oSwapChainDescriptor.BufferCount = 1;
    oSwapChainDescriptor.BufferDesc.Width = m_pRenderWindow->GetWidth();
    oSwapChainDescriptor.BufferDesc.Height = m_pRenderWindow->GetHeight();
    oSwapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    oSwapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
    oSwapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
    oSwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    oSwapChainDescriptor.OutputWindow = m_pRenderWindow->GetHwnd();
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
      uNumFeatureLevels, D3D11_SDK_VERSION, &oSwapChainDescriptor, &m_pDXGISwapChain,
      &m_pDevice, &oFeatureLevel, &m_pDeviceContext);
    if (FAILED(hr)) return hr;

    // Render target view
    hr = CreateRenderTargetView();
    if (FAILED(hr)) return hr;

    // COMPILE VERTEX SHADER
    ID3DBlob* pError = nullptr;
    hr = D3DCompileFromFile
    (
      L"D:\\Projects\\Personal\\ENY1N\\Engine\\Shaders\\VertexShader.hlsl",
      nullptr,
      D3D_COMPILE_STANDARD_FILE_INCLUDE,
      "vs_main",
      "vs_5_0",
      D3DCOMPILE_DEBUG,
      0,
      &m_pVertexShaderBlob,
      &pError
    );

    // COMPILE PIXEL SHADER
    hr = D3DCompileFromFile
    (
      L"D:\\Projects\\Personal\\ENY1N\\Engine\\Shaders\\PixelShader.hlsl",
      nullptr,
      D3D_COMPILE_STANDARD_FILE_INCLUDE,
      "ps_main",
      "ps_5_0",
      D3DCOMPILE_DEBUG,
      0,
      &m_pPixelShaderBlob,
      &pError
    );

    hr = m_pDevice->CreateVertexShader
    (
      m_pVertexShaderBlob->GetBufferPointer(),
      m_pVertexShaderBlob->GetBufferSize(),
      NULL,
      &m_pVertexShader
    );

    hr = m_pDevice->CreatePixelShader
    (
      m_pPixelShaderBlob->GetBufferPointer(),
      m_pPixelShaderBlob->GetBufferSize(),
      NULL,
      &m_pPixelShader
    );

    ID3D11InputLayout* input_layout_ptr = NULL;
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
      { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      /*
      { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      */
    };
    hr = m_pDevice->CreateInputLayout
    (
      inputElementDesc,
      ARRAYSIZE(inputElementDesc),
      m_pVertexShaderBlob->GetBufferPointer(),
      m_pVertexShaderBlob->GetBufferSize(),
      &m_pInputLayout
    );

    float lstVertexData[] =
    {
      0.0f,  0.5f,  0.0f, // point at top
      0.5f, -0.5f,  0.0f, // point at bottom-right
      -0.5f, -0.5f,  0.0f, // point at bottom-left
    };
    UINT vertex_stride = 3 * sizeof(float);
    UINT vertex_offset = 0;
    UINT vertex_count = 3;

    /*** load mesh data into vertex buffer **/
    D3D11_BUFFER_DESC oVertexBufferDescriptor = {};
    oVertexBufferDescriptor.ByteWidth = sizeof(lstVertexData);
    oVertexBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA oSubresourceData = { 0 };
    oSubresourceData.pSysMem = lstVertexData;
    hr = m_pDevice->CreateBuffer
    (
      &oVertexBufferDescriptor,
      &oSubresourceData,
      &m_pVertexBuffer
    );

    // Viewport
    ConfigureViewport();

    return S_OK;
  }
  // ------------------------------------
  HRESULT CRenderSystem::CreateRenderTargetView()
  {
    ID3D11Texture2D* pBackBuffer = nullptr;
    m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (!pBackBuffer) return -1;

    HRESULT hr = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
    pBackBuffer->Release();
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);

    return hr;
  }
  // ------------------------------------
  void CRenderSystem::UpdateRender()
  {
    /* clear the back buffer to cornflower blue for the new frame */
    float background_colour[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, background_colour);

    /**** Rasteriser state - set viewport area *****/
    RECT winRect;
    GetClientRect(m_pRenderWindow->GetHwnd(), &winRect);
    D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)(winRect.right - winRect.left), (FLOAT)(winRect.bottom - winRect.top), 0.0f, 1.0f };
    m_pDeviceContext->RSSetViewports(1, &viewport);

    /**** Output Merger *****/
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

    /***** Input Assembler (map how the vertex shader inputs should be read from vertex buffer) ******/
    UINT vertex_stride = 3 * sizeof(float);
    UINT vertex_offset = 0;
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pDeviceContext->IASetInputLayout(m_pInputLayout);
    m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &vertex_stride, &vertex_offset);

    /*** set vertex shader to use and pixel shader to use, and constant buffers for each ***/
    m_pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
    m_pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);

    /*** draw the vertex buffer with the shaders ****/
    m_pDeviceContext->Draw(3, 0);

    /**** swap the back and front buffers (show the frame we just drew) ****/
    m_pDXGISwapChain->Present(1, 0);
  }
  // ------------------------------------
  void CRenderSystem::ConfigureViewport()
  {
    if (m_pDeviceContext && m_pRenderWindow)
    {
      RECT rWinRect;
      GetClientRect(m_pRenderWindow->GetHwnd(), &rWinRect);

      D3D11_VIEWPORT vp = {};
      vp.Width = (FLOAT)(rWinRect.right - rWinRect.left);
      vp.Height = (FLOAT)(rWinRect.bottom - rWinRect.top);
      vp.MinDepth = 0.0f;
      vp.MaxDepth = 1.0f;

      m_pDeviceContext->RSSetViewports(1, &vp);
    }
  }
}