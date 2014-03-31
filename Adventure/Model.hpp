//
// Adventure
//

#pragma once

#include "Buffer.hpp"
#include "Frame.hpp"
#include "Geom.hpp"

namespace Ad
{
  class Model
  {
    Geom geom;

  public:
    Model (const Attrib* attribs, uint n_attribs);

    void draw (Frame& frame, v4f col, v3f pos, vsf ori, float scale);

  };

}
