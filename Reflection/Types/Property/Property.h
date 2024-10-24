#pragma once
class CProperty
{
public:
  // Constructor
  CProperty() = default;
  template<typename _Type = void>
  explicit CProperty(_Type* _pProperty, const char* _sPropertyName) 
  : m_pPtr(_pProperty), m_sPropertyName(_sPropertyName) {}
  // Get value from real ptr
  template<typename _Type = void>
  const _Type* TryGetValue() const { return (_Type*)(m_pPtr); }
  // Get property name
  const char* GetPropertyName() const { return m_sPropertyName; }
  // Get property type
private:
  const char* m_sPropertyName = nullptr;
  void* m_pPtr = nullptr;
};