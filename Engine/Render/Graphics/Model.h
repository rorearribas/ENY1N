#pragma once
#include "Libs/Math/Transform.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Mesh.h"

namespace render
{
  namespace gfx
  {
    class CModel
    {
    public:
      struct SModelData
      {
        std::vector<std::shared_ptr<render::gfx::CMesh>> Meshes;
        std::vector<render::gfx::SVertexData> Vertices;
      };

    public:
      CModel(const char* _sModelPath);
      ~CModel();

      void Draw();

      void SetPosition(const math::CVector3& _v3Position) { m_oTransform.SetPosition(_v3Position); }
      const math::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
      void SetRotation(const math::CVector3& _v3Rot) { m_oTransform.SetRotation(_v3Rot); }
      const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
      void SetScale(const math::CVector3& _v3Scale) { m_oTransform.SetScale(_v3Scale); }
      const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }

    private:
      void Clear();
      HRESULT InitModel(const char* _sModelPath);
      HRESULT CreateInputLayout();

    private:
      // DirectX
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      ID3D11InputLayout* m_pInputLayout = nullptr;

      // Model info
      SModelData m_oModelData = SModelData();
      math::CTransform m_oTransform = math::CTransform();
    };
  }
}

