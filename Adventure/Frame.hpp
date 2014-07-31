//
// Adventure
//

#pragma once

#include "Model.hpp"
#include "Geom.hpp"
#include "Mesh.hpp"

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
    
    v3f camera_pos [2];
    vsf camera_ori;

    float camera_fov,
          camera_near [2],
          camera_far  [2];

    struct MeshItem
    {
      uint layer,
           first_mesh,
           mesh_count;
      v3f  trn;
      vsf  rot;
      v3f  scale;
    };

    std::vector <Mesh>     mesh_buffer;
    std::vector <MeshItem> mesh_items;

    uint starfield_geom,
         starfield_size;

  /*u8* buf_ptr;
    u8* buf_end;*/

  public:
    int width, height;
    float alpha;

    vec4f clear_colour;

    void set_camera (float fov, v3f pos_solar, float zns, float zfs, v3f pos_field, float znf, float zff, vsf ori)
    {
      camera_fov = fov;
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
      const Mesh* meshes,
      size_t mesh_count,
      v3f translate,
      vsf rotate = identity,
      v3f scale = { 1.f, 1.f, 1.f })
    {
      auto first_mesh = mesh_buffer.size ();
      mesh_buffer.insert (mesh_buffer.end (), meshes, meshes + mesh_count);

      mesh_items.push_back (
        MeshItem { layer, (uint) first_mesh, (uint) mesh_count, translate, rotate, scale }
      );
    }

    void draw (
      FrameLayer layer,
      const Model& model,
      v3f translate,
      vsf rotate = identity,
      v3f scale = { 1.f, 1.f, 1.f })
    {
      draw (layer, model.meshes_begin (), model.mesh_count (), translate, rotate, scale);
    }

  };

}
