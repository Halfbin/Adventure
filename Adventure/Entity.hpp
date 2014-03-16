//
// Adventure
//

#pragma once

#include "Frame.hpp"

#include <memory>

namespace Ad
{
  class Entity
  {
  public:
    typedef std::unique_ptr <Entity> Ptr;

    virtual void tick (float time, float step) = 0;
    virtual void draw (Frame& frame) = 0;

    virtual ~Entity () = 0;

  };

}
