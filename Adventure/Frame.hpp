//
// Adventure
//

#pragma once

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

    struct DrawItem
    {
      uint texture,
           count;
      v2f  translate;
      cxf  rotate;
      v2f  tc_scale;
    };

    struct ModelItem
    {
      uint geom,
           n_idxs,
           idx_type;
      v4f  colour;
      v3f  trn;
      vsf  rot;
      v3f  scale;
    };

    mat4f eye_to_clip;
    v2f   camera;

    std::vector <DrawItem>  draw_items;
    std::vector <ModelItem> model_items;

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

    void set_camera (v2f cam)
    {
      camera = cam;
    }

    void draw (uint texture, const float* data, uint count, v2f translate, cxf rotate, v2f tc_scale = v2f (1, 1))
    {
      ptrdiff_t bytes = count * 4 * sizeof (float);
      if (buf_end - buf_ptr < bytes)
        return;
      draw_items.push_back (DrawItem { texture, count, translate, rotate, tc_scale });
      buf_ptr = Rk::copy (buf_ptr, (const u8*) data, bytes);
    }

    void draw (
      uint geom, 
      uint n_idxs, uint idx_type,
      v4f colour,
      v3f translate,
      vsf rotate = identity,
      v3f scale = { 1.f, 1.f, 1.f })
    {
      model_items.push_back (ModelItem { geom, n_idxs, idx_type, colour, translate, rotate, scale });
    }

  };

}
