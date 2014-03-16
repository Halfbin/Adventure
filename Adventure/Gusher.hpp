//
// Adventure
//

#pragma once

#include "Frame.hpp"

#include "Buffer.hpp"
#include "Geom.hpp"

namespace Ad
{
  class Gusher
  {
    Buffer data,
           idxs;
    Geom   geom;

  public:
    Gusher ();

    void draw (Frame& frame, v4f col, v3f pos, vsf ori, float scale);

  };

}
