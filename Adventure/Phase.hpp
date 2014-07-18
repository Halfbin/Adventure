//
// Adventure
//

#pragma once

#include "Frame.hpp"
#include "Input.hpp"
#include "Init.hpp"

#include <memory>

namespace Ad
{
  class Phase
  {
  public:
    typedef std::unique_ptr <Phase> Ptr;

    virtual void input  (const InputContext&) = 0;
    virtual void tick   (float time, float step) = 0;
    virtual void render (Frame&) = 0;

    virtual ~Phase () = default;

  };

  Phase::Ptr create_menu       (InitContext&);
  Phase::Ptr create_play_phase (InitContext&);

  inline Phase::Ptr create_phase_0 (InitContext& ctx)
  {
    return create_play_phase (ctx);
  }

}
