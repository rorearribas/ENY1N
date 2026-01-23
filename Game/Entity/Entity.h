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
    typedef std::array<CComponent*, s_iMaxComponents> TComponents;

  public:
    CEntity(std::string _sEntityName) : m_sEntityName(_sEntityName) {}
    ~CEntity();

    void Update(float _fDeltaTime);
    void DrawDebug();

    void SetTickEnabled(bool _bStatus) { m_bTickEnabled = _bStatus; }
    const bool IsTickable() const { return m_bTickEnabled; }

    const std::string& GetName() const { return m_sEntityName; }
    inline const math::CTransform& GetTransform() const { return m_oTransform; }

    void SetPos(const math::CVector3& _v3Pos);
    inline math::CVector3 GetPos() const { return m_oTransform.GetPos(); }
    void SetRot(const math::CVector3& _v3Rot);
    inline math::CVector3 GetRot() const { return m_oTransform.GetRot(); }
    void SetScl(const math::CVector3& _v3Scl);
    inline math::CVector3 GetScl() const { return m_oTransform.GetScl(); }

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
    void OnCollisionEnter(const collision::THitEvent&);
    void OnCollisionStay(const collision::THitEvent&);
    void OnCollisionExit(const collision::THitEvent&);

  private:
    void Clear();

  private:
    TComponents m_lstComponents = TComponents();
    math::CTransform m_oTransform;

    std::string m_sEntityName = {};
    int m_iRegisteredComponents = 0;
    bool m_bTickEnabled = true;
  };
}



