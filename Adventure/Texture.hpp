//
// Adventure
//

#pragma once

#include <Rk/string_ref.hpp>
#include <Rk/types.hpp>

#include <memory>

namespace Ad
{
  class Texture
  {
    uint glname;

  public:
    typedef std::unique_ptr <Texture> Ptr;

    Texture (Rk::cstring_ref path);
    ~Texture ();

    int name () const
    {
      return glname;
    }

    static inline Ptr create (Rk::cstring_ref path)
    {
      return std::make_unique <Texture> (path);
    }

  };

}
