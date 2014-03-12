//
// Adventure
//

#pragma once

#include <memory>

namespace Ad
{
  class Phase
  {
  public:
    typedef std::unique_ptr <Phase> Ptr;

    virtual void tick (float time, float step) = 0;
    virtual void render (int width, int height) = 0;

    virtual ~Phase () = default;

  };

  Phase::Ptr create_menu ();

  inline Phase::Ptr create_phase_0 ()
  {
    return create_menu ();
  }

}
