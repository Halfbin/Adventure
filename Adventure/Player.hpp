//
// Adventure
//

#pragma once

#include "Frame.hpp"
#include "Input.hpp"

#include <memory>

namespace Ad
{
  class Player
  {
  public:
    typedef std::unique_ptr <Player> Ptr;

    virtual void input  (const Event*, uint count, const KeyState*) = 0;
    virtual void tick (float time, float step) = 0;
    virtual void draw (Frame& frame) = 0;

  };

  Player::Ptr create_player ();

}
