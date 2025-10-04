#include "GameManager.h"
#include <algorithm>
#include <sstream>
#include "Libs/ImGui/imgui.h"
#include "Libs/Macros/GlobalMacros.h"

namespace game
{
  // ------------------------------------
  CGameManager::~CGameManager()
  {
    DestroyAll();
  }
  // ------------------------------------
  void CGameManager::Update(float _fDeltaTime)
  {
    static int iSelectedIdx = -1;
#ifdef DEBUG_MODE
    ImGui::Begin("Entity Selector");
    // Show the list of entities
    for (uint32_t uIndex = 0; uIndex < m_vctEntitiesList.CurrentSize(); ++uIndex)
    {
      CEntity* pEntity = m_vctEntitiesList[uIndex];
      if (pEntity)
      {
        std::string sLabel = pEntity->GetName() + "##" + std::to_string(uIndex);
        if (ImGui::Selectable(sLabel.c_str(), iSelectedIdx == static_cast<int>(uIndex)))
        {
          iSelectedIdx = uIndex;
        }
      }
    }
    ImGui::End();
#endif
    // Update
    for (uint32_t uIndex = 0; uIndex < m_vctEntitiesList.CurrentSize(); ++uIndex)
    {
      CEntity* pEntity = m_vctEntitiesList[uIndex];
      if (pEntity)
      {
        pEntity->Update(_fDeltaTime);
#ifdef DEBUG_MODE
        if (uIndex == static_cast<uint32_t>(iSelectedIdx))
        {
          ImGui::PushID(uIndex);
          pEntity->DrawDebug();
          ImGui::PopID();
        }
#endif
      }
    }
  }
  // ------------------------------------
  void CGameManager::DestroyAll()
  {
    m_vctEntitiesList.ClearAll();
  }
  // ------------------------------------
  CEntity* CGameManager::CreateEntity(const char* _sEntityName)
  {
    if (m_uRegisteredEntities >= m_vctEntitiesList.GetMaxSize())
      return nullptr;

    // Check collision name
    std::string sTargetEntityID = _sEntityName;
    uint32_t uNameSuffix = 1;

    for (uint32_t uIndex = 0; uIndex < m_uRegisteredEntities; uIndex++)
    {
      CEntity* pEntity = m_vctEntitiesList[uIndex];
      if (pEntity && pEntity->GetName() == sTargetEntityID)
      {
        std::ostringstream oStringStream;
        oStringStream << _sEntityName << "_" << uNameSuffix++;
        sTargetEntityID = oStringStream.str();
      }
    }

    // Register entity
    m_uRegisteredEntities++;
    CEntity* pEntity = m_vctEntitiesList.RegisterItem(sTargetEntityID.c_str());
    return pEntity;
  }
  // ------------------------------------
  bool CGameManager::DestroyEntity(const char* _sEntityName)
  {
    CEntity* pEntity = nullptr;
    for (uint32_t uIndex = 0; uIndex < m_vctEntitiesList.GetMaxSize(); ++uIndex)
    {
      pEntity = m_vctEntitiesList[uIndex];
      if (pEntity && pEntity->GetName() == _sEntityName)
      {
        break;
      }
    }
    return m_vctEntitiesList.RemoveItem(pEntity);
  }
}