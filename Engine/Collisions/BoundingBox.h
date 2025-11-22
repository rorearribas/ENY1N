#pragma once
#include "Collider.h"
#include "Libs/Math/Math.h"
#include "Libs/Math/Vector3.h"
#include "Libs/Math/Transform.h"

namespace collision
{
  class CBoundingBox
  {
  public:
    CBoundingBox() : m_v3Min(math::CVector3::Zero), m_v3Max(math::CVector3::Zero) {}
    CBoundingBox(const math::CVector3& _v3Min, const math::CVector3& _v3Max) : m_v3Min(_v3Min), m_v3Max(_v3Max) {}
    ~CBoundingBox() {}

    inline void SetMin(const math::CVector3& _v3Min) { m_v3Min = _v3Min; }
    inline const math::CVector3& GetMin() const { return m_v3Min; }
    inline void SetMax(const math::CVector3& _v3Max) { m_v3Max = _v3Max; }
    inline const math::CVector3& GetMax() const { return m_v3Max; }

    void GetExtents(math::CVector3 lstExtents[8]) const;
    inline math::CVector3 GetCenter() const { return math::CVector3(m_v3Max + m_v3Min) * 0.5f; }
    inline math::CVector3 GetHalfSize() const { return math::CVector3(m_v3Max - m_v3Min) * 0.5f; }
    inline math::CVector3 GetSize() const { return math::CVector3(m_v3Max - m_v3Min); }

    void DrawDebug(math::CVector3 _v3Color = math::CVector3::Right) const;

  private:
    math::CVector3 m_v3Min;
    math::CVector3 m_v3Max;
  };

  // Global
  template<typename T>
  inline void ComputeLocalAABB(const std::vector<T>& _lstVertexData, collision::CBoundingBox& _rLocalAABB_)
  {
    if (_lstVertexData.empty())
    {
      return;
    }

    // Compute AABB
    math::CVector3 v3Min(FLT_MAX, FLT_MAX, FLT_MAX);
    math::CVector3 v3Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (const auto& rVertexData : _lstVertexData)
    {
      // Calculate Min
      v3Min.x = math::Min(v3Min.x, rVertexData.VertexPos.x);
      v3Min.y = math::Min(v3Min.y, rVertexData.VertexPos.y);
      v3Min.z = math::Min(v3Min.z, rVertexData.VertexPos.z);

      // Calculate Max
      v3Max.x = math::Max(v3Max.x, rVertexData.VertexPos.x);
      v3Max.y = math::Max(v3Max.y, rVertexData.VertexPos.y);
      v3Max.z = math::Max(v3Max.z, rVertexData.VertexPos.z);
    }

    // Set Local AABB
    _rLocalAABB_.SetMin(v3Min);
    _rLocalAABB_.SetMax(v3Max);
  }
  void ComputeWorldAABB(const collision::CBoundingBox& _rLocalAABB, const math::CTransform& _mTransform, collision::CBoundingBox& _rWorldAABB_);
}

