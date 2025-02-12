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

    // WIP

    class CShape
    {
    public:
      CShape(std::string _sShapeName) : m_pShapeId(_sShapeName) {}
      ~CShape() {}

      render::graphics::CMesh* const GetMesh() { return m_pMesh; }
      const std::string& GetShapeId() const { return m_pShapeId;}

    private:
      std::string m_pShapeId = std::string();
      render::graphics::CMesh* m_pMesh = nullptr;
    };
	}
}

