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
  struct ShipType
  {
    Rk::cstring_ref name,
                    model_path,
                    materials_path;
  };

  class ShipTypes
  {
    std::vector <char> buffer;
    std::map <Rk::cstring_ref, ShipType, CStringRefLess> ship_types;

  public:
    ShipTypes (
      std::vector <char> new_buffer,
      std::map <Rk::cstring_ref, ShipType, CStringRefLess> new_ship_types
    ) :
      buffer     (std::move (new_buffer)),
      ship_types (std::move (new_ship_types))
    { }

    const ShipType& operator [] (Rk::cstring_ref name) const
    {
      auto iter = ship_types.find (name);
      if (iter == ship_types.end ())
        throw std::runtime_error ("No such ship type");
      return iter->second;
    }

  };

  ShipTypes load_ship_types (InitContext&, Rk::cstring_ref path);

}
