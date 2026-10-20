#pragma once
#include <atomic>
namespace async
{
  class CThreadPool;

  class ITask
  {
  public:
    virtual ~ITask() = default;
    virtual bool Execute() = 0;

    // Main-tread will wait until the task is completed. This is a busy-wait loop and may not be efficient for long-running tasks.
    inline void WaitForTaskCompletion() { while (!IsCompleted()) {} }
    inline bool IsCompleted() const { return m_bIsCompleted.load(std::memory_order_acquire); }

  private:
    friend class async::CThreadPool;
    inline void SetCompleted(bool _bCompleted) { m_bIsCompleted.store(_bCompleted, std::memory_order_release); }

  private:
    std::atomic<bool> m_bIsCompleted = false;
  };
}
