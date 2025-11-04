#pragma once
#include "Components/Component.h"
#include "Libs/Math/Transform.h"
#include "Engine/Collisions/Collider.h"
#include <array>
#include <string>

namespace game { class CComponent; }

namespace game
{
  class CEntity
  {
  public:
    static int constexpr s_iMaxComponents = 25;
    typedef std::array<CComponent*, s_iMaxComponents> TComponentsList;

  public:
    CEntity(std::string _sEntityName) : m_sEntityName(_sEntityName) {}
    ~CEntity();

    void Update(float _fDeltaTime);
    void DrawDebug();

    void SetTickEnabled(bool _bStatus) { m_bTickEnabled = _bStatus; }
    const std::string& GetName() const { return m_sEntityName; }

    void SetPosition(const math::CVector3& _v3Position);
    const math::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
    void SetRotation(const math::CVector3& _v3Rot);
    const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
    void SetScale(const math::CVector3& _v3Scale);
    const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }

    template<typename T, typename ...Args>
    inline T* RegisterComponent(Args&&... args)
    {
      if (m_iRegisteredComponents >= s_iMaxComponents) return nullptr;
      T* pComponent = new T(this, std::forward<Args>(args)...);
      m_lstComponents[m_iRegisteredComponents++] = pComponent;
      return static_cast<T*>(pComponent);
    }
    template<typename T>
    inline T* GetComponent()
    {
      // @TODO: Hay que evitar usar dynamic cast!!
      for (CComponent* pComp : m_lstComponents)
      {
        T* pComponent = dynamic_cast<T*>(pComp);
        if (pComponent) return pComponent;
      }
      return nullptr;
    }

    // Notifications
    void OnCollisionEnter(const collision::SHitEvent&);
    void OnCollisionStay(const collision::SHitEvent&);
    void OnCollisionExit(const collision::SHitEvent&);

  private:
    void Clear();

  private:
    TComponentsList m_lstComponents = {};
    math::CTransform m_oTransform;

    std::string m_sEntityName = {};
    int m_iRegisteredComponents = 0;
    bool m_bTickEnabled = true;
  };
}



