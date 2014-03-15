//
// Adventure
//

#include "Player.hpp"

#include "Texture.hpp"

namespace Ad
{
  class PlayerImpl :
    public Player
  {
    Texture::Ptr tex_idle [4];

    v2f position;
    v2i dir,
        facing;

    float speed;
    Key up_key, down_key,left_key, right_key;

    void input (const Event* events, uint count, const KeyState* keys)
    {
      using namespace Keys;

      auto up_state      = keys [up_key   ].down (),
           up_changed    = keys [up_key   ].changed (),
           down_state    = keys [down_key ].down (),
           down_changed  = keys [down_key ].changed (),
           left_state    = keys [left_key ].down (),
           left_changed  = keys [left_key ].changed (),
           right_state   = keys [right_key].down (),
           right_changed = keys [right_key].changed ();

      if (up_state || down_state)
      {
        if      (!down_state) dir.y = -1;
        else if (!up_state)   dir.y =  1;
        else if (up_changed || down_changed) dir.y = -dir.y;
      }
      else
      {
        dir.y = 0;
      }

      if (left_state || right_state)
      {
        if      (!right_state)  dir.x = -1;
        else if (!left_state)   dir.x =  1;
        else if (left_changed || right_changed) dir.x = -dir.x;
      }
      else
      {
        dir.x = 0;
      }

      if (dir.x || dir.y)
        facing = dir;
    }

    void tick (float time, float step)
    {
      position += unit (v2f (dir)) * speed * step;
    }

    void draw (Frame& frame)
    {
      float quad [] = {
        -64, -64, 0, 0,
         64, -64, 1, 0,
         64,  64, 1, 1,
        -64,  64, 0, 1
      };

      uint tex;
      if      (facing.y == -1) tex = tex_idle [0] -> name ();
      else if (facing.y ==  1) tex = tex_idle [1] -> name ();
      else if (facing.x == -1) tex = tex_idle [2] -> name ();
      else if (facing.x ==  1) tex = tex_idle [3] -> name ();

      frame.draw (tex, quad, 4, position, cxf (1, 0), v2f (1/128.f, 1/128.f));
    }

  public:
    PlayerImpl ()
    {
      position = nil;
      dir = nil;
      facing = v2i (0, 1);

      speed = 250;

      up_key    = Keys::alpha_w;
      down_key  = Keys::alpha_s;
      left_key  = Keys::alpha_a;
      right_key = Keys::alpha_d;

      tex_idle [0] = Texture::create ("Art/PlayerUp.png");
      tex_idle [1] = Texture::create ("Art/PlayerDown.png");
      tex_idle [2] = Texture::create ("Art/PlayerLeft.png");
      tex_idle [3] = Texture::create ("Art/PlayerRight.png");
    }

  };

  Player::Ptr create_player ()
  {
    return std::make_unique <PlayerImpl> ();
  }

}
