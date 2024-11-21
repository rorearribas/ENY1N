#pragma once
#include <d3d11.h>
#include <vector>
#include "Libs/Maths/Vector3.h"
#include "Libs/Maths/Matrix4x4.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"

namespace render
{
  namespace primitive
  {
    class CPrimitive 
    {
    public:
      enum EPrimitiveType { SQUARE, TRIANGLE, CUBE };
      struct SPrimitiveInfo
      {
        maths::CVector3 Position = maths::CVector3::Zero;
        maths::CVector3 Color = maths::CVector3::One;
      };

      CPrimitive(const EPrimitiveType& _ePrimitiveType);
      CPrimitive(const std::vector<SPrimitiveInfo>& _vctVertexData);
      ~CPrimitive();

      // Draw primitive
      void Draw();

      void SetPosition(const maths::CVector3& _v3Position) { m_v3Pos = _v3Position; }
      const maths::CVector3& GetPosition() const { return m_v3Pos; }
      void SetRotation(const maths::CVector3& _v3Rot) { m_v3Rot = _v3Rot; }
      const maths::CVector3& GetRotation() const { return m_v3Rot; }
      void SetScale(const maths::CVector3& _v3Scale) { m_v3Scale = _v3Scale; }
      const maths::CVector3& GetScale() const { return m_v3Scale; }
      void SetColor(const maths::CVector3& _v3Color);
      const maths::CVector3& GetColor() const { return m_v3CurrentColor; }

      ID3D11Buffer* GetBuffer() { return m_pVertexBuffer; }
      ConstantBuffer<SConstantBuffer>& GetConstantBuffer() { return m_oConstantBuffer; }

      UINT GetIndexCount() { return m_uVertexCount; }
      ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; }

      ID3D11VertexShader* GetVertexShader() { return m_pVertexShader; }
      ID3D11PixelShader* GetPixelShader() { return m_pPixelShader; }

    private:
      HRESULT InitPrimitive();
      HRESULT CompileShaders();
      HRESULT InitShaders();
      HRESULT CreateInputLayout();
      HRESULT CreateBufferFromVertexData(const std::vector<CPrimitive::SPrimitiveInfo>& _vctPrimitiveInfo, const std::vector<UINT>& _vctIndexes = {});

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
      maths::CVector3 m_v3CurrentColor = maths::CVector3::One;

      // Transform
      maths::CVector3 m_v3Pos = maths::CVector3::Zero;
      maths::CVector3 m_v3Rot = maths::CVector3::Zero;
      maths::CVector3 m_v3Scale = maths::CVector3::One;
    };
  }
}
