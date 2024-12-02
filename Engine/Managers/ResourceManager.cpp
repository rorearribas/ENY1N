#include "ResourceManager.h"
#include <sstream>
#include <iostream>
#include "Libs/Macros/GlobalMacros.h"
#include <filesystem>

#define TINYOBJLOADER_IMPLEMENTATION
#include "Libs/Third-Party/objloader/tiny_obj_loader.h"
#include <algorithm>
#include <unordered_set>

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
//CResourceManager::SModelData CResourceManager::LoadModelData(const char* _sPath, const char* _sMode)
//{
//  CResourceManager::SModelData oModelData = {};
//  char* cBuffer = LoadResource(_sPath, _sMode);
//  if (!cBuffer) { return oModelData; }
//
//  std::istringstream oStream(cBuffer);
//  std::string sLine;
//
//  // Reserve
//  std::vector<maths::CVector3> vctNormal = {};
//  vctNormal.reserve(300);
//
//  // Texture coords
//  std::vector<maths::CVector2> vctTextureCoords = {};
//  vctTextureCoords.reserve(5000);
//
//  oModelData.m_vctVertexData.reserve(3000);
//  oModelData.m_vctIndexes.reserve(9000);
//
//  std::cout << "Loading model data from: " << _sPath << std::endl;
//  while (std::getline(oStream, sLine))
//  {
//    // Vertex position
//    if (sLine.rfind("v ", 0) == 0)
//    {
//      std::istringstream linestream(sLine.substr(2));
//      float fPosX, fPosY, fPosZ;
//      linestream >> fPosX >> fPosY >> fPosZ;
//      render::graphics::SVertexData oVertexInfo;
//      oVertexInfo.Position = maths::CVector3(fPosX, fPosY, fPosZ);
//      oModelData.m_vctVertexData.emplace_back(oVertexInfo);
//    }
//    // Texture coords
//    else if (sLine.rfind("vt", 0) == 0)
//    {
//      std::istringstream linestream(sLine.substr(2));
//      float fPosX, fPosY;
//      linestream >> fPosX >> fPosY;
//      vctTextureCoords.emplace_back(maths::CVector2(fPosX, fPosY));
//    }
//    // Vertex normals
//    else if (sLine.rfind("vn", 0) == 0)
//    {
//      // Comprobar si la línea comienza con "vn"
//      std::istringstream linestream(sLine.substr(2));
//      float fNormalX, fNormalY, fNormalZ;
//      linestream >> fNormalX >> fNormalY >> fNormalZ;
//      vctNormal.emplace_back(maths::CVector3(fNormalX, fNormalY, fNormalZ));
//    }
//    // Faces
//    else if (sLine.rfind("f ", 0) == 0)
//    {
//      std::istringstream linestream(sLine.substr(2));
//      std::string vertexData;
//
//      std::vector<uint32_t> vctIndexes = {};
//      while (linestream >> vertexData)
//      {
//        size_t firstSlash = vertexData.find('/');
//        size_t secondSlash = vertexData.find('/', firstSlash + 1);
//
//        // Vertex index
//        uint32_t uVertexIdx = std::stoi(vertexData.substr(0, firstSlash)) - 1;
//        vctIndexes.emplace_back(uVertexIdx);
//        // Texture coord index
//        uint32_t uTexCoordIdx = (firstSlash != std::string::npos && secondSlash != std::string::npos) ?
//          std::stoi(vertexData.substr(firstSlash + 1, secondSlash - firstSlash - 1)) - 1 : 0;
//        // Normal index
//        uint32_t uNormalIdx = (secondSlash != std::string::npos) ? std::stoi(vertexData.substr(secondSlash + 1)) - 1 : 0;
//
//
//        // Set normal
//        if (uVertexIdx < oModelData.m_vctVertexData.size() && uNormalIdx < vctNormal.size())
//        {
//          oModelData.m_vctVertexData[uVertexIdx].Normal = vctNormal[uNormalIdx];
//        }
//
//        // Set texture coord
//        if (uVertexIdx < oModelData.m_vctVertexData.size() && uTexCoordIdx < vctTextureCoords.size())
//        {
//          oModelData.m_vctVertexData[uVertexIdx].TexCoord = vctTextureCoords[uTexCoordIdx];
//        }
//      }
//
//      // Only for triangles
//      if (vctIndexes.size() == 3)
//      {
//        for (uint32_t uIndex = 0; uIndex < vctIndexes.size(); ++uIndex)
//        {
//          oModelData.m_vctIndexes.emplace_back(vctIndexes[uIndex]);
//        }
//      }
//      // More than 3 vertex
//      else if (vctIndexes.size() >= 4)
//      {
//        for (size_t i = 1; i < vctIndexes.size() - 1; ++i)
//        {
//          oModelData.m_vctIndexes.push_back(vctIndexes[0]);
//          oModelData.m_vctIndexes.push_back(vctIndexes[i]);
//          oModelData.m_vctIndexes.push_back(vctIndexes[i + 1]);
//        }
//      }
//      else
//      {
//        throw std::runtime_error("Invalid face detected in file: " + std::string(_sPath));
//      }
//    }
//  }
//
//  // Delete buffer
//  delete[] cBuffer;
//  cBuffer = nullptr;
//
//  std::cout << "Model data loaded" << std::endl;
//  return oModelData;
//}
// ------------------------------------
std::vector<render::material::CMaterial> CResourceManager::LoadMaterials(const char* _sPath, const char* _sMode)
{
  std::vector<render::material::CMaterial> vctMaterials = {};

  char* cBuffer = LoadResource(_sPath, _sMode);
  if (!cBuffer) { return vctMaterials; }

  vctMaterials.reserve(100);

  std::cout << "Loading materials from: " << _sPath << std::endl;
  std::istringstream oStream(cBuffer);
  std::string sLine;
  while (std::getline(oStream, sLine))
  {
    if (sLine.rfind("newmtl", 0) == 0)
    {
      std::istringstream linestream(sLine.substr(strlen("newmtl")));
      std::string sMaterialId;
      linestream >> sMaterialId;

      render::material::CMaterial oMaterial(sMaterialId.c_str());
      vctMaterials.emplace_back(oMaterial);
    }
    else if (sLine.rfind("Ns", 0) == 0)
    {
      std::istringstream linestream(sLine.substr(strlen("Ns")));
      float fSpecularExponent;
      linestream >> fSpecularExponent;
      vctMaterials[vctMaterials.size() - 1].SetSpecularExponent(fSpecularExponent);
    }
    else if (sLine.rfind("Ka", 0) == 0)
    {
      std::istringstream linestream(sLine.substr(strlen("Ka")));
      float fAmbientX, fAmbientY, fAmbientZ;
      linestream >> fAmbientX >> fAmbientY >> fAmbientZ;
      vctMaterials[vctMaterials.size() - 1].SetAmbientColor(maths::CVector3(fAmbientX, fAmbientY, fAmbientZ));
    }
    else if (sLine.rfind("Kd", 0) == 0)
    {
      std::istringstream linestream(sLine.substr(strlen("Kd")));
      float fDiffuseX, fDiffuseY, fDiffuseZ;
      linestream >> fDiffuseX >> fDiffuseY >> fDiffuseZ;
      vctMaterials[vctMaterials.size() - 1].SetDiffuseColor(maths::CVector3(fDiffuseX, fDiffuseY, fDiffuseZ));
    }
    else if (sLine.rfind("Ks", 0) == 0)
    {
      std::istringstream linestream(sLine.substr(strlen("Ks")));
      float fSpecularX, fSpecularY, fSpecularZ;
      linestream >> fSpecularX >> fSpecularY >> fSpecularZ;
      vctMaterials[vctMaterials.size() - 1].SetSpecularColor(maths::CVector3(fSpecularX, fSpecularY, fSpecularZ));
    }
    else if (sLine.rfind("Ni", 0) == 0)
    {
      std::istringstream linestream(sLine.substr(strlen("Ni")));
      float fOpticalDensity;
      linestream >> fOpticalDensity;
      vctMaterials[vctMaterials.size() - 1].SetOpticalDensity(fOpticalDensity);
    }
    else if (sLine.rfind("d", 0) == 0)
    {
      std::istringstream linestream(sLine.substr(strlen("d")));
      float fTransparent;
      linestream >> fTransparent;
      vctMaterials[vctMaterials.size() - 1].SetOpacity(fTransparent);
    }
    else if (sLine.rfind("map_Kd", 0) == 0)
    {
      std::istringstream linestream(sLine.substr(strlen("map_Kd")));
      std::filesystem::path oPath(linestream.str());
      std::string sValidFileName = oPath.filename().string();
      std::string sCompletePath = "C://Users//Ruben//Desktop//Model//Textures//FBX.fbm//";
      sCompletePath += sValidFileName;
      //vctMaterials[vctMaterials.size() - 1].LoadTexture(sCompletePath);
    }
  }

  delete[] cBuffer;
  cBuffer = nullptr;

  std::cout << "Materials loaded" << std::endl;
  return vctMaterials;
}
// ------------------------------------
std::vector<render::graphics::CMesh*> CResourceManager::LoadModel(const char* _sPath, const char* _sBaseModelMtlDir)
{
  tinyobj::attrib_t attributes;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warnings;
  std::string errors;

  tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings, &errors, _sPath, _sBaseModelMtlDir);

  // Register materials
  std::vector<render::material::CMaterial*> vctMaterials;
  for (auto& oMaterial : materials)
  {
    render::material::CMaterial* pRegisterMaterial = new render::material::CMaterial(oMaterial.name.c_str());

    pRegisterMaterial->SetAmbientColor({ oMaterial.ambient[0], oMaterial.ambient[1], oMaterial.ambient[2] });
    pRegisterMaterial->SetDiffuseColor({ oMaterial.diffuse[0], oMaterial.diffuse[1], oMaterial.diffuse[2] });
    pRegisterMaterial->SetSpecularColor({ oMaterial.specular[0], oMaterial.specular[1], oMaterial.specular[2] });

    pRegisterMaterial->RegisterPath(render::material::EModifierType::AMBIENT, oMaterial.ambient_texname);
    pRegisterMaterial->RegisterPath(render::material::EModifierType::DIFFUSE, oMaterial.diffuse_texname);
    pRegisterMaterial->RegisterPath(render::material::EModifierType::SPECULAR, oMaterial.specular_texname);
    pRegisterMaterial->RegisterPath(render::material::EModifierType::BUMP, oMaterial.bump_texname);
    pRegisterMaterial->RegisterPath(render::material::EModifierType::DISPLACEMENT, oMaterial.displacement_texname);
    pRegisterMaterial->RegisterPath(render::material::EModifierType::ALPHA, oMaterial.alpha_texname);
    pRegisterMaterial->RegisterPath(render::material::EModifierType::REFLECTION, oMaterial.reflection_texname);

    vctMaterials.emplace_back(pRegisterMaterial);
  }

  std::vector<maths::CVector3> vctVertices = {};
  for (size_t i = 0; i < attributes.vertices.size(); i += 3)
  {
    vctVertices.emplace_back
    (
      attributes.vertices[i],
      attributes.vertices[i + 1],
      attributes.vertices[i + 2]
    );
  }

  std::vector<maths::CVector3> vctNormals = {};
  for (size_t i = 0; i < attributes.normals.size(); i += 3)
  {
    vctNormals.emplace_back
    (
      attributes.normals[i],
      attributes.normals[i + 1],
      attributes.normals[i + 2]
    );
  }

  std::vector<maths::CVector2> vctCoords = {};
  for (size_t i = 0; i < attributes.texcoords.size(); i += 2)
  {
    vctCoords.emplace_back
    (
      attributes.texcoords[i],
      attributes.texcoords[i + 1]
    );
  }

  std::vector<render::graphics::CMesh*> vctMeshes = {};

  for (int i = 0; i < shapes.size(); i++)
  {
    tinyobj::shape_t& shape = shapes[i];
    tinyobj::mesh_t& mesh = shape.mesh;

    std::vector<uint32_t> vctIndexes = {};
    std::vector<render::graphics::SVertexData> vctVertexData = {};

    uint32_t uOffset = 0;
    render::graphics::CMesh* pMesh = new render::graphics::CMesh(shape.name);

    for (int j = 0; j < mesh.num_face_vertices.size(); j++) 
    {
      uint32_t uVertexCount = mesh.num_face_vertices[j];
      for (uint32_t k = 0; k < uVertexCount; k++)
      {
        tinyobj::index_t idx = mesh.indices[uOffset + k];

        render::graphics::SVertexData oVertexData;
        oVertexData.Position = vctVertices[idx.vertex_index];

        bool bValidNormal = idx.normal_index >= 0 && idx.normal_index < vctNormals.size();
        oVertexData.Normal = bValidNormal ? vctNormals[idx.normal_index] : maths::CVector3::Forward;

        bool bValidTexCoord = idx.texcoord_index >= 0 && idx.texcoord_index < vctCoords.size();
        oVertexData.TexCoord = bValidTexCoord ? vctCoords[idx.texcoord_index] : maths::CVector2::Zero;

        vctVertexData.emplace_back(oVertexData);
        vctIndexes.emplace_back((uint32_t)(vctVertexData.size() - 1)); // Register index
      }
      uOffset += uVertexCount;
    }

    pMesh->CreateMesh(vctVertexData, vctIndexes);
    if (mesh.material_ids.size() > 0)
    {
      pMesh->ApplyMaterial(std::move(vctMaterials[mesh.material_ids[0]]));
    }
    vctMeshes.emplace_back(pMesh);
  }

  std::cout << "loaded" << std::endl;
  return vctMeshes;
}

