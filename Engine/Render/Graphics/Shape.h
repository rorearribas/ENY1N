#pragma once
#include <string>
#include "Mesh.h"

namespace render
{
	namespace graphics
	{
    class CShape
    {
    public:
      CShape(std::string _sShapeName) : m_sShapeName(_sShapeName) {}
      ~CShape() {}


    private:
      std::string m_sShapeName = std::string();

      render::graphics::CMesh* m_pMesh = nullptr;
    };
	}
}

