#include "GlobalResources.h"
#include "Engine/Render/Graphics/Mesh.h"

namespace global
{
  // Memory tracker
  namespace mem
  {
    CMemoryTracker s_oMemoryTracker;
  }

  // Win handle
  namespace window
  {
    HWND s_oHwnd = nullptr;
  }
  // DirectX
  namespace dx
  {
    // Device
    ID3D11Device* s_pDevice = nullptr;
    ID3D11DeviceContext* s_pDeviceContext = nullptr;
  }
  // Global delegates
  namespace delegates
  {
    std::vector<utils::CDelegate<void(uint32_t, uint32_t)>> s_lstOnWindowResizeDelegates;
    utils::CDelegate<void(RAWKEYBOARD*)> s_oOnUpdateKeyboardDelegate;
    utils::CDelegate<void(RAWMOUSE*)> s_oUpdateMouseDelegate;
  }
}

void* operator new(std::size_t size)
{
  void* ptr = malloc(size);
  if (!ptr)
  {
    throw std::bad_alloc();
  }
  global::mem::s_oMemoryTracker.RegisterMem(size);
  return ptr;
}
void operator delete(void* ptr, size_t size) noexcept
{
  global::mem::s_oMemoryTracker.DeregisterMem(size);
  free(ptr);
}
void* operator new[](std::size_t size)
{
  void* ptr = malloc(size);
  if (!ptr)
  {
    throw std::bad_alloc();
  }
  global::mem::s_oMemoryTracker.RegisterMem(size);
  return ptr;
}
void operator delete[](void* ptr, size_t size) noexcept
{
  global::mem::s_oMemoryTracker.DeregisterMem(size);
  free(ptr);
}
