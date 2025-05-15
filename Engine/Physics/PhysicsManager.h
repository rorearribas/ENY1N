#pragma once
#include "Libs/Utils/Singleton.h"
#include "Libs/Utils/FixedList.h"
#include "Libs/Math/Vector3.h"
#include "Rigidbody.h"

namespace game { class CEntity; }

namespace physics
{
  class CPhysicsManager : public utils::CSingleton<CPhysicsManager>
  {
  public:
    static const uint32_t s_uMaxRigidbodys = 250;
    typedef utils::CFixedList<CRigidbody, s_uMaxRigidbodys> TRigidbodysList;

    CPhysicsManager() {}
    ~CPhysicsManager();

    void Update(float _fDeltaTime);

    CRigidbody* CreateRigidbody(ERigidbodyType _eRigidbodyType = ERigidbodyType::KINEMATIC);
    void DestroyRigidbody(CRigidbody*& _pRigidbody);

  private:
    void Clear();
    TRigidbodysList m_vctRigidbodys;
  };
}


