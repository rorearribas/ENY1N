#pragma once

namespace maths
{
  class CVector3
  {
  public:
    static CVector3 vEMPTY;
    static CVector3 vForward;
    static CVector3 vRight;
    static CVector3 vUp;

    CVector3(float _x, float _y, float _z) : m_x(_x), m_y(_y), m_z(_z) {}
    ~CVector3() {}

    inline void operator+=(const CVector3& _v3) { m_x += _v3.m_x; m_y += _v3.m_y; m_z += _v3.m_z; }
    inline void operator-=(const CVector3& _v3) { m_x -= _v3.m_x; m_y -= _v3.m_y; m_z -= _v3.m_z; }

    inline CVector3 operator+(const CVector3& _v3) const { return CVector3(m_x + _v3.m_x, m_y + _v3.m_y, m_z + _v3.m_z); }
    inline CVector3 operator-(const CVector3& _v3) const { return CVector3(m_x - _v3.m_x, m_y - _v3.m_y, m_z - _v3.m_z); }

    inline bool operator==(const CVector3& _v3) const { return m_x == _v3.m_x && m_y == _v3.m_y && m_z == _v3.m_z; }
    inline bool operator!=(const CVector3& _v3)  const { return m_x != _v3.m_x && m_y != _v3.m_y && m_z != _v3.m_z; }

    float DotProduct(const CVector3& _v3) const;
    CVector3 CrossProduct(const CVector3& _v3) const;
    void Normalize();

    const float& GetX() { return m_x; }
    const float& GetY() { return m_y; }
    const float& GetZ() { return m_z; }

  private:
    float m_x = 0.0f;
    float m_y = 0.0f;
    float m_z = 0.0f;
  };

}


