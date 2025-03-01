#pragma once
#include "Libs/Utils/Singleton.h"
#include "Libs/Utils/FixedList.h"
#include "Libs/Maths/Vector3.h"
#include "Rigidbody.h"

namespace game { class CEntity; }

namespace physics
{
  class CPhysicsManager : public utils::CSingleton<CPhysicsManager>
  {
  public:
    static const uint32_t s_uMaxRigidbodys = 250;
    typedef utils::CFixedList<physics::CRigidbody, s_uMaxRigidbodys> TRigidbodysList;

    CPhysicsManager() {}
    ~CPhysicsManager() {}

    void Update(float _fDeltaTime);

    physics::CRigidbody* CreateRigidbody(game::CEntity* _pEntity);
    void DestroyRigidbody(physics::CRigidbody*& _pRigidbody);

  private:
    void Flush();

    float m_fGravityForce = -9.8f;
    float m_fOverrideForce = 1.0f;

    TRigidbodysList m_vctRigidbodys;
  };
}


