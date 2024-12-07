#include "ResourceManager.h"
#include <iostream>
#include "Libs/Macros/GlobalMacros.h"
#include <unordered_set>

#define TINYOBJLOADER_IMPLEMENTATION
#include "Libs/Third-Party/objloader/tiny_obj_loader.h"

namespace internal_resource_manager
{
  std::vector<int> GetUniqueMaterials(const std::vector<int>& _vctInput)
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

  const size_t iMaxFileSize = lFileSize + 1;
  char* cBuffer = new char[iMaxFileSize];
  if (cBuffer == nullptr)
  {
    perror("Error allocating memory");
    fclose(pFile);
    return nullptr;
  }

  // Leer el archivo completo
  size_t iTotalSize = fread_s(cBuffer, iMaxFileSize, 1, lFileSize, pFile);
  cBuffer[iTotalSize] = '\0';
  fclose(pFile);

  return cBuffer;
}
// ------------------------------------
render::graphics::CModel::SModelData CResourceManager::LoadModel(const char* _sPath, const char* _sBaseModelMtlDir)
{
  // Create a model data instance
  render::graphics::CModel::SModelData oModelData = {};

  // Tiny obj
  tinyobj::attrib_t attributes;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warnings;
  std::string errors;
  // Load obj
  tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings, &errors, _sPath, _sBaseModelMtlDir);

  // Register material
  auto oRegisterMaterial = [](const tinyobj::material_t& _oMaterial, std::vector< render::material::CMaterial*>& _vctMaterials_)
  {
    render::material::CMaterial* pMaterial = new render::material::CMaterial(_oMaterial.name.c_str());
    pMaterial->SetAmbientColor({ _oMaterial.ambient[0], _oMaterial.ambient[1], _oMaterial.ambient[2] });
    pMaterial->SetDiffuseColor({ _oMaterial.diffuse[0], _oMaterial.diffuse[1], _oMaterial.diffuse[2] });
    pMaterial->SetSpecularColor({ _oMaterial.specular[0], _oMaterial.specular[1], _oMaterial.specular[2] });

    pMaterial->RegisterPath(render::material::EModifierType::AMBIENT, _oMaterial.ambient_texname);
    pMaterial->RegisterPath(render::material::EModifierType::DIFFUSE, _oMaterial.diffuse_texname);
    pMaterial->RegisterPath(render::material::EModifierType::SPECULAR, _oMaterial.specular_texname);
    pMaterial->RegisterPath(render::material::EModifierType::BUMP, _oMaterial.bump_texname);
    pMaterial->RegisterPath(render::material::EModifierType::DISPLACEMENT, _oMaterial.displacement_texname);
    pMaterial->RegisterPath(render::material::EModifierType::ALPHA, _oMaterial.alpha_texname);
    pMaterial->RegisterPath(render::material::EModifierType::REFLECTION, _oMaterial.reflection_texname);

    _vctMaterials_.emplace_back(pMaterial);
  };

  std::vector<render::material::CMaterial*> vctMaterials;
  vctMaterials.reserve(materials.size());
  for (const tinyobj::material_t& tMaterial : materials) { oRegisterMaterial(tMaterial, vctMaterials); }

  // Register vertex data
  std::unordered_map<maths::CVector3, uint32_t> VertexMap;
  for (uint32_t uIndex = 0; uIndex < static_cast<uint32_t>(shapes.size()); uIndex++)
  {
    tinyobj::shape_t& shape = shapes[uIndex];
    tinyobj::mesh_t& mesh = shape.mesh;

    render::graphics::CMesh* pMesh = nullptr;
    if (mesh.num_face_vertices.size() > 0)
      pMesh = new render::graphics::CMesh(shape.name);
    else
      continue;


    // Mesh
    std::vector<uint32_t> vctIndexes = {};
    uint32_t uOffset = 0;

    for (uint32_t uJ = 0; uJ < static_cast<uint32_t>(mesh.num_face_vertices.size()); uJ++)
    {
      uint32_t uVertexCount = mesh.num_face_vertices[uJ];
      int iMaterialId = mesh.material_ids[uJ];

      for (uint32_t uK = 0; uK < uVertexCount; uK++)
      {
        tinyobj::index_t idx = mesh.indices[uOffset + uK];
        render::graphics::SVertexData oVertexData;
        oVertexData.MaterialId = iMaterialId; // Set material id

        // Set position
        oVertexData.Position = maths::CVector3
        (
          attributes.vertices[3 * idx.vertex_index], 
          attributes.vertices[3 * idx.vertex_index + 1], 
          attributes.vertices[3 * idx.vertex_index + 2]
        );

        // Set normal
        bool bHasNormal = idx.normal_index >= 0 && idx.normal_index < (attributes.normals.size() / 3);
        oVertexData.Normal = bHasNormal ? 
        maths::CVector3
        (
          attributes.normals[3 * idx.normal_index],
          attributes.normals[3 * idx.normal_index + 1],
          attributes.normals[3 * idx.normal_index + 2]

        ) : maths::CVector3::Forward;

        // Set texture coord
        bool bHasTexCoord = idx.texcoord_index >= 0 && idx.texcoord_index < (attributes.texcoords.size() / 2);
        oVertexData.TexCoord = bHasTexCoord ? maths::CVector2
        (
          attributes.texcoords[2 * idx.texcoord_index],
          attributes.texcoords[2 * idx.texcoord_index + 1]

        ) : maths::CVector2::Zero;

        // Check if the vertex already exists
        auto it = VertexMap.find(oVertexData.Position);
        if (it != VertexMap.end()) 
        {
          vctIndexes.emplace_back(it->second);
        }
        else 
        {
          uint32_t uNewIdx = static_cast<uint32_t>(VertexMap.size());
          VertexMap[oVertexData.Position] = uNewIdx;
          oModelData.m_vctVertexData.emplace_back(oVertexData);
          vctIndexes.emplace_back(uNewIdx);
        }
      }

      // Add offset
      uOffset += uVertexCount;
    }

    // Create mesh
    HRESULT hr = pMesh->CreateMesh(vctIndexes);
    UNUSED_VARIABLE(hr);
    assert(!FAILED(hr));

    std::vector<int> vctMaterialIds = internal_resource_manager::GetUniqueMaterials(mesh.material_ids);
    for (uint32_t uMaterialIdx = 0; uMaterialIdx < vctMaterialIds.size(); uMaterialIdx++)
    {
      int iMaterialIdx = vctMaterialIds[uMaterialIdx];
      pMesh->AddMaterial(vctMaterials[iMaterialIdx], static_cast<uint32_t>(iMaterialIdx));
    }
    oModelData.m_vctMeshes.emplace_back(pMesh); // Add mesh
  }

  std::cout << "loaded" << std::endl;
  return oModelData;
}

