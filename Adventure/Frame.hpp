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

  enum FrameLayer
  {
    layer_solar = 0,
    layer_field
  };

  class Frame
  {
    friend class Renderer;

    struct ModelItem
    {
      uint layer,
           geom,
           idx_type,
           first_idx,
           idx_count,
           tex;
      v4f  colour;
      v3f  trn;
      vsf  rot;
      v3f  scale;
    };

    v3f camera_pos [2];
    vsf camera_ori;

    float camera_near [2],
          camera_far  [2];

    std::vector <ModelItem> model_items;

    uint starfield_geom,
         starfield_size;

  /*u8* buf_ptr;
    u8* buf_end;*/

  public:
    int width, height;
    float alpha;

    vec4f clear_colour;

    void set_camera (v3f pos_solar, float zns, float zfs, v3f pos_field, float znf, float zff, vsf ori)
    {
      camera_pos  [layer_solar] = pos_solar;
      camera_pos  [layer_field] = pos_field;
      camera_near [layer_solar] = zns;
      camera_far  [layer_solar] = zfs;
      camera_near [layer_field] = znf;
      camera_far  [layer_field] = zff;
      camera_ori = ori;
    }

    void set_starfield (const Geom& geom)
    {
      starfield_geom = geom.name ();
      starfield_size = geom.index_count ();
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
      FrameLayer layer,
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
        ModelItem { layer, geom.name (), geom.index_type (), first_idx, idx_count, tex, colour, translate, rotate, scale }
      );
    }

  };

}
