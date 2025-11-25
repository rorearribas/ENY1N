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
std::unique_ptr<render::gfx::CModel> CResourceManager::LoadModel(const char* _sPath)
{
  // Importer
  Assimp::Importer rImporter;

  // Read file
  LOG("Loading model -> " << _sPath);
  int32_t iFlags = aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes;
  const aiScene* pScene = rImporter.ReadFile(_sPath, iFlags);
  if (!pScene)
  {
    ERROR_LOG("Error loading model! " << rImporter.GetErrorString());
    return nullptr;
  }

  // Materials
  uint32_t uNumMateriales = pScene->mNumMaterials;
  std::vector<std::unique_ptr<render::mat::CMaterial>> lstMaterials;
  lstMaterials.reserve(uNumMateriales);

  for (uint32_t uMat = 0; uMat < uNumMateriales; uMat++)
  {
    // Get material
    aiMaterial* pAssimpMat = pScene->mMaterials[uMat];
    // Create new material
    auto pMaterial = std::make_unique<render::mat::CMaterial>(pAssimpMat->GetName().C_Str());

    // Set material values
    {
      math::CVector3 v3Diffuse, v3Ambient, v3Specular;
      pAssimpMat->Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(v3Diffuse));
      pAssimpMat->Get(AI_MATKEY_COLOR_AMBIENT, reinterpret_cast<aiColor3D&>(v3Ambient));
      pAssimpMat->Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(v3Specular));

      // Set
      pMaterial->SetDiffuseColor(v3Diffuse);
      pMaterial->SetAmbientColor(v3Ambient);
      pMaterial->SetSpecularColor(v3Specular);

      math::CVector3 v3Emissive, v3Transparent, v3Reflective;
      pAssimpMat->Get(AI_MATKEY_COLOR_EMISSIVE, reinterpret_cast<aiColor3D&>(v3Emissive));
      pAssimpMat->Get(AI_MATKEY_COLOR_TRANSPARENT, reinterpret_cast<aiColor3D&>(v3Transparent));
      pAssimpMat->Get(AI_MATKEY_COLOR_REFLECTIVE, reinterpret_cast<aiColor3D&>(v3Reflective));

      // Set
      pMaterial->SetEmissiveColor(v3Emissive);
      pMaterial->SetTransparentColor(v3Transparent);
      pMaterial->SetReflectiveColor(v3Reflective);

      float fOpacity = 0.0f, fTransparent = 0.0f;
      pAssimpMat->Get(AI_MATKEY_OPACITY, fOpacity);
      pAssimpMat->Get(AI_MATKEY_SHININESS, fTransparent);

      // Set
      pMaterial->SetOpacity(fOpacity);
      pMaterial->SetShininess(fTransparent);
    }

    // Load textures @Note: We only allow one texture per stage!
    using fs = std::filesystem::path;
    for (uint32_t uI = 0; uI < render::s_uTextureCount; uI++)
    {
      aiString sPath;
      aiTextureType eTextureType = (aiTextureType)(uI);
      if (pAssimpMat->GetTexture(eTextureType, 0, &sPath) == aiReturn_SUCCESS)
      {
        RegisterTexture(pMaterial, (render::ETexture)(uI), fs(_sPath).parent_path(), sPath.C_Str());
      }
    }

    // Add material
    lstMaterials.emplace_back(std::move(pMaterial));
  }

  // Create model data
  render::gfx::CModel::TModelData rModelData = render::gfx::CModel::TModelData();
  memcpy(rModelData.AssetPath, _sPath, 128); // Set path

  // Load meshes
  std::unordered_map<render::gfx::TVertexData, uint32_t> mVertexMap;
  for (uint32_t uI = 0; uI < pScene->mNumMeshes; uI++)
  {
    aiMesh* pSceneMesh = pScene->mMeshes[uI];
#ifdef _DEBUG
    assert(pSceneMesh);
#endif // DEBUG

    // Create internal mesh
    std::shared_ptr pMesh = std::make_shared<render::gfx::CMesh>(pSceneMesh->mName.C_Str());
    std::vector<uint32_t> lstIndices;

    for (uint32_t uJ = 0; uJ < pSceneMesh->mNumFaces; uJ++)
    {
      const aiFace& oFace = pSceneMesh->mFaces[uJ];
      for (uint32_t uK = 0; uK < oFace.mNumIndices; uK++)
      {
        render::gfx::TVertexData rVertexData = render::gfx::TVertexData();
        uint32_t uPosIdx = oFace.mIndices[uK];

        // Position
        aiVector3D v3Pos = pSceneMesh->mVertices[uPosIdx];
        rVertexData.VertexPos = math::CVector3(v3Pos.x, v3Pos.y, v3Pos.z);

        // Normal
        if (pSceneMesh->HasNormals())
        {
          aiVector3D v3Normal = pSceneMesh->mNormals[uPosIdx];
          rVertexData.Normal = math::CVector3(v3Normal.x, v3Normal.y, v3Normal.z);
        }
        else
        {
          WARNING_LOG("Normals are not defined correctly in: " + pMesh->GetID());
        }

        // Texture coords
        if (pSceneMesh->HasTextureCoords(0))
        {
          aiVector3D v3Coord = pSceneMesh->mTextureCoords[0][uPosIdx];
          rVertexData.TexCoord = math::CVector2(v3Coord.x, v3Coord.y);
        }

        // Add vertices
        auto it = mVertexMap.find(rVertexData);
        if (it != mVertexMap.end())
        {
          lstIndices.emplace_back(it->second);
        }
        else
        {
          uint32_t uNewIdx = static_cast<uint32_t>(mVertexMap.size());
          mVertexMap[rVertexData] = uNewIdx;
          rModelData.Vertices.emplace_back(std::move(rVertexData));
          lstIndices.emplace_back(uNewIdx);
        }
      }
    }

    // Index buffer
    HRESULT hResult = pMesh->CreateBuffer(lstIndices);
    UNUSED_VAR(hResult);
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif // DEBUG

    // Add material
    if (pSceneMesh->mMaterialIndex >= 0 && pSceneMesh->mMaterialIndex < (int)lstMaterials.size())
    {
      pMesh->SetMaterial(std::move(lstMaterials[pSceneMesh->mMaterialIndex]));
    }

    rModelData.Meshes.emplace_back(pMesh);
  }

  SUCCESS_LOG("Loaded model data! -> " << _sPath);
  std::unique_ptr<render::gfx::CModel> pModel = std::make_unique<render::gfx::CModel>(rModelData);
  SUCCESS_LOG("Created model! -> " << _sPath);
  return pModel;
}
// ------------------------------------
unsigned char* CResourceManager::LoadImage(const char* _sPath, int& _iWidth_, int& _iHeight_, int& _iChannels_)
{
  return stbi_load(_sPath, &_iWidth_, &_iHeight_, &_iChannels_, render::texture::CTexture2D<>::s_uChannels);
}
// ------------------------------------
void CResourceManager::RegisterTexture
(
  std::unique_ptr<render::mat::CMaterial>& pMaterial,
  render::ETexture _eType,
  const std::filesystem::path& _oBasePath,
  const std::string& _sTextureID
)
{
  using fs = std::filesystem::path;
  fs sPath = _oBasePath / fs(_sTextureID);
  if (sPath.has_filename() && std::filesystem::exists(sPath))
  {
    LOG("Loading texture...");
    using namespace render::texture;
    std::string sTextureID = sPath.filename().stem().string();

    int iWidth = 0, iHeight = 0, iChannels = 0;
    unsigned char* pBuffer = LoadImage(sPath.string().c_str(), iWidth, iHeight, iChannels);
#ifdef _DEBUG
    assert(pBuffer);
#endif // DEBUG
    SUCCESS_LOG("Texture loaded! -> " << sPath.filename());

    // Create texture
    auto pTexture = std::make_shared<render::texture::CTexture2D<render::EView::SHADER_RESOURCE>>();
    pMaterial->SetTexture(pTexture, _eType);

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
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif // DEBUG

    // Set shader resource cfg
    D3D11_SHADER_RESOURCE_VIEW_DESC oShaderResourceViewDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
    oShaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    oShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    oShaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Create shader resource view
    hResult = pTexture->CreateView(oShaderResourceViewDesc);
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif // DEBUG

    // Free image
    stbi_image_free(pBuffer);
  }
}

