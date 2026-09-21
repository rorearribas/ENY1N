#pragma once
#include "pugixml/pugixml.hpp"

namespace serialization
{
  class CXmlAttribute
  {
  public:
    CXmlAttribute() = default;
    CXmlAttribute(const pugi::xml_attribute& _rXmlAttribute);
    CXmlAttribute(CXmlAttribute&& _other) noexcept;
    CXmlAttribute(const CXmlAttribute& _other);
    ~CXmlAttribute() {}

  private:
    pugi::xml_attribute m_oXmlAttribute;
  };
}

