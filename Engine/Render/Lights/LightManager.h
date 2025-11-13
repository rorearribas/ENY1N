#pragma once
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include <array>

namespace render
{
  namespace lights
  {
    // Forward declaration
    class CLight;
    class CSpotLight;
    class CPointLight;
    class CDirectionalLight;

    // WIP -> Posiblemente en un futuro se utilice para hacer una gestion más global
    class CLightsManager
    {
    private:
      static constexpr uint32_t s_uMaxSpotLights = 100;
      static constexpr uint32_t s_uMaxPointLights = 100;

    public:
      typedef std::array<render::lights::CPointLight*, s_uMaxPointLights> TPointLightsList;
      typedef std::array<render::lights::CSpotLight*, s_uMaxSpotLights> TSpotLightsList;

    public:
      CLightsManager();
      ~CLightsManager();

      void Update();

      // Destruction
      void DestroyLight(render::lights::CLight*& pLight_);

      // Creation methods
      render::lights::CDirectionalLight* CreateDirectionalLight();
      render::lights::CPointLight* CreatePointLight();
      render::lights::CSpotLight* CreateSpotLight();

    private:
      void Clean();
      void DestroyPointLight(render::lights::CLight*& pLight_);
      void DestroySpotLight(render::lights::CLight*& pLight_);

      // Directional light
      render::lights::CDirectionalLight* m_pDirectionalLight = nullptr;

      // Point lights
      TPointLightsList m_lstPointLights = {};
      uint32_t m_uRegisteredPointLights = 0;

      // Spot lights
      TSpotLightsList m_lstSpotLights = {};
      uint32_t m_uRegisteredSpotLights = 0;

      // Global lightning buffer
      CConstantBuffer<SGlobalLightingData<s_uMaxPointLights, s_uMaxSpotLights>> m_oLightningBuffer;
    };
  }
}

