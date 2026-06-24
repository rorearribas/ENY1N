#pragma once
#include "Octree.h"

// ==================== COctreeNode ====================

template<typename T>
COctreeNode<T>::COctreeNode(const collision::CAABB& _oAABB, uint32_t _uDepth)
  : m_oAABB(_oAABB), m_uDepth(_uDepth)
{
  for (uint32_t uI = 0; uI < m_uDepth; ++uI)
  {
    m_lstChildren[uI] = nullptr;
  }
}

template<typename T>
void COctreeNode<T>::Insert(T* _pObject, const collision::CAABB& _oObjectBounds)
{
  // if the object doesn't intersect with this node's AABB, return
  if (m_oAABB.GetMin().x > _oObjectBounds.GetMax().x ||
      m_oAABB.GetMax().x < _oObjectBounds.GetMin().x ||
      m_oAABB.GetMin().y > _oObjectBounds.GetMax().y ||
      m_oAABB.GetMax().y < _oObjectBounds.GetMin().y ||
      m_oAABB.GetMin().z > _oObjectBounds.GetMax().z ||
      m_oAABB.GetMax().z < _oObjectBounds.GetMin().z)
  {
    return;
  }

  // if is leaf, add the object to this node
  if (IsLeaf())
  {
    m_lstObjects.push_back(_pObject);

    // if we exceed the max objects per node and haven't reached max depth, subdivide
    if (m_lstObjects.size() > s_uMaxObjectsPerNode && m_uDepth < s_uMaxDepth)
    {
      Subdivide();
    }
    return;
  }

  //  if we have already subdivided, insert into the children that intersect
  std::vector<int> lstOctants;
  GetIntersectingOctants(_oObjectBounds, lstOctants);

  for (int iIdx : lstOctants)
  {
    m_lstChildren[iIdx]->Insert(_pObject, _oObjectBounds);
  }
}

template<typename T>
void COctreeNode<T>::Query(const collision::CAABB& _oQueryBounds, std::vector<T*>& _lstResults_) const
{
  // If the query bounds do not intersect with this node's AABB, return
  if (m_oAABB.GetMin().x > _oQueryBounds.GetMax().x ||
      m_oAABB.GetMax().x < _oQueryBounds.GetMin().x ||
      m_oAABB.GetMin().y > _oQueryBounds.GetMax().y ||
      m_oAABB.GetMax().y < _oQueryBounds.GetMin().y ||
      m_oAABB.GetMin().z > _oQueryBounds.GetMax().z ||
      m_oAABB.GetMax().z < _oQueryBounds.GetMin().z)
  {
    return;
  }

  // Gather objects in this node
  for (T* pObj : m_lstObjects)
  {
    _lstResults_.push_back(pObj);
  }

  // if not is a leaf, search in children
  if (!IsLeaf())
  {
    for (uint32_t uI = 0; uI < m_uDepth; ++uI)
    {
      if (m_lstChildren[uI])
      {
        m_lstChildren[uI]->Query(_oQueryBounds, _lstResults_);
      }
    }
  }
}
// ------------------------------------
template<typename T>
bool COctreeNode<T>::Remove(T* _pObject, const collision::CAABB& _oObjectBounds)
{
  // if the object doesn't intersect with this node's AABB, return
  if (m_oAABB.GetMin().x > _oObjectBounds.GetMax().x ||
      m_oAABB.GetMax().x < _oObjectBounds.GetMin().x ||
      m_oAABB.GetMin().y > _oObjectBounds.GetMax().y ||
      m_oAABB.GetMax().y < _oObjectBounds.GetMin().y ||
      m_oAABB.GetMin().z > _oObjectBounds.GetMax().z ||
      m_oAABB.GetMax().z < _oObjectBounds.GetMin().z)
  {
    return false;
  }

  // Find in local
  auto it = std::find(m_lstObjects.begin(), m_lstObjects.end(), _pObject);
  if (it != m_lstObjects.end())
  {
    m_lstObjects.erase(it);
    return true;
  }

  // if not is a leaf, search in children
  if (!IsLeaf())
  {
    std::vector<int> lstOctants;
    GetIntersectingOctants(_oObjectBounds, lstOctants);

    for (int iIdx : lstOctants)
    {
      if (m_lstChildren[iIdx]->Remove(_pObject, _oObjectBounds))
        return true;
    }
  }

  return false;
}
// ------------------------------------
template<typename T>
void COctreeNode<T>::Clear()
{
  m_lstObjects.clear();

  for (int i = 0; i < 8; ++i)
  {
    if (m_lstChildren[i])
    {
      m_lstChildren[i]->Clear();
      m_lstChildren[i].reset();
    }
  }
}
// ------------------------------------
template<typename T>
void COctreeNode<T>::Subdivide()
{
  math::CVector3 v3Center = GetCenter();
  math::CVector3 v3HalfSize = m_oAABB.GetHalfSize();
  math::CVector3 v3Quarter = v3HalfSize * 0.5f;

  // Create children octants
  for (uint32_t ui = 0; ui < m_uDepth; ++ui)
  {
    math::CVector3 v3Offset = math::CVector3::Zero;
    v3Offset.x = (ui & 1) ? v3Quarter.x : -v3Quarter.x;
    v3Offset.y = (ui & 2) ? v3Quarter.y : -v3Quarter.y;
    v3Offset.z = (ui & 4) ? v3Quarter.z : -v3Quarter.z;

    math::CVector3 v3OctantCenter = v3Center + v3Offset;
    collision::CAABB rOctantBounds(v3OctantCenter - v3Quarter, v3OctantCenter + v3Quarter);

    m_lstChildren[ui] = std::make_unique<COctreeNode<T>>(rOctantBounds, m_uDepth + 1);
  }

  // redistribute objects to children
  auto lstObjectsCopy = m_lstObjects;
  m_lstObjects.clear();

  for (T* pObj : lstObjectsCopy)
  {
    // insert the object into the appropriate children
    for (uint32_t uI = 0; uI < m_uDepth; ++uI)
    {
      if (m_lstChildren[uI])
      {
        m_lstChildren[uI]->Insert(pObj, pObj->GetWorldAABB());
      }
    }
  }
}
// ------------------------------------
template<typename T>
void COctreeNode<T>::GetIntersectingOctants(const collision::CAABB& _rAABB, std::vector<int>& _lstIndices_)
{
  math::CVector3 v3Center = GetCenter();
  math::CVector3 v3HalfSize = m_oAABB.GetHalfSize();
  math::CVector3 v3Quarter = v3HalfSize * 0.5f;

  for (uint32_t uI = 0; uI < m_uDepth; ++uI)
  {
    math::CVector3 v3Offset = math::CVector3::Zero;
    v3Offset.x = (uI & 1) ? v3Quarter.x : -v3Quarter.x;
    v3Offset.y = (uI & 2) ? v3Quarter.y : -v3Quarter.y;
    v3Offset.z = (uI & 4) ? v3Quarter.z : -v3Quarter.z;

    math::CVector3 v3OctantCenter = v3Center + v3Offset;
    collision::CAABB roOctantBounds(v3OctantCenter - v3Quarter, v3OctantCenter + v3Quarter);

    // Check intersection between the octant bounds and the provided bounds
    if (!(roOctantBounds.GetMin().x > _rAABB.GetMax().x ||
      roOctantBounds.GetMax().x < _rAABB.GetMin().x ||
      roOctantBounds.GetMin().y > _rAABB.GetMax().y ||
      roOctantBounds.GetMax().y < _rAABB.GetMin().y ||
      roOctantBounds.GetMin().z > _rAABB.GetMax().z ||
      roOctantBounds.GetMax().z < _rAABB.GetMin().z))
    {
      _lstIndices_.push_back(uI);
    }
  }
}
// ------------------------------------
template<typename T>
math::CVector3 COctreeNode<T>::GetCenter() const
{
  return m_oAABB.GetCenter();
}
// ------------------------------------
#ifdef _DEBUG
template<typename T>
void COctreeNode<T>::DrawDebug() const
{
  // Compute color based on depth (for visualization purposes)
  float fDepthNormalized = static_cast<float>(m_uDepth) / static_cast<float>(s_uMaxDepth);

  math::CVector3 v3Color = math::CVector3::Zero;
  if (fDepthNormalized < 0.5f)
  {
    // Red to green (0.0 -> 0.5)
    float t = fDepthNormalized * 2.0f;
    v3Color.x = 1.0f - t;
    v3Color.y = t;
    v3Color.z = 0.0f;
  }
  else
  {
    // Green to Blue (0.5 -> 1.0)
    float t = (fDepthNormalized - 0.5f) * 2.0f;
    v3Color.x = 0.0f;
    v3Color.y = 1.0f - t;
    v3Color.z = t;
  }

  // Draw AABB of this node
  m_oAABB.DrawDebug(v3Color);

  // Draw children recursively
  if (!IsLeaf())
  {
    for (uint32_t uI = 0; uI < m_uDepth; ++uI)
    {
      if (m_lstChildren[uI])
      {
        m_lstChildren[uI]->DrawDebug();
      }
    }
  }
}
#endif

template<typename T>
COctree<T>::COctree(const collision::CAABB& _rWorldAABB, int _iMaxDepth)
  : m_uMaxDepth(_iMaxDepth), m_uObjectCount(0)
{
  m_pRoot = std::make_unique<COctreeNode<T>>(_rWorldAABB, 0);
}
// ------------------------------------
template<typename T>
void COctree<T>::Insert(T* _pObject, const collision::CAABB& _rObjectAABB)
{
  m_pRoot->Insert(_pObject, _rObjectAABB);
  m_uObjectCount++;
}
// ------------------------------------
template<typename T>
void COctree<T>::Query(const collision::CAABB& _rQueryAABB, std::vector<T*>& _lstResults_) const
{
  _lstResults_.clear();
  m_pRoot->Query(_rQueryAABB, _lstResults_);
}
// ------------------------------------
template<typename T>
bool COctree<T>::Remove(T* _pObject, const collision::CAABB& _rObjectAABB)
{
  if (m_pRoot->Remove(_pObject, _rObjectAABB))
  {
    m_uObjectCount--;
    return true;
  }
  return false;
}
// ------------------------------------
template<typename T>
void COctree<T>::Clear()
{
  m_pRoot->Clear();
  m_uObjectCount = 0;
}
// ------------------------------------
template<typename T>
void COctree<T>::Rebuild(const std::vector<std::pair<T*, collision::CAABB>>& _lstObjects)
{
  Clear();
  for (const auto& oPair : _lstObjects)
  {
    Insert(oPair.first, oPair.second);
  }
}
