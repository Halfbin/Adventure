//
// Adventure
//

#pragma once

#include <Rk/types.hpp>

#include <initializer_list>

namespace Ad
{
  struct Attrib
  {
    uint index,
         buffer,
         n,
         type,
         stride,
         offset;
  };

  class Geom
  {
    uint vao_name;

  public:
    Geom (const Attrib* attribs, uint count, uint index_buffer = 0);

    Geom (std::initializer_list <Attrib> attribs, uint index_buffer = 0) :
      Geom (attribs.begin (), attribs.size (), index_buffer)
    { }

    ~Geom ();

    uint name ()
    {
      return vao_name;
    }

  };

}
