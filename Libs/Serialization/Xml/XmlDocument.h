#pragma once
#include "pugixml/pugixml.hpp"
#include "XmlNode.h"

namespace serialization
{
  class CXmlDocument
  {
  public:
    CXmlDocument() = default;
    ~CXmlDocument() {}

    // Load XML
    bool LoadFile(const char* _sPath, uint32_t _uMode = pugi::parse_default, pugi::xml_encoding _eEncoding = pugi::encoding_auto);
    bool LoadFile(const wchar_t* _sPath, uint32_t _uMode = pugi::parse_default, pugi::xml_encoding _eEncoding = pugi::encoding_auto);
    bool LoadFromBuffer(const void* _pBuffer, size_t _tSize, uint32_t _uMode = pugi::parse_default, pugi::xml_encoding _eEncoding = pugi::encoding_auto);

    // Save XML
    bool Save(const char* _sPath, const char* _sIndent = PUGIXML_TEXT("\t"), uint32_t _uFlags = pugi::format_default, pugi::xml_encoding encoding = pugi::encoding_auto) const;
    bool Save(const wchar_t* _sPath, const char* _sIndent = PUGIXML_TEXT("\t"), uint32_t _uFlags = pugi::format_default, pugi::xml_encoding encoding = pugi::encoding_auto) const;

    // Get Root
    CXmlNode GetRoot() const;

  private:
    pugi::xml_document m_oXmlDoc;
  };
}



