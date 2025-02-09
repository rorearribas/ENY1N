#pragma once
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Scenes/Scene.h"
#include "Libs/Utils/Singleton.h"
#include <array>

namespace scene
{
  class CSceneManager
  {
  public:
    static int constexpr s_iMaxScenes = 5;
    typedef std::array<CScene*, s_iMaxScenes> TSceneList;

    CSceneManager();
    ~CSceneManager();

    const TSceneList& GetScenes() { return m_vctScenes; }
    scene::CScene* const GetCurrentScene() { return m_pCurrentScene; };

    void SetSceneEnabled(uint32_t _uIndex, bool _bEnabled) const;
    void DisableAllScenes() const;

    render::graphics::CPrimitive* CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData, uint32_t _uSceneIndex = 0);
    render::graphics::CPrimitive* CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType, uint32_t _uSceneIndex = 0);
    render::graphics::CModel* CreateModel(const char* _sModelPath, const char* _sBaseMltDir, uint32_t _uSceneIndex = 0);

    render::lights::CDirectionalLight* CreateDirectionalLight(uint32_t _uSceneIndex = 0);
    render::lights::CPointLight* CreatePointLight(uint32_t _uSceneIndex = 0);

    void DestroyModel(render::graphics::CModel*& pModel_, uint32_t _uSceneIndex = 0);
    void DestroyPrimitive(render::graphics::CPrimitive*& _pPrimitive_, uint32_t _uSceneIndex = 0);
    void DestroyLight(render::lights::CLight*& _pLight_, uint32_t _uSceneIndex = 0);

    void DestroyAllPrimimitives(uint32_t _uSceneIndex = 0);
    void DestroyAllModels(uint32_t _uSceneIndex = 0);

  private:
    void CreateScenes();
    void DestroyAllScenes();

    TSceneList m_vctScenes = {};
    mutable scene::CScene* m_pCurrentScene = nullptr;
  };
}



