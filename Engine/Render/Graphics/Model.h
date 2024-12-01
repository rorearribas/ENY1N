#pragma once
#include <d3d11.h>
#include "Libs/Maths/Transform.h"
#include "Libs/Maths/Vector2.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Render/Resources/Material.h"
#include "Engine/Render/Resources/Texture.h"
#include "Mesh.h"

namespace render
{
  namespace graphics
  {
    class CModel
    {
    public:
      typedef std::vector<material::CMaterial> TMaterialList;

    public:
      CModel(const char* _sModelPath);
      ~CModel();

      void DrawModel();

      void SetPosition(const maths::CVector3& _v3Position) { m_oModelTransform.SetPosition(_v3Position); }
      const maths::CVector3& GetPosition() const { return m_oModelTransform.GetPosition(); }
      void SetRotation(const maths::CVector3& _v3Rot) { m_oModelTransform.SetRotation(_v3Rot); }
      const maths::CVector3& GetRotation() const { return m_oModelTransform.GetRotation(); }
      void SetScale(const maths::CVector3& _v3Scale) { m_oModelTransform.SetScale(_v3Scale); }
      const maths::CVector3& GetScale() const { return m_oModelTransform.GetScale(); }

    private: 
      HRESULT InitModel(const char* _sPath);
      HRESULT CreateBuffersFromModelData(const CMesh::TVertexDataList& _vctPrimitiveInfo, const CMesh::TIndexesList& _vctIndexes);

      HRESULT CompileShaders();
      HRESULT CreateShaders();
      HRESULT CreateInputLayout();

      // Buffers
      ConstantBuffer<SConstantBuffer> m_oConstantBuffer;
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      ID3D11Buffer* m_pIndexBuffer = nullptr;

      // Vertex shader
      ID3DBlob* m_pVertexShaderBlob = nullptr;
      ID3D11VertexShader* m_pVertexShader = nullptr;

      // Pixel shader
      ID3DBlob* m_pPixelShaderBlob = nullptr;
      ID3D11PixelShader* m_pPixelShader = nullptr;

      // Input layout
      ID3D11InputLayout* m_pInputLayout = nullptr;

      // Data
      TMaterialList m_vctMaterialList = {};
      maths::CTransform m_oModelTransform;
      uint32_t m_uVertexCount = 0;

      render::texture::CTexture* m_pTexture = nullptr;
    };
  }
}

