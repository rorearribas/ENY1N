#include "ResourceManager.h"
#include <sstream>
#include <iostream>
#include "Libs/Macros/GlobalMacros.h"

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
  vctNormal.reserve(1500);
  oModelData.m_vctVertexInfo.reserve(1500);
  oModelData.m_vctIndexes.reserve(5000);

  std::cout << "Loading resource from: " << _sPath << std::endl;
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

        uint32_t uVertexIdx = std::stoi(vertexData.substr(0, firstSlash)) - 1;
        uint32_t uTexCoordIdx = (firstSlash != std::string::npos && secondSlash != std::string::npos) ?
          std::stoi(vertexData.substr(firstSlash + 1, secondSlash - firstSlash - 1)) - 1 : 0;
        UNUSED_VARIABLE(uTexCoordIdx);
        uint32_t uNormalIdx = (secondSlash != std::string::npos) ? std::stoi(vertexData.substr(secondSlash + 1)) - 1 : 0;

        vctIndexes.emplace_back(uVertexIdx);
        if (uNormalIdx < vctNormal.size())
        {
          oModelData.m_vctVertexInfo[uVertexIdx].Normal = vctNormal[uNormalIdx];
        }
      }

      // Triangle
      if (vctIndexes.size() == 3)
      {
        for (uint32_t uIndex = 0; uIndex < vctIndexes.size(); ++uIndex)
        {
          oModelData.m_vctIndexes.emplace_back(vctIndexes[uIndex]);
        }
      }
      // Quad
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
        std::cout << "error getting valid faces" << std::endl;
        abort();
      }
    }
  }

  // Delete buffer
  delete[] cBuffer;
  cBuffer = nullptr;

  std::cout << "Resource loaded" << std::endl;
  return oModelData;
}

