#pragma once

namespace reflection
{
  class CVariable
  {
  public:
    // Constructor
    explicit CVariable(uint32_t _uOffset, const char* _sVarName) : 
    m_uOffset(_uOffset), m_sVar(_sVarName) {}

    // Get var name
    const char* GetVarName() const { return m_sVar; }

    // Try get value
    template<typename T, typename Object>
    inline T* TryGetValue(Object* _rObject) const { return (T*)((char*)(_rObject) + m_uOffset); }

  private:
    const char* m_sVar = nullptr;
    uint32_t m_uOffset = 0;
  };
}

