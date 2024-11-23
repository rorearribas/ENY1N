#pragma once
namespace game { class CEntity; }

namespace game
{
  class CComponent
  {
  public:
    using Super = CComponent;

    CComponent(CEntity* _pOwner) : m_pOwner(_pOwner) {}
    virtual ~CComponent() { m_pOwner = nullptr; }

    virtual void Update(float _fDeltaTime) = 0;

  protected:
    CEntity* m_pOwner = nullptr;
  };
}


