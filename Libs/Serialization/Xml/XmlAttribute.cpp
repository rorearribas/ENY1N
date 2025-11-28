#include "XmlAttribute.h"

namespace serialization
{
  CXmlAttribute::CXmlAttribute(CXmlAttribute&& _other) noexcept
    : m_oXmlAttribute(std::move(_other.m_oXmlAttribute))
  {
  }
  // ------------------------------------
  CXmlAttribute::CXmlAttribute(const CXmlAttribute& _other)
    : m_oXmlAttribute(_other.m_oXmlAttribute)
  {
  }
  // ------------------------------------
  CXmlAttribute::CXmlAttribute(const pugi::xml_attribute& _oXmlAttribute)
    : m_oXmlAttribute(_oXmlAttribute)
  {


  }
}