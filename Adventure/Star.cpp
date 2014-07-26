//
// Adventure
//

#include "Entity.hpp"

#include "Gusher.hpp"

#include <GL/glew.h>

namespace Ad
{
/*void triangle (v3f a, v3f b, v3f c, int level, int max_level)
  {
    if (level == max_level)
    {
      // add /\abc to mesh and return
      return;
    }

    // Calculate side midpoints
    auto mab = unit (0.5f * (a + b));
    auto mbc = unit (0.5f * (b + c));
    auto mca = unit (0.5f * (c + a));

    // Recurse over subdivisions
    triangle (a, mab, mca, level + 1, max_level);
    triangle (b, mbc, mab, level + 1, max_level);
    triangle (c, mca, mbc, level + 1, max_level);
    triangle (mab, mbc, mca, level + 1, max_level);
  }*/


  class Star :
    public Entity
  {
    void advance (float time, float step)
    {

    }

    void draw (Frame& frame)
    {
      // sun equatorial radius ~700 000 km = 7e8 m
      //                                   = 7e2 su
      frame.draw (layer_solar, geom, GL_TRIANGLES, solar_pos, ori, v3f (7e2f, 7e2f, 7e2f));
    }

    void collide (v3f normal, float pdist)
    {

    }

    Geom geom;

  public:
    Star (v3f new_pos, vsf new_ori) :
      Entity (new_pos, new_ori)
    {
      geom = make_gusher ();
    }

    ~Star () = default;

  };

  Entity::Ptr make_star (v3f pos, vsf ori)
  {
    return std::make_unique <Star> (pos, ori);
  }

}
