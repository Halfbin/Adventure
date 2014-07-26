//
// Adventure
//

#pragma once

#include "Entity.hpp"

#include "ShipTypes.hpp"
#include "Input.hpp"
#include "Init.hpp"

namespace Ad
{
  class Player :
    public Entity
  {
  protected:
    Player (v3i spos, v3f fpos, vsf ori) :
      Entity (spos, fpos, ori)
    { }

  public:
    typedef std::unique_ptr <Player> Ptr;

    virtual void input (const InputContext& ctx) = 0;

  };

  Player::Ptr create_player (const InitContext& init, const ShipType& ship, v3i spos, v3f fpos, vsf ori);

}
