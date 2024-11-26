#pragma once
#include "Libs/Maths/Transform.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include <d3d11.h>

namespace render
{
  namespace graphics
  {
    class CModel
    {
    private:
      struct SModelInfo
      {
        maths::CVector3 Position = maths::CVector3::Zero;
        maths::CVector3 Color = maths::CVector3(0.2f, 0.2f, 0.2f);
        //maths::CVector3 Normal = maths::CVector3::Forward * -1.0f;
      };

    public:
      CModel(const char* _sPath);
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
      HRESULT CompileShaders();
      HRESULT InitShaders();
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
      UINT m_uVertexCount = 0;
      maths::CTransform m_oModelTransform;
    };
  }
}

