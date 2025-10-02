#include "ResourceManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include <iostream>
#include <cassert>
#include <unordered_set>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

namespace internal_resource_manager
{
  inline std::vector<int> GetUniqueMaterialsIDs(const std::vector<int>& _vctInput)
  {
    std::unordered_set<int> uniqueSet(_vctInput.begin(), _vctInput.end());
    return std::vector<int>(uniqueSet.begin(), uniqueSet.end());
  }
}
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
unsigned char* CResourceManager::LoadTexture(const char* _sPath, int& _iWidth_, int& _iHeight_, int& _iChannels_)
{
  return stbi_load(_sPath, &_iWidth_, &_iHeight_, &_iChannels_, 4);
}
// ------------------------------------
render::gfx::CModel::SModelData CResourceManager::LoadFBX(const char* _sPath)
{
  // Importer
  Assimp::Importer importer;

  // Read file
  const aiScene* pScene = importer.ReadFile
  (
    std::string(_sPath),
    aiProcess_ConvertToLeftHanded |
    aiProcess_Triangulate |
    aiProcess_GenNormals |
    aiProcess_JoinIdenticalVertices |
    aiProcess_GlobalScale
  );

  // Check
  if (!pScene) 
  {
    std::cerr << "Error loading FBX!" << importer.GetErrorString() << std::endl;
    return render::gfx::CModel::SModelData();
  }

  using namespace std::filesystem;
  using namespace render;
  gfx::CModel::SModelData oModelData = gfx::CModel::SModelData();

  // Materials
  std::vector<mat::CMaterial*> vctMaterials;
  vctMaterials.reserve(pScene->mNumMaterials);

  for (uint32_t uIndex = 0; uIndex < pScene->mNumMaterials; uIndex++) 
  {
    // Get material
    aiMaterial* pLoadedMaterial = pScene->mMaterials[uIndex];

    // Create new material
    auto* pMaterial = new mat::CMaterial(pLoadedMaterial->GetName().C_Str());

    // Set material values
    {
      aiColor3D v3Ambient(0.f, 0.f, 0.f), v3Diffuse(0.f, 0.f, 0.f), v3Specular(0.f, 0.f, 0.f);
      pLoadedMaterial->Get(AI_MATKEY_COLOR_AMBIENT, v3Ambient);
      pLoadedMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, v3Diffuse);
      pLoadedMaterial->Get(AI_MATKEY_COLOR_SPECULAR, v3Specular);

      pMaterial->SetAmbientColor({ v3Ambient.r, v3Ambient.g, v3Ambient.b });
      pMaterial->SetDiffuseColor({ v3Diffuse.r, v3Diffuse.g, v3Diffuse.b });
      pMaterial->SetSpecularColor({ v3Specular.r, v3Specular.g, v3Specular.b });

      aiColor3D v3Emissive(0.f, 0.f, 0.f), v3Transparent(0.f, 0.f, 0.f), v3Reflective(0.f, 0.f, 0.f);
      pLoadedMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, v3Emissive);
      pLoadedMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, v3Transparent);
      pLoadedMaterial->Get(AI_MATKEY_COLOR_REFLECTIVE, v3Reflective);

      pMaterial->SetEmissiveColor({ v3Emissive.r, v3Emissive.g, v3Emissive.b });
      pMaterial->SetTransparentColor({ v3Transparent.r, v3Transparent.g, v3Transparent.b });
      pMaterial->SetReflectiveColor({ v3Reflective.r, v3Reflective.g, v3Reflective.b });

      float fOpacity = 0.0f, fTransparent = 0.0f;
      pLoadedMaterial->Get(AI_MATKEY_OPACITY, fOpacity);
      pLoadedMaterial->Get(AI_MATKEY_SHININESS, fTransparent);

      pMaterial->SetOpacity(fOpacity);
      pMaterial->SetShininess(fTransparent);
    }

    // Load textures
    aiString sPath;
    // Diffuse
    if (pLoadedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, render::texture::ETextureType::DIFFUSE, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Specular
    if (pLoadedMaterial->GetTexture(aiTextureType_SPECULAR, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, texture::ETextureType::SPECULAR, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Ambient
    if (pLoadedMaterial->GetTexture(aiTextureType_AMBIENT, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, texture::ETextureType::AMBIENT, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Emissive
    if (pLoadedMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, texture::ETextureType::EMISSIVE, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Height
    if (pLoadedMaterial->GetTexture(aiTextureType_HEIGHT, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, texture::ETextureType::HEIGHT, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Normals
    if (pLoadedMaterial->GetTexture(aiTextureType_NORMALS, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, texture::ETextureType::NORMAL, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Shininess
    if (pLoadedMaterial->GetTexture(aiTextureType_SHININESS, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, texture::ETextureType::SHININESS, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Displacement
    if (pLoadedMaterial->GetTexture(aiTextureType_DISPLACEMENT, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, texture::ETextureType::DISPLACEMENT, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Light map
    if (pLoadedMaterial->GetTexture(aiTextureType_LIGHTMAP, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, texture::ETextureType::LIGHTMAP, path(_sPath).parent_path(), sPath.C_Str());
    }
    // Reflection
    if (pLoadedMaterial->GetTexture(aiTextureType_REFLECTION, 0, &sPath) == aiReturn_SUCCESS)
    {
      RegisterTexture(pMaterial, texture::ETextureType::REFLECTION, path(_sPath).parent_path(), sPath.C_Str());
    }

    vctMaterials.emplace_back(pMaterial);
  }

  // Load meshes
  std::unordered_map<render::gfx::SVertexData, uint32_t> dctVertexMap;
  for (unsigned int m = 0; m < pScene->mNumMeshes; m++) 
  {
    aiMesh* pSceneMesh = pScene->mMeshes[m];
    assert(pSceneMesh);

    // Create mesh
    render::gfx::CMesh* pMesh = new render::gfx::CMesh(pSceneMesh->mName.C_Str());
    std::vector<uint32_t> vctIndices;

    for (unsigned int uI = 0; uI < pSceneMesh->mNumFaces; uI++) 
    {
      aiFace& oFace = pSceneMesh->mFaces[uI];
      for (unsigned int uJ = 0; uJ < oFace.mNumIndices; uJ++) 
      {
        render::gfx::SVertexData oVertexData;
        oVertexData.MaterialId = pSceneMesh->mMaterialIndex;

        // Position
        unsigned int uPosIdx = oFace.mIndices[uJ];
        aiVector3D pos = pSceneMesh->mVertices[uPosIdx];
        oVertexData.Position = math::CVector3(pos.x, pos.y, pos.z);

        // Normal
        if (pSceneMesh->HasNormals())
        {
          aiVector3D n = pSceneMesh->mNormals[uPosIdx];
          oVertexData.Normal = math::CVector3(n.x, n.y, n.z);
        }
        else 
        {
          oVertexData.Normal = math::CVector3::Forward;
        }

        // UV
        if (pSceneMesh->HasTextureCoords(0))
        {
          aiVector3D uv = pSceneMesh->mTextureCoords[0][uPosIdx];
          oVertexData.TexCoord = math::CVector2(uv.x, 1.0f - uv.y);
        }
        else 
        {
          oVertexData.TexCoord = math::CVector2::Zero;
        }

        // Add vertices
        auto it = dctVertexMap.find(oVertexData);
        if (it != dctVertexMap.end()) 
        {
          vctIndices.emplace_back(it->second);
        }
        else 
        {
          uint32_t uNewIdx = static_cast<uint32_t>(dctVertexMap.size());
          dctVertexMap[oVertexData] = uNewIdx;
          oModelData.m_vctVertexData.emplace_back(oVertexData);
          vctIndices.emplace_back(uNewIdx);
        }
      }
    }

    // Index buffer
    HRESULT hResult = pMesh->AssignIndexBuffer(vctIndices);
    UNUSED_VAR(hResult);
    assert(!FAILED(hResult));

    // Set material ID
    if (pSceneMesh->mMaterialIndex >= 0 && pSceneMesh->mMaterialIndex < (int)vctMaterials.size())
    {
      pMesh->AddMaterial(vctMaterials[pSceneMesh->mMaterialIndex], pSceneMesh->mMaterialIndex);
    }

    oModelData.m_vctMeshes.emplace_back(pMesh);
  }

  return oModelData;
}

// ------------------------------------
render::gfx::CModel::SModelData CResourceManager::LoadOBJ(const char* _sPath, const char* _sBaseModelMtlDir)
{
  // Tiny obj declarations
  tinyobj::attrib_t attributes;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> vctMaterialsIds;
  std::string sWarnings;
  std::string sErrors;

  // Load obj
  tinyobj::LoadObj(&attributes, &shapes, &vctMaterialsIds, &sWarnings, &sErrors, _sPath, _sBaseModelMtlDir);

  // Check errors
  if (sErrors.size() > 0)
  {
    std::cout << "Error loading OBJ" << std::endl;
    return render::gfx::CModel::SModelData();
  }

  // Register material
  auto oRegisterMaterialFunc = [&](const tinyobj::material_t& _oMaterial, std::vector<render::mat::CMaterial*>& _vctMaterials_)
  {
    // Create material
    render::mat::CMaterial* pMaterial = new render::mat::CMaterial(_oMaterial.name.c_str());

    // Set colors
    pMaterial->SetAmbientColor({ _oMaterial.ambient[0], _oMaterial.ambient[1], _oMaterial.ambient[2] });
    pMaterial->SetDiffuseColor({ _oMaterial.diffuse[0], _oMaterial.diffuse[1], _oMaterial.diffuse[2] });
    pMaterial->SetSpecularColor({ _oMaterial.specular[0], _oMaterial.specular[1], _oMaterial.specular[2] });

    // Get path
    using EType = render::texture::ETextureType;
    std::filesystem::path oBasePath(_sBaseModelMtlDir);

    // Register textures
    RegisterTexture(pMaterial, EType::DIFFUSE, oBasePath, _oMaterial.diffuse_texname);
    RegisterTexture(pMaterial, EType::SPECULAR, oBasePath, _oMaterial.specular_texname);
    RegisterTexture(pMaterial, EType::AMBIENT, oBasePath, _oMaterial.ambient_texname);
    RegisterTexture(pMaterial, EType::NORMAL, oBasePath, _oMaterial.bump_texname);
    RegisterTexture(pMaterial, EType::SHININESS, oBasePath, _oMaterial.specular_highlight_texname);
    RegisterTexture(pMaterial, EType::DISPLACEMENT, oBasePath, _oMaterial.displacement_texname);
    RegisterTexture(pMaterial, EType::OPACITY, oBasePath, _oMaterial.alpha_texname);
    RegisterTexture(pMaterial, EType::REFLECTION, oBasePath, _oMaterial.reflection_texname);

    // Add material
    _vctMaterials_.emplace_back(pMaterial);
  };

  // Create a model data
  size_t tTotalVertices = attributes.vertices.size() / 3;
  render::gfx::CModel::SModelData oModelData = render::gfx::CModel::SModelData();
  oModelData.m_vctVertexData.reserve(tTotalVertices);

  std::vector<render::mat::CMaterial*> vctMaterials;
  vctMaterials.reserve(vctMaterialsIds.size());
  for (const tinyobj::material_t& tMaterial : vctMaterialsIds) 
  { 
    oRegisterMaterialFunc(tMaterial, vctMaterials);
  }

  // Register vertex data
  std::unordered_map<render::gfx::SVertexData, uint32_t> dctVertexMap;
  dctVertexMap.reserve(tTotalVertices);

  for (uint32_t uIndex = 0; uIndex < static_cast<uint32_t>(shapes.size()); uIndex++)
  {
    tinyobj::shape_t& shape = shapes[uIndex];
    tinyobj::mesh_t& mesh = shape.mesh;

    render::gfx::CMesh* pMesh = nullptr;
    if (mesh.num_face_vertices.size() > 0)
    {
      pMesh = new render::gfx::CMesh(shape.name);
    }
    else
    {
      // @TODO: Tiene que funcionar con lines y points
      continue;
    }

    // Mesh
    std::vector<uint32_t> vctIndices = std::vector<uint32_t>();
    uint32_t uIndexOffset = 0;

    for (uint32_t uJ = 0; uJ < static_cast<uint32_t>(mesh.num_face_vertices.size()); uJ++)
    {
      uint32_t uVertexCount = mesh.num_face_vertices[uJ];
      int iMaterialId = mesh.material_ids[uJ];

      for (uint32_t uK = 0; uK < uVertexCount; uK++)
      {
        tinyobj::index_t idx = mesh.indices[uIndexOffset + uK];
        render::gfx::SVertexData oVertexData;
        oVertexData.MaterialId = iMaterialId; // Set material id

        // Set Position
        oVertexData.Position = math::CVector3
        (
          attributes.vertices[3 * idx.vertex_index + 0], 
          attributes.vertices[3 * idx.vertex_index + 1], 
          attributes.vertices[3 * idx.vertex_index + 2]
        );

        // Set Normal
        bool bHasNormal = idx.normal_index >= 0 && idx.normal_index < (attributes.normals.size() / 3);
        oVertexData.Normal = bHasNormal ? 
        math::CVector3
        (
          attributes.normals[3 * idx.normal_index + 0],
          attributes.normals[3 * idx.normal_index + 1],
          attributes.normals[3 * idx.normal_index + 2]

        ) : math::CVector3::Forward;

        // Set UV
        bool bHasTexCoord = idx.texcoord_index >= 0 && idx.texcoord_index < (attributes.texcoords.size() / 2);
        oVertexData.TexCoord = bHasTexCoord ? math::CVector2
        (
          attributes.texcoords[2 * idx.texcoord_index + 0],
          1.0f - attributes.texcoords[2 * idx.texcoord_index + 1] // We have to invert this value

        ) : math::CVector2::Zero;

        // Check if the vertex already exists
        auto it = dctVertexMap.find(oVertexData);
        if (it != dctVertexMap.end()) 
        {
          vctIndices.emplace_back(it->second);
        }
        else 
        {
          uint32_t uNewIdx = static_cast<uint32_t>(dctVertexMap.size());
          dctVertexMap[oVertexData] = uNewIdx;
          oModelData.m_vctVertexData.emplace_back(oVertexData);
          vctIndices.emplace_back(uNewIdx);
        }
      }

      // Add offset
      uIndexOffset += uVertexCount;
    }

    // Create mesh
    HRESULT hResult = pMesh->AssignIndexBuffer(vctIndices);
    UNUSED_VAR(hResult);
    assert(!FAILED(hResult));

    std::vector<int> vctMaterialIds = internal_resource_manager::GetUniqueMaterialsIDs(mesh.material_ids);
    for (uint32_t uMatIndx = 0; uMatIndx < static_cast<uint32_t>(vctMaterialIds.size()); uMatIndx++)
    {
      int iMaterialIdx = vctMaterialIds[uMatIndx];
      if (iMaterialIdx >= 0)
      {
        pMesh->AddMaterial(vctMaterials[iMaterialIdx], static_cast<uint32_t>(iMaterialIdx));
      }
    }
    oModelData.m_vctMeshes.emplace_back(pMesh); // Add mesh
  }

  return oModelData;
}
// ------------------------------------
void CResourceManager::RegisterTexture(render::mat::CMaterial*& pMaterial, render::texture::ETextureType _eType,
  const std::filesystem::path& _oBasePath, const std::string& _sTextureName)
{
  std::filesystem::path oTargetPath = _oBasePath / std::filesystem::path(_sTextureName);
  if (oTargetPath.has_filename() && std::filesystem::exists(oTargetPath))
  {
    int iWidth = 0, iHeight = 0, iChannels = 0;
    unsigned char* pBuffer = LoadTexture(oTargetPath.string().c_str(), iWidth, iHeight, iChannels);
    assert(pBuffer);

    render::texture::CTexture* pTexture = pMaterial->CreateTexture(_eType, oTargetPath.filename().stem().string());
    pTexture->SetTexture(pBuffer, iWidth, iHeight, iChannels);
    stbi_image_free(pBuffer);
  }
}

