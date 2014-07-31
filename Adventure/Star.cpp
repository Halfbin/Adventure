//
// Adventure
//

#include "Solar.hpp"

#include "Gusher.hpp"
#include "Grab.hpp"

#include <GL/glew.h>

namespace Ad
{
  namespace
  {
    class StarFactory :
      public SolarFactory
    {
      int radius;

      bool param (Rk::cstring_ref key, Rk::cstring_ref value)
      {
        if (key == "radius")
          grab_int (radius, value, 1);
        else
          return SolarFactory::param (key, value);

        return true;
      }

      Solar create ()
      {
        return Solar (make_gusher (), pos, ori, v3i {radius,radius,radius});
      }

    };

    class StarType :
      public SolarType
    {
      SolarFactory::Ptr make_factory ()
      {
        return std::make_unique <StarFactory> ();
      }

    public:
      StarType () :
        SolarType ("star")
      { }

    } star_type;

  }

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

}
