//
// Adventure
//

#pragma once

#include <Rk/matrix.hpp>
#include <Rk/memory.hpp>

#include <complex>
#include <vector>

namespace Ad
{
  typedef std::complex <float> cxf;

  class Frame
  {
    friend class Renderer;

    struct DrawItem
    {
      uint texture,
           count;
      v2f  translate;
      cxf  rotate;
      v2f  tc_scale;
    };

    mat4f eye_to_clip,
          world_to_eye;

    std::vector <DrawItem> draw_items;

    u8* buf_ptr;
    u8* buf_end;

  public:
    int width, height;
    float alpha;

    vec4f clear_colour;

    void set_eye_to_clip (mat4f e2c)
    {
      eye_to_clip = e2c;
    }

    void set_world_to_eye (mat4f w2e)
    {
      world_to_eye = w2e;
    }

    void draw (uint texture, const float* data, uint count, v2f translate, cxf rotate, v2f tc_scale = v2f (1, 1))
    {
      ptrdiff_t bytes = count * 4 * sizeof (float);
      if (buf_end - buf_ptr < bytes)
        return;
      draw_items.push_back (DrawItem { texture, count, translate, rotate, tc_scale });
      buf_ptr = Rk::copy (buf_ptr, (const u8*) data, bytes);
    }

  };

}
