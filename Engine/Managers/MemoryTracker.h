#pragma once

class CMemoryTracker
{
public:
  void RegisterMem(size_t _tSize);
  void DeregisterMem(size_t _tSize);

  size_t GetAllocatedSize() const { return m_tAllocatedSize; }
  size_t GetMemoryPeak() const { return m_tAllocatedSize; }

  void PrintStats() const;    

private:
  size_t m_tAllocatedSize = 0;
  size_t m_tMemoryPeak = 0;
};