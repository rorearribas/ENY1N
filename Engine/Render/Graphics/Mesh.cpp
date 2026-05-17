#include "Mesh.h"

namespace render
{
  namespace gfx
  {
    CMesh::CMesh(std::vector<uint32_t>& _lstIndices)
    {
      m_uIndexCount = static_cast<uint32_t>(_lstIndices.size());
      m_lstIndices = std::move(_lstIndices);
    }
  }
}
