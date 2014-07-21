//
// Adventure
//

#pragma once

#include <Rk/types.hpp>

#include <initializer_list>
#include <utility>

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
    uint vao_name,
         idx_type,
         idx_count;

  public:
    Geom () :
      vao_name  (0),
      idx_type  (0),
      idx_count (0)
    { }

    Geom             (const Geom&) = delete;
    Geom& operator = (const Geom&) = delete;

    Geom (Geom&& other) :
      vao_name  (other.vao_name),
      idx_type  (other.idx_type),
      idx_count (other.idx_count)
    {
      other.vao_name  = 0;
      other.idx_type  = 0;
      other.idx_count = 0;
    }

    Geom& operator = (Geom&& other)
    {
      clear ();
      std::swap (vao_name  = 0, other.vao_name);
      std::swap (idx_type  = 0, other.idx_type);
      std::swap (idx_count = 0, other.idx_count);
      return *this;
    }

    Geom (const Attrib* attribs, uint count, uint index_buffer, uint indx_count, uint index_type);

    Geom (std::initializer_list <Attrib> attribs, uint index_buffer, uint index_count, uint index_type) :
      Geom (attribs.begin (), (uint) attribs.size (), index_buffer, index_count, index_type)
    { }

    ~Geom ()
    {
      clear ();
    }

    void clear ();

    uint name () const
    {
      return vao_name;
    }

    uint index_type () const
    {
      return idx_type;
    }

    uint index_count () const
    {
      return idx_count;
    }

    explicit operator bool () const
    {
      return vao_name != 0;
    }

  };

}
