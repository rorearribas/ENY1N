#pragma once

#include "Engine/Render/Lighting/DirectionalLight.h"
#include "Engine/Render/Lighting/PointLight.h"
#include "Engine/Render/Lighting/SpotLight.h"
#include "Engine/Render/Graphics/ShadowMap.h"
#include "Engine/Render/Resources/Texture2D.h"

#include "Engine/Render/Buffers/BufferTypes.h"
#include "Engine/Render/Buffers/ConstantBuffer.h"

#include "Libs/Utils/Singleton.h"
#include "Libs/Utils/FixedPool.h"

namespace render
{
  namespace lights
  {
    // Forward declaration
    class CLight;

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

      // Handle lights
      utils::CWeakPtr<render::lights::CDirectionalLight> GetDirectionalLight();
      inline const TPointLights& GetPointLights() const { return m_lstPointLights; }
      inline const TSpotLights& GetSpotLights() const { return m_lstSpotLights; }

      // Lights creation
      utils::CWeakPtr<render::lights::CDirectionalLight> const CreateDirectionalLight();
      utils::CWeakPtr<render::lights::CPointLight> const CreatePointLight();
      utils::CWeakPtr<render::lights::CSpotLight> const CreateSpotLight();
      bool DestroyLight(utils::CWeakPtr<render::lights::CLight> _wpLight);

      // Shadow mapping
      inline const TShadowMaps& GetShadowMaps() const { return m_lstShadowMaps; }

    private:
      void Clean();
      HRESULT Setup();

    private:
      // Lighting
      utils::CUniquePtr<render::lights::CDirectionalLight> m_pDirectionalLight;
      TPointLights m_lstPointLights = TPointLights();
      TSpotLights m_lstSpotLights = TSpotLights();

      // Global lighting buffer
      TLightingBuffer m_oLightingBuffer;
      TShadowMaps m_lstShadowMaps;
    };
  }
}

