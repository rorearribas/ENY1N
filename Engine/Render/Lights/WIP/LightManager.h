#pragma once
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include <array>

namespace render
{
  namespace lights
  {
    // Forward declaration
    class CBaseLight;
    class CSpotLight;
    class CPointLight;
    class CDirectionalLight;

    // WIP -> Posiblemente en un futuro se utilice para hacer una gestion más global
    class CLightsManager
    {
    private:
      static uint32_t constexpr s_uMaxSpotLights = 100;
      static uint32_t constexpr s_uMaxPointLights = 100;

    public:
      typedef std::array<render::lights::CPointLight*, s_uMaxPointLights> TPointLightsList;
      typedef std::array<render::lights::CSpotLight*, s_uMaxSpotLights> TSpotLightsList;

    public:
      CLightsManager();
      ~CLightsManager();

      void Update();

      // Destruction
      void DestroyLight(render::lights::CBaseLight*& pLight_);

      // Creation methods
      render::lights::CDirectionalLight* CreateDirectionalLight();
      render::lights::CPointLight* CreatePointLight();
      render::lights::CSpotLight* CreateSpotLight();

    private:
      void Clean();
      void DestroyPointLight(render::lights::CBaseLight*& pLight_);
      void DestroySpotLight(render::lights::CBaseLight*& pLight_);

      // Directional light
      render::lights::CDirectionalLight* m_pDirectionalLight = nullptr;

      // Point lights
      TPointLightsList m_vctPointLights = {};
      uint32_t m_uRegisteredPointLights = 0;

      // Spot lights
      TSpotLightsList m_vctSpotLights = {};
      uint32_t m_uRegisteredSpotLights = 0;

      // Global lightning buffer
      CConstantBuffer<SGlobalLightingData<s_uMaxPointLights, s_uMaxSpotLights>> m_oLightningBuffer;
    };
  }
}

