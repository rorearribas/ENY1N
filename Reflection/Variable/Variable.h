#pragma once
namespace reflection
{
  class CVariable
  {
  public:
    // Constructor
    template<typename _Type = void>
    explicit CVariable(_Type* _pProperty, const char* _sVarName)
      : m_pPtr(_pProperty), m_sVariableName(_sVarName) {
    }
    // Get value from real ptr
    template<typename _Type = void>
    const _Type* TryGetValue() const { return (_Type*)(m_pPtr); }
    // Get property name
    const char* GetVariableName() const { return m_sVariableName; }
    // Get property type
  private:
    const char* m_sVariableName = nullptr;
    void* m_pPtr = nullptr;
  };
}

