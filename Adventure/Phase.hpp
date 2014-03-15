//
// Adventure
//

#pragma once

#include "Frame.hpp"
#include "Input.hpp"

#include <memory>

namespace Ad
{
  class Phase
  {
  public:
    typedef std::unique_ptr <Phase> Ptr;

    virtual void input  (const Event*, uint count, const KeyState*) = 0;
    virtual void tick   (float time, float step) = 0;
    virtual void render (Frame&) = 0;

    virtual ~Phase () = default;

  };

  Phase::Ptr create_menu ();
  Phase::Ptr create_play_phase ();

  inline Phase::Ptr create_phase_0 ()
  {
    return create_play_phase ();
  }

}
