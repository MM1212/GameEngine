#include "utils/rgb.h"
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

namespace Engine {
  glm::vec3 Rgb2hsv(glm::vec3 in)
  {
    glm::vec3 out;
    float min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min < in.b ? min : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max > in.b ? max : in.b;

    out.z = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
      out.s = 0;
      out.x = 0; // undefined, maybe nan?
      return out;
    }
    if (max > 0.f) { // NOTE: if Max is == 0, this divide would cause a crash
      out.s = (delta / max);                  // s
    }
    else {
      // if max is 0, then r = g = b = 0              
      // s = 0, h is undefined
      out.s = 0.f;
      out.x = NAN;                            // its now undefined
      return out;
    }
    if (in.r >= max)                           // > is bogus, just keeps compilor happy
      out.x = (in.g - in.b) / delta;        // between yellow & magenta
    else
      if (in.g >= max)
        out.x = 2.f + (in.b - in.r) / delta;  // between cyan & yellow
      else
        out.x = 4.f + (in.r - in.g) / delta;  // between magenta & cyan

    out.x *= 60.f;                              // degrees

    if (out.x < 0.f)
      out.x += 360.f;

    return out;
  }


  glm::vec3 Hsv2rgb(glm::vec3 in)
  {
    float hh, p, q, t, ff;
    long i;
    glm::vec3 out{};

    if (in.y <= 0.f) {       // < is bogus, just shuts up warnings
      out.r = in.z;
      out.g = in.z;
      out.b = in.z;
      return out;
    }
    hh = in.x;
    if (hh >= 360.f) hh = 0.f;
    hh /= 60.f;
    i = (long)hh;
    ff = hh - i;
    p = in.z * (1.f - in.y);
    q = in.z * (1.f - (in.y * ff));
    t = in.z * (1.f - (in.y * (1.f - ff)));

    switch (i) {
    case 0:
      out.r = in.z;
      out.g = t;
      out.b = p;
      break;
    case 1:
      out.r = q;
      out.g = in.z;
      out.b = p;
      break;
    case 2:
      out.r = p;
      out.g = in.z;
      out.b = t;
      break;

    case 3:
      out.r = p;
      out.g = q;
      out.b = in.z;
      break;
    case 4:
      out.r = t;
      out.g = p;
      out.b = in.z;
      break;
    case 5:
    default:
      out.r = in.z;
      out.g = p;
      out.b = q;
      break;
    }
    return out;
  }
}