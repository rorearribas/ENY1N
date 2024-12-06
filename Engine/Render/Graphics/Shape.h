#pragma once
#include <string>
#include "Mesh.h"

namespace render
{
	namespace graphics
	{
    struct SLine 
    {
    };

    class CShape
    {
    public:
      CShape(std::string _sShapeName) : m_sShapeName(_sShapeName) {}
      ~CShape() {}

      render::graphics::CMesh* const GetMesh() { return m_pMesh; }

    private:
      std::string m_sShapeName = std::string();

      render::graphics::CMesh* m_pMesh = nullptr;
    };
	}
}

