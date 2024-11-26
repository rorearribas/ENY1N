#include "ResourceManager.h"

char* CResourceManager::LoadResource(const char* _sPath)
{
  FILE* pFile = nullptr;
  fopen_s(&pFile, _sPath, "rb");
  if (pFile == NULL)
  {
    perror("Error opening file");
    return nullptr;
  }

  fseek(pFile, 0, SEEK_END);
  long fileSize = ftell(pFile);
  rewind(pFile);

  char* buffer = new char[fileSize + 1];
  if (buffer == nullptr)
  {
    perror("Error allocating memory");
    fclose(pFile);
    return nullptr;
  }

  // Leer el archivo completo
  size_t iTotalSize = fread(buffer, 1, fileSize, pFile);
  buffer[iTotalSize] = '\0';
  fclose(pFile);

  return buffer;
}
