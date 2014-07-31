//
// Adventure
//

#include "Player.hpp"

#include "Constants.hpp"
#include "FLModel.hpp"
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
    Model model;

    float cam_log_dist,
          cam_elevation;
    vsf   cam_azimuth,
          cam_ori;

    float pitching, yawing, rolling;

    v3f prev_pos;
    v3f speed;
    v3f dir;
    
    Key pitch_forward_key, pitch_back_key,
        rudder_left_key,   rudder_right_key,
        roll_cw_key,       roll_ccw_key;

    void input (const InputContext& ctx)
    {
      using namespace Keys;

      // Rotate
      auto pitchf = ctx.keys [pitch_forward_key],
           pitchb = ctx.keys [pitch_back_key],
           rudl   = ctx.keys [rudder_left_key],
           rudr   = ctx.keys [rudder_right_key],
           rollcw = ctx.keys [roll_cw_key ],
           rollac = ctx.keys [roll_ccw_key];

      key_axis (pitching, pitchf, pitchb);
      key_axis (yawing,   rudl,   rudr);
      key_axis (rolling,  rollcw, rollac);

      vsf pitch = Rk::rotation (0.020f * pitching, v3f (0, 1, 0)),
          yaw   = Rk::rotation (0.020f * yawing,   v3f (0, 0, 1)),
          roll  = Rk::rotation (0.020f * rolling,  v3f (1, 0, 0));

      ori = Rk::unit (ori * pitch * yaw * roll);

      cam_log_dist = Rk::clamp (cam_log_dist + ctx.wheel * 0.001f, 1.4f, 10.0f);

      cam_elevation = Rk::clamp (cam_elevation + 0.001f * ctx.pointer.y, -1.5f, 1.5f);
      cam_azimuth = Rk::unit (cam_azimuth * Rk::rotation (-0.001f * ctx.pointer.x, v3f {0,0,1}));

      /*vsf cam_pitch = Rk::rotation (cam_elevation, Rk::conj (cam_azimuth, v3f {0,1,0}));

      cam_ori = cam_pitch * cam_azimuth;*/

      vsf cam_pitch = Rk::rotation (cam_elevation, v3f {0,1,0});

      cam_ori = cam_azimuth * cam_pitch;

      // Translate
    /*auto forward = ctx.keys [forward_key],
           left    = ctx.keys [left_key   ],
           back    = ctx.keys [back_key   ],
           right   = ctx.keys [right_key  ],
           drop    = ctx.keys [drop_key   ],
           rise    = ctx.keys [rise_key   ];

      key_axis (dir.x, forward, back );
      key_axis (dir.y, left,    right);
      key_axis (dir.z, rise,    drop );*/
    }

    void advance (float time, float step)
    {
      prev_pos = field_pos;
      auto vel = Rk::conj (ori, speed * unit (dir));
      field_pos += vel * step;
    }

    void draw (Frame& frame)
    {
      auto cam_dist = powf (10.0f, cam_log_dist);
      auto eye_off = Rk::conj (cam_ori, v3f {-cam_dist, 0, 0});
      auto draw_pos = Rk::lerp (prev_pos, field_pos, frame.alpha);

      auto s = solar_scale;
      frame.set_camera (22.0f, solar_pos + (draw_pos + eye_off) / s, 1e2f, 1e6f, draw_pos + eye_off, 0.1f, 10000.f, cam_ori);

      frame.draw (layer_field, model, draw_pos, ori);
    }

    void collide (v3f normal, float pdist)
    {
      field_pos += normal * pdist;
      if (dot (dir, normal) < 0)
        dir = nil;
    }

  public:
    PlayerImpl (const InitContext& init, const ShipType& ship, v3i new_spos, v3f new_fpos, vsf new_ori) :
      Player (new_spos, new_fpos, new_ori)
    {
      cam_log_dist = 1.4f;
      cam_elevation = 0.0f;
      cam_azimuth = identity;

      model = load_fl_cmp (init, ship.model_path);

      bbox_mins = {-0.4f,-0.4f, 0.0f};
      bbox_maxs = { 0.4f, 0.4f, 1.8f};

      pitching = yawing = rolling = 0;

      prev_pos = new_fpos;
      dir = nil;

      speed = {100, 40, 40};

      using namespace Keys;
      pitch_forward_key = alpha_w;
      pitch_back_key    = alpha_s;
      rudder_left_key   = alpha_q;
      rudder_right_key  = alpha_e;
      roll_cw_key       = alpha_d;
      roll_ccw_key      = alpha_a;
    }

    ~PlayerImpl () = default;

  };

  Player::Ptr create_player (const InitContext& init, const ShipType& ship, v3i spos, v3f fpos, vsf ori)
  {
    return std::make_unique <PlayerImpl> (init, ship, spos, fpos, ori);
  }

}
