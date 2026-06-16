#pragma once
#include "Components/Component.h"
#include "Engine/Collisions/Collider.h"

#include "Libs/Math/Transform.h"
#include "Libs/Utils/ArenaPool.h"
#include "Libs/Macros/GlobalMacros.h"

namespace game { class CComponent; }

namespace game
{
  class CEntity
  {
  private:
    static constexpr uint32_t s_uMaxBlockComponents = 1024u;
    static constexpr uint32_t s_uMaxComponents = 8u;

  public:
    CEntity(std::string _sEntityName) : m_sEntityName(_sEntityName) {}
    ~CEntity();

    CEntity(CEntity&& _rOther) noexcept;
    CEntity& operator=(CEntity&& _rOther) noexcept;
    CEntity(const CEntity& _rOther) = delete;
    CEntity& operator=(const CEntity& _rOther) = delete;

    void Update(float _fDeltaTime);
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
    inline T* RegisterComponent(Args&&... _rArgs)
    {
      T* pComponent = m_lstComponents.Alloc<T>(this, std::forward<Args>(_rArgs)...);
      return pComponent;
    }
    template<typename T>
    inline T* GetComponent()
    {
      for (uint16_t uI = 0; uI < m_lstComponents.GetSize(); uI++)
      {
        T* pComponent = static_cast<T*>(m_lstComponents[uI]);
        if (pComponent)
        {
          return pComponent;
        }
      }
      return nullptr;
    }

    // Notifications
    void OnCollisionEnter(const collision::THitEvent&);
    void OnCollisionStay(const collision::THitEvent&);
    void OnCollisionExit(const collision::THitEvent&);

#ifdef ENABLE_IMGUI
    void ShowGizmo();
#endif // ENABLE_IMGUI
#ifdef _DEBUG
    void DrawDebug();
#endif

  private:
    void Clear();

    utils::CArenaPool<CComponent, s_uMaxBlockComponents, s_uMaxComponents> m_lstComponents;
    math::CTransform m_oTransform = math::CTransform();

    std::string m_sEntityName = std::string();
    bool m_bTickEnabled = true;
  };
}



