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

    Buffer make_stars (int count, int seed)
    {
      std::mt19937 gen (seed);
      std::uniform_real_distribution <float> uni_dist (0.0f, 1.0f);
      std::exponential_distribution  <float> exp_dist (3.5f);

      std::vector <Star> stars (count);
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

        float r = 1.2f * exp_dist (gen);

        float scale = Rk::clamp (r, 0.0f, 1.0f);

        star.col = v4f {
          scale * (0.7f + 0.3f * uni_dist (gen)),
          scale * (0.7f + 0.3f * uni_dist (gen)),
          scale * (0.7f + 0.3f * uni_dist (gen)),
          r   
        };
      }

      return Buffer (count * 32, stars.data ());
    }

  }

  Geom make_starfield (uint size, uint seed)
  {
    auto stars = make_stars (size, seed);

    return Geom (
      { { StarfieldShader::attrib_vertpos, stars.name (), 4, GL_FLOAT, 32,  0 },
        { StarfieldShader::attrib_colour,  stars.name (), 4, GL_FLOAT, 32, 16 } },
      0,
      size,
      0
    );
  }

}
