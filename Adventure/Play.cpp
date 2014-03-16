//
// Adventure
//

#include "Phase.hpp"

#include "Texture.hpp"
#include "Player.hpp"
#include "Item.hpp"

#include <Rk/transform.hpp>

#include <cmath>

namespace Ad
{
  cxf rotation (float theta)
  {
    return cxf { std::cos (theta), std::sin (theta) };
  }

  mat4f ortho (int iw, int ih, float dr)
  {
    auto w = float (iw),
         h = float (ih);

    return Rk::matrix_rows {
      v4f { 2 / w,    0,      0,     0 },
      v4f {   0,   -2 / h,    0,     0 },
      v4f {   0,      0,   -1 / dr,  0 },
      v4f {   0,      0,      0,     1 }
    };
  }

  class PlayPhase :
    public Phase
  {
    Player::Ptr player;
    std::vector <Entity::Ptr> entities;
    
    Texture::Ptr cobble;

    float t0, t1;

    void add_entity (Entity::Ptr ent)
    {
      entities.push_back (std::move (ent));
    }

    void input (const Event* events, uint count, const KeyState* keys)
    {
      player -> input (events, count, keys);
    }

    void tick (float time, float step)
    {
      t0 = time;
      t1 = time + step;

      for (auto&& ent : entities)
        ent -> tick (time, step);

      player -> tick (time, step);
    }

    void render (Frame& frame)
    {
      frame.clear_colour = vec4f (0.00f, 0.26f, 0.51f, 1.0f);

      auto eye_to_clip = ortho (frame.width, frame.height, 1000.0f);
      frame.set_eye_to_clip (eye_to_clip);

      float geom [] = {
        -480, -320, -7.50, -4,
         480, -320,  7.50, -4,
         480,  320,  7.50,  4,
        -480,  320, -7.50,  4
      };

      frame.draw (cobble -> name (), geom, 4, v2f (0, 0), cxf (1, 0), v2f (1/64.f, 1/64.f));

      for (auto&& ent : entities)
        ent -> draw (frame);

      player -> draw (frame);
    }

  public:
    PlayPhase ()
    {
      player = create_player ();
      add_entity (create_item ("Art/Globe1a.png", v2i (0, -0)));

      cobble = Texture::create ("Art/Flags1a.png", TexFlags::nearest);
    }

    ~PlayPhase () = default;

  };

  Phase::Ptr create_play_phase ()
  {
    return std::make_unique <PlayPhase> ();
  }

}
