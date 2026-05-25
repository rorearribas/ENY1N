#pragma once
#include "Engine/Render/Buffers/BufferTypes.h"
#include "Engine/Render/Buffers/ConstantBuffer.h"
#include "Engine/Render/Lighting/PointLight.h"
#include "Engine/Render/Lighting/SpotLight.h"
#include "Engine/Render/Graphics/ShadowMap.h"
#include "Engine/Render/Resources/Texture2D.h"

#include "Libs/Utils/Singleton.h"
#include "Libs/Utils/FixedPool.h"

namespace render
{
  namespace lights
  {
    // Forward declaration
    class CLight;
    class CDirectionalLight;

    class CLightManager
    {
    public:
      static constexpr uint16_t s_uMaxShadowMaps = 4u;
      static constexpr uint16_t s_uMaxSpotLights = 100u;
      static constexpr uint16_t s_uMaxPointLights = 100u;

      typedef utils::CFixedPool<render::lights::CPointLight, s_uMaxPointLights> TPointLights;
      typedef utils::CFixedPool<render::lights::CSpotLight, s_uMaxSpotLights> TSpotLights;
      typedef utils::CFixedPool<render::gfx::CShadowMap, s_uMaxShadowMaps> TShadowMaps;

    private:
      typedef TGlobalLighting<s_uMaxPointLights, s_uMaxSpotLights> TLightingData;
      typedef CConstantBuffer<TLightingData> TLightingBuffer;

    public:
      CLightManager();
      ~CLightManager();

      // Push lights
      void ComputeShadows();
      void ApplyLighting();

      // Shadow maps
      const TShadowMaps& GetShadowMaps() { return m_lstShadowMaps; }
      render::lights::CDirectionalLight* const GetDirectionalLight() { return m_pDirectionalLight.get(); }

      // Handle lights
      render::lights::CDirectionalLight* const CreateDirectionalLight();
      render::lights::CPointLight* const CreatePointLight();
      render::lights::CSpotLight* const CreateSpotLight();

      // Destruction
      bool DestroyLight(render::lights::CLight*& pLight_);

    private:
      void Clean();
      HRESULT Setup();

    private:
      // Lights
      std::unique_ptr<render::lights::CDirectionalLight> m_pDirectionalLight;
      TPointLights m_lstPointLights = TPointLights();
      TSpotLights m_lstSpotLights = TSpotLights();

      // Global lighting buffer
      TLightingBuffer m_oLightingBuffer;
      TShadowMaps m_lstShadowMaps;
    };
  }
}

