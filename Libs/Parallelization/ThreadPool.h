#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <deque>
#include <condition_variable>

namespace async { class ITask; }

namespace async
{
  class CThreadPool
  {
  private:
    enum class EThreadPoolStatus { E_INIT, E_RUNNING, E_STOPPED };

  public:
    CThreadPool() : m_uActiveTaskCount(0u), m_eStatus(EThreadPoolStatus::E_STOPPED) {}
    ~CThreadPool() { Stop(); }

    void Start(uint32_t _uThreadCount = std::thread::hardware_concurrency());
    void Stop();

    void AddTask(async::ITask* _pTask);
    void WaitForAllTasks();

    bool IsInitialized() const { return m_eStatus.load(std::memory_order_acquire) == EThreadPoolStatus::E_INIT; }
    bool IsRunning() const     { return m_eStatus.load(std::memory_order_acquire) == EThreadPoolStatus::E_RUNNING; }
    bool IsStopped() const     { return m_eStatus.load(std::memory_order_acquire) == EThreadPoolStatus::E_STOPPED; }

  private:
    void WorkerThread();

  private:
    std::vector<std::thread>       m_lstThreads;
    std::deque<async::ITask*>      m_lstTasks;
                                   
    std::mutex                     m_oMutex;
    std::condition_variable        m_oConditionVar;
                                   
    std::atomic<EThreadPoolStatus> m_eStatus;
    std::atomic<uint32_t>          m_uThreadCount;
    std::atomic<uint32_t>          m_uActiveTaskCount;
  };
}
