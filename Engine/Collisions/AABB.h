#pragma once
#include "Collider.h"
#include "Libs/Math/Math.h"
#include "Libs/Math/Vector3.h"
#include "Libs/Math/Transform.h"

namespace render { namespace gfx { struct TVertexData; } }

namespace collision
{
  class CAABB
  {
  public:
    CAABB() : m_v3Min(math::CVector3::Zero), m_v3Max(math::CVector3::Zero) {}
    CAABB(const math::CVector3& _v3Min, const math::CVector3& _v3Max) : m_v3Min(_v3Min), m_v3Max(_v3Max) {}
    ~CAABB() {}

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
    math::CVector3 m_v3Min = math::CVector3::Zero;
    math::CVector3 m_v3Max = math::CVector3::Zero;
  };

  // AABB
  void ComputeLocalAABB(const std::vector<math::CVector3>& _lstVertices, collision::CAABB& _rLocalAABB_);
  void ComputeLocalAABB(const std::vector<render::gfx::TVertexData>& _lstVertexData, collision::CAABB& _rLocalAABB_);
  void ComputeWorldAABB(const collision::CAABB& _rLocalAABB, const math::CTransform& _rTransform, collision::CAABB& _rWorldAABB_);
}

