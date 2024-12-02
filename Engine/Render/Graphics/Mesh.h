#pragma once
#include "Libs/Maths/Vector3.h"
#include "Libs/Maths/Vector2.h"
#include "Engine/Render/Resources/Material.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"

#include <vector>
#include <functional>
#include "Libs/Maths/Transform.h"

namespace render
{
  namespace graphics
  {
    struct SVertexData
    {
      maths::CVector3 Position = maths::CVector3::Zero;
      maths::CVector3 Normal = maths::CVector3::Zero;
      maths::CVector3 Color = maths::CVector3::One;
      maths::CVector2 TexCoord = maths::CVector2::Zero;

      bool operator==(const SVertexData& _other) const
      {
        return Position == _other.Position && Normal == _other.Normal && TexCoord == _other.TexCoord;
      }
      bool operator!=(const SVertexData& _other) const 
      {
        return !(*this == _other);
      }
    };
    
    class CMesh
    {
    public:
      typedef std::vector<render::graphics::SVertexData> TVertexDataList;
      typedef std::vector<uint32_t> TIndexesList;

    public:
      CMesh(const std::string& _sMeshName);
      ~CMesh();

      void DrawMesh(const maths::CTransform& _oTransform);
      void ApplyMaterial(render::material::CMaterial* _pMaterial);
      HRESULT CreateMesh(TVertexDataList& _vctVertexData, TIndexesList& _vctIndexes);

    private:
      HRESULT InitMesh();
      HRESULT CreateShaders();
      HRESULT CreateInputLayout();
      HRESULT CompileShaders();

      // Info
      std::string m_sMeshName = std::string();

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

      uint32_t m_uVertexCount = 0;

      render::material::CMaterial* m_pMaterial = nullptr;
      render::texture::CTexture* m_pTexture = nullptr;
    };
  }
}

namespace std
{
  template<>
  struct hash<render::graphics::SVertexData>
  {
    std::size_t operator()(const render::graphics::SVertexData& v) const
    {
      // Combinar los hashes de los miembros
      size_t h1 = std::hash<maths::CVector3>()(v.Position);
      size_t h2 = std::hash<maths::CVector3>()(v.Normal);
      size_t h3 = std::hash<maths::CVector2>()(v.TexCoord);
      return h1 ^ (h2 << 1) ^ (h3 << 2); // Combinar los hashes
    }
  };
}