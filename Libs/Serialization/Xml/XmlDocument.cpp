#include "XmlDocument.h"

namespace serialization
{
  // ------------------------------------
  bool CXmlDocument::LoadFile(const char* _sPath, uint32_t _uMode, pugi::xml_encoding _eEncoding)
  {
    return m_oXmlDoc.load_file(_sPath, _uMode, _eEncoding);
  }
  // ------------------------------------
  bool CXmlDocument::LoadFile(const wchar_t* _sPath, uint32_t _uMode, pugi::xml_encoding _eEncoding)
  {
    return m_oXmlDoc.load_file(_sPath, _uMode, _eEncoding);
  }
  // ------------------------------------
  bool CXmlDocument::LoadFromBuffer(const void* _pBuffer, size_t _tSize, uint32_t _uMode, pugi::xml_encoding _eEncoding)
  {
    return m_oXmlDoc.load_buffer(_pBuffer, _tSize, _uMode, _eEncoding);
  }
  // ------------------------------------
  bool CXmlDocument::Save(const char* _sPath, const char* _sIndent, uint32_t _uFlags, pugi::xml_encoding _eEncoding) const
  {
    return m_oXmlDoc.save_file(_sPath, _sIndent, _uFlags, _eEncoding);
  }
  // ------------------------------------
  bool CXmlDocument::Save(const wchar_t* _sPath, const char* _sIndent, uint32_t _uFlags, pugi::xml_encoding _eEncoding) const
  {
    return m_oXmlDoc.save_file(_sPath, _sIndent, _uFlags, _eEncoding);
  }
  // ------------------------------------
  CXmlNode CXmlDocument::GetRoot() const
  {
    return CXmlNode(m_oXmlDoc.document_element());
  }
}