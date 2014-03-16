//
// Adventure
//

#pragma once

#include "Entity.hpp"
#include "Input.hpp"

namespace Ad
{
  class Player :
    public Entity
  {
  public:
    typedef std::unique_ptr <Player> Ptr;

    virtual void input  (const Event*, uint count, const KeyState*) = 0;

  };

  Player::Ptr create_player ();

}
