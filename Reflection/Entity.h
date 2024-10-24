#pragma once
#include "Type.h"

class CEntity
{
  DECLARE_CLASS_IN_REFLECTION(CEntity)

public:
  explicit CEntity(const char* _sEntityName) : m_sEntityName(_sEntityName) {}
  ~CEntity() {}

private:
  void Callback();
  void Callback2(const char* _sCallback);

  float m_fLife = 100.0f;
  const char* m_sEntityName = nullptr;
};

