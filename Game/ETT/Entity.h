#pragma once
#include "Components/Component.h"
#include "Libs/Maths/Transform.h"
#include <array>

namespace game { class CComponent; }
namespace game
{
  class CEntity
  {
  public:
    static int constexpr s_iMaxComponents = 25;
    typedef std::array<CComponent*, s_iMaxComponents> TComponentsList;

  public:
    CEntity(const char* _sEntityName) : m_sEntityName(_sEntityName) {}
    ~CEntity();

    void Update(float _fDeltaTime);
    void DrawDebug();

    void SetTickEnabled(bool _bStatus) { m_bTickEnabled = _bStatus; }
    const char* GetEntityName() { return m_sEntityName; }

    void SetPosition(const maths::CVector3& _v3Position);
    const maths::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
    void SetRotation(const maths::CVector3& _v3Rot);
    const maths::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
    void SetScale(const maths::CVector3& _v3Scale);
    const maths::CVector3& GetScale() const { return m_oTransform.GetScale(); }

    template<typename T>
    inline T* RegisterComponent()
    {
      if (m_iRegisteredComponents >= s_iMaxComponents) return nullptr;
      CComponent* pComponent = new T(this);
      m_vctComponents[m_iRegisteredComponents++] = pComponent;
      return static_cast<T*>(pComponent);
    }
    template<typename T>
    inline T* GetComponent()
    {
      for (CComponent* pComp : m_vctComponents)
      {
        T* pComponent = static_cast<T*>(pComp);
        if (pComponent) return pComponent;
      }
      return nullptr;
    }

  private:
    void DestroyAllComponents();

    TComponentsList m_vctComponents = {};
    maths::CTransform m_oTransform;

    const char* m_sEntityName = nullptr;
    bool m_bTickEnabled = true;
    int m_iRegisteredComponents = 0;
  };
}


