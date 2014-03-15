//
// Adventure
//

#pragma once

#include <Rk/string_ref.hpp>
#include <Rk/types.hpp>

#include <memory>

namespace Ad
{
  namespace TexFlags
  {
    enum TexFlag : int
    {
      defaults = 0x00,

      repeat   = 0x00,
      clamp    = 0x01,

      linear   = 0x00,
      nearest  = 0x02,

      colour   = 0x00,
      alpha    = 0x04
    };

  }

  using TexFlags::TexFlag;

  class Texture
  {
    uint glname;

  public:
    typedef std::unique_ptr <Texture> Ptr;

    Texture (Rk::cstring_ref path, int flags = TexFlag::defaults);
    ~Texture ();

    int name () const
    {
      return glname;
    }

    template <typename... Args>
    static inline Ptr create (Args&&... args)
    {
      return std::make_unique <Texture> (std::forward <Args> (args)...);
    }

  };

}
