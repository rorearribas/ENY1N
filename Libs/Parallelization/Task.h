#pragma once
#include <atomic>
namespace async
{
  class CThreadPool;

  class ITask
  {
  public:
    enum class ETaskStatus { PENDING, RUNNING, FAILED, COMPLETED };

  public:
    ITask() : m_eTaskStatus(ETaskStatus::PENDING) {}
    virtual ~ITask() = default;

    ITask(const ITask&) = delete;
    ITask& operator=(const ITask&) = delete;
    ITask(ITask&&) = delete;
    ITask& operator=(ITask&&) = delete;

    // Execute the task. This method will be called by the thread pool worker threads.
    virtual bool Execute() = 0;

    // Main-tread will wait until the task is completed. This is a busy-wait loop and may not be efficient for long-running tasks.
    inline void WaitForTaskCompletion() { while (!IsCompleted() && !IsFailed()) {} }

    // Getters for task status
    inline bool IsPending()   const { return GetTaskStatus() == ETaskStatus::PENDING;   }
    inline bool IsRunning()   const { return GetTaskStatus() == ETaskStatus::RUNNING;   }
    inline bool IsFailed()    const { return GetTaskStatus() == ETaskStatus::FAILED;    }
    inline bool IsCompleted() const { return GetTaskStatus() == ETaskStatus::COMPLETED; }

  private:
    friend class async::CThreadPool;
    inline ETaskStatus GetTaskStatus() const { return m_eTaskStatus.load(std::memory_order_acquire); }
    inline void SetStatus(ETaskStatus _eTaskStatus) { m_eTaskStatus.store(_eTaskStatus, std::memory_order_release); }

  private:  
    std::atomic<ETaskStatus> m_eTaskStatus;
  };
}
