//
// Adventure
//

#include "Player.hpp"

#include "Constants.hpp"
#include "Texture.hpp"

#include <Rk/clamp.hpp>

namespace Ad
{
  template <typename T>
  void key_axis (T& value, KeyState positive, KeyState negative)
  {
    if (positive || negative)
    {
      if (!negative)
        value =  1;
      else if (!positive)
        value = -1;
      else if (positive.changed || negative.changed)
        value = -value;
    }
    else
    {
      value = 0;
    }
  }

  class PlayerImpl :
    public Player
  {
    v3f solar_pos;
    v3f prev_pos;
    v3f speed;
    v3f dir;
    float rolling;
    Key forward_key, left_key, back_key, right_key, drop_key, rise_key, cw_key, ccw_key;

    void input (const InputContext& ctx)
    {
      using namespace Keys;

      // Rotate
      auto cw  = ctx.keys [cw_key ],
           ccw = ctx.keys [ccw_key];

      key_axis (rolling, cw, ccw);

      vsf roll  = Rk::rotation ( 0.020f * rolling,       v3f (1, 0, 0));
      vsf pitch = Rk::rotation ( 0.001f * ctx.pointer.y, v3f (0, 1, 0));
      vsf yaw   = Rk::rotation (-0.001f * ctx.pointer.x, v3f (0, 0, 1));

      ori = unit (ori * roll * pitch * yaw);

      // Translate
      auto forward = ctx.keys [forward_key],
           left    = ctx.keys [left_key   ],
           back    = ctx.keys [back_key   ],
           right   = ctx.keys [right_key  ],
           drop    = ctx.keys [drop_key   ],
           rise    = ctx.keys [rise_key   ];

      key_axis (dir.x, forward, back );
      key_axis (dir.y, left,    right);
      key_axis (dir.z, rise,    drop );
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
      frame.set_camera (solar_pos + pos, 100.f, 1000000.f, pos + eye_off, 0.1f, 1000.f, ori);
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

      solar_pos = {2000.f, 0.f, 0.f};

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
