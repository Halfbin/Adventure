//
// Adventure
//

#include "Player.hpp"

#include "Texture.hpp"

#include <Rk/clamp.hpp>

namespace Ad
{
  class PlayerImpl :
    public Player
  {
    v3f position,
        prev_position,
        dir;

    vsf   orientation,
          azimuth;
    float elevation;
    bool  crouching;

    float speed;
    Key up_key, left_key, down_key, right_key, crouch_key;

    void input (const InputContext& ctx)
    {
      using namespace Keys;

      auto up     = ctx.keys [up_key    ],
           left   = ctx.keys [left_key  ],
           down   = ctx.keys [down_key  ],
           right  = ctx.keys [right_key ],
           crouch = ctx.keys [crouch_key];

      if (up || down)
      {
        if (!down)
          dir.x =  1;
        else if (!up)
          dir.x = -1;
        else if (up.changed || down.changed)
          dir.x = -dir.x;
      }
      else
      {
        dir.x = 0;
      }

      if (left || right)
      {
        if (!right)
          dir.y =  1;
        else if (!left) 
          dir.y = -1;
        else if (left.changed || right.changed)
          dir.y = -dir.y;
      }
      else
      {
        dir.y = 0;
      }

      crouching = crouch.down != 0;

      azimuth = Rk::rotation (-0.001f * ctx.pointer.x, v3f (0, 0, 1))
              * azimuth;

      elevation = Rk::clamp (elevation + 0.001f * ctx.pointer.y, -1.55f, 1.55f);
      auto elev_axis = Rk::conj (azimuth, v3f (0, 1, 0));

      orientation = unit (Rk::rotation (elevation, elev_axis) * azimuth);
    }

    void tick (float time, float step)
    {
      prev_position = position;
      auto velocity = speed * Rk::conj (azimuth, unit (dir));
      position += velocity * step;
    }

    void draw (Frame& frame)
    {
      v3f eye_off { 0, 0, 1.7f };
      if (crouching)
        eye_off *= 0.5f;

      auto pos = Rk::lerp (prev_position, position, frame.alpha);
      frame.set_camera (pos + eye_off, orientation);
    }

  public:
    PlayerImpl ()
    {
      position = v3f (-15, 0, 0);
      prev_position = position;
      dir = nil;

      orientation = identity;
      azimuth = identity;
      elevation = 0.f;

      speed = 25;

      using namespace Keys;
      up_key     = alpha_w;
      left_key   = alpha_a;
      down_key   = alpha_s;
      right_key  = alpha_d;
      crouch_key = lt_ctrl;
    }

    ~PlayerImpl () = default;

  };

  Player::Ptr create_player ()
  {
    return std::make_unique <PlayerImpl> ();
  }

}
