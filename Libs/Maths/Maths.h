#pragma once

namespace maths
{
  float Lerp(float a, float b, float f)
  {
    return static_cast<float>(a * (1.0 - f) + (b * f));
  }
}
