#pragma once
#include "Engine/Render/Graphics/Mesh.h"
#include "Engine/Render/Graphics/RenderInstance.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Collisions/BoundingBox.h"

#include "Libs/Utils/FixedPool.h"
#include "Libs/Math/Transform.h"

namespace render
{
  namespace gfx
  {
    struct TVertexData
    {
      math::CVector3 VertexPos = math::CVector3::Zero;
      math::CVector3 Normal = math::CVector3::Zero;
      math::CVector3 Color = math::CVector3::One;
      math::CVector2 TexCoord = math::CVector2::Zero;

      bool operator==(const TVertexData& _other) const;
      bool operator!=(const TVertexData& _other) const;
    };

    struct TInstanceData
    {
      math::CMatrix4x4 Transform = math::CMatrix4x4::Identity;
    };

    static constexpr uint32_t s_uMaxInstances = 50000u; // Testing
    typedef utils::CFixedPool<render::gfx::CRenderInstance, s_uMaxInstances> TInstances;

    class CModel
    {
    public:
      struct TModelData
      {
        std::vector<std::shared_ptr<render::gfx::CMesh>> Meshes;
        std::vector<render::gfx::TVertexData> Vertices;
        char AssetPath[128];
      };

    public:
      CModel(const TModelData& _rModelData);
      ~CModel();

      void Draw();
      void DrawInstances(const std::vector<uint32_t>& lstDrawableInstances);

      TInstances& GetInstances() { return m_lstInstances; }
      const TInstances& GetInstances() const { return m_lstInstances; }

      void ComputeBoundingBox(const math::CMatrix4x4& _mTransform, collision::CBoundingBox& _rBoundingBox_) const;
      inline const collision::CBoundingBox& GetBoundingBox() const { return m_oBoundingBox; }

      void SetCullingEnabled(bool _bCull);
      inline const bool& IsCullingEnabled() const { return m_bCullEnabled; }

      inline void SetVisible(bool _bVisible) { m_bVisible = _bVisible; }
      inline const bool& IsVisible() const { return m_bVisible; }

      void SetPosition(const math::CVector3& _v3Pos);
      inline const math::CVector3& GetPosition() const { return m_oTransform.GetTranslation(); }
      void SetRotation(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
      void SetScale(const math::CVector3& _v3Scl);
      inline const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }

      CRenderInstance* CreateInstance();
      bool RemoveInstance(uint32_t _uInstanceID);

      const bool HasInstances() const { return m_lstInstances.GetCurrentSize() > 0; }
      const bool AllowInstances() const { return m_lstInstances.GetCurrentSize() < m_lstInstances.GetMaxSize(); }
      std::string GetAssetPath() const { return std::string(m_oModelData.AssetPath); }

    private:
      HRESULT InitModel(const TModelData& _rModelData);
      void Clear();

    private:
      // Buffers
      ID3D11Buffer* m_pVertexBuffer = nullptr;
      ID3D11Buffer* m_pInstanceBuffer = nullptr;

    private:
      TModelData m_oModelData = TModelData();
      TInstances m_lstInstances = TInstances();

    private:
      math::CTransform m_oTransform = math::CTransform();
      collision::CBoundingBox m_oBoundingBox = collision::CBoundingBox();

      bool m_bCullEnabled = true;
      bool m_bVisible = true;
    };
  }
}

namespace std
{
  template <>
  struct hash<render::gfx::TVertexData>
  {
    size_t operator()(const render::gfx::TVertexData& v) const
    {
      auto oFunc = [](float f) { return static_cast<int>(f * 1000); };
      size_t h1 = hash<int>()(oFunc(v.VertexPos.x)) ^ hash<int>()(oFunc(v.VertexPos.y)) ^ hash<int>()(oFunc(v.VertexPos.z));
      size_t h2 = hash<int>()(oFunc(v.Normal.x)) ^ hash<int>()(oFunc(v.Normal.y)) ^ hash<int>()(oFunc(v.Normal.z));
      size_t h3 = hash<int>()(oFunc(v.TexCoord.x)) ^ hash<int>()(oFunc(v.TexCoord.y));
      return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
  };
}