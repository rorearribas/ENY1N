#pragma once

#include <vector>
#include <memory>
#include "Engine/Collisions/AABB.h"

template<typename T>
class COctreeNode
{
public:
  static constexpr uint32_t s_uMaxObjectsPerNode = 8u;
  static constexpr uint32_t s_uMaxDepth = 8u;

  COctreeNode(const collision::CAABB& _oAABB, uint32_t _uDepth = 0);
  ~COctreeNode() = default;

  void Insert(T* _pObject, const collision::CAABB& _rObjectAABB);
  void Query(const collision::CAABB& _rQueryAABB, std::vector<T*>& _lstResults_) const;

  bool Remove(T* _pObject, const collision::CAABB& _rObjectAABB);
  void Clear();

  // Getters
  inline const collision::CAABB& GetAABB() const { return m_oAABB; }
  inline const std::vector<T*>& GetObjects() const { return m_lstObjects; }
  inline bool IsLeaf() const { return m_lstChildren[0] == nullptr; }
  inline uint32_t GetDepth() const { return m_uDepth; }

#ifdef _DEBUG
  void DrawDebug() const;
#endif
 
private:
  void Subdivide();
  void GetIntersectingOctants(const collision::CAABB& _rAABB, std::vector<int>& _lstIndices_);
  math::CVector3 GetCenter() const;

private:
  std::vector<T*> m_lstObjects;
  std::unique_ptr<COctreeNode<T>> m_lstChildren[s_uMaxDepth];

  collision::CAABB m_oAABB = collision:CAABB();
  uint32_t m_uDepth;
};

template<typename T>
class COctree
{
public:
  explicit COctree(const collision::CAABB& _rWorldAABB, int _iMaxDepth = COctreeNode<T>::s_uMaxDepth);
  ~COctree() = default;

  // Handle octree
  void Insert(T* _pObject, const collision::CAABB& _rObjectAABB);
  void Query(const collision::CAABB& _rQueryAABB, std::vector<T*>& _lstResults_) const;
  bool Remove(T* _pObject, const collision::CAABB& _rObjectAABB);
  void Clear();

  // Rebuild the octree with a new set of objects
  void Rebuild(const std::vector<std::pair<T*, collision::CAABB>>& _lstObjects);

  inline size_t GetObjectCount() const { return m_uObjectCount; }
  inline const collision::CAABB& GetWorldBounds() const { return m_pRoot->GetAABB(); }
  inline int GetMaxDepth() const { return m_uMaxDepth; }

#ifdef _DEBUG
  inline void DrawDebug() const { if (m_pRoot) { m_pRoot->DrawDebug(); } }
#endif

private:
  std::unique_ptr<COctreeNode<T>> m_pRoot;
  uint32_t m_uMaxDepth;
  size_t m_uObjectCount;
};

#include "Octree.cpp"
