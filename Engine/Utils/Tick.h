#pragma once
#include <chrono>
#include <thread>

namespace utils
{
  class CTick {
  public:
    CTick(int targetTicksPerSecond) : m_iTargetTickDuration(1000 / targetTicksPerSecond), 
    lastTickTime(std::chrono::high_resolution_clock::now()) {}

    // Llamar a esta función al final de cada tick.
    void Update() 
    {
      auto currentTime = std::chrono::high_resolution_clock::now();
      auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTickTime).count();

      if (elapsedTime < m_iTargetTickDuration) 
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_iTargetTickDuration - elapsedTime));
      }
      lastTickTime = std::chrono::high_resolution_clock::now();
    }

  private:
    int m_iTargetTickDuration;  // Duración deseada de cada tick en milisegundos
    std::chrono::high_resolution_clock::time_point lastTickTime;
  };

}


