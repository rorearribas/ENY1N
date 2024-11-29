#include "ResourceManager.h"
#include <sstream>
#include <iostream>
#include "Libs/Macros/GlobalMacros.h"
#include <filesystem>

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
CResourceManager::SModelData CResourceManager::LoadModelData(const char* _sPath, const char* _sMode)
{
  CResourceManager::SModelData oModelData = {};
  char* cBuffer = LoadResource(_sPath, _sMode);
  if (!cBuffer) { return oModelData; }

  std::istringstream oStream(cBuffer);
  std::string sLine;

  // Reserve
  std::vector<maths::CVector3> vctNormal = {};
  vctNormal.reserve(300);

  // Texture coords
  std::vector<maths::CVector2> vctTextureCoords = {};
  vctTextureCoords.reserve(5000);

  oModelData.m_vctVertexInfo.reserve(3000);
  oModelData.m_vctIndexes.reserve(9000);

  std::cout << "Loading model data from: " << _sPath << std::endl;
  while (std::getline(oStream, sLine))
  {
    // Vertex position
    if (sLine.rfind("v ", 0) == 0)
    {
      std::istringstream linestream(sLine.substr(2));
      float fPosX, fPosY, fPosZ;
      linestream >> fPosX >> fPosY >> fPosZ;
      render::graphics::CModel::SVertexInfo oVertexInfo;
      oVertexInfo.Position = maths::CVector3(fPosX, fPosY, fPosZ);
      oModelData.m_vctVertexInfo.emplace_back(oVertexInfo);
    }
    // Texture coords
    else if (sLine.rfind("vt", 0) == 0)
    {
      std::istringstream linestream(sLine.substr(2));
      float fPosX, fPosY;
      linestream >> fPosX >> fPosY;
      vctTextureCoords.emplace_back(maths::CVector2(fPosX, fPosY));
    }
    // Vertex normals
    else if (sLine.rfind("vn", 0) == 0)
    {
      // Comprobar si la línea comienza con "vn"
      std::istringstream linestream(sLine.substr(2));
      float fNormalX, fNormalY, fNormalZ;
      linestream >> fNormalX >> fNormalY >> fNormalZ;
      vctNormal.emplace_back(maths::CVector3(fNormalX, fNormalY, fNormalZ));
    }
    // Faces
    else if (sLine.rfind("f ", 0) == 0)
    {
      std::istringstream linestream(sLine.substr(2));
      std::string vertexData;

      std::vector<uint32_t> vctIndexes = {};
      while (linestream >> vertexData)
      {
        size_t firstSlash = vertexData.find('/');
        size_t secondSlash = vertexData.find('/', firstSlash + 1);

        // Vertex index
        uint32_t uVertexIdx = std::stoi(vertexData.substr(0, firstSlash)) - 1;
        vctIndexes.emplace_back(uVertexIdx);
        // Texture coord index
        uint32_t uTexCoordIdx = (firstSlash != std::string::npos && secondSlash != std::string::npos) ?
          std::stoi(vertexData.substr(firstSlash + 1, secondSlash - firstSlash - 1)) - 1 : 0;
        // Normal index
        uint32_t uNormalIdx = (secondSlash != std::string::npos) ? std::stoi(vertexData.substr(secondSlash + 1)) - 1 : 0;


        // Set normal
        if (uVertexIdx < oModelData.m_vctVertexInfo.size() && uNormalIdx < vctNormal.size())
        {
          oModelData.m_vctVertexInfo[uVertexIdx].Normal = vctNormal[uNormalIdx];
        }

        // Set texture coord
        if (uVertexIdx < oModelData.m_vctVertexInfo.size() && uTexCoordIdx < vctTextureCoords.size())
        {
          oModelData.m_vctVertexInfo[uVertexIdx].TextureCoord = vctTextureCoords[uTexCoordIdx];
        }
      }

      // Only for triangles
      if (vctIndexes.size() == 3)
      {
        for (uint32_t uIndex = 0; uIndex < vctIndexes.size(); ++uIndex)
        {
          oModelData.m_vctIndexes.emplace_back(vctIndexes[uIndex]);
        }
      }
      // More than 3 vertex
      else if (vctIndexes.size() >= 4)
      {
        for (size_t i = 1; i < vctIndexes.size() - 1; ++i)
        {
          oModelData.m_vctIndexes.push_back(vctIndexes[0]);
          oModelData.m_vctIndexes.push_back(vctIndexes[i]);
          oModelData.m_vctIndexes.push_back(vctIndexes[i + 1]);
        }
      }
      else
      {
        throw std::runtime_error("Invalid face detected in file: " + std::string(_sPath));
      }
    }
  }

  // Delete buffer
  delete[] cBuffer;
  cBuffer = nullptr;

  std::cout << "Model data loaded" << std::endl;
  return oModelData;
}
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
      vctMaterials[vctMaterials.size() - 1].SetTransparent(fTransparent);
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

