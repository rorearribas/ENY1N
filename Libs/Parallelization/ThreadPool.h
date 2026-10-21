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
    enum class EThreadPoolState { E_INIT, E_RUNNING, E_STOPPED, INVALID };

  public:
    CThreadPool() : m_eThreadPoolState(EThreadPoolState::INVALID), m_uActiveTaskCount(0u) {}
    ~CThreadPool() { Stop(); }

    void Start(uint32_t _uThreadCount = std::thread::hardware_concurrency());
    void Stop();

    void AddTask(async::ITask* _pTask);
    void WaitForAllTasks();

    inline bool IsInitialized() const { return m_eThreadPoolState.load(std::memory_order_acquire) == EThreadPoolState::E_INIT; }
    inline bool IsRunning()     const { return m_eThreadPoolState.load(std::memory_order_acquire) == EThreadPoolState::E_RUNNING; }
    inline bool IsStopped()     const { return m_eThreadPoolState.load(std::memory_order_acquire) == EThreadPoolState::E_STOPPED; }

  private:
    void WorkerThread();

  private:
    std::vector<std::thread>       m_lstThreads;
    std::deque<async::ITask*>      m_lstTasks;
                                   
    std::mutex                     m_oMutex;
    std::condition_variable        m_oConditionVar;
                                   
    std::atomic<EThreadPoolState>  m_eThreadPoolState;
    std::atomic<uint32_t>          m_uActiveTaskCount;
  };
}
