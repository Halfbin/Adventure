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
  protected:
    Player (v3f pos, vsf ori) :
      Entity (pos, ori)
    { }

  public:
    typedef std::unique_ptr <Player> Ptr;

    virtual void input (const InputContext& ctx) = 0;

  };

  Player::Ptr create_player ();

}
