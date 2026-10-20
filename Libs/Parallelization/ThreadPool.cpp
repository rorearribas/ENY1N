#include "ThreadPool.h"

namespace async
{
  namespace internal
  {
    static const uint32_t s_uDefaultThreads = 2;
  }
  // ------------------------------------
  CThreadPool::CThreadPool(uint32_t _uThreadCount)
  {
    // Set max threads
    uint32_t uMaxThreads = std::thread::hardware_concurrency();
    uMaxThreads = (uMaxThreads == 0) ? internal::s_uDefaultThreads : std::min(_uThreadCount, uMaxThreads);
    m_uThreadCount.store(uMaxThreads, std::memory_order_release);

    // Set initial state
    m_eStatus.store(EThreadPoolStatus::E_INIT, std::memory_order_release);
  }
  // ------------------------------------
  void CThreadPool::Start()
  {
    // Wait for any previous threads to finish
    Stop();

    // Create threads
    for (uint32_t uI = 0; uI < GetThreadCount(); uI++)
    {
      std::thread rThread = std::thread(&CThreadPool::WorkerThread, this);
      m_lstThreads.emplace_back(std::move(rThread));
    }

    // Update state
    m_eStatus.store(EThreadPoolStatus::E_RUNNING, std::memory_order_release);
  }
  // ------------------------------------
  void CThreadPool::Stop()
  {
    m_eStatus.store(EThreadPoolStatus::E_STOPPED, std::memory_order_release);
    m_oCondition.notify_all();

    for (std::thread& rThread : m_lstThreads)
    {
      if (rThread.joinable())
      {
        rThread.join();
      }
    }

    // Flush tasks
    m_lstThreads.clear();
  }
  // ------------------------------------
  void CThreadPool::AddTask(async::ITask* _pTask)
  {
    std::lock_guard<std::mutex> lock(m_oMutex);
    m_lstTasks.emplace_back(_pTask);
    m_oCondition.notify_one();
  }
  // ------------------------------------
  void CThreadPool::WorkerThread()
  {
    while (true)
    {
      async::ITask* pTask = nullptr;
      {
        std::unique_lock<std::mutex> oLock(m_oMutex);
        m_oCondition.wait(oLock, [this]() { return !m_lstTasks.empty() || IsStopped(); });

        if (IsStopped() && m_lstTasks.empty())
        {
          break;
        }

        pTask = m_lstTasks.front();
        m_lstTasks.pop_front();
      }
      if(pTask && pTask->Execute())
      {
        pTask->SetCompleted(true);
      }
    }
  }
}

