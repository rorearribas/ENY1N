#include "ThreadPool.h"
#include "Task.h"
#include <algorithm>

namespace async
{
  namespace internal
  {
    static const uint32_t s_uDefaultThreads = 2;
  }
  // ------------------------------------
  void CThreadPool::Start(uint32_t _uThreadCount)
  {
    // Wait for any previous threads to finish
    Stop();

    // Set initial state
    m_eThreadPoolState.store(EThreadPoolState::E_INIT, std::memory_order_release);

    // Set max threads
    uint32_t uMaxThreads = std::thread::hardware_concurrency();
    uMaxThreads = (uMaxThreads == 0) ? internal::s_uDefaultThreads : std::min(_uThreadCount, uMaxThreads);

    // Create threads
    for (uint32_t uI = 0; uI < uMaxThreads; uI++)
    {
      std::thread oThread = std::thread(&CThreadPool::WorkerThread, this);
      m_lstThreads.emplace_back(std::move(oThread));
    }

    // Update state
    m_eThreadPoolState.store(EThreadPoolState::E_RUNNING, std::memory_order_release);
  }
  // ------------------------------------
  void CThreadPool::Stop()
  {
    m_eThreadPoolState.store(EThreadPoolState::E_STOPPED, std::memory_order_release);
    m_oConditionVar.notify_all();

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
    // Add the task to the queue
    std::lock_guard<std::mutex> lock(m_oMutex);
    m_uActiveTaskCount.fetch_add(1u, std::memory_order_relaxed);
    m_lstTasks.emplace_back(_pTask);

    // Notify one worker thread that a new task is available
    m_oConditionVar.notify_one();
  }
  // ------------------------------------
  void CThreadPool::WaitForAllTasks()
  {
    while (m_uActiveTaskCount.load(std::memory_order_acquire) > 0)
    {
      std::this_thread::yield();
    }
  }
  // ------------------------------------
  void CThreadPool::WorkerThread()
  {
    while (true)
    {
      std::unique_lock<std::mutex> unique_lock(m_oMutex);
      m_oConditionVar.wait(unique_lock, [this]() { return IsStopped() || !m_lstTasks.empty(); });

      // If the thread pool is stopped and there are no tasks left, exit the loop
      if (IsStopped() && m_lstTasks.empty())
      {
        break;
      }

      // Get the next task from the queue
      async::ITask* pTask = m_lstTasks.front();
      m_lstTasks.pop_front();

      // Update task status to running
      pTask->SetStatus(ITask::ETaskStatus::RUNNING);

      // Execute the task
      bool bSuccess = pTask->Execute();
      pTask->SetStatus(bSuccess ? ITask::ETaskStatus::COMPLETED : ITask::ETaskStatus::FAILED);
      m_uActiveTaskCount.fetch_sub(1u, std::memory_order_relaxed);
    }
  }
}

