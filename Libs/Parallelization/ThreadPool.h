#pragma once
#include "Task.h"
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>

namespace async
{
  class CThreadPool
  {
  private:
    enum class EThreadPoolStatus { E_INIT, E_RUNNING, E_STOPPED };

  public:
    CThreadPool(uint32_t _uThreadCount = std::thread::hardware_concurrency());
    ~CThreadPool() { Stop(); }

    void Start();
    void Stop();

    void AddTask(async::ITask* _pTask);
    uint32_t GetThreadCount() const { return m_uThreadCount.load(std::memory_order_acquire); }

    bool IsInitialized() const { return m_eStatus.load(std::memory_order_acquire) == EThreadPoolStatus::E_INIT; }
    bool IsRunning() const     { return m_eStatus.load(std::memory_order_acquire) == EThreadPoolStatus::E_RUNNING; }
    bool IsStopped() const     { return m_eStatus.load(std::memory_order_acquire) == EThreadPoolStatus::E_STOPPED; }

  private:
    void WorkerThread();

  private:
    std::deque<async::ITask*> m_lstTasks;
    std::atomic<EThreadPoolStatus> m_eStatus;

    std::vector<std::thread> m_lstThreads;
    std::atomic<uint32_t> m_uThreadCount;

    std::mutex m_oMutex;
    std::condition_variable m_oCondition;
  };
}
