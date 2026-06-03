#pragma once
#include <new>
#include <memory>
#include <cstddef>

namespace utils
{
  template<typename T, uint32_t CHUNK_SIZE, uint32_t MAX_ITEMS = 1024u>
  class CArenaPool
  {
  public:
    CArenaPool() : m_tCurrentOffset(0), m_uRegisteredItems(0)
    {
      m_pBuffer = new unsigned char[CHUNK_SIZE];
    }

    ~CArenaPool() 
    { 
      Clear(); 
      delete[] m_pBuffer;
    }

    CArenaPool(const CArenaPool&) = delete;
    CArenaPool& operator=(const CArenaPool&) = delete;
    CArenaPool(CArenaPool&&) = delete;
    CArenaPool& operator=(CArenaPool&&) = delete;

    template<typename _T = T, typename ...Args>
    inline _T* Alloc(Args&&... args)
    {
      void* pCurrentPtr = m_pBuffer + m_tCurrentOffset;
      size_t tRemainingSpace = static_cast<size_t>(CHUNK_SIZE - m_tCurrentOffset);

      void* pObjectMem = static_cast<unsigned char*>(pCurrentPtr);
      void* pAlignedObjectPtr = std::align(alignof(_T), sizeof(_T), pObjectMem, tRemainingSpace);
      if (!pAlignedObjectPtr)
      {
        return nullptr;
      }

      // Calculate next size
      size_t tBytesConsumed = (static_cast<unsigned char*>(pAlignedObjectPtr) + sizeof(_T)) - static_cast<unsigned char*>(pCurrentPtr);
      if ((m_tCurrentOffset + tBytesConsumed) > CHUNK_SIZE)
      {
        return nullptr;
      }

      // Create object
      _T* pNewObject = ::new (pAlignedObjectPtr) _T(std::forward<Args>(args)...);
      m_tCurrentOffset += tBytesConsumed;
      m_lstOffsetTable[m_uRegisteredItems++] = static_cast<uint32_t>(static_cast<unsigned char*>(pAlignedObjectPtr) - m_pBuffer);
      return pNewObject;
    }

    inline T* operator[](uint32_t _uIdx)
    {
      if (_uIdx >= m_uRegisteredItems)
      {
        return nullptr;
      }
      return reinterpret_cast<T*>(m_pBuffer + m_lstOffsetTable[_uIdx]);
    }

    inline const T* operator[](uint32_t _uIdx) const
    {
      if (_uIdx >= m_uRegisteredItems)
      {
        return nullptr;
      }
      return reinterpret_cast<T*>(m_pBuffer + m_lstOffsetTable[_uIdx]);
    }

    inline void Clear()
    {
      if constexpr (!std::is_trivially_destructible_v<T>)
      {
        for (uint32_t uI = 0; uI < m_uRegisteredItems; uI++)
        {
          T* pObj = reinterpret_cast<T*>(m_pBuffer + m_lstOffsetTable[uI]);
          pObj->~T();
        }
      }

      if (m_uRegisteredItems > 0)
      {
        std::memset(m_lstOffsetTable, 0, m_uRegisteredItems * sizeof(uint32_t));
      }

      m_tCurrentOffset = 0;
      m_uRegisteredItems = 0;
    }

    inline uint32_t GetSize() const { return m_uRegisteredItems; }
    inline uint32_t GetMaxSize() const { return MAX_ITEMS; }

  private:
    unsigned char* m_pBuffer = nullptr;
    uint32_t m_lstOffsetTable[MAX_ITEMS];

    size_t m_tCurrentOffset;
    uint32_t m_uRegisteredItems;
  };
}
