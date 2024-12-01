#include "Mesh.h"

namespace render
{
	namespace graphics
	{
    CMesh::~CMesh()
    {
      m_vctVertexData.clear();
      m_vctIndexes.clear();
    }
  }
}
