//
// Adventure
//

#pragma once

#include <Rk/string_ref.hpp>

#include <algorithm>
#include <vector>
#include <map>

#include "Init.hpp"

namespace Ad
{
  struct SystemCfg
  {
    Rk::cstring_ref name;
    Rk::cstring_ref path;
  };

  struct CStringLess
  {
    bool operator () (Rk::cstring_ref lhs, Rk::cstring_ref rhs)
    {
      return std::lexicographical_compare (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
    }

  };

  class UniverseCfg
  {
    std::vector <char> buffer;
    std::map <Rk::cstring_ref, SystemCfg, CStringLess> systems; // name -> path

    Rk::cstring_ref start_sys;

  public:
    UniverseCfg (
      std::vector <char> new_buffer,
      std::map <Rk::cstring_ref, SystemCfg, CStringLess> new_systems,
      Rk::cstring_ref new_start_sys
    ) :
      buffer    (std::move (new_buffer)),
      systems   (std::move (new_systems)),
      start_sys (new_start_sys)
    { }

    const SystemCfg& operator [] (Rk::cstring_ref name) const
    {
      auto iter = systems.find (name);
      if (iter == systems.end ())
        throw std::runtime_error ("No such system");
      return iter->second;
    }

    Rk::cstring_ref start_system () const
    {
      return start_sys;
    }

  };

  UniverseCfg load_universe (InitContext&, Rk::cstring_ref path);

}
