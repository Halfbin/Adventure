//
// Adventure
//

#pragma once

#include <vector>
#include <map>

#include "CStringRefLess.hpp"
#include "Init.hpp"

namespace Ad
{
  struct SystemCfg
  {
    Rk::cstring_ref name;
    Rk::cstring_ref path;
  };

  class UniverseCfg
  {
    std::vector <char> buffer;
    std::map <Rk::cstring_ref, SystemCfg, CStringRefLess> systems;
    
    Rk::cstring_ref start_sys,
                    start_ship;

  public:
    UniverseCfg (
      std::vector <char> new_buffer,
      std::map <Rk::cstring_ref, SystemCfg, CStringRefLess> new_systems,
      Rk::cstring_ref new_start_sys,
      Rk::cstring_ref new_start_ship
    ) :
      buffer     (std::move (new_buffer)),
      systems    (std::move (new_systems)),
      start_sys  (new_start_sys),
      start_ship (new_start_ship)
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
    
    Rk::cstring_ref start_ship_type () const
    {
      return start_ship;
    }

  };

  UniverseCfg load_universe (InitContext&, Rk::cstring_ref path);

}
