#pragma once
#include <d3d11.h>
#include "Libs/Math/Transform.h"
#include "Libs/Math/Vector2.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Render/Resources/Material.h"
#include "Engine/Render/Resources/Texture.h"
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
        std::vector<render::gfx::CMesh*> m_vctMeshes;
        std::vector<render::gfx::SVertexData> m_vctVertexData;
      };

    public:
      CModel(const char* _sModelPath, const char* _sBaseMltDir);
      ~CModel();

      void DrawModel();
      void UseGlobalLighting(bool _bEnabled);

      void SetPosition(const math::CVector3& _v3Position) { m_oModelTransform.SetPosition(_v3Position); }
      const math::CVector3& GetPosition() const { return m_oModelTransform.GetPosition(); }
      void SetRotation(const math::CVector3& _v3Rot) { m_oModelTransform.SetRotation(_v3Rot); }
      const math::CVector3& GetRotation() const { return m_oModelTransform.GetRotation(); }
      void SetScale(const math::CVector3& _v3Scale) { m_oModelTransform.SetScale(_v3Scale); }
      const math::CVector3& GetScale() const { return m_oModelTransform.GetScale(); }

    private:
      HRESULT InitModel(const char* _sModelPath, const char* _sBaseMltDir);
      HRESULT CreateInputLayout();

      // DirectX
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      ID3D11InputLayout* m_pInputLayout = nullptr;
      CConstantBuffer<SConstantMatrix> m_oConstantBuffer;

      // Model info
      SModelData m_oModelData = SModelData();
      math::CTransform m_oModelTransform = math::CTransform();
    };
  }
}

