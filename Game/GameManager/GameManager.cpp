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
    static std::vector<std::string> lstDeleteActors = {};
    ImGui::Begin("Entity Selector");
    // Show the list of entities
    for (uint32_t uIndex = 0; uIndex < m_lstEntitiesList.GetSize(); ++uIndex)
    {
      utils::CWeakPtr<CEntity> pEntity = m_lstEntitiesList[uIndex];
      std::string sLabel = pEntity->GetName() + "##" + std::to_string(uIndex);
      if (ImGui::Selectable(sLabel.c_str(), iSelectedIdx == static_cast<int>(uIndex)))
      {
        iSelectedIdx = uIndex;
      }

      ImGui::SameLine();
      ImGui::PushID(uIndex);
      if (ImGui::Button("Destroy"))
      {
        lstDeleteActors.emplace_back(pEntity->GetName());
      }
      ImGui::PopID();
    }
    ImGui::End();

    for (std::string sActorID : lstDeleteActors)
    {
      DestroyEntity(sActorID.c_str());
    }
    lstDeleteActors.clear();
#endif
    // Update
    for (uint32_t uIndex = 0; uIndex < m_lstEntitiesList.GetSize(); ++uIndex)
    {
      utils::CWeakPtr<CEntity> pEntity = m_lstEntitiesList[uIndex];
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
  // ------------------------------------
  void CGameManager::DestroyAll()
  {
    m_lstEntitiesList.Clear();
  }
  // ------------------------------------
  utils::CWeakPtr<CEntity> CGameManager::CreateEntity(const char* _sEntityName)
  {
    if (m_uRegisteredEntities >= m_lstEntitiesList.GetMaxSize())
    {
      return utils::CWeakPtr<CEntity>();
    }

    // Check collision name
    std::string sTarget = _sEntityName;
    if (m_uSetNames.count(sTarget))
    {
      uint32_t& uNextSuffix = m_uMapNextSuffix[_sEntityName];
      if (uNextSuffix == 0) 
      {
        uNextSuffix = 1;
      }
      while (m_uSetNames.count(sTarget)) 
      {
        std::ostringstream oStringStream;
        oStringStream << _sEntityName << "_" << uNextSuffix;
        sTarget = oStringStream.str();
        uNextSuffix++;
      }
    }
    m_uSetNames.emplace(sTarget);

    // Register entity
    m_uRegisteredEntities++;
    utils::CWeakPtr<CEntity> wpEntity = m_lstEntitiesList.Create(sTarget.c_str());
    return wpEntity;
  }
  // ------------------------------------
  bool CGameManager::DestroyEntity(const char* _sEntityName)
  {
    utils::CWeakPtr<CEntity> wpEntity = utils::CWeakPtr<CEntity>();
    for (uint32_t uIndex = 0; uIndex < m_lstEntitiesList.GetSize(); ++uIndex)
    {
      wpEntity = m_lstEntitiesList[uIndex];
      if (wpEntity.IsValid() && wpEntity->GetName() == _sEntityName)
      {
        break;
      }
    }
    return m_lstEntitiesList.Remove(wpEntity);
  }
}