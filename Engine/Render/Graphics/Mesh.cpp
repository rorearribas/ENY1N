#include "Mesh.h"

namespace render
{
  namespace gfx
  {
    CMesh::CMesh(CMesh&& _rOther) noexcept : 
      m_pMaterial(std::move(_rOther.m_pMaterial)),
      m_oIdxBufferHandler(_rOther.m_oIdxBufferHandler) 
    {
    }
    // ------------------------------------
    render::gfx::CMesh& CMesh::operator=(CMesh&& _rOther) noexcept
    {
      if (this != &_rOther)
      {
        m_pMaterial = std::move(_rOther.m_pMaterial);
        m_oIdxBufferHandler = _rOther.m_oIdxBufferHandler;
      }
      return *this;
    }
  }
}
