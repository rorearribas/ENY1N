#include "ResourceManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include <iostream>
#include <cassert>
#include <unordered_set>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

  // ------------------------------------
char* CResourceManager::LoadFile(const char* _sPath, const char* _sMode)
{
  FILE* pFile = nullptr;
  fopen_s(&pFile, _sPath, _sMode);
  if (!pFile) { return nullptr; }

  fseek(pFile, 0, SEEK_END);
  long lFileSize = ftell(pFile);
  rewind(pFile);

  const size_t tMaxSize = lFileSize;
  char* cBuffer = new char[tMaxSize + 1];
  if (cBuffer == nullptr)
  {
    perror("Error allocating memory");
    fclose(pFile);
    return nullptr;
  }

  // Read buffer
  size_t tSize = fread_s(cBuffer, tMaxSize, 1, lFileSize, pFile);
  cBuffer[tSize] = '\0';
  fclose(pFile);

  return cBuffer;
}
// ------------------------------------
render::gfx::CModel::SModelData CResourceManager::LoadModel(const char* _sPath)
{
  // Importer
  Assimp::Importer importer;

  // Read file
  LOG("Loading model -> " << _sPath);
  uint32_t uFlags = aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_GenNormals /*| aiProcess_GlobalScale*/;
  const aiScene* pScene = importer.ReadFile(_sPath, uFlags);
  if (!pScene)
  {
    ERROR_LOG("Error loading model! " << importer.GetErrorString());
    return render::gfx::CModel::SModelData();
  }

  // Materials
  std::vector<render::mat::CMaterial*> vctMaterials;
  vctMaterials.reserve(pScene->mNumMaterials);

  for (uint32_t uIndex = 0; uIndex < pScene->mNumMaterials; uIndex++)
  {
    // Get material
    aiMaterial* pLoadedMaterial = pScene->mMaterials[uIndex];

    // Create new material
    auto* pMaterial = new render::mat::CMaterial(pLoadedMaterial->GetName().C_Str());

    // Set material values
    {
      math::CVector3 v3Diffuse, v3Ambient, v3Specular;
      pLoadedMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(v3Diffuse));
      pLoadedMaterial->Get(AI_MATKEY_COLOR_AMBIENT, reinterpret_cast<aiColor3D&>(v3Ambient));
      pLoadedMaterial->Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(v3Specular));

      // Set
      pMaterial->SetDiffuseColor(v3Diffuse);
      pMaterial->SetAmbientColor(v3Ambient);
      pMaterial->SetSpecularColor(v3Specular);

      math::CVector3 v3Emissive, v3Transparent, v3Reflective;
      pLoadedMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, reinterpret_cast<aiColor3D&>(v3Emissive));
      pLoadedMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, reinterpret_cast<aiColor3D&>(v3Transparent));
      pLoadedMaterial->Get(AI_MATKEY_COLOR_REFLECTIVE, reinterpret_cast<aiColor3D&>(v3Reflective));

      // Set
      pMaterial->SetEmissiveColor(v3Emissive);
      pMaterial->SetTransparentColor(v3Transparent);
      pMaterial->SetReflectiveColor(v3Reflective);

      float fOpacity = 0.0f, fTransparent = 0.0f;
      pLoadedMaterial->Get(AI_MATKEY_OPACITY, fOpacity);
      pLoadedMaterial->Get(AI_MATKEY_SHININESS, fTransparent);

      pMaterial->SetOpacity(fOpacity);
      pMaterial->SetShininess(fTransparent);
    }

    // Diffuse
    aiString sPath;
    using namespace std::filesystem;
    if (pLoadedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::DIFFUSE, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Specular
    if (pLoadedMaterial->GetTexture(aiTextureType_SPECULAR, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::SPECULAR, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Ambient
    if (pLoadedMaterial->GetTexture(aiTextureType_AMBIENT, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::AMBIENT, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Emissive
    if (pLoadedMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::EMISSIVE, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Height
    if (pLoadedMaterial->GetTexture(aiTextureType_HEIGHT, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::HEIGHT, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Normals
    if (pLoadedMaterial->GetTexture(aiTextureType_NORMALS, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::NORMAL, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Shininess
    if (pLoadedMaterial->GetTexture(aiTextureType_SHININESS, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::SHININESS, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Displacement
    if (pLoadedMaterial->GetTexture(aiTextureType_DISPLACEMENT, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::DISPLACEMENT, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Light map
    if (pLoadedMaterial->GetTexture(aiTextureType_LIGHTMAP, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::LIGHTMAP, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Reflection
    if (pLoadedMaterial->GetTexture(aiTextureType_REFLECTION, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::REFLECTION, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Base color
    if (pLoadedMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::BASE_COLOR, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Normal camera
    if (pLoadedMaterial->GetTexture(aiTextureType_NORMAL_CAMERA, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::NORMAL_CAMERA, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Emission color
    if (pLoadedMaterial->GetTexture(aiTextureType_EMISSION_COLOR, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::EMISSION_COLOR, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Metalness
    if (pLoadedMaterial->GetTexture(aiTextureType_METALNESS, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::METALNESS, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Diffuse roughness
    if (pLoadedMaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::DIFFUSE_ROUGHNESS, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Ambient occlusion
    if (pLoadedMaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::AMBIENT_OCCLUSSION, path(_sPath).parent_path(), sPath.C_Str());
    }

    // Add material
    vctMaterials.emplace_back(std::move(pMaterial));
  }

  // Load meshes
  render::gfx::CModel::SModelData oModelData = render::gfx::CModel::SModelData();
  std::unordered_map<render::gfx::SVertexData, uint32_t> mVertexMap;

  for (unsigned int uI = 0; uI < pScene->mNumMeshes; uI++)
  {
    aiMesh* pSceneMesh = pScene->mMeshes[uI];
    assert(pSceneMesh);

    // Create internal mesh
    render::gfx::CMesh* pMesh = new render::gfx::CMesh(pSceneMesh->mName.C_Str());
    std::vector<uint32_t> vctIndices;

    for (uint32_t uJ = 0; uJ < pSceneMesh->mNumFaces; uJ++)
    {
      aiFace& oFace = pSceneMesh->mFaces[uJ];
      for (uint32_t uK = 0; uK < oFace.mNumIndices; uK++)
      {
        render::gfx::SVertexData oVertexData = render::gfx::SVertexData();
        uint32_t uPosIdx = oFace.mIndices[uK];
        oVertexData.MaterialID = pSceneMesh->mMaterialIndex;

        // Position
        aiVector3D v3Pos = pSceneMesh->mVertices[uPosIdx];
        oVertexData.Position = math::CVector3(v3Pos.x, v3Pos.y, v3Pos.z);

        // Normal
        if (pSceneMesh->HasNormals())
        {
          aiVector3D v3Normal = pSceneMesh->mNormals[uPosIdx];
          oVertexData.Normal = math::CVector3(v3Normal.x, v3Normal.y, v3Normal.z);
        }
        else
        {
          WARNING_LOG("Normals are not defined correctly in: " + pMesh->GetMeshID());
        }

        // Color
        if (pSceneMesh->HasVertexColors(0))
        {
          aiColor4D v3Color = pSceneMesh->mColors[0][uPosIdx];
          oVertexData.Color = math::CVector3(v3Color.r, v3Color.g, v3Color.b);
        }

        // UV
        if (pSceneMesh->HasTextureCoords(0))
        {
          aiVector3D v3Coord = pSceneMesh->mTextureCoords[0][uPosIdx];
          oVertexData.UV = math::CVector2(v3Coord.x, v3Coord.y);
        }

        // Add vertices
        auto it = mVertexMap.find(oVertexData);
        if (it != mVertexMap.end())
        {
          vctIndices.emplace_back(it->second);
        }
        else
        {
          uint32_t uNewIdx = static_cast<uint32_t>(mVertexMap.size());
          mVertexMap[oVertexData] = uNewIdx;
          oModelData.VertexData.emplace_back(std::move(oVertexData));
          vctIndices.emplace_back(uNewIdx);
        }
      }
    }

    // Index buffer
    HRESULT hResult = pMesh->CreateBuffer(vctIndices);
    UNUSED_VAR(hResult);
    assert(!FAILED(hResult));

    // Set material ID
    if (pSceneMesh->mMaterialIndex >= 0 && pSceneMesh->mMaterialIndex < (int)vctMaterials.size())
    {
      pMesh->AddMaterial(vctMaterials[pSceneMesh->mMaterialIndex], pSceneMesh->mMaterialIndex);
    }

    oModelData.Meshes.emplace_back(pMesh);
  }

  SUCCESS_LOG("Model loaded! -> " << _sPath);
  return oModelData;
}
// ------------------------------------
unsigned char* CResourceManager::LoadImage(const char* _sPath, int& _iWidth_, int& _iHeight_, int& _iChannels_)
{
  return stbi_load(_sPath, &_iWidth_, &_iHeight_, &_iChannels_, render::texture::CTexture2D<>::s_uChannels);
}
// ------------------------------------
void CResourceManager::RegisterTexture(render::mat::CMaterial*& pMaterial, render::ETextureType _eType,
  const std::filesystem::path& _oBasePath, const std::string& _sTextureID)
{
  std::filesystem::path oTargetPath = _oBasePath / std::filesystem::path(_sTextureID);
  if (oTargetPath.has_filename() && std::filesystem::exists(oTargetPath))
  {
    LOG("Loading texture...");
    int iWidth = 0, iHeight = 0, iChannels = 0;
    unsigned char* pBuffer = LoadImage(oTargetPath.string().c_str(), iWidth, iHeight, iChannels);
    assert(pBuffer);
    SUCCESS_LOG("Texture loaded! -> " << oTargetPath.filename());

    // Register texture
    using namespace render::texture;
    CTexture2D<render::SHADER_RESOURCE>* pTexture = pMaterial->RegisterTexture(_eType, oTargetPath.filename().stem().string());

    // Set texture config
    D3D11_TEXTURE2D_DESC oTextureDesc = D3D11_TEXTURE2D_DESC();
    oTextureDesc.Width = iWidth;
    oTextureDesc.Height = iHeight;
    oTextureDesc.MipLevels = 1;
    oTextureDesc.ArraySize = 1;
    oTextureDesc.SampleDesc.Count = 1;
    oTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA
    oTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind shader resource

    // Create texture
    HRESULT hResult = pTexture->CreateTexture(pBuffer, oTextureDesc, iChannels);
    assert(!FAILED(hResult));

    // Set shader resource cfg
    D3D11_SHADER_RESOURCE_VIEW_DESC oShaderResourceViewDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
    oShaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    oShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    oShaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Create shader resource view
    hResult = pTexture->CreateView(oShaderResourceViewDesc);
    assert(!FAILED(hResult));

    // Set sampler cfg
    D3D11_SAMPLER_DESC oSamplerDesc = D3D11_SAMPLER_DESC();
    oSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    oSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    oSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    oSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    oSamplerDesc.MipLODBias = 0.0f;
    oSamplerDesc.MaxAnisotropy = 16u;
    oSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    oSamplerDesc.BorderColor[0] = 0.0f;
    oSamplerDesc.BorderColor[1] = 0.0f;
    oSamplerDesc.BorderColor[2] = 0.0f;
    oSamplerDesc.BorderColor[3] = 0.0f;
    oSamplerDesc.MinLOD = 0.0f;
    oSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create sampler
    hResult = pTexture->CreateSampler(oSamplerDesc);
    assert(!FAILED(hResult));

    stbi_image_free(pBuffer);
  }
}

