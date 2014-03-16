//
// Adventure
//

#include "Player.hpp"

#include "Texture.hpp"

namespace Ad
{
  namespace
  {
    v2f round (v2f v)
    {
      return floor (v + v2f (0.5f, 0.5f));
    }

  }

  class PlayerImpl :
    public Player
  {
    Texture::Ptr tex_idle [4];

    v2f position,
        prev_position;
    v2i dir,
        facing;

    float speed;
    Key up_key, down_key, left_key, right_key;

    void input (const Event*, uint, const KeyState* keys)
    {
      using namespace Keys;

      auto up    = keys [up_key   ],
           down  = keys [down_key ],
           left  = keys [left_key ],
           right = keys [right_key];

      if (up || down)
      {
        if (!down)
          dir.y = -1;
        else if (!up)
          dir.y =  1;
        else if (up.changed () || down.changed ())
          dir.y = -dir.y;
      }
      else
      {
        dir.y = 0;
      }

      if (left || right)
      {
        if (!right)
          dir.x = -1;
        else if (!left) 
          dir.x =  1;
        else if (left.changed () || right.changed ())
          dir.x = -dir.x;
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
      prev_position = position;
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

      auto pos = round (Rk::lerp (prev_position, position, frame.alpha));

      frame.set_camera (pos);

      frame.draw (tex, quad, 4, pos, cxf (1, 0), v2f (1/128.f, 1/128.f));
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

      using namespace TexFlags;
      tex_idle [0] = Texture::create ("Art/PlayerUp.png",    nearest);
      tex_idle [1] = Texture::create ("Art/PlayerDown.png",  nearest);
      tex_idle [2] = Texture::create ("Art/PlayerLeft.png",  nearest);
      tex_idle [3] = Texture::create ("Art/PlayerRight.png", nearest);
    }

    ~PlayerImpl () = default;

  };

  Player::Ptr create_player ()
  {
    return std::make_unique <PlayerImpl> ();
  }

}
