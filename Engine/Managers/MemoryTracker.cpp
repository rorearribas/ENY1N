#include "MemoryTracker.h"
#include <iostream>

namespace global
{
  namespace mem
  {
    void CMemoryTracker::RegisterMem(size_t _tSize)
    {
      m_tAllocatedSize += _tSize;
      m_tMemoryPeak += _tSize;
    }
    // ------------------------------------
    void CMemoryTracker::DeregisterMem(size_t _tSize)
    {
      m_tAllocatedSize -= _tSize;
    }
    // ------------------------------------
    void CMemoryTracker::PrintStats() const
    {
      printf("Current size: %zu - Memory peak: %zu\n", m_tAllocatedSize, m_tMemoryPeak);
    }
  }
}
