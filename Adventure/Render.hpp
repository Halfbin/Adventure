//
// Adventure
//

#pragma once

#include "Frame.hpp"

namespace Ad
{
  class Renderer
  {
    struct Impl;
    Impl* impl;

  public:
    Renderer ();
    ~Renderer ();

    Frame& begin (int width, int height, float alpha);
    void   end ();

  };

}
