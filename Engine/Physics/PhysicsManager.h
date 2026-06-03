#pragma once
#include "Libs/Utils/Singleton.h"
#include "Libs/Utils/FixedPool.h"
#include "Libs/Math/Vector3.h"
#include "Rigidbody.h"

namespace game { class CEntity; }

namespace physics
{
  class CPhysicsManager : public utils::CSingleton<CPhysicsManager>
  {
  public:
    static const uint32_t s_uMaxRigidbodys = 250;
    typedef utils::CFixedPool<CRigidbody, s_uMaxRigidbodys> TRigidbodysList;

    CPhysicsManager() {}
    ~CPhysicsManager();

    void Update(float _fDeltaTime);

    utils::CWeakPtr<CRigidbody> CreateRigidbody(ERigidbodyType _eRigidbodyType = ERigidbodyType::KINEMATIC);
    bool DestroyRigidbody(utils::CWeakPtr<CRigidbody> _wpRigidbody);

  private:
    void Clear();
    TRigidbodysList m_lstRigidbodys;
  };
}


