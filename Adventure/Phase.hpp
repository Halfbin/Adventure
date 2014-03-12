//
// Adventure
//

#pragma once

namespace Ad
{
  class Phase
  {
  public:
    virtual void tick (float time, float step) = 0;
    virtual void render () = 0;

  };

}
