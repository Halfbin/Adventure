//
// Adventure
//

#include "Phase.hpp"

#include "Texture.hpp"

#include <Rk/transform.hpp>

#include <cmath>

namespace Ad
{
  cxf rotation (float theta)
  {
    return cxf { std::cos (theta), std::sin (theta) };
  }

  mat4f ortho (int iw, int ih)
  {
    auto w = float (iw),
         h = float (ih);

    return Rk::matrix_rows {
      v4f { 2 / w,    0,    0,  0 }, 
      v4f {   0,   -2 / h,  0,  0 },
      v4f {   0,      0,   -1,  0 },
      v4f {   0,      0,    0,  1 }
    };
  }

  class PlayPhase :
    public Phase
  {
    Texture::Ptr cobble;

    float t0, t1;

    void tick (float time, float step)
    {
      t0 = time;
      t1 = time + step;
    }

    void render (Frame& frame)
    {
      frame.clear_colour = vec4f (0.00f, 0.26f, 0.51f, 1.0f);

      auto eye_to_clip = ortho (frame.width, frame.height);
      auto world_to_eye = mat4f::identity ();

      frame.set_eye_to_clip (eye_to_clip);
      frame.set_world_to_eye (world_to_eye);

      float geom [] = {
        -240, -320, 0, 0,
         240, -320, 0, 0,
         240,  320, 0, 0,
        -240,  320, 0, 0
      };

      auto theta = 0.2 * Rk::lerp (t0, t1, frame.alpha);

      frame.draw (cobble -> name (), geom, 4, v2f (0, 0), rotation (theta), v2f (1/64.f, 1/64.f));
    }

  public:
    PlayPhase ()
    {
      cobble = Texture::create ("Art/Flags1a.png", TexFlags::nearest);
    }

    ~PlayPhase () = default;

  };

  Phase::Ptr create_play_phase ()
  {
    return std::make_unique <PlayPhase> ();
  }

}
