#include "XmlNode.h"

namespace serialization
{
  CXmlNode::CXmlNode(CXmlNode&& _other) noexcept
    : m_oXmlNode(std::move(_other.m_oXmlNode))
  {
  }
  // ------------------------------------
  CXmlNode::CXmlNode(const CXmlNode& _other)
    : m_oXmlNode(_other.m_oXmlNode)
  {
  }
  // ------------------------------------
  CXmlNode::CXmlNode(const pugi::xml_node& _oXmlNode)
    : m_oXmlNode(_oXmlNode)
  {
  }
  // ------------------------------------
  CXmlNode CXmlNode::AddChild(const char* _sName)
  {
    return CXmlNode(m_oXmlNode.append_child(_sName));
  }
  // ------------------------------------
  bool CXmlNode::RemoveChild(const char* _sName)
  {
    return m_oXmlNode.remove_child(_sName);
  }
  // ------------------------------------
  bool CXmlNode::RemoveChildren()
  {
    return m_oXmlNode.remove_children();
  }
  // ------------------------------------
  CXmlNode CXmlNode::GetChild(const char* _sName) const
  {
    return CXmlNode(m_oXmlNode.child(_sName));
  }
  // ------------------------------------
  bool CXmlNode::HasChild(const char* _sName) const
  {
    return m_oXmlNode.child(_sName);
  }
  // ------------------------------------
  CXmlNode CXmlNode::GetFirstChild() const
  {
    return CXmlNode(m_oXmlNode.first_child());
  }
  // ------------------------------------
  CXmlNode CXmlNode::GetNextSibling() const
  {
    return CXmlNode(m_oXmlNode.next_sibling());
  }
  // ------------------------------------
  bool CXmlNode::RemoveAttribute(const char* _sName)
  {
    return m_oXmlNode.remove_attribute(_sName);
  }
  // ------------------------------------
  bool CXmlNode::RemoveAttributes()
  {
    return m_oXmlNode.remove_attributes();
  }
  // ------------------------------------
  const char* CXmlNode::GetAttribute(const char* _sName) const
  {
    return m_oXmlNode.attribute(_sName).value();
  }
  // ------------------------------------
  bool CXmlNode::HasAttribute(const char* _sName) const
  {
    return m_oXmlNode.attribute(_sName);
  }
  // ------------------------------------
  bool CXmlNode::AddAttribute(const char* _sName, const char* _sValue)
  {
    return m_oXmlNode.append_attribute(_sName).set_value(_sValue);
  }
}
