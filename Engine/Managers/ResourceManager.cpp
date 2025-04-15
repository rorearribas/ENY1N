#include "ResourceManager.h"
#include <iostream>
#include "Libs/Macros/GlobalMacros.h"
#include <unordered_set>

#define TINYOBJLOADER_IMPLEMENTATION
#include "Libs/Third-Party/objloader/tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Libs/Third-Party/stb/stb_image.h"

namespace internal_resource_manager
{
  static std::vector<int> GetUniqueMaterialsIDs(const std::vector<int>& _vctInput)
  {
    std::unordered_set<int> uniqueSet(_vctInput.begin(), _vctInput.end());
    return std::vector<int>(uniqueSet.begin(), uniqueSet.end());
  }
}
// ------------------------------------
char* CResourceManager::LoadResource(const char* _sPath, const char* _sMode)
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

  // Leer el archivo completo
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
render::graphics::CModel::SModelData CResourceManager::LoadModel(const char* _sPath, const char* _sBaseModelMtlDir)
{
  // Create a model data
  render::graphics::CModel::SModelData oModelData = {};

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
    return oModelData;
  }

  // Register material
  auto oRegisterMaterialFunc = [=](const tinyobj::material_t& _oMaterial, std::vector<render::material::CMaterial*>& _vctMaterials_)
  {
    // Create material
    render::material::CMaterial* pMaterial = new render::material::CMaterial(_oMaterial.name.c_str());

    // Set colors
    pMaterial->SetAmbientColor({ _oMaterial.ambient[0], _oMaterial.ambient[1], _oMaterial.ambient[2] });
    pMaterial->SetDiffuseColor({ _oMaterial.diffuse[0], _oMaterial.diffuse[1], _oMaterial.diffuse[2] });
    pMaterial->SetSpecularColor({ _oMaterial.specular[0], _oMaterial.specular[1], _oMaterial.specular[2] });

    // Get path
    using EType = render::material::CMaterial::EModifierType;
    std::filesystem::path oBasePath(_sBaseModelMtlDir);

    // Register textures
    RegisterTexture(pMaterial, EType::AMBIENT, oBasePath, _oMaterial.ambient_texname);
    RegisterTexture(pMaterial, EType::DIFFUSE, oBasePath, _oMaterial.diffuse_texname);
    RegisterTexture(pMaterial, EType::SPECULAR, oBasePath, _oMaterial.specular_texname);
    RegisterTexture(pMaterial, EType::DISPLACEMENT, oBasePath, _oMaterial.displacement_texname);
    RegisterTexture(pMaterial, EType::SPECULAR_HIGHLIGHT, oBasePath, _oMaterial.specular_highlight_texname);
    RegisterTexture(pMaterial, EType::REFLECTION, oBasePath, _oMaterial.reflection_texname);
    RegisterTexture(pMaterial, EType::ALPHA, oBasePath, _oMaterial.alpha_texname);
    RegisterTexture(pMaterial, EType::BUMP, oBasePath, _oMaterial.bump_texname);

    // Add material
    _vctMaterials_.emplace_back(pMaterial);
  };

  std::vector<render::material::CMaterial*> vctMaterials;
  vctMaterials.reserve(vctMaterialsIds.size());
  for (const tinyobj::material_t& tMaterial : vctMaterialsIds) 
  { 
    oRegisterMaterialFunc(tMaterial, vctMaterials);
  }

  // Register vertex data
  std::unordered_map<render::graphics::SVertexData, uint32_t> dctVertexMap;
  for (uint32_t uIndex = 0; uIndex < static_cast<uint32_t>(shapes.size()); uIndex++)
  {
    tinyobj::shape_t& shape = shapes[uIndex];
    tinyobj::mesh_t& mesh = shape.mesh;

    render::graphics::CMesh* pMesh = nullptr;
    if (mesh.num_face_vertices.size() > 0)
    {
      pMesh = new render::graphics::CMesh(shape.name);
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
        render::graphics::SVertexData oVertexData;
        oVertexData.MaterialId = iMaterialId; // Set material id

        // Set position
        oVertexData.Position = maths::CVector3
        (
          attributes.vertices[3 * idx.vertex_index + 0], 
          attributes.vertices[3 * idx.vertex_index + 1], 
          attributes.vertices[3 * idx.vertex_index + 2]
        );

        // Set normal
        bool bHasNormal = idx.normal_index >= 0 && idx.normal_index < (attributes.normals.size() / 3);
        oVertexData.Normal = bHasNormal ? 
        maths::CVector3
        (
          attributes.normals[3 * idx.normal_index + 0],
          attributes.normals[3 * idx.normal_index + 1],
          attributes.normals[3 * idx.normal_index + 2]

        ) : maths::CVector3::Forward;

        // Set texture coord
        bool bHasTexCoord = idx.texcoord_index >= 0 && idx.texcoord_index < (attributes.texcoords.size() / 2);
        oVertexData.TexCoord = bHasTexCoord ? maths::CVector2
        (
          attributes.texcoords[2 * idx.texcoord_index + 0],
          1.0f - attributes.texcoords[2 * idx.texcoord_index + 1] // We have to invert this value

        ) : maths::CVector2::Zero;

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
    UNUSED_VARIABLE(hResult);
    assert(!FAILED(hResult));

    std::vector<int> vctMaterialIds = internal_resource_manager::GetUniqueMaterialsIDs(mesh.material_ids);
    for (uint32_t uMatIndx = 0; uMatIndx < static_cast<uint32_t>(vctMaterialIds.size()); uMatIndx++)
    {
      int iMaterialIdx = vctMaterialIds[uMatIndx];
      pMesh->AddMaterial(vctMaterials[iMaterialIdx], static_cast<uint32_t>(iMaterialIdx));
    }
    oModelData.m_vctMeshes.emplace_back(pMesh); // Add mesh
  }

  return oModelData;
}
// ------------------------------------
void CResourceManager::RegisterTexture(render::material::CMaterial*& pMaterial, render::material::CMaterial::EModifierType _eModifierType,
  const std::filesystem::path& _oBasePath, const std::string& _sTextureName)
{
  std::filesystem::path oTargetTexturePath = _oBasePath / std::filesystem::path(_sTextureName);
  if (oTargetTexturePath.has_filename() && std::filesystem::exists(oTargetTexturePath))
  {
    int iWidth = 0, iHeight = 0, iChannels = 0;
    unsigned char* pTextureData = LoadTexture(oTargetTexturePath.string().c_str(), iWidth, iHeight, iChannels);
    assert(pTextureData);
    render::texture::CTexture* pTexture = pMaterial->RegisterTexture(_eModifierType, oTargetTexturePath.filename().stem().string());
    pTexture->SetTexture(pTextureData, iWidth, iHeight);
    stbi_image_free(pTextureData);
  }
}

