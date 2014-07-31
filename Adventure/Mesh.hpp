//
// Adventure
//

#pragma once

#include <Rk/types.hpp>

namespace Ad
{
  struct Mesh
  {
    uint geom,
         index_type,
         prim_type,
         vertex_base,
         vertex_max,
         index_offset,
         index_count,
         material;

    Mesh () = default;

    Mesh (uint g, uint idx_t, uint prim_t, uint v_base, uint v_max, uint idx_off, uint idx_count, uint mat) :
      geom         (g),
      index_type   (idx_t),
      prim_type    (prim_t),
      vertex_base  (v_base),
      vertex_max   (v_max),
      index_offset (idx_off),
      index_count  (idx_count),
      material     (mat)
    { }

  };

}
