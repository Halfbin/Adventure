//
// Adventure
//

#pragma once

#include "Geom.hpp"

#include <Rk/matrix.hpp>
#include <Rk/memory.hpp>
#include <Rk/versor.hpp>

#include <complex>
#include <vector>

namespace Ad
{
  typedef std::complex <float> cxf;

  class Frame
  {
    friend class Renderer;

    struct ModelItem
    {
      uint geom,
           idx_type,
           first_idx,
           idx_count,
           tex;
      v4f  colour;
      v3f  trn;
      vsf  rot;
      v3f  scale;
    };

    v3f camera_pos;
    vsf camera_ori;

    std::vector <ModelItem> model_items;

  /*u8* buf_ptr;
    u8* buf_end;*/

  public:
    int width, height;
    float alpha;

    vec4f clear_colour;

    void set_camera (v3f pos, vsf ori)
    {
      camera_pos = pos;
      camera_ori = ori;
    }

  /*void draw (uint texture, const float* data, uint count, v2f translate, cxf rotate, v2f tc_scale = v2f (1, 1))
    {
      ptrdiff_t bytes = count * 4 * sizeof (float);
      if (buf_end - buf_ptr < bytes)
        return;
      draw_items.push_back (DrawItem { texture, count, translate, rotate, tc_scale });
      buf_ptr = Rk::copy (buf_ptr, (const u8*) data, bytes);
    }*/

    void draw (
      const Geom& geom, 
      uint first_idx,
      uint idx_count,
      uint tex,
      v4f colour,
      v3f translate,
      vsf rotate = identity,
      v3f scale = { 1.f, 1.f, 1.f })
    {
      model_items.push_back (
        ModelItem { geom.name (), geom.index_type (), first_idx, idx_count, tex, colour, translate, rotate, scale }
      );
    }

  };

}
