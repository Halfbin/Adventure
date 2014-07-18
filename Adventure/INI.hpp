//
// Adventure
//

#pragma once

#include <Rk/string_ref.hpp>

#include <vector>
#include <map>

namespace Ad
{
  enum INIStatus
  {
    done = 0,
    got_section,
    done_section,
    got_pair
  };

  class INILoader
  {
    struct Impl;
    Impl* impl;

  public:
    INILoader (Rk::cstring_ref path);

    INIStatus proceed ();

    Rk::cstring_ref section () const;
    Rk::cstring_ref key     () const;
    Rk::cstring_ref value   () const;

  };

}
