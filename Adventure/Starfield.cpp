//
// Adventure
//

#include "Starfield.hpp"

#include "StarfieldShader.hpp"
#include "Buffer.hpp"

#include <Rk/vector.hpp>
#include <Rk/clamp.hpp>

#include <random>

namespace Ad
{
  namespace
  {
    struct Star
    {
      v4f pos;
      v4f col;
    };

    static_assert (sizeof (Star) == 32, "uh oh");

    Buffer make_stars (uint size, uint seed, float tight, float scale)
    {
      std::mt19937 gen (seed);
      std::uniform_real_distribution <float> uni_dist (0.0f, 1.0f);
      std::exponential_distribution  <float> exp_dist (tight);

      std::vector <Star> stars (size);
      for (auto& star : stars)
      {
        float theta = 2.0f * 3.14159265f * uni_dist (gen);
        float z = 2.0f * uni_dist (gen) - 1.0f;
        float sinphi = std::sin (std::acos (z));

        star.pos = v4f {
          std::cos (theta) * sinphi,
          std::sin (theta) * sinphi, 
          z,
          1
        };

        float r = scale * exp_dist (gen);
        float dim = Rk::clamp (r, 0.0f, 1.0f);

        star.col = v4f {
          dim * (0.7f + 0.3f * uni_dist (gen)),
          dim * (0.7f + 0.3f * uni_dist (gen)),
          dim * (0.7f + 0.3f * uni_dist (gen)),
          r   
        };
      }

      return Buffer (stars.size () * sizeof (Star), stars.data ());
    }

  }

  Geom make_starfield (uint size, uint seed, float tight, float scale)
  {
    auto stars = make_stars (size, seed, tight, scale);

    auto geom = Geom (
      { { StarfieldShader::attrib_vertpos, stars.name (), 4, GL_FLOAT, 32,  0 },
        { StarfieldShader::attrib_colour,  stars.name (), 4, GL_FLOAT, 32, 16 } },
      0,
      size,
      0
    );

    stars.release ();
    return geom;
  }

}
