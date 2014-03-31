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
    v3f prev_pos,
        dir,
        vel;

    vsf   azimuth;
    float elevation;
    bool  crouching,
          on_ground;

    v3f speed;
    Key up_key, left_key, down_key, right_key, crouch_key, jump_key;

    void input (const InputContext& ctx)
    {
      using namespace Keys;

      // Crouching
      auto crouch = ctx.keys [crouch_key];
      crouching = crouch.down != 0;

      // Looking
      azimuth = Rk::rotation (-0.001f * ctx.pointer.x, v3f (0, 0, 1))
              * azimuth;

      elevation = Rk::clamp (elevation + 0.001f * ctx.pointer.y, -1.55f, 1.55f);
      auto elev_axis = Rk::conj (azimuth, v3f (0, 1, 0));

      ori = unit (Rk::rotation (elevation, elev_axis) * azimuth);

      // Walking
      auto up     = ctx.keys [up_key    ],
           left   = ctx.keys [left_key  ],
           down   = ctx.keys [down_key  ],
           right  = ctx.keys [right_key ],
           jump   = ctx.keys [jump_key  ];

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

      if (jump)
        dir.z = 1;
    }

    void advance (float time, float step)
    {
      prev_pos = pos;

      if (on_ground)
      {
        vel = speed * Rk::conj (azimuth, unit (dir));
        if (vel.z != 0)
          on_ground = false;
      }
      else
      {
        auto gravity = v3f {0,0,-1} * 20;
        auto drag = v3f {0,0,1} * pow (Rk::abs (vel), 2.0f) * 0.01;
        vel += (gravity + drag) * step;
      }

      pos += vel * step;
    }

    void draw (Frame& frame)
    {
      v3f eye_off { 0, 0, 1.7f };
      if (crouching)
        eye_off *= 0.5f;

      auto draw_pos = Rk::lerp (prev_pos, pos, frame.alpha);
      frame.set_camera (pos + eye_off, ori);
    }

    void collide (v3f normal, float pdist)
    {
      pos += normal * pdist;
      if (normal.z * normal.z > normal.x * normal.x + normal.y * normal.y)
        on_ground = true;
      dir.z = 0;
    }

  public:
    PlayerImpl () :
      Player ({-15,0,5}, identity)
    {
      bbox_mins = {-0.4f,-0.4f, 0.0f};
      bbox_maxs = { 0.4f, 0.4f, 1.8f};

      prev_pos = pos;
      dir = nil;
      vel = nil;

      azimuth = identity;
      elevation = 0.f;

      crouching = false;
      on_ground = false;

      speed = {8,8,12};

      using namespace Keys;
      up_key     = alpha_w;
      left_key   = alpha_a;
      down_key   = alpha_s;
      right_key  = alpha_d;
      crouch_key = lt_ctrl;
      jump_key   = spacebar;
    }

    ~PlayerImpl () = default;

  };

  Player::Ptr create_player ()
  {
    return std::make_unique <PlayerImpl> ();
  }

}
