#pragma once
#include "pugixml/pugixml.hpp"

namespace serialization
{
  class CXmlAttribute
  {
  public:
    CXmlAttribute() = default;
    CXmlAttribute(CXmlAttribute&& _other) noexcept;
    CXmlAttribute(const CXmlAttribute& _other);
    CXmlAttribute(const pugi::xml_attribute& _oXmlAttribute);
    ~CXmlAttribute() {}


  private:
    pugi::xml_attribute m_oXmlAttribute;
  };
}

