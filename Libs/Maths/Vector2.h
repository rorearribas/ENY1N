#pragma once

namespace maths
{
  class CVector2
  {
  public:
    static CVector2 vEMPTY;
    static CVector2 vRight;
    static CVector2 vUp;

    CVector2(float _x, float _y) : m_x(_x), m_y(_y) {}
    ~CVector2() {}

    inline void operator+=(const CVector2& _v2) { m_x += _v2.m_x; m_y += _v2.m_y; }
    inline void operator-=(const CVector2& _v2) { m_x -= _v2.m_x; m_y -= _v2.m_y; }

    inline CVector2 operator+(const CVector2& _v2) const { return CVector2(m_x + _v2.m_x, m_y + _v2.m_y); }
    inline CVector2 operator-(const CVector2& _v2) const { return CVector2(m_x - _v2.m_x, m_y - _v2.m_y); }

    inline bool operator==(const CVector2& _v2) const { return m_x == _v2.m_x && m_y == _v2.m_y; }
    inline bool operator!=(const CVector2& _v2) const { return m_x != _v2.m_x && m_y != _v2.m_y; }

    float DotProduct(const CVector2& _v2) const;
    float CrossProduct(const CVector2& _v2) const;
    void Normalize();

    const float& GetX() { return m_x; }
    const float& GetY() { return m_y; }

  private:
    float m_x = 0.0f;
    float m_y = 0.0f;
  };
}



