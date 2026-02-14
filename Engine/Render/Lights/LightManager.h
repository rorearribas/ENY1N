#pragma once
#include "Engine/Render/ConstantBuffer/BufferTypes.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Render/Resources/Texture2D.h"
#include "Libs/Utils/Singleton.h"
#include "Libs/Utils/FixedPool.h"
#include "PointLight.h"
#include "SpotLight.h"

namespace render
{
  namespace lights
  {
    // Forward declaration
    class CLight;
    class CDirectionalLight;

    class CLightManager
    {
    private:
      static constexpr uint16_t s_uMaxShadowMaps = 4u;
      static constexpr uint16_t s_uMaxSpotLights = 100u;
      static constexpr uint16_t s_uMaxPointLights = 100u;

      typedef utils::CFixedPool<texture::CTexture2D<EView::DEPTH_STENCIL>, s_uMaxShadowMaps> TShadowMaps;
      typedef utils::CFixedPool<render::lights::CPointLight, s_uMaxPointLights> TPointLights;
      typedef utils::CFixedPool<render::lights::CSpotLight, s_uMaxSpotLights> TSpotLights;

    private:
      typedef TGlobalLighting<s_uMaxPointLights, s_uMaxSpotLights> TLightingData;
      typedef CConstantBuffer<TLightingData> TLightingBuffer;

    public:
      CLightManager();
      ~CLightManager();

      // push lights
      void ApplyLighting();

      // Handle lights
      render::lights::CDirectionalLight* const CreateDirectionalLight();
      render::lights::CPointLight* const CreatePointLight();
      render::lights::CSpotLight* const CreateSpotLight();

      // Destruction
      bool DestroyLight(render::lights::CLight*& pLight_);

    private:
      void Clean();

    private:
      // Lights
      render::lights::CDirectionalLight* m_pDirectionalLight = nullptr;
      TPointLights m_lstPointLights = TPointLights();
      TSpotLights m_lstSpotLights = TSpotLights();

      // Global lighting buffer
      TLightingBuffer m_oLightingBuffer;

      //texture::CTexture2D<EView::DEPTH_STENCIL> m_oShadowDepthMap;
      //texture::CTexture2D<EView::SHADER_RESOURCE> m_oShadowMapTexture;
    };
  }
}

