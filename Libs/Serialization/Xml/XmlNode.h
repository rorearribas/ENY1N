#pragma once
#include "pugixml/pugixml.hpp"

namespace serialization
{
  class CXmlNode
  {
  public:
    CXmlNode() = default;
    CXmlNode(CXmlNode&& _other) noexcept;
    CXmlNode(const CXmlNode& _other);
    CXmlNode(const pugi::xml_node& _oXmlNode);
    ~CXmlNode() {}

    // ---------------------------
    // Child nodes
    // ---------------------------
    CXmlNode AddChild(const char* _sName);
    bool RemoveChild(const char* _sName);
    bool RemoveChildren();

    CXmlNode GetChild(const char* _sName) const;
    bool HasChild(const char* _sName) const;

    // Iterate
    CXmlNode GetFirstChild() const;
    CXmlNode GetNextSibling() const;

    // ---------------------------
    // Attributes
    // ---------------------------
    bool RemoveAttribute(const char* _sName);
    bool RemoveAttributes();

    const char* GetAttribute(const char* _sName) const;
    bool HasAttribute(const char* _sName) const;

    // Modify attributes
    bool AddAttribute(const char* _sName, const char* _sValue);
    template<typename T>
    inline bool SetAttribute(const char* _sName, T Value)
    {
      if (HasAttribute(_sName))
      {
        m_oXmlNode.attribute(_sName).set_value(Value);
      }
    }
    // ---------------------------
    // Node value
    // ---------------------------
    inline const char* GetName() const { return m_oXmlNode.name(); }
    inline const char* GetValue() const { return m_oXmlNode.value(); }
    inline const bool HasValue() const { return GetValue(); }

  private:
    pugi::xml_node m_oXmlNode;
  };
}