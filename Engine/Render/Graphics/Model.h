#pragma once
#include <d3d11.h>
#include "Libs/Maths/Transform.h"
#include "Libs/Maths/Vector2.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Render/Resources/Material.h"
#include "Engine/Render/Resources/Texture.h"
#include "Mesh.h"
#include "Shape.h"

namespace render
{
  namespace graphics
  {
    class CModel
    {
    public:
      struct SModelData
      {
        std::vector<render::graphics::CMesh*> m_vctMeshes = {};
        std::vector<render::graphics::SVertexData> m_vctVertexData = {};
      };
    public:

      CModel(const char* _sModelPath, const char* _sBaseMltDir);
      ~CModel();

      void DrawModel(ID3D11PixelShader* _pPixelShader, ID3D11VertexShader* _pVertexShader);

      void SetPosition(const maths::CVector3& _v3Position) { m_oModelTransform.SetPosition(_v3Position); }
      const maths::CVector3& GetPosition() const { return m_oModelTransform.GetPosition(); }
      void SetRotation(const maths::CVector3& _v3Rot) { m_oModelTransform.SetRotation(_v3Rot); }
      const maths::CVector3& GetRotation() const { return m_oModelTransform.GetRotation(); }
      void SetScale(const maths::CVector3& _v3Scale) { m_oModelTransform.SetScale(_v3Scale); }
      const maths::CVector3& GetScale() const { return m_oModelTransform.GetScale(); }

    private: 
      HRESULT InitModel(const char* _sModelPath, const char* _sBaseMltDir);
      HRESULT CreateInputLayout();

      // Buffer
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      ConstantBuffer<SConstantMatrix> m_oConstantBuffer;

      // Input layout
      ID3D11InputLayout* m_pInputLayout = nullptr;

      // Model data
      SModelData m_oModelData;
      maths::CTransform m_oModelTransform;
    };
  }
}

