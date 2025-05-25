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
    class CLightManager
    {
    private:
      static int constexpr s_iMaxSpotLights = 100;
      static int constexpr s_iMaxPointLights = 100;

    public:
      typedef std::array<render::lights::CPointLight*, s_iMaxPointLights> TPointLightsList;
      typedef std::array<render::lights::CSpotLight*, s_iMaxSpotLights> TSpotLightsList;

    public:
      CLightManager();
      ~CLightManager();

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
      CConstantBuffer<SGlobalLightingData<s_iMaxPointLights, s_iMaxSpotLights>> m_oLightningBuffer;
    };
  }
}

