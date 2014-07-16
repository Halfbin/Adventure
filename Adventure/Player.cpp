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
    v3f prev_pos;
    v3f speed;
    v3f dir;
    float rolling;
    Key forward_key, left_key, back_key, right_key, drop_key, rise_key, cw_key, ccw_key;

    void input (const InputContext& ctx)
    {
      using namespace Keys;

      // Looking
      auto cw  = ctx.keys [cw_key ],
           ccw = ctx.keys [ccw_key];

      if (cw || ccw)
      {
        if (!ccw)
          rolling =  1;
        else if (!cw) 
          rolling = -1;
        else if (cw.changed || ccw.changed)
          rolling = -rolling;
      }
      else
      {
        rolling = 0;
      }

      vsf yaw   = Rk::rotation (-0.001f * ctx.pointer.x, v3f (0, 0, 1));
      vsf pitch = Rk::rotation ( 0.001f * ctx.pointer.y, v3f (0, 1, 0));
      vsf roll  = Rk::rotation ( 0.020f * rolling,       v3f (1, 0, 0));

      ori = unit (ori * pitch * yaw * roll);

      // Walking
      auto forward = ctx.keys [forward_key],
           left    = ctx.keys [left_key   ],
           back    = ctx.keys [back_key   ],
           right   = ctx.keys [right_key  ],
           drop    = ctx.keys [drop_key   ],
           rise    = ctx.keys [rise_key   ];

      if (forward || back)
      {
        if (!back)
          dir.x =  1;
        else if (!forward)
          dir.x = -1;
        else if (forward.changed || back.changed)
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

      if (rise || drop)
      {
        if (!drop)
          dir.z =  1;
        else if (!rise) 
          dir.z = -1;
        else if (rise.changed || drop.changed)
          dir.z = -dir.z;
      }
      else
      {
        dir.z = 0;
      }
    }

    void advance (float time, float step)
    {
      prev_pos = pos;
      auto vel = Rk::conj (ori, speed * unit (dir));
      pos += vel * step;
    }

    void draw (Frame& frame)
    {
      v3f eye_off { 0, 0, 0 };
      auto draw_pos = Rk::lerp (prev_pos, pos, frame.alpha);
      frame.set_camera (pos + eye_off, ori);
    }

    void collide (v3f normal, float pdist)
    {
      pos += normal * pdist;
      if (dot (dir, normal) < 0)
        dir = nil;
    }

  public:
    PlayerImpl () :
      Player ({-15,0,5}, identity)
    {
      bbox_mins = {-0.4f,-0.4f, 0.0f};
      bbox_maxs = { 0.4f, 0.4f, 1.8f};

      prev_pos = pos;
      dir = nil;
      rolling = 0.0f;

      speed = {10,4,4};

      using namespace Keys;
      forward_key = alpha_w;
      left_key    = alpha_a;
      back_key    = alpha_s;
      right_key   = alpha_d;
      drop_key    = lt_ctrl;
      rise_key    = spacebar;
      cw_key      = alpha_e;
      ccw_key     = alpha_q;
    }

    ~PlayerImpl () = default;

  };

  Player::Ptr create_player ()
  {
    return std::make_unique <PlayerImpl> ();
  }

}
